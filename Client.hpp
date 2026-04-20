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
	bool		_passwordValidated;
	std::string _recvBuffer;
	std::string _sendBuffer;
	
	
public:
	Client(int fd);
	~Client();

	int getFD() const;
	bool isRegistered() const;
	void setRegistered(bool val);
	void	setName(const std::string &name);
	void	setNick(const std::string &nick);
	std::string getNick() const;
	std::string getName() const;
	void	setPasswordValidated(bool val);
	bool	isPasswordValidated(void) const;
	std::string& getRecvBuffer(void);
	std::string& getSendBuffer(void);
	void setRecvBuffer(const std::string& buffer);
};



