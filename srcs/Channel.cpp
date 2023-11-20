#include "main.hpp"

Channel::Channel(std::string channel_name) : _channel_name(channel_name), _topic("") {
}

Channel::~Channel()
{
    this->_users.clear();
    this->_chanops.clear();
	this->_voices.clear();
	this->_banList.clear();
}

int Channel::getUsersnumber() const
{
    return (this->_users.size() + this->_chanops.size() + this->_voices.size());
}

std::string Channel::getTopic() const
{
    return this->_topic;
}

std::string Channel::getChannelname() const
{
    return this->_channel_name;
}

std::map<int, User*> & Channel::getUsers()
{
    return this->_users;
}

std::map<int, User*> & Channel::getChanops()
{
    return this->_chanops;
}

std::map<int, User*> & Channel::getVoices()
{
    return this->_voices;
}

std::map<std::string, std::string> & Channel::getBanList()
{
    return this->_banList;
}

std::string Channel::getMode() const
{
    return this->_mode;
}

std::string Channel::getKey() const
{
    return this->_key;
}

int Channel::getMaxUser() const
{
    return this->_maxUser;
}

void Channel::setTopic(std::string topic)
{
    this->_topic = topic;
}

void Channel::setMode(std::string mode)
{
    this->_mode = mode;
}

void Channel::setKey(std::string key)
{
    this->_key = key;
}

void Channel::setMaxUser(int maxUser)
{
    this->_maxUser = maxUser;
}

void Channel::addUser(int sd, User *user)
{
    this->_users.insert(std::make_pair(sd, user));
}

void Channel::addChanops(int sd, User *user)
{
    this->_chanops.insert(std::make_pair(sd, user));
}

void Channel::addVoices(int sd, User *user)
{
    this->_voices.insert(std::make_pair(sd, user));
}

void Channel::leftUser(int sd)
{
    std::map<int, User*>::iterator it;
    if ((it = this->_users.find(sd)) != this->_users.end())
        this->_users.erase(it);
    else if ((it = this->_chanops.find(sd)) != this->_chanops.end())
        this->_chanops.erase(it);
    else if ((it = this->_voices.find(sd)) != this->_voices.end())
        this->_voices.erase(it);
}

int Channel::searchUserByNickname(std::string nickname)
{
    for (std::map<int, User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
    {
        if (nickname.compare(it->second->getNickname()) == 0)
            return it->first;
    }
    for (std::map<int, User*>::iterator it = this->_chanops.begin(); it != this->_chanops.end(); it++)
    {
        if (nickname.compare(it->second->getNickname()) == 0)
            return it->first;
    }
    for (std::map<int, User*>::iterator it = this->_voices.begin(); it != this->_voices.end(); it++)
    {
        if (nickname.compare(it->second->getNickname()) == 0)
            return it->first;
    }
    return (-1);
}

std::string Channel::get_list_of_user_in_chan()
{
    std::string output;
    for (std::map<int, User *>::iterator it = this->_chanops.begin(); it != this->_chanops.end(); it++)
    {
        if (!output.empty())
            output += " ";
        output += "@";
        output += it->second->getNickname();
    }
    for (std::map<int, User *>::iterator it = this->_voices.begin(); it != this->_voices.end(); it++)
    {
        if (!output.empty())
            output += " ";
        output += "+";
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

std::string Channel::get_list_of_user_ban()
{
    std::string output;
    for (std::map<std::string, std::string>::iterator it = this->_banList.begin(); it != this->_banList.end(); it++)
    {
        if (!output.empty())
            output += "\n";
        output += it->second;
    }
    return (output);
}

bool    Channel::isChanop(int sd)
{
    if (this->_chanops.find(sd) != this->_chanops.end())
        return (true);
    return (false);
}

bool    Channel::isVoices(int sd)
{
    if (this->_voices.find(sd) != this->_voices.end())
        return (true);
    return (false);
}

bool Channel::isBan(std::string username)
{
    if (this->_banList.find(username) != this->_banList.end())
        return (true);
    return (false);
}