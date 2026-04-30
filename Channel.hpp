#pragma once
#include <iostream>
#include "Client.hpp"
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <ctime>

class Channel
{
private:
	const std::string	_channel;
	std::string			_topic;
	std::string 		_topicSetter;
	std::string 		_topicTime;
	std::string 		_key;
	std::unordered_set<int> _inviteList; // quick way to check if a specific fd is in the list
	bool				_topicRestricted;
	bool				_inviteOnly;
	//uint32_t 			_userLimit;
	std::unordered_map<int, Client*> _members;
	std::unordered_set<int> _operators;
	
public:
	Channel(const std::string& channel);
	~Channel();

	void				addMember(Client* client);
	void				removeMember(Client* client);
	void				addOperator(Client* client);
	void				removeOperator(Client* client);
	bool				isOperator(Client* client) const;
	const std::string&	getKey(void) const;
	void				setKey(const std::string& key);
	void				removeKey();
	bool				isMember(Client* client) const;
	
	const std::unordered_map<int, Client*>& getMembers() const;
	const std::unordered_set<int>& getOperators() const;
	
	Client*				getMemberByNick(const std::string& nick) const;
	const std::string&	getTopic(void) const;
	void				setTopic(const std::string& topic, const std::string& setter);
	const std::string&	getTopicSetter(void) const;
	const std::string&	getTopicTime(void) const;
	bool				isTopicRestricted(void) const;
	const std::string&	getChannel(void) const;
	bool				isInviteOnly(void) const;
	void				addInvite(Client* client);
	bool				isInvited(Client* client) const;
	void				setInviteOnly(Channel* channel);
	void				removeInviteOnly(Channel* channel);
	void				setTopicRestricted(Channel* channel);
	void				removeTopicRestricted(Channel* channel);

};


