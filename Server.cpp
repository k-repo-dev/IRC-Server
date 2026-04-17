#include "Server.hpp"

Server::Server(int port) : _port(port), _server_fd(-1), _client_fd(-1), _epoll_fd(-1)
{
	if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("socket() failed");
	int opt = 1;
	setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address)); // address our server will use and set everything to 0
	address.sin_family = AF_INET; // set type (IPv4)
	address.sin_addr.s_addr = INADDR_ANY; //accept connections from any IP on this machine
	address.sin_port = htons(_port); // makes sure the port number is understood correctly outside our computer
	if (bind(_server_fd, (sockaddr *)&address, sizeof(address)) < 0) // assigns address (IP and port) to our socket
		throw std::runtime_error("bind() failed");
	if ((listen(_server_fd, 10)) < 0) // Listening (backlog = 10 pending connections)
		throw std::runtime_error("listen() failed");
	_epoll_fd = epoll_create1(EPOLL_CLOEXEC); // CHECK if fail
	struct epoll_event server_ev;
	server_ev.events = EPOLLIN;
	server_ev.data.fd = _server_fd;
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fd, &server_ev); // CHECK if fail
	std::cout << "Listening on PORT: " << _port << std::endl;
}

Server::~Server() {}

void Server::runServer()
{
	struct epoll_event events[MAX_EVENT];
	while (true)
	{
		int needAttention = epoll_wait(_epoll_fd, events, MAX_EVENT, -1);
		for (int i = 0; i < needAttention; i++)
		{
			int currentfd = events[i].data.fd;
			if (currentfd == _server_fd)
			{
				acceptClient();
			}
			else
			{
				handleClient(currentfd, events[i]);
			}
		}
	}
}

void Server::acceptClient()
{
	_client_fd = accept(_server_fd, NULL, NULL); // CHECK IF FAIL / accept blocks here until a client connects
	Client* client = new Client(_client_fd);
	_clientList[_client_fd] = client;
	struct epoll_event client_ev;
	client_ev.events = EPOLLIN;
	client_ev.data.fd = _client_fd;
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _client_fd, &client_ev);
	std::cout << "New client connected to server" << std::endl;
}

void Server::handleClient(int currentfd, const struct epoll_event& event)
{
	if (event.events & EPOLLIN) // client sent something, need to read it
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
				pos = client->getRecvBuffer().find("\r\n");
			}
			//TODO after processMessage puts something into the send buffer: 
			// -check for if the buffer isn't empty 
			//- register epoll_event, watch for EPOLLIN and EPOLLOUT
			//- epoll_ctl() EPOLL_CTL_MOD
			}	
			}
			if (event.events & EPOLLOUT) // have something to send to client, go write it
			{

			}
}

void Server::processMessage(Client* client, const std::string& message)
{
	(void)client;
	std::cout << "received: " << message << std::endl;

	//FOR NOW JUST TO SEE WHAT IS THE MESSAGE RECEIVED, LATER USED TO PARSE AND HANDLE COMMANDS
	// PUTTING INTO THE SEND CLIENT BUFFER 
}