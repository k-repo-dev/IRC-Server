#include "Server.hpp"

void Server::handleQuit(Client* client, std::vector<std::string>& params)
{
	// a reason is always given, even if QUIT is called without params
	// if empty, then reason is just "Quit: "
	std::string reason = "Quit: ";
	if (!params.empty())
		reason += params[0];
	
	
}