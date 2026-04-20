#pragma once

#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "Client.hpp"


#define BUFFER_SIZE 1024
#define MAX_EVENT 64

class Server
{
	public:
		Server(int port);
		~Server();
		void runServer();
		void processMessage(Client* client, const std::string& message);

	private:
		int _port; 
		int _server_fd;
		int _client_fd;
		int _epoll_fd;
		//char _buffer[BUFFER_SIZE];
		const std::string _password = "1234-2567";                                                                                            
		std::map<int, Client*> _clientList;
		//Channel _channels[];

		void acceptClient();
		void handleClient(int currentfd,  const struct epoll_event& event);
		void removeClient();
		void setNonBlocking(int fd);
};