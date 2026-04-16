#pragma once
#include <iostream>

#define BUFFERSIZE 1024
#define MAX_IRC_LINE 512 //irc line max 512 including \r\n  

class Client
{
private:
	const int	_clientfd;
	std::string	_name;
	std::string _nick;
	bool 		_registered;
	std::string _recvBuffer;
	std::string _sendBuffer;
	
	
public:
	Client(int fd);
	~Client();
	void	setName(const std::string &name);
	void	setNick(const std::string &nick);
	std::string& getRecvBuffer(void);
	std::string& getSendBuffer(void);
};



	Client::Client(int fd): _clientfd(fd), _name(""), _nick(""), _registered(false){

	}

	Client::~Client(){

	}

	std::string& Client::getRecvBuffer(void){
		return _recvBuffer;
	}

	std::string& Client::getSendBuffer(void){
		return _sendBuffer;
	}

	void	Client::setName(const std::string& name){
		_name = name;
		if (_nick != "")
			_registered = true;
	}

	void	Client::setNick(const std::string& nick){
		_nick = nick;
		if (_name != "")
			_registered = true;
	}