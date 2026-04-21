#include "Server.hpp"

void Server::handleUser(Client* client, std::vector<std::string>& params)
{
	// ERR_ALREADYREGISTERED (462)
	// if USER is sent after registration is complete, reject it
	if (client->isRegistered())
	{
		sendToClient(client,
			":server 462 " + client->getNick() + " :You may not register\r\n");
		return;
	}

	// ERR_NEEDMOREPARAMS (461)
	// needed: <username> <0> <*> <realname> - 4 params
	// if <username> is empty, treat as missing params
	if (params.size() < 4 || params[0].empty())
	{
		sendToClient(client,
			":server 461 * USER :Not enough parameters\r\n");
		return;
	}
	
}