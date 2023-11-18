#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "main.hpp"

class User;

class Channel
{
    private:

    std::map<int, User*> _users;
    std::map<int, User*> _chanops;
    std::map<int, User*> _voices;
    std::string _channel_name;
    std::string _topic;
    std::string _mode;
    std::string _key;
    int _maxUser;
    std::map<std::string, std::string> _banList;
    std::set<std::string> _exceptionList;
    std::set<std::string> _whiteList;
    std::set<std::string> _inviteList;

    public:

    Channel(std::string channel_name);
    ~Channel();
    std::map<int, User*> & getUsers();
    std::map<int, User*> & getChanops();
    std::map<int, User*> & getVoices();
    std::string getTopic() const;
    std::string getChannelname() const;
    std::map<std::string, std::string> & getBanList();
    std::set<std::string> & getExceptionList();
    std::set<std::string> & getWhiteList();
    std::set<std::string> & getInviteList();
    int getUsersnumber() const;
    std::string getMode() const;
    std::string getKey() const;
    int getMaxUser() const;
    void setTopic(std::string topic);
    void setMode(std::string mode);
    void setKey(std::string key);
    void setMaxUser(int maxUser);
    void addUser(int sd, User *user);
    void addChanops(int sd, User *user);
    void addVoices(int sd, User *user);
    void leftUser(int sd);
    int searchUserByNickname(std::string nickname);
    std::string get_list_of_user_in_chan();
    std::string get_list_of_user_ban();
    bool isChanop(int sd);
    bool isBan(std::string username);
    bool isException(std::string username);
    bool isWhiteList(std::string username);
    bool isInvited(std::string username);
};

#endif