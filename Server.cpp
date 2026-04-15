#include "Server.hpp"

void Server::runServer()
{
	if ((_serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "socket failed" << std::endl;
		return;
	}
	sockaddr_in address; // address our server will use
	std::memset(&address, 0, sizeof(address)); // set everything to 0
	address.sin_family = AF_INET; // set type (IPv4)
	address.sin_addr.s_addr = INADDR_ANY; //accept connections from any IP on this machine
	address.sin_port = htons(PORT); // makes sure the port number is understood correctly outside our computer
	if (bind(_serverfd, (sockaddr *)&address, sizeof(address)) < 0) // assigns address (IP and port) to our socket
	{
		std::cerr << "bind failed" << std::endl;
		return;
	}

}