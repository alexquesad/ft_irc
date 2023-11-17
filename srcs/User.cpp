#include "main.hpp"

int max_clients = 10;

User::User(std::string nickname, std::string username, std::string hostname, std::string real_name) : _nickname(nickname), _username(username), _hostname(hostname), _real_name(real_name){
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

std::set<std::string> & User::getChannels()
{
    return this->_channels;
}

size_t User::getChannelsSize() const
{
    return this->_channels.size();
}

void User::add_channel(std::string channel_name)
{
    if (this->_channels.find(channel_name) == this->_channels.end())
        this->_channels.insert(channel_name);
}

void User::setNick(std::string new_nickname)
{
    this->_nickname = new_nickname;
}