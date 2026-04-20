#include "Server.hpp"

void Server::handlePass(Client *client, std::vector<std::string> params)
{
	std::string nick = client->getNick().empty() ? "*" : client->getNick(); // if the nick hasn't been set yet, we use a placeholder

	if (client->isRegistered())
	{
		client->getSendBuffer() += "462 " + nick + " :You may not reregister\r\n";
		return;
	}
	if (params.empty())
	{
		client->getSendBuffer() += "461 " + nick + " PASS :Not enough parameters\r\n";
		return;
	}
	if (params[0] != _password)
	{
		client->getSendBuffer() += "464 " + nick + " :Password incorrect\r\n";
		return;
	}
	client->setPasswordValidated(true);
}