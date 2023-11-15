#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Server.hpp"

class Channel
{
    private:

    std::map<int, Client *> _users;
    std::map<int, Client *> _opers;
    std::string _channel_name;
    std::string _topic;
    //mode
    //blacklist
    //whitelist

    public:

    Channel(std::string channel_name, int sd, Client *user);
    ~Channel();
    std::map<int, Client *> getUsers();
    std::string getTopic() const;
    std::string getChannelname() const;
    int getUsersnumber() const;// if numberofuser == 0 map.erase channel
    void addUser(int sd, Client *user);
    void addOper(int sd, Client *user);
    void leftUser(int sd);
};

#endif