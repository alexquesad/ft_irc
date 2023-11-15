#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Server.hpp"

class Channel
{
    private:

    std::map<int, User*> _users;
    std::map<int, User*> _opers;
    std::string _channel_name;
    std::string _topic;
    //mode
    //blacklist
    //whitelist

    public:

    Channel(std::string channel_name);
    Channel(const Channel &T);
    Channel &operator=(const Channel &T);
    ~Channel();
    std::map<int, User*> & getUsers();
    std::string getTopic() const;
    std::string getChannelname() const;
    int getUsersnumber() const;// if numberofuser == 0 map.erase channel
    void addUser(int sd, User *user);
    void addOper(int sd, User *user);
    void leftUser(int sd);
};

#endif