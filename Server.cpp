#include "Server.hpp"

Server::Server(int port, const std::string& password)
	: _port(port), _server_fd(-1), _epoll_fd(-1), _password(password)
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
	
	setNonBlocking(_server_fd);
	
	_epoll_fd = epoll_create1(EPOLL_CLOEXEC); // CHECK if fail
	if (_epoll_fd < 0)
		throw std::runtime_error("epoll_create1() failed");

	struct epoll_event server_ev;
	server_ev.events = EPOLLIN;
	server_ev.data.fd = _server_fd;
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _server_fd, &server_ev); // CHECK if fail

	std::cout << "Listening on PORT: " << _port << std::endl;
}

Server::~Server() 
{
	for (std::map<int, Client*>::iterator it = _clientList.begin();
		it != _clientList.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
	close(_server_fd);
	close(_epoll_fd);
}

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
				acceptClient();
			else
			{
				if (events[i].events & EPOLLIN)
					handleClient(currentfd);
				if (events[i].events & EPOLLOUT)
					flushClient(currentfd);
			}
		}
	}
}

void Server::acceptClient()
{
	int client_fd = accept(_server_fd, NULL, NULL); // CHECK IF FAIL / accept blocks here until a client connects
	if (client_fd < 0)
		return;

	setNonBlocking(client_fd);

	struct epoll_event client_ev;
	client_ev.events = EPOLLIN | EPOLLET;
	client_ev.data.fd = client_fd;
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);

	_clientList[client_fd] = new Client(client_fd);
	std::cout << "New client: fd= " << client_fd << std::endl;
}

void Server::handleClient(int fd)
{
	char buffer[BUFFER_SIZE];

	while (true)
	{
		int bytes = recv(fd, buffer, BUFFER_SIZE - 1, 0);

		if (bytes == -1 && errno == EAGAIN)
			break; // fully drained, wait for next epoll event

		if (bytes <= 0)
		{
			removeClient(fd);
			return ;
		}

		buffer[bytes] = '\0';
 		_clientList[fd]->getRecvBuffer() += buffer; // accumulate into per-client buffer
	}

	// do something with the complete buffer
	std::string& data = _clientList[fd]->getRecvBuffer();
	size_t pos;
	while ((pos = data.find("\r\n")) != std::string::npos ||
			(pos = data.find("\n")) != std::string::npos)
	{
		std::string line = data.substr(0, pos);
		data.erase(0, pos + 2);
		if (!line.empty())
			processMessage(_clientList[fd], line);
	}
	// partial line stays in data - handled when rest arrives
}

void Server::processMessage(Client* client, const std::string& line)
{
	client->setRecvBuffer(line);
	detectCommands(client);
	// placeholder - just echo the parsed line back for now
	// std::cout << "fd=" << client->getFD() << " | line: [" << line << "]\n";
	// sendToClient(client, "echo: " + line + "\r\n");
}

void Server::sendToClient(Client* client, const std::string& msg)
{
	client->getSendBuffer() += msg;

	// register EPOLLOUT so epoll wakes us when socket is writable
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	ev.data.fd = client->getFD();
	epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, client->getFD(), &ev);
}

void Server::flushClient(int fd)
{
	std::string& buf = _clientList[fd]->getSendBuffer();

	while (!buf.empty())
	{
		int sent = write(fd, buf.c_str(), buf.size());
		if (sent == -1 && errno == EAGAIN)
			break; // kernel buffer full, retry next EPOLLOUT
		if (sent <= 0)
		{
			removeClient(fd);
			return;
		}
		buf.erase(0, sent);
	}

	// nothing left to send - stop watching for EPOLLOUT
	if (buf.empty())
	{
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = fd;
		epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev);
	}
}

void Server::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0); // read current flags
	fcntl(fd, F_SETFL, flags | O_NONBLOCK); // write them back with O_NONBLOCK added
}

void Server::removeClient(int fd)
{
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
	close(fd);
	_clientList.erase(fd);
	std::cout << "Client fd=" << fd << " disconnected\n"; 
}