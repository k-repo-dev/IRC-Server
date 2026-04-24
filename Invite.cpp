#include "Server.hpp"

void Server::handleInvite(Client *client, std::vector<std::string> params)
{
	if (params.size() < 2)
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 461 " + client->getNick() + " INVITE:Not enough parameters\r\n");
		return;
	}

	std::string toBeInvited = params[0];
	std::string channelName = params[1];

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
	if (channel->isInviteOnly() && !channel->isOperator(client))
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 482 " + client->getNick() + "" + channelName + " :You're not channel operator\r\n");
		return;
	}

	Client* target = getClientByNick(toBeInvited);
}