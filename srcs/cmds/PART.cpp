#include "main.hpp"

void    part(Server *serv, std::string buffer, int sd)
{
    int i = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && sep.find(buf[5 + i]) == std::string::npos; i++);
    std::string channels_name(buf.substr(5, i));
    if (channels_name.empty())
    {
        sendMessage(send_rpl_err(461, serv, FIND_USER(sd), "PART", ""), sd);
        return;
    }
    int nb_of_channels = 1 + std::count(channels_name.begin(), channels_name.end(), ',');
    for (int i = 0; i < nb_of_channels; i++)
    {
        std::string channel_name = channels_name.substr(0, channels_name.find(","));
        channels_name.erase(0, channels_name.find(",") + 1);
        if (serv->getChannels().find(channel_name) == serv->getChannels().end())
            sendMessage(send_rpl_err(403, serv, FIND_USER(sd), channel_name, ""), sd);
        else if (FIND_USER(sd)->getChannels().find(channel_name) == FIND_USER(sd)->getChannels().end())
            sendMessage(send_rpl_err(442, serv, FIND_USER(sd), channel_name, ""), sd);
        else
        {
            std::string user_answer = userOutput(FIND_USER(sd));
            user_answer += buffer;
            if (FIND_CHANNEL(channel_name)->getMode().find("a") == std::string::npos)
                sendEveryoneInChan(user_answer, FIND_CHANNEL(channel_name));
            else
                sendMessage(user_answer, sd);
            FIND_CHANNEL(channel_name)->leftUser(sd);
            if (FIND_CHANNEL(channel_name)->getUsersnumber() == 0)
            {
                delete serv->getChannels().find(channel_name)->second;
                serv->getChannels().erase(channel_name);
            }
            FIND_USER(sd)->getChannels().erase(channel_name);
        }
    }
}