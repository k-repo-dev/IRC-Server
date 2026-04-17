#include "Client.hpp"
#include "Server.hpp"

int main() {
    try {
        Server server(4241);
        server.runServer();
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
}