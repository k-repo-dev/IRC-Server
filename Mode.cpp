#include "Server.hpp"

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

void Server::applyModeChanges(Channel* channel, std::string modeChanges, std::vector<std::string> arguments)
{
	struct parsedModes {char sign; char modeLetter; std::string args;};
	char	sign = '+';
	int		argIndex = 0;
	std::string arg;
	std::vector<parsedModes> list;

	// parsing modes to apply
	for (int i = 0; i < modeChanges.size(); i++)
	{
		if (modeChanges[i] == '+' || modeChanges[i] == '-')
			sign = modeChanges[i];
		else
		{
			if (modeChanges[i] == 'i' || modeChanges[i] == 't'
				|| (sign == '-' && modeChanges[i] == 'k')
				|| (sign == '-' && modeChanges[i] == 'l'))
			{
				arg = "";
			}
			else if (argIndex < arguments.size())
			{
				arg = arguments[argIndex++];
			}
			else
				continue;
			
			list.push_back({sign, modeChanges[i], arg});
		}
	}

	// loop for applying the parsed modes
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i].sign == '+')
		{
			switch (list[i].modeLetter)
			{
			case 'i':
				channel->setInviteOnly(channel);
				sendToChannel(channel,
					"Invite only-mode set\r\n");
				break;
			case 't':
				channel->setTopicRestricted(channel);
				sendToChannel(channel,
					"Topic restricted-mode set\r\n");
				break;
			default:
				break;
			}
		}
		else
		{
			switch (list[i].modeLetter)
			{
			case 'i':
				channel->removeInviteOnly(channel);
				sendToChannel(channel,
					"Invite only-mode removed\r\n");
				break;
			case 't':
				channel->removeTopicRestricted(channel);
				sendToChannel(channel,
					"Topic restricted-mode removed\r\n");
				break;
			default:
				break;
			}
		}

	}
}

void Channel::setInviteOnly(Channel* channel)
{
	channel->_inviteOnly = true;
}

void Channel::removeInviteOnly(Channel* channel)
{
	channel->_inviteOnly = false;
}

void Channel::setTopicRestricted(Channel* channel)
{
	channel->_topicRestricted = true;
}

void Channel::removeTopicRestricted(Channel* channel)
{
	channel->_topicRestricted = false;
}