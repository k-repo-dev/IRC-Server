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

void Server::handlePart(Client* client, std::vector<std::string>& params){
	if (params.empty()){
		sendToClient(client, std::string(":") + SERVER_NAME + " 461 " + client->getNick() + " PART: Not enough parameters\r\n");
		return ;
	}
	std::vector<std::string> channels = separateParams(params[0]);
	std::string reason = "";
	if (params.size() > 1){
		reason = " :";
		for (size_t i = 1; i < params.size(); i++)
			reason = reason + params[i] + " ";
	}
	for (size_t i = 0; i < channels.size(); i++){
		std::map<std::string, Channel*>::iterator it = _channelList.find(channels[i]);
		if (it == _channelList.end()){
			sendToClient(client, std::string(":") + SERVER_NAME + " 403 " + client->getNick() + " " + channels[i] + " PART :No such Channel\r\n");
			continue;
		}
		Channel* channel = it->second;
		if (!channel->isMember(client)){
			sendToClient(client, std::string(":") + SERVER_NAME + " 442 " + client->getNick() + " " + channel->getChannel() + " PART :You're not on that channel\r\n");
			continue;
		}
		if (channel->isOperator(client))
			channel->removeOperator(client);	
		sendToChannel(channel, ":" + client->getNick() + "!" + client->getUserName() + "@" + HOST 
		+ " PART " + channel->getChannel() + reason + "\r\n");
		channel->removeMember(client);
	}
}
