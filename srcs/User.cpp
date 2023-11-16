#include "main.hpp"

int max_clients = 10;

User::User(std::string nickname, std::string username, std::string hostname, std::string real_name) : _nickname(nickname), _username(username), _hostname(hostname), _real_name(real_name){
}

User &User::operator=(const User &T)
{
    if (this == &T)
        return (*this);
    this->_nickname = T._nickname;
    this->_username = T._username;
    this->_hostname = T._hostname;
    this->_real_name = T._real_name;
    return (*this);
}

User::User(const User &T)
{
	*this = T;
	return ;
}

User::~User(){}

std::string User::getNickname() const
{
    return this->_nickname;
}

std::string User::getUsername() const
{ 
    return this->_username;
}

std::string User::getHostname() const
{
    return this->_hostname;
}

std::string User::getReal_name() const
{
    return this->_real_name;
}

std::vector<std::string> User::getChannels() const
{
    return this->_channels;
}

void User::add_channel(std::string channel_name)
{
    std::vector<std::string>::iterator it = find(this->_channels.begin(), this->_channels.end(), channel_name);
    if (it == this->_channels.end())
        this->_channels.push_back(channel_name);
}

void User::setNick(std::string new_nickname)
{
    this->_nickname = new_nickname;
}