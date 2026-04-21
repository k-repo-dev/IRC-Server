#pragma once
#include <iostream>

#define BUFFERSIZE 1024
#define MAX_IRC_LINE 512 //irc line max 512 including \r\n  

class Client
{
private:
	const int	_clientfd;
	std::string _nick;
	std::string	_username;
	std::string _realname;
	bool 		_registered;
	bool		_passwordValidated;
	bool		_hasUser;
	std::string _recvBuffer;
	std::string _sendBuffer;
	
	
public:
	Client(int fd);
	~Client();

	int 			getFD() const;
	bool 			isRegistered() const;
	void 			setRegistered(bool val);

	void			setNick(const std::string &nick);
	std::string		getNick() const;

	void			setUserName(const std::string &username);
	void			setRealName(const std::string &realname);
	std::string 	getUserName() const;
	std::string		getRealName() const;
	bool			getHasUser() const;

	void			setPasswordValidated(bool val);
	bool			isPasswordValidated(void) const;
	
	std::string&	getRecvBuffer(void);
	std::string& 	getSendBuffer(void);
	void 			setRecvBuffer(const std::string& buffer);
};



