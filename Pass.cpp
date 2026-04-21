#include "Server.hpp"

void Server::handlePass(Client *client, std::vector<std::string> params)
{
	std::string nick = client->getNick().empty() ? "*" : client->getNick(); // if the nick hasn't been set yet, we use a placeholder

	if (client->isRegistered())
	{
		sendToClient(client,
			"462 " + nick + " :You may not reregister\r\n");
		return;
	}
	if (params.empty() || params.size() > 1)
	{
		sendToClient(client,
			"461 " + nick + " PASS :Not enough parameters\r\n");
		return;
	}
	if (params[0] != _password)
	{
		sendToClient(client,
			"464 " + nick + " :Password incorrect\r\n");
		return;
	}
	client->setPasswordValidated(true);
	std::cout << " :Password correct\r\n";
	
}