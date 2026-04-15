#include "Server.hpp"

void Server::runServer()
{
	if (_serverfd = socket(AF_INET, SOCK_STREAM, 0) < 0)
	{
		std::cerr << "socket failed" << std::endl;
		// exit cleanly
	}
}