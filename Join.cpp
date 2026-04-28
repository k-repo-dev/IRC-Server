#include "Server.hpp"

static std::vector<std::string> separateParams(const std::string& par){
	std::string arg;
	std::vector<std::string> params;
	std::istringstream stream(par);
	while (std::getline(stream, arg, ',')) {
		if (!arg.empty())
			params.push_back(arg);
	}
	return params;
}

static std::string memberList(Channel* channel){
	std::string list;
	
	for (std::unordered_map<int, Client*> :: const_iterator it = channel->getMembers().begin();
	it!=channel->getMembers().end(); it++){
		int fd = it->first;
		Client *client = it->second;
		std::string name = client->getNick();
		if (channel->getOperators().find(fd) != channel->getOperators().end())
			name = "@" + name;
		list = list + name + " ";
	}
	return list;
}

void	Server::joinChannel(Client* client, std::string& chan, std::string& key){
	if (chan[0] != '#'){
		sendToClient(client, std::string(":") + SERVER_NAME + " 403 " + client->getNick() + " " + chan + " :No such Channel\r\n");
		return ;
	}

	std::map<std::string, Channel*>::iterator it = _channelList.find(chan);
	if (it == _channelList.end()){ //no existing channel
		Channel* channel = new Channel(chan);
		_channelList.insert({channel->getChannel(), channel});
		channel->addMember(client);
		channel->addOperator(client);
		if (key != "")
			channel->setKey(key);
		sendToChannel(channel, ":" + client->getNick() + "!" + client->getUserName() + "@localhost JOIN " + channel->getChannel() + "\r\n");
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 353 " + client->getNick() + " = " + channel->getChannel() + " :" + memberList(channel) + "\r\n");
		sendToClient(client,
				std::string(":") + SERVER_NAME + " 366 " + client->getNick() + " " + channel->getChannel() + " :End of /NAMES list\r\n");
	}
	else{ //channel exists
		Channel* channel = it->second;
		if (key != "" && channel->getKey() != key){
			sendToClient(client, std::string(":") + SERVER_NAME + " 475 " + client->getNick() + " " + chan + " :Bad channel key\r\n");
			return ;
		}
		if (channel->isInviteOnly() && !channel->isInvited(client)){
			sendToClient(client, std::string(":") + SERVER_NAME + " 473 " + client->getNick() + " " + chan + " :Invite only channel\r\n");
			return ;
		}
		if (!channel->isMember(client)){
			channel->addMember(client);
			sendToChannel(channel, ":" + client->getNick() + "!" + client->getUserName() + "@localhost JOIN " + channel->getChannel() + "\r\n");
			if (!channel->getTopic().empty()){
				sendToClient(client,
				std::string(":") + SERVER_NAME + " 332 " + client->getNick() + " " + channel->getChannel() + " :" + channel->getTopic() + "\r\n");
				sendToClient(client,
				std::string(":") + SERVER_NAME + " 333 " + client->getNick() + " " + channel->getChannel() + " " + channel->getTopicSetter() + " " + channel->getTopicTime() + "\r\n");
			}
			sendToClient(client,
				std::string(":") + SERVER_NAME + " 353 " + client->getNick() + " = " + channel->getChannel() + " :" + memberList(channel) + "\r\n");
			sendToClient(client,
				std::string(":") + SERVER_NAME + " 366 " + client->getNick() + " " + channel->getChannel() + " :End of /NAMES list\r\n");
		}
	}
}
void	Server::handleJoin(Client* client, std::vector<std::string>& params){

	if (!client->isRegistered()){
		return;
	}
	if (params.empty()){
		sendToClient(client, std::string(":") + SERVER_NAME + " 461 " + client->getNick() + " JOIN: Not enough parameters\r\n");
		return ;
	}
	std::vector<std::string> channels = separateParams(params[0]);
	std::vector<std::string> keys;
	if (params.size() > 1)
		keys = separateParams(params[1]);

	for (size_t i = 0; i < channels.size(); i++){
		std::string key = "";
		if (i < keys.size())
			key = keys[i];
		joinChannel(client, channels[i], key);
	}
}
