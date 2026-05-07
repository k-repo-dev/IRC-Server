#include "Server.hpp"

void Server::handleTopic(Client* client, std::vector<std::string>& params)
{
	if (params.empty())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 461 " + client->getNick() + " TOPIC :Not enough parameters\r\n");
		return;
	}

	std::string channelName = params[0];

	if (_channelList.find(channelName) == _channelList.end())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
		return;
	}

	Channel* channel = _channelList[channelName];
	if (!channel->isMember(client))
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 442 " + client->getNick() + " " + channelName + " :You're not on that channel\r\n");
		return;
	}
	
	if (params.size() == 1) // if the command only has channel name it's for viewing the topic
	{
		if (channel->getTopic().empty())
			sendToClient(client,
				std::string(":") + SERVER_NAME + " 331 " + client->getNick() + " " + channelName + " :No topic is set\r\n");
		else
		{
			sendToClient(client,
				std::string(":") + SERVER_NAME + " 332 " + client->getNick() + " " + channelName + " :" + channel->getTopic() + "\r\n");
			sendToClient(client,
				std::string(":") + SERVER_NAME + " 333 " + client->getNick() + " " + channelName + " " + channel->getTopicSetter() + " " + channel->getTopicTime() + "\r\n");
		}
		return;
	}

	// if the command has channel name + topic it's for changing the topic
	if (channel->isTopicRestricted() && !channel->isOperator(client))
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 482 " + client->getNick() + "" + channelName + " :You're not channel operator\r\n");
		return;
	}

	std::string newTopic = params[1];
	channel->setTopic(newTopic, client->getNick());

	std::string TopicMess = ":" + client->getNick() + "!" + client->getUserName() + "@localhost TOPIC " + channelName + " :" + newTopic + "\r\n";
	const std::unordered_map<int, Client*>& members = channel->getMembers();
	for (auto it = members.begin(); it != members.end(); it++)
		sendToClient(it->second, TopicMess);
}