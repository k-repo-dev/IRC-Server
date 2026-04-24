 #include "Server.hpp"

static command	getCommands(std::string cmd){
	if (cmd == "NICK")
		return NICK;
	if (cmd == "USER")
		return USER;
	if (cmd == "PASS")
		return PASS;
	if (cmd == "QUIT")
		return QUIT;
	if (cmd == "JOIN")
		return JOIN;
	if (cmd == "PART")
		return PART;
	if (cmd == "PRIVMSG")
		return PRIVMSG;
	if (cmd == "PING")
		return PING;
	if (cmd == "KICK")
		return KICK;
	if (cmd == "INVITE")
		return INVITE;
	if (cmd == "TOPIC")
		return TOPIC;
	if (cmd == "MODE")
		return MODE;
	else
		return DEFAULT;

}

 void   Server::detectCommands(Client* client, const std::string&line){
	std::string cmd;
	std::string arg;
	std::vector<std::string> params;
	std::istringstream stream(line);
    stream >> cmd; 
	while (stream >> arg){
		if (arg[0] == ':')
		{
		// 	needs to handle rest as one param
		std::string trailing;
		std::getline(stream, trailing);
		params.push_back(arg.substr(1) + trailing); // strips the ':'
		break;
		}
		params.push_back(arg);
	}

	switch(getCommands(cmd)){
		case NICK:
			handleNick(client, params);
			break;
		case USER:
			handleUser(client, params);
			break; 
		case PASS:
			handlePass(client, params);
			break;
		case QUIT:
			handleQuit(client, params);
			break;
		case JOIN:
			break;
		case PART:
			break;
		case PRIVMSG:
			break;
		case PING:
			handlePing(client, params);
			break;
		case KICK:
			handleKick(client, params);
			break;
		case INVITE:
			break;
		case TOPIC:
			handleTopic(client, params);
			break;
		case MODE:
			break;
		default:
			std::cout << "no command\n";
			break;
 }
}