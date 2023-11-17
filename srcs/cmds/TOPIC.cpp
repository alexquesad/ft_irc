#include "main.hpp"

void topic(Server *serv, char *buffer, int sd)
{
    (void)sd;
    int i = 0;
    std::string buf(buffer);
    for (; buf[6 + i] && buf[6 + i] != ' ' && buf[6 + i] != '\r' && buf[6 + i] != '\n';i++);
    std::string channel_name(buf.substr(6, i));
    // if (serv->getChannels().find(channel_name) != serv->getChannels().end())
    // {
    //     if (serv->getUsers().find(sd)->second->getChannels().find(channel_name) != serv->getUsers().find(sd)->second->getChannels().end())
    //     {
    //         if (serv->getChannels().find(channel_name)->second->getOpers().find(sd) != serv->getChannels().find(channel_name)->second->getOpers().end())
    //         {
    //             if (buf.find(':') == std::string::npos)
    //             {
    //                 sendMessage(send_rpl_err(461, serv, serv->getUsers().find(sd)->second, "TOPIC", ""), sd);
    //                 return ;
    //             }
    //             std::string topic(buf.substr(buf.find(':') + 1));
    //             topic = topic.substr(0, topic.length() - 2);
    //             std::cout << "[" << topic << "]" << std::endl;
    //             serv->getChannels().find(channel_name)->second->setTopic(topic);
    //             sendMessage(send_rpl_err(332, serv, serv->getUsers().find(sd)->second, channel_name, serv->getChannels().find(channel_name)->second->getTopic()), sd);
    //         }
    //     }
    //     else
    //         sendMessage(send_rpl_err(442, serv, serv->getUsers().find(sd)->second, channel_name, ""), sd);

    // }
    // else
    //     sendMessage(send_rpl_err(403, serv, serv->getUsers().find(sd)->second, channel_name, ""), sd);





    if (channel_name.empty())
        sendMessage(send_rpl_err(461, serv, serv->getUsers().find(sd)->second, "TOPIC", ""), sd);
    else if (serv->getChannels().find(channel_name) == serv->getChannels().end())
        sendMessage(send_rpl_err(403, serv, serv->getUsers().find(sd)->second, channel_name, ""), sd);
    else if (serv->getUsers().find(sd)->second->getChannels().find(channel_name) == serv->getUsers().find(sd)->second->getChannels().end())
        sendMessage(send_rpl_err(442, serv, serv->getUsers().find(sd)->second, channel_name, ""), sd);
    else if (serv->getChannels().find(channel_name)->second->getOpers().find(sd) == serv->getChannels().find(channel_name)->second->getOpers().end())
        sendMessage(user_output(serv->getUsers().find(sd)->second) + "PRIVMSG " + channel_name + " :You do not have access to change the topic on this channel", sd);
    else if (buf.find(':') == std::string::npos)
        sendMessage(send_rpl_err(461, serv, serv->getUsers().find(sd)->second, "TOPIC", ""), sd);
    else
    {
        std::string topic(buf.substr(buf.find(':') + 1));
        topic = topic.substr(0, topic.length() - 2);
        std::cout << "[" << topic << "]" << std::endl;
        serv->getChannels().find(channel_name)->second->setTopic(topic);
        sendMessage(send_rpl_err(332, serv, serv->getUsers().find(sd)->second, channel_name, serv->getChannels().find(channel_name)->second->getTopic()), sd);
    }
}