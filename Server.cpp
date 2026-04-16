#include "Server.hpp"

void Server::runServer()
{
	if ((_serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "socket failed" << std::endl;
		return;
	}
	sockaddr_in address = std::memset(&address, 0, sizeof(address)); // address our server will use and set everything to 0
	address.sin_family = AF_INET; // set type (IPv4)
	address.sin_addr.s_addr = INADDR_ANY; //accept connections from any IP on this machine
	address.sin_port = htons(PORT); // makes sure the port number is understood correctly outside our computer
	if (bind(_serverfd, (sockaddr *)&address, sizeof(address)) < 0) // assigns address (IP and port) to our socket
	{
		std::cerr << "bind failed" << std::endl;
		return;
	}
	listen(_serverfd, 10); // Listening (backlog = 10 pending connections)
	std::cout << "Listening on PORT: " << PORT << std::endl;
	while (1)
	{
		_clientfd = accept(_serverfd, NULL, NULL); // accept blocks here until a client connects

		int n = read(_clientfd, _buffer, BUFFER_SIZE -1); // reading what the client sent
		buffer[n] = '\0';
		std::cout << "Received: " << _buffer << "\n";

		char *resp = "Hello from server!\n"; // temporary respons to client
		write(_clientfd, resp, std::strlen(resp));

		close(_clientfd); // Done with this client
	}

	close(_serverfd);
}