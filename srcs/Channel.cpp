#include "Channel.hpp"

Channel::Channel(std::string channel_name, int sd, Client *user) : _channel_name(channel_name), _topic()
{
    //addUser(sd, user);
    addOper(sd, user);
}

Channel::~Channel(){}

int Channel::getUsersnumber() const
{
    return this->_users.size();
}

std::string Channel::getTopic() const
{
    return this->_topic;
}

std::string Channel::getChannelname() const
{
    return this->_channel_name;
}

std::map<int, Client *> Channel::getUsers()
{
    return this->_users;
}

void Channel::addUser(int sd, Client *user)
{
    this->_users.insert(std::make_pair(sd, user));
}

void Channel::addOper(int sd, Client *user)
{
    this->_opers.insert(std::make_pair(sd, user));
}

void Channel::leftUser(int sd)
{
    std::map<int, Client *>::iterator it;
    if ((it = this->_users.find(sd)) != this->_users.end())
    {
        this->_users.erase(it);
        if ((it = this->_opers.find(sd)) != this->_opers.end())
            this->_opers.erase(it);
    }
}