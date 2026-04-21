#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include "Client.hpp"
#include <vector>
#include <sstream>


#define BUFFER_SIZE 1024
#define MAX_EVENT 64
#define NETWORK_NAME "ft_irc"
#define HOST "localhost"

class Server
{
	public:
		Server(int port, const std::string& password);
		~Server();
		void runServer();

	private:
		int _port; 
		int _server_fd;
		int _epoll_fd;
		const std::string _password;                                                                                            
		std::map<int, Client*> _clientList;
		//Channel _channels[];

		void acceptClient();
		void handleClient(int fd);
		void flushClient(int fd);
		void removeClient(int fd);
		void setNonBlocking(int fd);
		void sendToClient(Client* client, const std::string& msg);
		void processMessage(Client* client, const std::string& line);
		void handlePass(Client *client, std::vector<std::string> params);
		void handleNick(Client *client, std::vector<std::string> params);
		bool isValidNick(const std::string& nick);
		void detectCommands(Client* client);
		void checkRegistered(Client *client);
};

enum command{
	NICK,
	USER,
	PASS,
	QUIT,
	JOIN,
	PART,
	PRIVMSG,
	PING,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	DEFAULT,
};

