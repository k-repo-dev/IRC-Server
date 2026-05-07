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

    if (params[0][0] != '#')
	{
    	return;
	}

	if (_channelList.find(targetChannel) == _channelList.end())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 403 " + client->getNick() + " " + targetChannel + " :No such channel\r\n");
		return;
	}

	Channel* channel = _channelList[targetChannel];
	if (!channel->isMember(client))
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 442 " + client->getNick() + " " + targetChannel + " :You're not on that channel\r\n");
			return;
	}
	if (params.size() < 2)
	{
		sendChannelModes(client, channel, targetChannel);
		return;
	}
	if (!channel->isOperator(client))
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 482 " + client->getNick() 
			+ " " + targetChannel + " :You're not channel operator\r\n");
		return;
	}
	std::vector<std::string> arguments(params.begin() + 2, params.end());
	std::vector<ParsedMode> list = parseModeString(client, channel, targetChannel, params[1], arguments);
	applyParsedModes(client, channel, targetChannel, list);
}

std::vector<ParsedMode> Server::parseModeString(
	Client* client,
	Channel* channel,
	const std::string& channelName,
	const std::string& modeChanges,
	const std::vector<std::string>& arguments)
{
	std::vector<ParsedMode> list;
	char sign = '+';
	size_t argIndex = 0;

	for (size_t i = 0; i < modeChanges.size(); i++)
	{
		if (modeChanges[i] == '+' || modeChanges[i] == '-')
		{
			sign = modeChanges[i];
			continue;
		}
		char m = modeChanges[i];
		std::string arg;
		if (m == 'i' || m == 't'
			|| (sign == '-' && m == 'k')
			|| (sign == '-' && m == 'l'))
		{
			arg = "";
		}
		else if (m == 'k' && sign == '+')
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
		else if (m == 'l' && sign == '+')
		{
			if (argIndex >= arguments.size() || arguments[argIndex].empty())
			{
				sendToClient(client,
					std::string(":") + SERVER_NAME + " 696 " + client->getNick()
					+ " " + channelName + " l * :Limit must not be empty\r\n");
				continue;
			}
			const std::string& limitArg = arguments[argIndex];
			bool valid = !limitArg.empty();
			for (size_t j = 0; j < limitArg.size(); j++)
				valid = isdigit(limitArg[j]);
			if (!valid || std::atoi(limitArg.c_str()) <= 0)
			{
				sendToClient(client,
					std::string(":") + SERVER_NAME + " 696 " + client->getNick()
					+ " " + channelName + " l * :Limit must be a positive integer\r\n");
				continue;
			}
			arg = arguments[argIndex++];
		}
		else if (m == 'o')
		{
			if (argIndex >= arguments.size() || arguments[argIndex].empty())
			{
				sendToClient(client,
					std::string(":") + SERVER_NAME + " 696 " + client->getNick()
					+ " " + channelName + " o * :No target nick given\r\n");
				continue;
			}
			if (!channel->getMemberByNick(arguments[argIndex]))
			{
				sendToClient(client,
					std::string(":") + SERVER_NAME + " 441 " + client->getNick() + " "
					+ arguments[argIndex] + " " + channelName + " o * :No target nick given\r\n");
				continue;
			}
			arg = arguments[argIndex++];
		}
		else 
		{
			continue;
		}
		list.push_back({sign, m, arg});
	}
	return list;
}

void Server::applyParsedModes(
	Client *client,
	Channel* channel,
	const std::string& channelName,
	const std::vector<ParsedMode>& list)
{
	std::string modeStr;
	std::string argStr;
	char lastSign = 0;

	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i].sign != lastSign)
		{
			modeStr += list[i].sign;
			lastSign = list[i].sign;
		}
		modeStr += list[i].modeLetter;

		if (!list[i].arg.empty())
			argStr += (argStr.empty() ? "" : " ") + list[i].arg;
		
		if (list[i].sign == '+')
		{
			switch (list[i].modeLetter)
			{
				case 'i': channel->setInviteOnly(channel);	break;
				case 't': channel->setTopicRestricted(channel);	break;
				case 'k': channel->setKey(list[i].arg);	break;
				case 'l': channel->setLimit(std::atoi(list[i].arg.c_str()));	break;
				case 'o':
				{
					Client* target = channel->getMemberByNick(list[i].arg);
					if (target)
						channel->addOperator(target);
					break;
				}
				default:	break;
			}
		}
		else
		{
			switch (list[i].modeLetter)
			{
			case 'i': channel->removeInviteOnly(channel);	break;
			case 't': channel->removeTopicRestricted(channel);	break;
			case 'k': channel->removeKey();	break;
			case 'l': channel->removeLimit();	break;
			case 'o':
			{
					Client* target = channel->getMemberByNick(list[i].arg);
					if (target)
						channel->removeOperator(target);
					break;
				}
			default:	break;
			}
		}
	}

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

void Server::sendChannelModes(Client* client, Channel* channel, const std::string& channelName)
{
	std::string modeStr = "+";
	std::string argStr;

	if (channel->isInviteOnly())
		modeStr += "i";
	if (channel->isTopicRestricted())
		modeStr += "t";
	if (channel->hasKey())
	{
		modeStr += "k";
		argStr += (argStr.empty() ? "" : " ") + channel->getKey();
	}
	if (channel->hasLimit())
	{
		modeStr += "l";
		argStr += (argStr.empty() ? "" : " ") + std::to_string(channel->getLimit());
	}

	sendToClient(client,
		std::string(":") + SERVER_NAME + " 324 " + client->getNick()
		+ " " + channelName
		+ " " + modeStr
		+ (argStr.empty() ? "" : " " + argStr) + "\r\n");
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