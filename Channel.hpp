#pragma once
#include <iostream>
#include "Client.hpp"
#include <unordered_map>
#include <unordered_set>
#include <cstdint>

class Channel
{
private:
	const std::string	_channel;
	std::string			_topic;
	std::string 		_key;
	//bool				_inviteOnly;
	//bool				_topicRestricted;
	//uint32_t 			_userLimit;
	std::unordered_map<int, Client*> _members;
	std::unordered_set<int> _operators;
	
public:
	Channel(const std::string& channel);
	~Channel();
	void addMember(Client* client);
	void removeMember(Client* client);
	void addOperator(Client* client);
	void removeOperator(Client* client);
	bool isOperator(Client* client) const;
	const std::string& getKey(void) const;
	void setKey(const std::string& key);
};


