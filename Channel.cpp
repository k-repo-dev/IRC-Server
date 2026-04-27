#include "Channel.hpp"

Channel::Channel(const std::string& channel): _channel(channel), _topic(""),_key(""), _topicSetter(""), _topicTime(""),_topicRestricted(false)/*, _inviteOnly(false),
, _userLimit(0)*/{
}

Channel::~Channel()
{
}

void	Channel::addMember(Client* client){
	_members.insert({client->getFD(), client});
}
void	Channel::removeMember(Client* client){
	_members.erase(client->getFD());
}
void	Channel::addOperator(Client* client){
	_operators.insert(client->getFD());
}
void	Channel::removeOperator(Client* client){
	_operators.erase(client->getFD());
}
bool	Channel::isOperator(Client* client) const{
	return _operators.find(client->getFD()) != _operators.end();
}

const std::string&	Channel::getKey(void) const{
	return _key;
}
void	Channel::setKey(const std::string& key){
	_key = key;
}

bool Channel::isMember(Client* client) const
{
	return _members.count(client->getFD()); // returns true if that client is in the channel
}

const std::unordered_map<int, Client*>& Channel::getMembers() const
{
	return _members;
}

Client* Channel::getMemberByNick(const std::string& nick) const
{
	for (auto it = _members.begin(); it != _members.end(); it++)
	{
		if (it->second->getNick() == nick) //check nicks of all clients to see if it matches
			return it->second; // return client object
	}
	return nullptr;
}

const std::string& Channel::getTopic(void) const
{
	return _topic;
}
 
void Channel::setTopic(const std::string& topic, const std::string& setter) // called when a client changes the topic
{
	_topic = topic;
	_topicSetter = setter;
	_topicTime = std::to_string(std::time(nullptr)); //gets current time and converts to string (IRC client converts the string to a readable date)
}
const std::string& Channel::getTopicSetter(void) const
{
	return _topicSetter;
}

const std::string& Channel::getTopicTime(void) const
{
	return _topicTime;
}

bool Channel::isTopicRestricted(void) const
{
	return _topicRestricted;
}