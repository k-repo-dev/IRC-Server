#pragma once

#include <iostream>
#include <string>
#include <map>

#define PORT 4241

class Server
{
	public:
		void runServer();
	private:
		int _serverfd;
		const std::string _password = "1234-2567";
		std::map<int, Client*> _clientList;
		Channel _channels[];
	protected:
};