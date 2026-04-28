#include "Server.hpp"

static void applyModeChanges(Channel* channel, std::string modeChanges, std::vector<std::string> arguments);


void Server::handleMode(Client* client, std::vector<std::string>& params)
{
	if (params.empty())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 461 " + client->getNick() + " MODE :Not enough parameters\r\n");
		return;
	}

	std::string targetChannel = params[0];

	if (_channelList.find(targetChannel) == _channelList.end())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 403 " + client->getNick() + " " + targetChannel + " :No such channelr\r\n");
		return;
	}

	Channel* channel = _channelList[targetChannel];
	if (!channel->isMember(client))
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 442 " + client->getNick() + " " + targetChannel + " :You're not on that channel\r\n");
			return;
	}

	std::string modeChanges = params[1];
	std::vector<std::string> arguments;
	for (auto i = 2; i < params.size(); i++)
		arguments.push_back(params[i]);
	applyModeChanges(channel, modeChanges, arguments); 
}

static void applyModeChanges(Channel* channel, std::string modeChanges, std::vector<std::string> arguments)
{

}

void Channel::setInviteOnly(Channel* channel)
{
	channel->_inviteOnly = true;
}

void Channel::removeInviteOnly(Channel* channel)
{
	channel->_inviteOnly = false;
}