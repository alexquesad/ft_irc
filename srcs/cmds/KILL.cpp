#include "main.hpp"

void kill(Server *serv, char *buffer, int sd)
{
    std::string buf(buffer);
    std::string name;
    std::string message;

    if (FIND_USER(sd)->getMode().find('o') == std::string::npos)
    {
        sendMessage(send_rpl_err(481, serv, FIND_USER(sd), "", ""), sd);
        return ;
    }
    name = buf.substr(buf.find(' ') + 1, buf.find(':') - buf.find(' ') - 2);
    if (serv->searchUserByNickname(name) == -1)
    {
        sendMessage(send_rpl_err(401, serv, FIND_USER(sd), name, ""), sd);
        return ;
    }
    int i = buf.find(':') + 1;
    int j = 0;
    for (; buf[i + j] && buf[i + j] != '\r' && buf[i + j] != '\n'; j++);
    message = buf.substr(buf.find(':') + 1, j);
    disconnectUser(serv, serv->searchUserByNickname(name));
    close(serv->searchUserByNickname(name));
    // std::cout << "["<< name << "] [" << message << "]" << std::endl;
}