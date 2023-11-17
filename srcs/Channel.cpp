#include "main.hpp"

Channel::Channel(std::string channel_name) : _channel_name(channel_name), _topic(""){
}

Channel::~Channel(){}

int Channel::getUsersnumber() const
{
    return (this->_users.size() + this->_opers.size());
}

std::string Channel::getTopic() const
{
    return this->_topic;
}

void Channel::setTopic(std::string topic)
{
    this->_topic = topic;
}

std::string Channel::getChannelname() const
{
    std::cout << "bon" << std::endl;
    if (this->_channel_name.empty())
        std::cout << this->_channel_name << std::endl;
    return this->_channel_name;
}

std::map<int, User*> & Channel::getUsers()
{
    return this->_users;
}

std::map<int, User*> & Channel::getOpers()
{
    return this->_opers;
}

void Channel::addUser(int sd, User *user)
{
    this->_users.insert(std::make_pair(sd, user));
}

void Channel::addOper(int sd, User *user)
{
    this->_opers.insert(std::make_pair(sd, user));
}

void Channel::leftUser(int sd)
{
    std::map<int, User*>::iterator it;
    if ((it = this->_users.find(sd)) != this->_users.end())
        this->_users.erase(it);
    else if ((it = this->_opers.find(sd)) != this->_opers.end())
        this->_opers.erase(it);
}

int Channel::searchUserByNickname(std::string nickname)
{
    for (std::map<int, User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
    {
        if (nickname.compare(it->second->getNickname()) == 0)
            return it->first;
    }
    for (std::map<int, User*>::iterator it = this->_opers.begin(); it != this->_opers.end(); it++)
    {
        if (nickname.compare(it->second->getNickname()) == 0)
            return it->first;
    }
    return (-1);
}

std::string Channel::get_list_of_user_in_chan()
{
    std::string output;
    for (std::map<int, User *>::iterator it = this->_opers.begin(); it != this->_opers.end(); it++)
    {
        if (!output.empty())
            output += " ";
        output += "@";
        output += it->second->getNickname();
    }
    for (std::map<int, User *>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
    {
        if (!output.empty())
            output += " ";
        output += it->second->getNickname();
    }
    return (output);
}