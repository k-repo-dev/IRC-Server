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


#define BUFFER_SIZE 1024
#define MAX_EVENT 64

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
};