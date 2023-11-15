#include "Command.hpp"
#include "Server.hpp"


void nick(Server *serv, char *buffer, int sd)
{
    (void)serv;
    (void) buffer;
    (void)sd;
    std::cout << "test1" << std::endl;
}

void join(Server *serv, char *buffer, int sd)
{
    std::cout << buffer << std::endl;
    int i = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && buf[5 + i] != ' ' && buf[5 + i] != '\r' && buf[5 + i] != '\n';i++);
    std::string channel_name(buf.substr(5, i));
    std::cout << "channel_name : " << channel_name << "lol" << std::endl;
    Channel new_channel(channel_name, sd, serv->getUsers().find(sd)->second);
    std::cout << new_channel.getChannelname() << std::endl;
    std::pair<std::map<std::string, Channel *>::iterator,bool> ret;
    ret = serv->getChannels().insert(std::make_pair(channel_name, &new_channel));
    // if (ret.second == true)
    // {
    //     std::cout << "ntm" << std::endl;
    // }
    // std::cout << "ret : " << ret.first << " " << ret.second << std::endl;
    serv->sendMessage("#funfunfun test", sd);
    // std::cout << serv->getChannels().size() << std::endl; // return 0 (should be 1, didnt inserted well)
}

void privmsg(Server *serv, char *buffer, int sd)
{
    std::cout << buffer << std::endl;
    int i = 0;
    std::string buf(buffer);
    for (; buf[8 + i] && buf[8 + i] != ' ' && buf[8 + i] != '\r' && buf[8 + i] != '\n';i++);
    std::string channel_name(buf.substr(8, i));
    std::cout << "channel_name : " << channel_name << "lol" << std::endl;
    std::map<std::string, Channel *> channels = serv->getChannels();
    std::map<std::string, Channel *>::iterator test =  channels.find(channel_name);
    if (test == channels.end())
        std::cout << "ntm" << std::endl;
    Channel *channel = test->second;
    // std::cout << "buffer : " << &buffer[8] << std::endl;

    std::map<int, Client *> users = channel->getUsers();
    std::map<int, Client*>::iterator end = users.end();
    for (std::map<int, Client*>::iterator it = users.begin(); it != end; it++)
    {
        if (sd != it->first)
            serv->sendMessage(buffer, it->first);
    }
}

// void ping(Server *serv, char *buffer, int sd)
// {
//     buffer[1] = 'O';
//     serv->sendMessage(buffer);
//     std::cout << "test2 " << buffer << std::endl;
// }

// void pong(Server *serv, char *buffer, int sd)
// {
//     buffer[1] = 'I';
//     serv->sendMessage(buffer);
//     std::cout << "test3" << std::endl;
// }

// void user()
// {
//     std::cout << "test2" << std::endl;
// }