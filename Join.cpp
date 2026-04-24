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

void	Server::joinChannel(Client* client, std::string& chan, std::string& key){

	if (chan[0] != '#'){
		sendToClient(client, "403 " + client->getNick() + " " + chan + " :No such Channel\r\n");
		return ;
	}

	std::map<std::string, Channel*>::iterator it = _channelList.find(chan);
	if (it == _channelList.end()){
		Channel* channel = new Channel(chan);
		_channelList.insert({channel->getChannel(), channel});
		channel->addMember(client);
		channel->addOperator(client);
		if (key != "")
			channel->setKey(key);
		sendToChannel(channel, ":" + client->getNick() + " JOIN :" + chan + "\r\n");
		std::cout << client->getNick() << " created and joined new channel " + chan + " \r\n";
		
	}
	else{
		Channel* channel = it->second;
		if (key != "" && channel->getKey() != key){
			sendToClient(client, "475 " + client->getNick() + " " + chan + " :Bad channel key\r\n");
		}

		if (!channel->isMember(client)){
			channel->addMember(client);
			sendToChannel(channel, ":" + client->getNick() + " JOIN :" + chan + "\r\n");
			std::cout << client->getNick() << "is joining channel " + chan + " \r\n";
		}
	}
}
void	Server::handleJoin(Client* client, std::vector<std::string>& params){

	if (!client->isRegistered()){
		return;
	}
	if (params.empty()){
		sendToClient(client, "461 " + client->getNick() + " JOIN: Not enough parameters\r\n");
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




/*ERR_NEEDMOREPARAMS (461)
ERR_NOSUCHCHANNEL (403)
ERR_TOOMANYCHANNELS (405)
ERR_BADCHANNELKEY (475)
ERR_BANNEDFROMCHAN (474)
ERR_CHANNELISFULL (471)
ERR_INVITEONLYCHAN (473)
ERR_BADCHANMASK (476)
RPL_TOPIC (332)
RPL_TOPICWHOTIME (333)
RPL_NAMREPLY (353)
RPL_ENDOFNAMES (366)*/