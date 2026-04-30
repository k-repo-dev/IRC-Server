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
	for (size_t i = 2; i < params.size(); i++)
		arguments.push_back(params[i]);
	applyModeChanges(client, channel, targetChannel, modeChanges, arguments); 
}

void Server::applyModeChanges(Client* client, Channel* channel, std::string channelName, std::string modeChanges, std::vector<std::string> arguments)
{
	struct parsedModes {char sign; char modeLetter; std::string args;};
	char	sign = '+';
	size_t		argIndex = 0;
	std::string arg;
	std::vector<parsedModes> list;

	// parsing modes to apply
	for (size_t i = 0; i < modeChanges.size(); i++)
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
			else if (modeChanges[i] == 'k' && sign == '+')
			{
				if (argIndex >= arguments.size() || arguments[argIndex].empty())
				{
					sendToClient(client,
						std::string(":") + SERVER_NAME + " 696 " + client->getNick()
						+ " " + channelName + " k * :Key must not be empty\r\n");
					continue;
				}
				arg = arguments[argIndex++];
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

	std::string modeStr = "";
	std::string argStr = "";
	char	lastSign = 0;

	// loop for applying the parsed modes
	for (size_t i = 0; i < list.size(); i++)
	{
		// only appned sign when it changes
		if (list[i].sign != lastSign)
		{
			modeStr += list[i].sign;
			lastSign = list[i].sign;
		}

		// always append the mode letter
		modeStr += list[i].modeLetter;

		if (!list[i].args.empty())
			argStr += (argStr.empty() ? "" : " ") + list[i].args;

		// apply the actual change to the channel object
		if (list[i].sign == '+')
		{
			switch (list[i].modeLetter)
			{
			case 'i':
				channel->setInviteOnly(channel);
				break;
			case 't':
				channel->setTopicRestricted(channel);
				break;
			case 'k':
				channel->setKey(list[i].args);
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
				break;
			case 't':
				channel->removeTopicRestricted(channel);
				break;
			case 'k':
				channel->removeKey();
				break;
			default:
				break;
			}
		}

	}

	// send full MODE message to all on channel for applied mode changes
	if (!modeStr.empty())
	{
		std::string msg = ":" + client->getNick() + "!" + client->getUserName()
						+ "@" + HOST
						+ " MODE " + channelName
						+ " " + modeStr
						+ (argStr.empty() ? "" : " " + argStr) + "\r\n";
		sendToChannel(channel, msg);
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