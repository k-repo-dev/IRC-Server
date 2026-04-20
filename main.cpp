#include "Client.hpp"
#include "Server.hpp"

int main(int ac, char *av[])
{
    if ( ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }
    try {
        Server server(std::atoi(av[1]), av[2]);
        server.runServer();
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
}