#include "main.hpp"

void topic(Server *serv, std::string buffer, int sd)
{
    int i = 0;
    std::string buf(buffer);
    for (; buf[6 + i] && sep.find(buf[6 + i]) == std::string::npos; i++);
    std::string channelName(buf.substr(6, i));
    if (channelName.empty())
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "TOPIC", ""), sd);
    else if (serv->getChannels().find(channelName) == serv->getChannels().end())
        sendMessage(sendRplErr(403, serv, FIND_USER(sd), channelName, ""), sd);
    else if (FIND_USER(sd)->getChannels().find(channelName) == FIND_USER(sd)->getChannels().end())
        sendMessage(sendRplErr(442, serv, FIND_USER(sd), channelName, ""), sd);
    else if ((FIND_CHANNEL(channelName)->getMode().find("t") != std::string::npos) && (FIND_USER(sd)->getMode().find('r') != std::string::npos))
        sendMessage(sendRplErr(484, serv, FIND_USER(sd), "", ""), sd);
    else if ((FIND_CHANNEL(channelName)->getMode().find("t") != std::string::npos) && (FIND_CHANNEL(channelName)->getChanops().find(sd) == FIND_CHANNEL(channelName)->getChanops().end()))
        sendMessage(sendRplErr(482, serv, FIND_USER(sd), channelName, ""), sd);
    else if (buf.find(':') == std::string::npos)
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "TOPIC", ""), sd);
    else
    {
        std::string topic(buf.substr(buf.find(':') + 1));
        topic = topic.substr(0, topic.find('\r') != std::string::npos ? topic.length() - 2 : topic.length() - 1);
        FIND_CHANNEL(channelName)->setTopic(topic);
        sendMessage(sendRplErr(332, serv, FIND_USER(sd), channelName, FIND_CHANNEL(channelName)->getTopic()), sd);
        std::string userAnswer = userOutput(FIND_USER(sd));
        if (FIND_CHANNEL(channelName)->getMode().find("a") != std::string::npos)
            userAnswer = anonymousOutput();
        userAnswer += buffer;
        sendEveryoneInChan(userAnswer, FIND_CHANNEL(channelName));
    }
}