#include "main.hpp"

Channel::Channel(std::string channel_name) : _channel_name(channel_name), _topic(){
}

Channel &Channel::operator=(const Channel &T)
{
    if (this == &T)
        return (*this);
    while (!this->_users.empty())
    {
        this->_users.erase(this->_users.begin());
    }
    if (T._users.begin() != T._users.end())
        this->_users.insert(T._users.begin(), T._users.end());
    while (!this->_opers.empty())
    {
        this->_opers.erase(this->_opers.begin());
    }
    if (T._opers.begin() != T._opers.end())
        this->_opers.insert(T._opers.begin(), T._opers.end());
    std::cout << "use of this" << std::endl;
    this->_channel_name = T._channel_name;
    this->_topic = T._topic;
    return (*this);
}

Channel::Channel(const Channel &T)
{
	*this = T;
    std::cout << "yo" << std::endl;
	return ;
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