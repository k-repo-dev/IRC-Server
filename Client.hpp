#pragma once
#include <iostream>

#define BUFFER 1024
#define MAX_IRC_LINE 510 //irc line max 512 including \r\n  

class Client
{
private:
	const int	_clientfd;
	std::string	_name;
	std::string _nick;
	bool 		_registered;
	std::string _bufferIn[BUFFER];
	std::string _bufferOut[BUFFER];
	
	
public:
	Client(int fd);
	~Client();
	void	setName(std::string name);
	void	setNick(std::string nick);
};

//calling the constructor form server:
//int fd = accept(server_fd);
//client(fd);

	Client::Client(int fd): _clientfd(fd), _name(NULL), _nick(NULL), _registered(false){

	}

	Client::~Client(){

	}

	void	Client::setName(std::string name){
		_name = name;
	}

	void	Client::setNick(std::string nick){
		_nick = nick;
	}