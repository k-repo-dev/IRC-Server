#include "Client.hpp"


Client::Client(int fd): _clientfd(fd), _name(""), _nick(""), _registered(false), _passwordValidated(false), _recvBuffer(""), _sendBuffer(""){
		(void)_clientfd;
}

Client::~Client(){}

std::string& Client::getRecvBuffer(void)
{
	return _recvBuffer;
}

std::string& Client::getSendBuffer(void){
	return _sendBuffer;
}

void Client::setRecvBuffer(const std::string& buffer)
{
	_recvBuffer = buffer;
}


void	Client::setName(const std::string& name)
{
	_name = name;
}

void	Client::setNick(const std::string& nick)
{
	_nick = nick;
}
int Client::getFD() const
{
	return _clientfd;
}

bool Client::isRegistered() const
{
	return _registered;
}

std::string Client::getNick() const
{
	return _nick;
}

std::string Client::getName() const
{
	return _name;
}

void Client::setRegistered(bool val)
{
	_registered = val;
}
void	Client::setPasswordValidated(bool val)
{
	_passwordValidated = val;
}

bool	Client::isPasswordValidated(void) const
{
	return _passwordValidated;
}
