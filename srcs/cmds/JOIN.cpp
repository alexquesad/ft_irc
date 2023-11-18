#include "main.hpp"

// void leave_all_channels(Server *serv, int sd)
// {

// }

bool checkInvalidCharacter(char c)
{
    return (c == '\0' || c == '\a' || c == '\r' || c == '\n' || c == ' ' || c == ',' || c == ':');
}

bool channelNameInvalid(std::string name)
{
    if (name.size() > 50)
        return false;
    std::string::iterator it = name.begin();
    for (; it != name.end() && !checkInvalidCharacter(*it); it++);
    return (it == name.end());
}

int count_keys(std::string str, int nb_of_channels)
{
    int i = 0;
    int result = nb_of_channels;
    while (str.find(",x", i) != std::string::npos)
    {
        result--;
        i += str.find(",x", i) + 1;
    }
    return (result);
}

void join(Server *serv, char *buffer, int sd)
{
    int i = 0;
    // int j = 0;
    // int nb_of_keys = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && buf[5 + i] != ' ' && buf[5 + i] != '\r' && buf[5 + i] != '\n'; i++);
    std::string channels_name(buf.substr(5, i));
    // if (channels_name.compare("0"))
    // {
    //     leave_all_channels(serv, sd);
    //     return;
    // }
    int nb_of_channels = 1 + std::count(channels_name.begin(), channels_name.end(), ',');
    // std::string keys_for_channels = "";
    // if (buf[5 + i] && buf[5 + i] == ' ')
    // {
    //     for (; buf[6 + i + j] && buf[6 + i + j] != ' ' && buf[6 + i + j] != '\r' && buf[6 + i + j] != '\n'; j++);
    //     keys_for_channels = buf.substr(6 + i, j);
    // }
    // if (!keys_for_channels.empty())
    //     nb_of_keys = count_keys(keys_for_channels, nb_of_channels); 

    //IMPORTANT ASSOCIATE KEYS WITH PRIVATE CHANNELS
    for (int i = 0; i < nb_of_channels; i++)
    {
        std::string channel_name = channels_name.substr(0, channels_name.find(","));
        channels_name.erase(0, channels_name.find(",") + 1);
        if (!(FIND_USER(sd)->getChannelsSize() < 10))
        {
            sendMessage(send_rpl_err(405, serv, FIND_USER(sd), channel_name, ""), sd);
            break;
        }
        if (!channelNameInvalid(channel_name))
        {
            sendMessage(channel_name + ":Erroneous Channel Name", sd);
            continue ;
        }
        std::cout << "[" << i << "]{" << nb_of_channels << "}" << std::endl;
        if (serv->getChannels().find(channel_name) == serv->getChannels().end())
        {
            Channel *chan = new Channel(channel_name);
            serv->setChannels(channel_name, chan);
        }
        //On ajoute le client a notre serveur
        if (FIND_CHANNEL(channel_name)->getUsersnumber() == 0)
            FIND_CHANNEL(channel_name)->addChanops(sd, FIND_USER(sd));
        else
            FIND_CHANNEL(channel_name)->addUser(sd, FIND_USER(sd));
        FIND_USER(sd)->add_channel(channel_name);
        std::string user_answer = user_output(FIND_USER(sd));
        user_answer += buffer;
        sendEveryone(user_answer, FIND_CHANNEL(channel_name));
        if (FIND_CHANNEL(channel_name)->getTopic() == "")
            sendMessage(send_rpl_err(331, serv, FIND_USER(sd), channel_name, ""), sd);
        else
            sendMessage(send_rpl_err(332, serv, FIND_USER(sd), channel_name, FIND_CHANNEL(channel_name)->getTopic()), sd);
        std::string list_of_user = FIND_CHANNEL(channel_name)->get_list_of_user_in_chan();
        sendMessage(send_rpl_err(353, serv, FIND_USER(sd), channel_name, list_of_user), sd);
        sendMessage(send_rpl_err(366, serv, FIND_USER(sd), channel_name, ""), sd);
    }
}