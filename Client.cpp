#include "Client.hpp"


Client::Client(int fd): _clientfd(fd), _registered(false),
	 _passwordValidated(false), _hasUser(false), _recvBuffer(""), _sendBuffer("") {}

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


void	Client::setUserName(const std::string& username)
{
	_username = username;
	_hasUser = true;
}

void	Client::setRealName(const std::string& realname)
{
	_realname = realname;
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

bool Client::getHasUser() const
{
	return _hasUser;
}

std::string Client::getNick() const
{
	return _nick;
}

std::string Client::getUserName() const
{
	return _username;
}

std::string Client::getRealName() const
{
	return _realname;
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
