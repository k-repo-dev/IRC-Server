#include "Server.hpp"

void Server::handleKick(Client *client, std::vector<std::string> params)
{
	std::string nick = client->getNick().empty() ? "*" : client->getNick();

	if (params.size() < 2) // needs at least channel and user to kick
	{
		sendToClient(client,
			":server 461 " + client->getNick() + " :Not enough parameters\r\n");
		return;
	}
	std::string channelName = params[0];
	std::string reasonToKick = params.size() > 2 ? params[2] : nick;
	if (_channelList.find(channelName) == _channelList.end()) // look for channel in map to see if it exists
	{
		sendToClient(client,
			":server 403 " + client->getNick() + "" + channelName + " :No such channel\r\n");
		return;
	}

	Channel* channel = _channelList[channelName];
	if (!channel->isMember(client))
	{
		sendToClient(client,
			":server 442 " + client->getNick() + "" + channelName + " :You're not on that channel\r\n");
		return;
	}
	if (!channel->isOperator(client))
	{
		sendToClient(client,
			":server 482 " + client->getNick() + "" + channelName + " :You're not channel operator\r\n");
		return;
	}

	std::vector<std::string> targets;
	


}