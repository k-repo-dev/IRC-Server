#include "../incl/Server.hpp"

void Server::cleanupClient(Client* client, const std::string& reason)
{
	std::string announceQuit = ":" + client->getNick() + "!" + client->getUserName()
								+ "@" + HOST + " QUIT :" + reason + "\r\n";
	sendToUnique(client, announceQuit);
	for (auto it = _channelList.begin(); it != _channelList.end();)
	{
		Channel* channel = it->second;
		if (channel->isMember(client))
		{
			channel->removeMember(client);
			channel->removeOperator(client);
			if (channel->getMembers().empty())
			{
				delete channel;
				it = _channelList.erase(it);
				continue;
			}
		}
		it++;
	}
}

void Server::removeClient(int fd)
{
	std::map<int, Client*>::iterator it = _clientList.find(fd);
	if (it == _clientList.end())
		return;
	Client* client = it->second;
	cleanupClient(client, "Quit: Client disconnected");
	delete it->second;
	_clientList.erase(it);
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
	close(fd);
	std::cout << "Client fd=" << fd << " disconnected\n"; 
}