#include "main.hpp"

bool checkInvalidCharacter(char c)
{
    return (c == '\0' || c == '\a' || c == '\r' || c == '\n' || c == ' ' || c == ',' || c == ':');
}

bool channelNameInvalid(std::string name)
{
    if (name.size() > 50)
        return false;
    std::string::iterator it = name.begin();
    std::string channelId = "#&+";
    if (!name.empty() && channelId.find(name[0]) == std::string::npos)
        return false;
    for (; it != name.end() && !checkInvalidCharacter(*it); it++);
    return (it == name.end());
}

void join(Server *serv, std::string buffer, int sd)
{
    int i = 0, j = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && sep.find(buf[5 + i]) == std::string::npos; i++);
    std::string channelsName(buf.substr(5, i));
    if (channelsName.empty())
    {
        sendMessage(send_rpl_err(461, serv, FIND_USER(sd), "JOIN", ""), sd);
        return;
    }
    int nb_of_channels = 1 + std::count(channelsName.begin(), channelsName.end(), ',');
    std::string keys_for_channels = "";
    if (buf[5 + i] && buf[5 + i] == ' ')
    {
        for (; buf[6 + i + j] && sep.find(buf[6 + j + i]) == std::string::npos; j++);
        keys_for_channels = buf.substr(6 + i, j);
    }
    for (int i = 0; i < nb_of_channels; i++)
    {
        std::string channel_name = channelsName.substr(0, channelsName.find(","));
        channelsName.erase(0, channelsName.find(",") + 1);
        if (!(FIND_USER(sd)->getChannelsSize() < 10))
        {
            sendMessage(send_rpl_err(405, serv, FIND_USER(sd), channel_name, ""), sd);
            break;
        }
        if (!channelNameInvalid(channel_name))
        {
            sendMessage(channel_name + ":Erroneous Channel Name", sd);
            continue;
        }
        std::string key = keys_for_channels.substr(0, keys_for_channels.find(","));
        keys_for_channels.erase(0, keys_for_channels.find(",") + 1);
        if (serv->getChannels().find(channel_name) == serv->getChannels().end())
        {
            Channel *chan = new Channel(channel_name);
            serv->setChannels(channel_name, chan);
        }
        if (FIND_CHANNEL(channel_name)->getMode().find("b") != std::string::npos)
        {
            if (FIND_CHANNEL(channel_name)->isBan(FIND_USER(sd)->getUsername()) == true)
            {
                sendMessage(send_rpl_err(474, serv, FIND_USER(sd), channel_name, ""), sd);
                continue;
            }
        }
        if (FIND_CHANNEL(channel_name)->getMode().find("k") != std::string::npos)
        {
            if (key.empty())
            {
                sendMessage(send_rpl_err(461, serv, FIND_USER(sd), "JOIN", ""), sd);
                return;
            }
            if (FIND_CHANNEL(channel_name)->getKey().compare(key) != 0)
            {
                sendMessage(send_rpl_err(475, serv, FIND_USER(sd), channel_name, ""), sd);
                continue;
            }
        }
        if (FIND_CHANNEL(channel_name)->getMode().find("l") != std::string::npos)
        {
            if (FIND_CHANNEL(channel_name)->getMaxUser() <= FIND_CHANNEL(channel_name)->getUsersNumber())
            {
                sendMessage(send_rpl_err(471, serv, FIND_USER(sd), channel_name, ""), sd);
                continue;
            }
        }
        //Adding client to server
        if (FIND_CHANNEL(channel_name)->getUsersNumber() == 0)
            FIND_CHANNEL(channel_name)->addChanops(sd, FIND_USER(sd));
        else
            FIND_CHANNEL(channel_name)->addUser(sd, FIND_USER(sd));
        FIND_USER(sd)->addChannel(channel_name);
        std::string user_answer = userOutput(FIND_USER(sd));
        user_answer += buffer;
        if (FIND_CHANNEL(channel_name)->getMode().find("a") == std::string::npos)
            sendEveryoneInChan(user_answer, FIND_CHANNEL(channel_name));
        if (FIND_CHANNEL(channel_name)->getTopic() == "")
            sendMessage(send_rpl_err(331, serv, FIND_USER(sd), channel_name, ""), sd);
        else
            sendMessage(send_rpl_err(332, serv, FIND_USER(sd), channel_name, FIND_CHANNEL(channel_name)->getTopic()), sd);
        std::string list_of_user = FIND_CHANNEL(channel_name)->getListOfUserInChan();
        if (FIND_CHANNEL(channel_name)->getMode().find("a") == std::string::npos)
        {
            sendMessage(send_rpl_err(353, serv, FIND_USER(sd), channel_name, list_of_user), sd);
            sendMessage(send_rpl_err(366, serv, FIND_USER(sd), channel_name, ""), sd);
        }
        if (!FIND_CHANNEL(channel_name)->getMode().empty())
            sendMessage(send_rpl_err(324, serv, FIND_USER(sd), channel_name, FIND_CHANNEL(channel_name)->getMode()), sd);
    }
}
