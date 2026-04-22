#include "Server.hpp"

void Server::handleJoin(Client* client, std::vector<std::string>& params){

    if (!client->isRegistered()){
        return;
    }
    if (params.empty()){
        sendToClient(client, "461 JOIN: Not enough parameters\r\n");
        return ;
    }
    std::map<std::string, Channel*>::iterator it = _channelList.find(params[0]);
    if (it == _channelList.end()){
        Channel* channel = new Channel(params[0]);
        _channelList.insert({channel->getChannel(), channel});
       // _channelList[params[0]] = new Channel(params[0]);
        channel->addMember(client);
        channel->addOperator(client);
        sendToClient(client, client->getNick() + "created and joined new channel " + params[0] + " \r\n");
        std::cout << client->getNick() << "created and joined new channel " + params[0] + " \r\n";
        
    }
    else{
        Channel* channel = it->second;
        channel->addMember(client);
        sendToClient(client, client->getNick() + " joined channel " + params[0] + " \r\n");
        std::cout << client->getNick() << " joined channel " + params[0] + " \r\n";
    }
}


/*ERR_NEEDMOREPARAMS (461)
ERR_NOSUCHCHANNEL (403)
ERR_TOOMANYCHANNELS (405)
ERR_BADCHANNELKEY (475)
ERR_BANNEDFROMCHAN (474)
ERR_CHANNELISFULL (471)
ERR_INVITEONLYCHAN (473)
ERR_BADCHANMASK (476)
RPL_TOPIC (332)
RPL_TOPICWHOTIME (333)
RPL_NAMREPLY (353)
RPL_ENDOFNAMES (366)*/