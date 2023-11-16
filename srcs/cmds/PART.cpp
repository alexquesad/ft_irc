#include "main.hpp"

void    part(Server *serv, char *buffer, int sd)
{
    int i = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && buf[5 + i] != ' ' && buf[5 + i] != '\r' && buf[5 + i] != '\n'; i++);
    std::string channels_name(buf.substr(5, i));
    int nb_of_channels = 1 + std::count(channels_name.begin(), channels_name.end(), ',');
    for (int i = 0; i < nb_of_channels; i++)
    {
        std::string channel_name = channels_name.substr(0, channels_name.find(","));
        channels_name.erase(0, channels_name.find(",") + 1);
        if (serv->getUsers().find(sd)->second->getChannels().find(channel_name) != serv->getUsers().find(sd)->second->getChannels().end())
        {
            std::string user_answer = user_output(serv->getUsers().find(sd)->second);
            user_answer += buffer;
            sendEveryone(user_answer, serv->getChannels().find(channel_name)->second);
            serv->getChannels().find(channel_name)->second->leftUser(sd);
            if (serv->getChannels().find(channel_name)->second->getUsersnumber() == 0)
                serv->getChannels().erase(serv->getChannels().find(channel_name)->first);
            serv->getUsers().find(sd)->second->getChannels().erase(channel_name);
        }
    }
}