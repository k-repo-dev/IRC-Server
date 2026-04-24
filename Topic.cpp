#include "Server.hpp"

void Server::handleTopic(Client* client, std::vector<std::string>& params)
{
	std::string nick = client->getNick().empty() ? "*" : client->getNick();

	if (params.empty())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 461 " + nick + " TOPIC :Not enough parameters\r\n");
	}

	std::string channelName = params[0];

	if (_channelList.find(channelName) == _channelList.end())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 403 " + client->getNick() + "" + channelName + " :No such channel\r\n");
		return;
	}

	Channel* channel = _channelList[channelName];
	if (!channel->isMember(client))
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 442 " + client->getNick() + "" + channelName + " :You're not on that channel\r\n");
		return;
	}
	
	if (params.size() == 1) // if the command only has channel name it's for viewing the topic
	{
		if (channel->getTopic().empty())
			sendToClient(client,
			std::string(":") + SERVER_NAME + " 331 " + client->getNick() + "" + channelName + " :No topic is set\r\n");
	}
	// if the command has channel name + topic it's for changing the topic
}