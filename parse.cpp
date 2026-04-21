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

 void   Server::detectCommands(Client* client){
	std::string cmd;
	std::string arg;
	std::vector<std::string> params;
	std::string buf = client->getRecvBuffer();
	std::cout << buf;
	std::istringstream stream(client->getRecvBuffer());
    stream >> cmd;
	while (stream >> arg){
		// if (arg[0] == ':'){
		// 	needs to handle rest as one param
		// }
		params.push_back(arg);
	}

	switch(getCommands(cmd)){
		case NICK:
			break;
		case USER:
			break; 
		case PASS:
			handlePass(client, params);
			break;
		case QUIT:
			break;
		case JOIN:
			break;
		case PART:
			break;
		case PRIVMSG:
			break;
		case PING:
			break;
		case KICK:
			break;
		case INVITE:
			break;
		case TOPIC:
			break;
		case MODE:
			break;
		default:
			std::cout << "no command\n";
			break;
 }
}