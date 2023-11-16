#include "main.hpp"

void privmsg(Server *serv, char *buffer, int sd)
{
    int i = 0;
    // int j;
    std::string buf(buffer);
    for (; buf[8 + i] && buf[8 + i] != ' ' && buf[8 + i] != '\r' && buf[8 + i] != '\n';i++);
    std::string channel_name(buf.substr(8, i));
    // if (buf[8 + i] && buf[8 + i] == ' ')
    // {
    //     j = 0;
    //     for (; buf[10 + i + j] && buf[10 + i + j] != '\r' && buf[10 + i + j] != '\n';j++);
    //     std::string message(buf.substr(10 + i, j));
    //     std::cout << "[" << message << "]" << std::endl;
    // }
    std::string user_answer = user_output(serv->getUsers().find(sd)->second);
    user_answer += buffer;
    sendEveryone(user_answer, serv->getChannels().find(channel_name)->second, sd);
}