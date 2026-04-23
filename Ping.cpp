#include "Server.hpp"

void Server::handlePing(Client* client, std::vector<std::string>& params)
{
	// ERR_NEEDMOREPARAMS (461) - no token at all
	if (params.empty())
	{
		sendToClient(client,
			":server 461 " + client->getNick() + " :Not enough parameters\r\n");
		return;
	}

	// ERR_NOORIGIN (409) - token is empty
	if (params[0].empty())
	{
		sendToClient(client,
			":server 409 " + client->getNick() + " :No origin specified\r\n");
		return;
	}

	sendToClient(client,
		":server PONG server :" + params[0] + "\r\n");
}