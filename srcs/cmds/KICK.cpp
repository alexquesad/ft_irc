#include "main.hpp"

void kick(Server *serv, std::string buffer, int sd)
{
    std::string buf(buffer);
    int kickCount = std::count(buf.begin(), buf.end(), '\n');
    int i = 0;
    for (; buf[5 + i] && sep.find(buf[5 + i]) == std::string::npos; i++);
    std::string channelsName(buf.substr(5, i));
    if (channelsName.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "KICK", ""), sd);
        return;
    }
    int nbOfChannels = 1 + std::count(channelsName.begin(), channelsName.end(), ',');
    int j = 0;
    int occBeginNick = i;
    for (; buf[i] && buf[i] != ':' && endBuf.find(buf[i]) == std::string::npos; i++);
    for (; buf[i + j] && endBuf.find(buf[i + j]) == std::string::npos; j++);
    std::string message = buf.substr(i + 1, j - 1);
    for (int k = 0; k < kickCount; k++)
    {
        j = 0;
        for (; buf[6 + occBeginNick + j] && sep.find(buf[6 + occBeginNick + j]) == std::string::npos; j++);
        std::string usersNick = buf.substr(occBeginNick + 6, j);
        int nbUsers = 1 + std::count(usersNick.begin(), usersNick.end(), ',');
        if (usersNick.empty())
        {
            sendMessage(sendRplErr(461, serv, FIND_USER(sd), "KICK", ""), sd);
            return;
        }
        for (int j = 0; j < nbUsers; j++)
        {
            std::string userNick = usersNick.substr(0, usersNick.find(","));
            usersNick.erase(0, usersNick.find(",") + 1);
            for (int i = 0; i < nbOfChannels; i++)
            {
                std::string channelName = channelsName.substr(0, channelsName.find(","));
                channelsName.erase(0, channelsName.find(",") + 1);
                if (serv->getChannels().find(channelName) == serv->getChannels().end())
                {
                    if (j == 0)
                        sendMessage(sendRplErr(403, serv, FIND_USER(sd), channelName, ""), sd);
                    continue;
                }
                if (FIND_CHANNEL(channelName)->getMode().find("a") != std::string::npos)
                    continue;
                if (FIND_CHANNEL(channelName)->searchUserByNickname(FIND_USER(sd)->getNickname()) == -1)
                {
                    sendMessage(sendRplErr(442, serv, FIND_USER(sd), channelName, ""), sd);
                    continue;
                }
                if (FIND_USER(sd)->getMode().find('r') != std::string::npos)
                {
                    sendMessage(sendRplErr(484, serv, FIND_USER(sd), "", ""), sd);
                    continue ;
                }
                if (!FIND_CHANNEL(channelName)->isChanop(sd))
                {
                    sendMessage(sendRplErr(482, serv, FIND_USER(sd), channelName, ""), sd);
                    continue ;
                }
                int userToKickSd;
                if ((userToKickSd = FIND_CHANNEL(channelName)->searchUserByNickname(userNick)) == -1)
                {
                    sendMessage(sendRplErr(441, serv, FIND_USER(sd), userNick, channelName), sd);
                    continue;
                }
                std::string userAnswer = userOutput(FIND_USER(sd));
                userAnswer += buffer;
                sendEveryoneInChanExceptUser(userAnswer, FIND_CHANNEL(channelName), userToKickSd);
                FIND_CHANNEL(channelName)->leftUser(userToKickSd);
                if (FIND_CHANNEL(channelName)->getUsersNumber() == 0)
                {
                    delete serv->getChannels().find(channelName)->second;
                    serv->getChannels().erase(channelName);
                }
                FIND_USER(userToKickSd)->getChannels().erase(channelName);
                userAnswer = userOutput(FIND_USER(userToKickSd));
                userAnswer += "PART " + channelName;
                sendMessage(userAnswer, userToKickSd);
            }
        }
        if (buf.find('\r') != std::string::npos)
            buf.erase(0, buf.find("\r\n") + 2);
        else
            buf.erase(0, buf.find('\n') + 1);
    }
}