#include "main.hpp"

void topic(Server *serv, char *buffer, int sd)
{
    int i = 0;
    std::string buf(buffer);
    for (; buf[6 + i] && sep.find(buf[6 + i]) == std::string::npos;i++);
    std::string channel_name(buf.substr(6, i));
    if (channel_name.empty())
        sendMessage(send_rpl_err(461, serv, FIND_USER(sd), "TOPIC", ""), sd);
    else if (serv->getChannels().find(channel_name) == serv->getChannels().end())
        sendMessage(send_rpl_err(403, serv, FIND_USER(sd), channel_name, ""), sd);
    else if (FIND_USER(sd)->getChannels().find(channel_name) == FIND_USER(sd)->getChannels().end())
        sendMessage(send_rpl_err(442, serv, FIND_USER(sd), channel_name, ""), sd);
    else if ((FIND_CHANNEL(channel_name)->getMode().find("t") != std::string::npos) && (FIND_USER(sd)->getMode().find('r') != std::string::npos))
        sendMessage(send_rpl_err(484, serv, FIND_USER(sd), "", ""), sd);
    else if ((FIND_CHANNEL(channel_name)->getMode().find("t") != std::string::npos) && (FIND_CHANNEL(channel_name)->getChanops().find(sd) == FIND_CHANNEL(channel_name)->getChanops().end()))
        sendMessage(send_rpl_err(482, serv, FIND_USER(sd), channel_name, ""), sd);
    else if (buf.find(':') == std::string::npos)
        sendMessage(send_rpl_err(461, serv, FIND_USER(sd), "TOPIC", ""), sd);
    else
    {
        std::string topic(buf.substr(buf.find(':') + 1));
        topic = topic.substr(0, topic.length() - 2);
        FIND_CHANNEL(channel_name)->setTopic(topic);
        sendMessage(send_rpl_err(332, serv, FIND_USER(sd), channel_name, FIND_CHANNEL(channel_name)->getTopic()), sd);
        std::string user_answer = user_output(FIND_USER(sd));
        if (FIND_CHANNEL(channel_name)->getMode().find("a") != std::string::npos)
            user_answer = anonymous_output();
        user_answer += buffer;
        sendEveryoneInChan(user_answer, FIND_CHANNEL(channel_name));
    }
}