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
			std::string(":") + SERVER_NAME + " 403 " + client->getNick() + " " + channelName + " :No such channel\r\n");
		return;
	}

	Channel* channel = _channelList[channelName];
	if (!channel->isMember(client)) // is the person inviting in the channel?
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 442 " + client->getNick() + " " + channelName + " :You're not on that channel\r\n");
		return;
	}
	if (channel->isInviteOnly() && !channel->isOperator(client))
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 482 " + client->getNick() + " " + channelName + " :You're not channel operator\r\n");
		return;
	}

	Client* target = getClientByNick(toBeInvited); // search for nick of the client to invite in the whole server
	if (target == nullptr)
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 401 " + client->getNick() + " " + toBeInvited + " :No such nick\r\n");
		return;
	}
	if (channel->isMember(target)) // is the person to be invited already in the channel?
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 443 " + client->getNick() + " " + toBeInvited + " " + channelName + " :is already on channel\r\n");
		return;
	}
	sendToClient(client,
        std::string(":") + SERVER_NAME + " 341 " + client->getNick() + " " + toBeInvited + " " + channelName + "\r\n"); // notifies the one who sent the invite
    sendToClient(target,
        ":" + client->getNick() + "!" + client->getUserName() + "@localhost INVITE " + toBeInvited + " " + channelName + "\r\n");  // notifies the one invited 
    channel->addInvite(target); // add fd of the client to the _inviteList
}