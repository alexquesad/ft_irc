#include "main.hpp"

bool availableMode(char c, std::string availableMode)
{
    return (!(availableMode.find(c) == std::string::npos));
}

void channelMode(Server *serv, Channel *channel, std::string mode, int sd, std::string buffer)
{
    std::string channelMode = channel->getMode();
    int i;
    (void)buffer;

    if (mode[0] == '-')
    {
        std::string deletedMode;
        for (i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], "aimnptlk") == false)
                sendMessage(send_rpl_err(501, serv, FIND_USER(sd), "", ""), sd);
            else if (channelMode.find(mode[i]) != std::string::npos)
            {
                deletedMode += mode[i];
                channelMode.erase(channelMode.find(mode[i]), 1);
            }
        }
        channel->setMode(channelMode);
        std::string user_answer = user_output(FIND_USER(sd));
        if (!deletedMode.empty())
            user_answer += "MODE " + channel->getChannelname() + " -" + deletedMode;
        sendMessage(user_answer, sd);
    }
    else
    {
        std::string addedMode;
        for ((mode[0] != '+') ? i = 0: i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], "aimnptlk") == false)
                sendMessage(send_rpl_err(501, serv, FIND_USER(sd), "", ""), sd);
            else if (channelMode.find(mode[i]) == std::string::npos)
                addedMode += mode[i];
        }
        channel->setMode(channelMode + addedMode);
        std::string user_answer = user_output(FIND_USER(sd));
        if (!addedMode.empty())
            user_answer += "MODE " + channel->getChannelname() + " +" + addedMode;
        sendMessage(user_answer, sd);
    }
}

void userMode(Server *serv, User *user, std::string mode, int sd)
{
    std::string userMode = user->getMode();
    int i;

    if (mode[0] == '-')
    {
        std::string deletedMode;
        for (i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], USER_MODE) == false)
                sendMessage(send_rpl_err(501, serv, user, "", ""), sd);
            else if (userMode.find(mode[i]) != std::string::npos)
            {
                deletedMode += mode[i];
                userMode.erase(userMode.find(mode[i]), 1);
            }
        }
        user->setMode(userMode);
        std::string user_answer = user_output(FIND_USER(sd));
        if (!deletedMode.empty())
            user_answer += "MODE " + user->getNickname() + " -" + deletedMode;
        sendMessage(user_answer, sd);
    }
    else
    {
        std::string addedMode;
        for ((mode[0] != '+') ? i = 0: i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], USER_MODE) == false)
                sendMessage(send_rpl_err(501, serv, user, "", ""), sd);
            else if (userMode.find(mode[i]) == std::string::npos)
                addedMode += mode[i];
        }
        user->setMode(userMode + addedMode);
        std::string user_answer = user_output(FIND_USER(sd));
        if (!addedMode.empty())
            user_answer += "MODE " + user->getNickname() + " +" + addedMode;
        sendMessage(user_answer, sd);
    }
}

void mode(Server *serv, char *buffer, int sd)
{
    (void)serv;
    (void)sd;
    int i = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && buf[5 + i] != ' ' && buf[5 + i] != '\r' && buf[5 + i] != '\n';i++);
    std::string msgtarget(buf.substr(5, i));
    std::string idOfChannel = "#&+";

    std::string mode(&buf[i + 6]);
    for (i = 0; mode[i] && mode[i] != ' ' && mode[i] != '\r' && mode[i] != '\n';i++);
    mode.erase(i, mode.length());
    if (mode.empty())
            sendMessage(send_rpl_err(221, serv, FIND_USER(sd), '+' + FIND_USER(sd)->getMode(), ""), sd);
    else if (!msgtarget.empty() && idOfChannel.find(msgtarget[0]) != std::string::npos)
    {
        if (serv->getChannels().find(msgtarget) == serv->getChannels().end())
            sendMessage(send_rpl_err(403, serv, FIND_USER(sd), msgtarget, ""), sd);
        else
            channelMode(serv, FIND_CHANNEL(msgtarget), mode, sd, buffer);
    }
    else
    {
        int userSd;
        if ((userSd = serv->searchUserByNickname(msgtarget)) == -1)
            sendMessage(send_rpl_err(401, serv, FIND_USER(sd), msgtarget, ""), sd);
        else
            userMode(serv, FIND_USER(userSd), mode, sd);
    }
    
}
