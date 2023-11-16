#include "main.hpp"

bool isAllSpace(std::string str)
{
    int i = 0;
    while (str[i])
    {
        if (str[i] != ' ')
            return (false);
        i++;
    }
    return (true);
}

void topic(Server *serv, char *buffer, int sd)
{
    (void)sd;
    int i = 0;
    std::string buf(buffer);
    for (; buf[6 + i] && buf[6 + i] != ' ' && buf[6 + i] != '\r' && buf[6 + i] != '\n';i++);
    std::string channel_name(buf.substr(6, i));
    if (serv->getChannels().find(channel_name) != serv->getChannels().end())
    {
        if (serv->getChannels().find(channel_name)->second->getOpers().find(sd) != serv->getChannels().find(channel_name)->second->getOpers().end())
        {
            if (buf.find(':') == std::string::npos)
            {
                sendMessage(send_rpl_err(461, serv, serv->getUsers().find(sd)->second, "TOPIC", ""), sd);
                return ;
            }
            std::string topic(buf.substr(buf.find(':') + 1));
            topic = topic.substr(0, topic.length() - 2);
            if (isAllSpace(topic))
            {
                sendMessage(send_rpl_err(461, serv, serv->getUsers().find(sd)->second, "TOPIC", ""), sd);
                return ;
            }
            std::cout << "[" << topic << "]" << std::endl;
            serv->getChannels().find(channel_name)->second->setTopic(topic);
            sendMessage(send_rpl_err(332, serv, serv->getUsers().find(sd)->second, channel_name, serv->getChannels().find(channel_name)->second->getTopic()), sd);
        }
    }
    // if (serv->getChannels().find(channel_name)->second->getTopic() == "")
    // {
    //     sendMessage(send_rpl_err(331, serv, serv->getUsers().find(sd)->second, channel_name, ""), sd);
    //     break;
    // }
}