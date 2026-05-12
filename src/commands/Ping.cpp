#include "Server.hpp"

void Server::handlePing(Client* client, std::vector<std::string>& params)
{
	if (!client->isRegistered())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 451 " + client->getNick() + " :You have not registered\r\n");
		return;
	}
	// ERR_NEEDMOREPARAMS (461) - no token at all
	if (params.empty())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 461 " + client->getNick() + " PING :Not enough parameters\r\n");
		return;
	}

	// ERR_NOORIGIN (409) - token is empty
	if (params[0].empty())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 409 " + client->getNick() + " :No origin specified\r\n");
		return;
	}

	sendToClient(client,
		std::string(":") + SERVER_NAME + " PONG " + SERVER_NAME + " :" + params[0] + "\r\n");
}