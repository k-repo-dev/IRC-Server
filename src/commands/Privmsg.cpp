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

void Server::privmsgToChannel(Client *client, std::string channel, bool op, std::string& msg){
	std::map<std::string, Channel*>::iterator it = _channelList.find(channel);
	if (it == _channelList.end())
		sendToClient(client, std::string(":") + SERVER_NAME + " 403 " + client->getNick() + " " + channel + " :No such channel\r\n");
	else {
		if (op == true){
			if (!it->second->isOperator(client))
				sendToClient(client,  ":" + client->getNick() + "!" + client->getUserName() + "@" + HOST + " PRIVMSG " +  it->second->getChannel() + " " + msg + "\r\n");
			sendToChannelOperators(it->second,  ":" + client->getNick() + "!" + client->getUserName() + "@" + HOST + " PRIVMSG " +  it->second->getChannel() + " " + msg + "\r\n");
		}
		else
			sendToChannel(it->second,  ":" + client->getNick() + "!" + client->getUserName() + "@" + HOST + " PRIVMSG " +  it->second->getChannel() + " " + msg + "\r\n");
	}
}

void Server::handlePrivmsg(Client *client, std::vector<std::string> params){

		if (params.empty()){
		sendToClient(client, std::string(":") + SERVER_NAME + " 411 " + client->getNick() + " :No recipient given (PRIVMSG)\r\n");
		return ;
	}
	std::vector<std::string> targets = separateParams(params[0]);
	if (params.size() <= 1){
		sendToClient(client, std::string(":") + SERVER_NAME + " 412 " + client->getNick() + " :No text to send\r\n");
		return ;
	}
	std::string msg = params[1];
	for (size_t i = 0; i < targets.size(); i++){
		if (targets[i][0] == '#'|| targets[i][0] == '&' ){
			privmsgToChannel(client, targets[i], false, msg);
			continue ;
		}
		else if (targets[i][0] == '@'){
			if(targets[i][1] != '#' && targets[i][1] != '&')
				continue ;
			else{
				targets[i] = targets[i].substr(1);
				privmsgToChannel(client, targets[i], true, msg);
			}
		}
		else{
			Client* target = getClientByNick(targets[i]);
			if (!target){
				sendToClient(client, std::string(":") + SERVER_NAME + " 401 " + client->getNick() + " " + targets[i] + " :No such nick\r\n");
				continue ;
			}
			if (target == client)
				continue;
			sendToClient(target,  ":" + client->getNick() + "!" + client->getUserName() + "@" + HOST + " PRIVMSG " + target->getNick() + " "+ msg + "\r\n");
	}
	}
}
