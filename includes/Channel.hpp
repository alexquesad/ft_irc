#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "main.hpp"

class User;

class Channel
{
    private:

    std::map<int, User*> _users;
    std::map<int, User*> _opers;
    std::string _channel_name;
    std::string _topic;
    std::string _mode;
    std::string _key;
    //mode
    //blacklist
    //whitelist

    public:

    Channel(std::string channel_name);
    ~Channel();
    std::map<int, User*> & getUsers();
    std::map<int, User*> & getOpers();
    std::string getTopic() const;
    std::string getChannelname() const;
    void setTopic(std::string topic);
    int getUsersnumber() const;// if numberofuser == 0 map.erase channel
    std::string getMode() const;
    void setMode(std::string mode);
    void addUser(int sd, User *user);
    void addOper(int sd, User *user);
    void leftUser(int sd);
    int searchUserByNickname(std::string nickname);
    std::string get_list_of_user_in_chan();
};

#endif
