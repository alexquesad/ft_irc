#include "main.hpp"

void kill(Server *serv, std::string buffer, int sd)
{
    std::string buf(buffer);
    std::string name;
    std::string message;

    if (FIND_USER(sd)->getMode().find('o') == std::string::npos)
    {
        sendMessage(sendRplErr(481, serv, FIND_USER(sd), "", ""), sd);
        return ;
    }
    name = buf.substr(buf.find(' ') + 1, buf.find('\r') != std::string::npos ? buf.find(':') - buf.find(' ') - 2 : buf.find(':') - buf.find(' ') - 1);
    if (name.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "KILL", ""), sd);
        return;
    }
    if (serv->searchUserByNickname(name) == -1)
    {
        sendMessage(sendRplErr(401, serv, FIND_USER(sd), name, ""), sd);
        return ;
    }
    int i = buf.find(':') + 1;
    int j = 0;
    for (; buf[i + j] && endBuf.find(buf[i + j]) == std::string::npos; j++);
    message = buf.substr(buf.find(':') + 1, j);
    disconnectUser(serv, serv->searchUserByNickname(name));
}