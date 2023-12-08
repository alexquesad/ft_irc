#include "irc.hpp"

Channel::Channel(std::string channelName) : _channelName(channelName), _topic("") {
}

Channel::~Channel()
{
    this->_users.clear();
    this->_chanops.clear();
	this->_voices.clear();
	this->_banList.clear();
    this->_invitedUsers.clear();
}

int Channel::getUsersNumber() const
{
    return (this->_users.size() + this->_chanops.size() + this->_voices.size());
}

std::string Channel::getTopic() const
{
    return this->_topic;
}

std::string Channel::getChannelName() const
{
    return this->_channelName;
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

// Member function to remove a user from the channel based on socket descriptor.
void Channel::leftUser(int sd)
{
    // Declare an iterator for maps containing pointers to User objects.
    std::map<int, User*>::iterator it;

    // Look for the user in the regular users map (_users) using the socket descriptor.
    if ((it = this->_users.find(sd)) != this->_users.end())
        this->_users.erase(it); // If found, erase the user from the _users map.
    // If the user is not found in _users, look for them in the channel operators map (_chanops).
    else if ((it = this->_chanops.find(sd)) != this->_chanops.end())
        this->_chanops.erase(it); // If found, erase the user from the _chanops map.
    // If the user is not found in _chanops, look for them in the voices map (_voices).
    else if ((it = this->_voices.find(sd)) != this->_voices.end())
        this->_voices.erase(it); // If found, erase the user from the _voices map.

    // If the user is not found in any maps, no operation is performed.
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

// Function to get a concatenated string of all user nicknames in the channel.
std::string Channel::getListOfUserInChan()
{
    // Declare an empty string to store the output list of user nicknames.
    std::string output;

    // Iterate over the map containing channel operators (chanops).
    for (std::map<int, User*>::iterator it = this->_chanops.begin(); it != this->_chanops.end(); it++)
    {
        // For each channel operator, if the output string is not empty, add a space before the next nickname.
        if (!output.empty())
            output += " ";
        // Prefix the nickname with '@' to indicate channel operator status and append it to the output string.
        output += "@";
        output += it->second->getNickname();
    }

    // Iterate over the map containing users with voice privilege (voices).
    for (std::map<int, User*>::iterator it = this->_voices.begin(); it != this->_voices.end(); it++)
    {
        // For each voice, if the output string is not empty, add a space before the next nickname.
        if (!output.empty())
            output += " ";
        // Prefix the nickname with '+' to indicate voice status and append it to the output string.
        output += "+";
        output += it->second->getNickname();
    }

    // Iterate over the regular users map.
    for (std::map<int, User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
    {
        // For each regular user, if the output string is not empty, add a space before the next nickname.
        if (!output.empty())
            output += " ";
        // Append the user's nickname to the output string without any prefix.
        output += it->second->getNickname();
    }

    // Return the concatenated string of nicknames.
    return (output);
}

std::string Channel::getListOfUserBan()
{
    std::string output;
    for (std::map<std::string, std::string>::iterator it = this->_banList.begin(); it != this->_banList.end(); it++)
    {
        if (!output.empty())
            output += ",";
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

bool Channel::isBan(std::string nickname)
{
    if (this->_banList.find(nickname) != this->_banList.end())
        return (true);
    return (false);
}

void Channel::addInvitee(const std::string& nickname) 
{
    _invitedUsers.insert(nickname);
}
    
bool Channel::isUserInvited(const std::string& nickname) const 
{
    return _invitedUsers.find(nickname) != _invitedUsers.end();
}