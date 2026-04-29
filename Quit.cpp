#include "Server.hpp"

void Server::handleQuit(Client* client, std::vector<std::string>& params)
{
	// a reason is always given, even if QUIT is called without params
	// if empty, then reason is just "Quit: "
	std::string reason = "Quit: ";
	if (!params.empty())
	{
		std::string joined = params[0];
		for (size_t i = 1; i < params.size(); i++)
			joined += " " + params[i];
		reason += joined;
	}

	std::string announceQuit = ":" + client->getNick() + "!" + client->getUserName() + "@localhost QUIT :" + reason + "\r\n";
	for (auto it = _channelList.begin(); it != _channelList.end();)
	{
		Channel* channel = it->second;
		if (channel->isMember(client)) // check if the quitting client is in Channel
		{
			sendToChannel(channel, announceQuit); // send to everyone where that client was
			channel->removeMember(client);
			if (channel->getMembers().empty()) // was it the last member?
			{
				delete channel;
				it = _channelList.erase(it); // point to next valid position and erase the channel from map
				continue; // ignore the ++it after erase, because "it" is already pointing to next
			}
		}
		it++; //advance the loop in the case the if condition is false
	}

	// acknowledging the client using QUIT command with an ERROR message
	std::string errorMsg =
		"ERROR :Closing link: " + client->getNick()
		+ "[" + client->getUserName() + "@localhost] ("
		+ reason + ")\r\n";
	send(client->getFD(), errorMsg.c_str(), errorMsg.size(), 0);

	removeClient(client->getFD()); //remove client from server
}