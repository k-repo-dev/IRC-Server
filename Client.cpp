#include "Client.hpp"


Client::Client(int fd): _clientfd(fd), _name(""), _nick(""), _registered(false), _recvBuffer(""), _sendBuffer(""){
		(void)_clientfd;
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