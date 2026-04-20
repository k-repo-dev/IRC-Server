#include "Server.hpp"

void Server::handlePass(Client *client, std::vector<std::string> params)
{
	std::string nick = client->getNick();
	if (client->isRegistered())
	{
		client->getSendBuffer() += ""
	}
	if (params.empty())
	{

	}
	if (params[0] != _password)
	{

	}
}