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
	cleanupClient(client, reason);
	// acknowledging the client using QUIT command with an ERROR message
	std::string errorMsg =
		"ERROR :Closing link: " + client->getNick()
		+ "[" + client->getUserName() + "@localhost] ("
		+ reason + ")\r\n";
	send(client->getFD(), errorMsg.c_str(), errorMsg.size(), 0);
	removeClient(client->getFD()); //remove client from server
}