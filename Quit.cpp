#include "Server.hpp"

void Server::handleQuit(Client* client, std::vector<std::string>& params)
{
	// a reason is always given, even if QUIT is called without params
	// if empty, then reason is just "Quit: "
	std::string reason = "Quit: ";
	if (!params.empty())
		reason += params[0];
	
	// acknowledging the client f receiving the QUIT command with an ERROR message
	sendToClient(client,
		"ERROR :Closing link: " + client->getNick()
		+ "[" + client->getUserName() + "localhost] ("
		+ reason + ")\r\n");

	// PLACEHOLDER for notifiying other clients that share channels with the client whos quitting
	// need to be done when JOIN command is built
	// something like broadcastQuitToSharedChannels(client, reason);

	removeClient(client->getFD());
}