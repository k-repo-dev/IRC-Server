#include "Server.hpp"

void Server::handleUser(Client* client, std::vector<std::string>& params)
{
	std::string nick = client->getNick().empty() ? "*" : client->getNick();

	if (!client->isPasswordValidated())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 451 " + nick + " :You have not registered\r\n");
		return;
	}

	// ERR_ALREADYREGISTERED (462)
	// if USER is sent after registration is complete, reject it
	if (client->isRegistered())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 462 " + client->getNick() + " :You may not register\r\n");
		return;
	}

	// ERR_NEEDMOREPARAMS (461)
	// needed: <username> <0> <*> <realname> - 4 params
	// if <username> is empty, treat as missing params
	if (params.size() < 4 || params[0].empty())
	{
		sendToClient(client,
			std::string(":") + SERVER_NAME + " 461 * USER :Not enough parameters\r\n");
		return;
	}

	// params[0] = username
	// params[1] = 0
	// params[2] = *
	// params[3] = realname (trailing ':' already stripped by detectCommands)
	std::string username = params[0];
	std::string realname = params[3];

	// IRC conventional USERLEN max is 10
	const size_t USERLEN = 10;
	if (username.size() > USERLEN)
		username = username.substr(0, USERLEN);

	// prepending '~' when no indent server is running
	username = "~" + username;

	client->setUserName(username);
	client->setRealName(realname);
	// check if registration is now complete
	checkRegistered(client);
}