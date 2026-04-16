#include "Server.hpp"

void Server::runServer()
{
	if ((_serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "socket failed" << std::endl;
		return;
	}
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address)); // address our server will use and set everything to 0
	address.sin_family = AF_INET; // set type (IPv4)
	address.sin_addr.s_addr = INADDR_ANY; //accept connections from any IP on this machine
	address.sin_port = htons(PORT); // makes sure the port number is understood correctly outside our computer
	if (bind(_serverfd, (sockaddr *)&address, sizeof(address)) < 0) // assigns address (IP and port) to our socket
	{
		std::cerr << "bind failed" << std::endl;
		return;
	}
	if ((listen(_serverfd, 10)) < 0) // Listening (backlog = 10 pending connections)
	{
		std::cerr << "listen failed" << std::endl;
		return;
	}
	int epoll_fd = epoll_create1(EPOLL_CLOEXEC); // CHECK if fail
	struct epoll_event server_ev;
	server_ev.events = EPOLLIN;
	server_ev.data.fd = _serverfd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _serverfd, &server_ev); // CHECK if fail
	struct epoll_event events[MAX_EVENT];
	std::cout << "Listening on PORT: " << PORT << std::endl;
	while (true)
	{
		int needAttention = epoll_wait(epoll_fd, events, MAX_EVENT, -1);
		for (int i = 0; i < needAttention; i++)
		{
			int currentfd = events[i].data.fd;
			if (currentfd == _serverfd)
			{
				_clientfd = accept(_serverfd, NULL, NULL); // CHECK IF FAIL / accept blocks here until a client connects
				Client* client = new Client(_clientfd);
				_clientList[_clientfd] = client;
				struct epoll_event client_ev;
				client_ev.events = EPOLLIN;
				client_ev.data.fd = _clientfd;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, _clientfd, &client_ev);
				std::cout << "New client connected to server" << std::endl;
			}
			else
			{
				if (events[i].events & EPOLLIN) // client sent something, need to read it
				{
					char buffer[BUFFER_SIZE]; //temporary buffer
					int bytes = recv(currentfd, buffer, sizeof(buffer),0);
					if (bytes > 0) // there's some data to read
					{
						Client* client = _clientList[currentfd]; //client connected to its file descriptor
						client->getRecvBuffer().append(buffer, bytes); // put it in the buffer of that specific client

						size_t pos = client->getRecvBuffer().find("\r\n");
						while (pos != std::string::npos) //extract full message
						{
							std::string mess = client->getRecvBuffer().substr(0, pos);
							client->getRecvBuffer().erase(0, pos + 2); //remove everything up to and including the \r\n
							processMessage(client, mess);
						}
						//TODO after processMessage puts something into the send buffer: 
						// -check for if the buffer isn't empty 
						//- register epoll_event, watch for EPOLLIN and EPOLLOUT
						//- epoll_ctl() EPOLL_CTL_MOD
					}	
				}
				if (events[i].events & EPOLLOUT) // have something to send to client, go write it
				{

				}
			}
		}
	}
}

void Server::processMessage(Client* client, const std::string& message)
{
	(void)client;
	std::cout << "received: " << message << std::endl;

	//FOR NOW JUST TO SEE WHAT IS THE MESSAGE RECEIVED, LATER USED TO PARSE AND HANDLE COMMANDS
	// PUTTING INTO THE SEND CLIENT BUFFER 
}