#include "main.hpp"

void topic(Server *serv, char *buffer, int sd)
{
    (void)sd;
    int i = 0;
    std::string buf(buffer);
    for (; buf[6 + i] && buf[6 + i] != ' ' && buf[6 + i] != '\r' && buf[6 + i] != '\n';i++);
    std::string channel_name(buf.substr(6, i));
    if (channel_name.empty())
        sendMessage(send_rpl_err(461, serv, FIND_USER(sd), "TOPIC", ""), sd);
    else if (serv->getChannels().find(channel_name) == serv->getChannels().end())
        sendMessage(send_rpl_err(403, serv, FIND_USER(sd), channel_name, ""), sd);
    else if (FIND_USER(sd)->getChannels().find(channel_name) == FIND_USER(sd)->getChannels().end())
        sendMessage(send_rpl_err(442, serv, FIND_USER(sd), channel_name, ""), sd);
    else if (FIND_CHANNEL(channel_name)->getOpers().find(sd) == FIND_CHANNEL(channel_name)->getOpers().end())
        sendMessage(user_output(FIND_USER(sd)) + "PRIVMSG " + channel_name + " :You do not have access to change the topic on this channel", sd);
    else if (buf.find(':') == std::string::npos)
        sendMessage(send_rpl_err(461, serv, FIND_USER(sd), "TOPIC", ""), sd);
    else
    {
        std::string topic(buf.substr(buf.find(':') + 1));
        topic = topic.substr(0, topic.length() - 2);
        std::cout << "[" << topic << "]" << std::endl;
        FIND_CHANNEL(channel_name)->setTopic(topic);
        sendMessage(send_rpl_err(332, serv, FIND_USER(sd), channel_name, FIND_CHANNEL(channel_name)->getTopic()), sd);
    }
}