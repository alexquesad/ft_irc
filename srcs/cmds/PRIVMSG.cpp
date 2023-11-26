#include "main.hpp"

void privmsg(Server *serv, std::string buffer, int sd)
{
    int occAfterCmd;
    std::string buf(buffer);
    if (buf.compare(0, 6, "NOTICE") == 0)
        occAfterCmd = 7;
    else
        occAfterCmd = 8;
    size_t i;
    std::string msgtarget = "";
    if ((i = buf.find_first_not_of(sep, occAfterCmd)) != std::string::npos)
        msgtarget = buf.substr(i, (buf.find_first_of(sep, i) - i));
    std::string msg = "";
    size_t j;
    j = buf.find_first_of(sep, i);
    if ((j = buf.find_first_not_of(sep, j)) != std::string::npos)
        msg = buf.substr(j, (buf.find_first_of(endBuf, j) - j));
    std::string idOfChannel = "#&+";
    int userToSendSd;
    std::string userAnswer;
    userAnswer = userOutput(FIND_USER(sd));
    userAnswer += "PRIVMSG " + msgtarget + " " + msg;
    if (!msgtarget.empty() && idOfChannel.find(msgtarget[0]) != std::string::npos)
    {
        if (serv->getChannels().find(msgtarget) == serv->getChannels().end())
            sendMessage(sendRplErr(401, serv, FIND_USER(sd), msgtarget, ""), sd);
        else if ((FIND_CHANNEL(msgtarget)->getMode().find("n") != std::string::npos) && (FIND_CHANNEL(msgtarget)->searchUserByNickname(FIND_USER(sd)->getNickname()) == -1))
            sendMessage(sendRplErr(404, serv, FIND_USER(sd), msgtarget, ""), sd);
        else if ((FIND_CHANNEL(msgtarget)->getMode().find("m") != std::string::npos) && (!FIND_CHANNEL(msgtarget)->isChanop(sd)) && (!FIND_CHANNEL(msgtarget)->isVoices(sd)))
            sendMessage(sendRplErr(404, serv, FIND_USER(sd), msgtarget, ""), sd);
		else if (FIND_CHANNEL(msgtarget)->isBan(FIND_USER(sd)->getNickname()) == true)
            sendMessage(sendRplErr(404, serv, FIND_USER(sd), msgtarget, ""), sd);
        else if ((FIND_CHANNEL(msgtarget)->getMode().find("a") != std::string::npos))
        {
            userAnswer = anonymousOutput() + "PRIVMSG " + msgtarget + " " + msg;
            sendEveryoneInChanExceptUser(userAnswer, FIND_CHANNEL(msgtarget), sd);
        }
        else
            sendEveryoneInChanExceptUser(userAnswer, FIND_CHANNEL(msgtarget), sd);
    }
    else
    {
        if ((userToSendSd = serv->searchUserByNickname(msgtarget)) == -1)
            sendMessage(sendRplErr(401, serv, FIND_USER(sd), msgtarget, ""), sd);
        else
            sendMessage(userAnswer, userToSendSd);
    }
    
}