#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include "Client.hpp"
#include <vector>
#include <sstream>
#include "Channel.hpp"
#include <signal.h>

extern volatile bool g_running; // shared variable
#define BUFFER_SIZE 1024
#define MAX_EVENT 64
#define NETWORK_NAME "Swifties Server"
#define SERVER_NAME "swifties.local"
#define HOST "localhost"

struct ParsedMode { char sign; char modeLetter; std::string arg; };

class Server
{
	public:
		Server(int port, const std::string& password);
		~Server();
		void runServer();

	private:
		int _port; 
		int _server_fd;
		int _epoll_fd;
		const std::string _password;                                                                                            
		std::map<int, Client*> _clientList;
		std::map<std::string, Channel*> _channelList;

		void acceptClient();
		void handleClient(int fd);
		void flushClient(int fd);
		void removeClient(int fd);
		void setNonBlocking(int fd);
		void sendToClient(Client* client, const std::string& msg);
		void processMessage(Client* client, const std::string& line);
		void handlePass(Client *client, std::vector<std::string> params);
		void handleNick(Client *client, std::vector<std::string> params);
		bool isValidNick(const std::string& nick);
		void detectCommands(Client* client, const std::string&line);
		void checkRegistered(Client *client);
		void sendToChannel(Channel* channel, const std::string& msg);
		void sendToChannelOperators(Channel* channel, const std::string& msg);
		void sendToUnique(Client* client, const std::string& msg);
		Client* getClientByNick(const std::string&nick);

		void handleUser(Client* client, std::vector<std::string>& params);
		void handleQuit(Client* client, std::vector<std::string>& params);
		void handleKick(Client *client, std::vector<std::string> params);
		void handlePing(Client* client, std::vector<std::string>& params);
		void handleTopic(Client* client, std::vector<std::string>& params);
		void handleJoin(Client* client, std::vector<std::string>& params);
		void joinChannel(Client* client, std::string& chan, std::string& key);
		void handlePart(Client* client, std::vector<std::string>& params);
		void handlePrivmsg(Client *client, std::vector<std::string> params);
		void privmsgToChannel(Client *client, std::string channel, bool op, std::string& msg);
		void handleInvite(Client *client, std::vector<std::string> params);
		void handleMode(Client* client, std::vector<std::string>& params);
		std::vector<ParsedMode> parseModeString(Client* client, Channel* channel, const std::string& channelName, const std::string& modeChanges, const std::vector<std::string>& arguments);
		void applyParsedModes(Client* client, Channel* channel, const std::string& channelName, const std::vector<ParsedMode>& list);
		void sendChannelModes(Client* client, Channel* channel, const std::string& channelName);

};

void handle_sigint(int);

enum command{
	NICK,
	USER,
	PASS,
	QUIT,
	JOIN,
	PART,
	PRIVMSG,
	PING,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	DEFAULT,
};