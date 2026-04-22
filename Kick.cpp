#include "Server.hpp"

void Server::handleKick(Client *client, std::vector<std::string> params)
{
	std::string nick = client->getNick().empty() ? "*" : client->getNick();

	if (params.size() < 2) // needs at least channel and user to kick
	{
		sendToClient(client,
			":461 " + client->getNick() + " :Not enough parameters\r\n");
		return;
	}
	std::string channelName = params[0];
	std::string reasonToKick = params.size() > 2 ? params[2] : nick;
	if (_channelList.find(channelName) == _channelList.end()) // look for channel in map to see if it exists
	{
		sendToClient(client,
			":403 " + client->getNick() + "" + channelName + " :No such channel\r\n");
		return;
	}

	Channel* channel = _channelList[channelName];
	if (!channel->isMember(client))
	{
		sendToClient(client,
			":442 " + client->getNick() + "" + channelName + " :You're not on that channel\r\n");
		return;
	}
	if (!channel->isOperator(client))
	{
		sendToClient(client,
			":482 " + client->getNick() + "" + channelName + " :You're not channel operator\r\n");
		return;
	}

	std::vector<std::string> kicked;
	std::stringstream stream(params[1]); // put into a stream so we can read from it
	std::string temp;
	while (std::getline(stream, temp, ',')) // reads from stream and keeps putting everything up to , into temp (goes until no more ,)
		kicked.push_back(temp);

	for (size_t i = 0; i < kicked.size(); i++)
	{
		Client* target = channel->getMemberByNick(kicked[i]); // we get the client object of the nick we're looking for
		if (target == nullptr)
		{
			sendToClient(client,
				":441 " + client->getNick() + "" + kicked[i] + "" + channelName + " :They aren't on that channel\r\n");
			continue; // if we have several targets, we need to try to kick each one
		}

		std::string kickMess = ":" + nick + "!" + client->getUserName() + "@localhost KICK " + channelName + " " + kicked[i] + " :" + reasonToKick + "\r\n";
		const std::unordered_map<int, Client*>& members = channel->getMembers();
		for (auto it = members.begin(); it != members.end(); it++)
			sendToClient(it->second, kickMess);
		channel->removeMember(target);
	}
}