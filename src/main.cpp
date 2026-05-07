#include "Client.hpp"
#include "Server.hpp"


int main(int ac, char *av[])
{
    if ( ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }
    //Validate PORT
    size_t pos;
    int port;
    try
    {
        port = std::stoi(av[1], &pos);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: invalid port\n";
        return 1;
    }
    if (pos != std::string(av[1]).length() || port < 1024 || port > 65535)
    {
        std::cerr << "Error: port must be between 1024 and 65535\n"; // below 1024 it needs root privileges and above 65535 doesn't exist
        return 1;
    }
    //Validate PASSWORD
    std::string pass = av[2];
    if (pass.empty())
    {
        std::cerr << "Error: password can't be empty\n";
        return 1;
    }
    for (size_t i = 0; i < pass.size(); i++)
    {
        if (isspace(pass[i]))
        {
            std::cerr << "Error: password can't have spaces\n";
            return 1;
        }
    }
    
    signal(SIGINT, handle_sigint);
    try {
        Server server(port, pass);
        server.runServer();
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
    return 0;
}