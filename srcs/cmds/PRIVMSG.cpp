#include "main.hpp"

void privmsg(Server *serv, char *buffer, int sd)
{
    int i = 0;
    std::string buf(buffer);
    for (; buf[8 + i] && buf[8 + i] != ' ' && buf[8 + i] != '\r' && buf[8 + i] != '\n';i++);
    std::string msgtarget(buf.substr(8, i));
    std::string idOfChannel = "#&+";

    int userToSendSd;
    std::string user_answer = user_output(FIND_USER(sd));
    user_answer += buffer;
    if (!msgtarget.empty() && idOfChannel.find(msgtarget[0]) != std::string::npos)
    {
        if (FIND_CHANNEL(msgtarget)->isBan(FIND_USER(sd)->getNickname()) == true)
            sendMessage(send_rpl_err(404, serv, FIND_USER(sd), msgtarget, ""), sd);
        else if (serv->getChannels().find(msgtarget) == serv->getChannels().end())
            sendMessage(send_rpl_err(401, serv, FIND_USER(sd), msgtarget, ""), sd);
        else
            sendEveryone(user_answer, FIND_CHANNEL(msgtarget), sd);
    }
    else
    {
        if ((userToSendSd = serv->searchUserByNickname(msgtarget)) == -1)
            sendMessage(send_rpl_err(401, serv, FIND_USER(sd), msgtarget, ""), sd);
        else
            sendMessage(user_answer, userToSendSd);
    }
    
}