#include "Server.hpp"

volatile bool g_running = true;

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
	
	_epoll_fd = epoll_create1(EPOLL_CLOEXEC);
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
	for (std::map<std::string, Channel*>::iterator it = _channelList.begin();
		it != _channelList.end(); ++it)
	{
		delete it->second;
	}
	close(_server_fd);
	close(_epoll_fd);
}

void Server::runServer()
{
	struct epoll_event events[MAX_EVENT];
	while (g_running)
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
	int client_fd = accept(_server_fd, NULL, NULL); // accept blocks here until a client connects
	if (client_fd == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK) // no clients waiting
			return;
		if (errno == EINTR) // pressed ctrl + c
			return;
		std::cerr << "accept() failed\n";
		return;
	}

	setNonBlocking(client_fd);

	struct epoll_event client_ev;
	client_ev.events = EPOLLIN;
	client_ev.data.fd = client_fd;
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);

	_clientList[client_fd] = new Client(client_fd);
	std::cout << "New client: fd= " << client_fd << std::endl;
}

void Server::handleClient(int fd)
{
	char buffer[BUFFER_SIZE];

	int bytes = recv(fd, buffer, BUFFER_SIZE - 1, 0);

	if (bytes == -1 && errno == EAGAIN)
			return;
	if (bytes <= 0)
	{
		removeClient(fd);
		return ;
	}

	buffer[bytes] = '\0';
	if (_clientList[fd]->getRecvBuffer().size() + bytes > MAX_BUFFER)
	{
		removeClient(fd);
		return ;
	}
 	_clientList[fd]->getRecvBuffer() += buffer; // accumulate into per-client buffer
	

	// do something with the complete buffer
	std::string& data = _clientList[fd]->getRecvBuffer();
	size_t pos;
	while ((pos = data.find("\r\n")) != std::string::npos ||
			(pos = data.find("\n")) != std::string::npos)
	{
		std::string line = data.substr(0, pos);
		size_t delimLen = (pos + 1 < data.size() && data[pos] == '\r'
			&& data[pos + 1] == '\n') ? 2 : 1;
		data.erase(0, pos + delimLen);
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if (!line.empty())
		{
			detectCommands(_clientList[fd], line);
			if (_clientList.find(fd) == _clientList.end())
				return;
		}
	}
	// partial line stays in data - handled when rest arrives
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
	std::map<int, Client*>::iterator it = _clientList.find(fd);
	if (it != _clientList.end())
	{
		delete it->second;
		_clientList.erase(it);
	}
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
	close(fd);
	std::cout << "Client fd=" << fd << " disconnected\n"; 
}

void Server::sendToChannel(Channel* channel, const std::string& msg){
	for (std::unordered_map<int, Client*> :: const_iterator it = channel->getMembers().begin();
		it!=channel->getMembers().end(); it++){
			sendToClient(it->second, msg);
	}
}
void Server::sendToChannelOperators(Channel* channel, const std::string& msg){
	for (std::unordered_map<int, Client*> :: const_iterator it = channel->getMembers().begin();
		it!=channel->getMembers().end(); it++){
			if (channel->isOperator(it->second))
				sendToClient(it->second, msg);
	}
}

void Server::sendToUnique(Client* client, const std::string& msg){
	std::unordered_map<int, Client*> uniqMembers;
	for (std::map<std::string, Channel*> :: const_iterator ch = _channelList.begin();
         ch != _channelList.end(); ++ch)
	{	
		if (ch->second->isMember(client))
		{
			for (std::unordered_map<int, Client*> :: const_iterator it = ch->second->getMembers().begin();
			it != ch->second->getMembers().end(); it++){
				uniqMembers.insert({it->first, it->second});
			}
		}
	}
	for (std::unordered_map<int, Client*> :: const_iterator it = uniqMembers.begin();it!=uniqMembers.end(); it++){
		if (it->second != client)
			sendToClient(it->second, msg);
	}
}

Client* Server::getClientByNick(const std::string&nick) //see if a client is in the server
{
	for (auto it = _clientList.begin(); it != _clientList.end(); it++)
	{
		if (it->second->getNick() == nick) //check nicks of all clients to see if it matches
			return it->second; // return client object
	}
	return nullptr;
}

void handle_sigint(int)
{
    g_running = false;
}