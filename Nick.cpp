#include "Server.hpp"

void Server::checkRegistered(Client *client)
{
	if (client->isPasswordValidated() && !client->getNick().empty() && !client->getUserName().empty())
	{
		client->setRegistered(true);
		//client->getSendBuffer() += "001 " + client->getNick() + " :Welcome to the " + NETWORK_NAME + " Network, " + client->getNick() + "!" + client->getUserName() + "@" + HOST + "\r\n";
		sendToClient(client,
			"001 " + client->getNick() + " :Welcome to the " + NETWORK_NAME + " Network, " + client->getNick() + "!" 
			+ client->getUserName() + "@" + HOST + "\r\n");
	}
}

bool Server::isValidNick(const std::string& nick)
{
	if (nick[0] == '#' || nick[0] == ':' || isdigit(nick[0]))
		return false;

	for (size_t i = 0; i < nick.size(); i++)
	{
		char c = nick[i];
		if (!isalnum(c) && c != '[' && c != ']' && c != '{' && c != '}' && c != '\\' && c != '-' && c != '_' && c != '|')
			return false;
	}

	return true;
}


void Server::handleNick(Client *client, std::vector<std::string> params)
{
	std::string nick = client->getNick().empty() ? "*" : client->getNick(); // if the nick hasn't been set yet, we use a placeholder

	if (!client->isPasswordValidated())
	{
		client->getSendBuffer() += "451 " + nick + " :You have not registered\r\n";
		std::cout << "not registered\n";
		return;
	}
	if (params.empty())
	{
		client->getSendBuffer() += "431 " + nick + " :No nickname given\r\n";
		std::cout << "no nickname\n";
		return;
	}
	if (params.size() > 1 || !isValidNick(params[0]))
	{
		client->getSendBuffer() += "432 " + nick + " " + params[0] + " :Erroneus nickname\r\n";
		std::cout << "wrong nickname\n";
		return;
	}
	for (std::map<int, Client*>::iterator it = _clientList.begin(); it != _clientList.end(); it++)
	{
		if (it->second->getNick() == params[0])
		{
			client->getSendBuffer() += "433 " + nick + " " + params[0] + " :Nickname is already in use\r\n";
			std::cout << "nick name in use\n";
			return;
		}
	}
	
	std::string oldNick = nick; // save the old nick or * if not set
	client->setNick(params[0]);
	client->getSendBuffer() += ":" + oldNick + " NICK " + params[0] + "\r\n"; // success message
	std::cout << "nickname is " + params[0] + '\n';
	checkRegistered(client);
}