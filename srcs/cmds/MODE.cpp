
#include "main.hpp"

void mode_o(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    if (!channel->isChanop(sd))
    {
        sendMessage(send_rpl_err(482, serv, FIND_USER(sd), channel->getChannelname(), ""), sd);
        return ;
    }
    int i = 0;
    for (int j = 0;buffer[i] && j < 3; i++)
        if (buffer[i] == ' ')
            j++;
    std::string name = buffer.substr(i, (buffer.length() - 2) - i);
    int userSd = channel->searchUserByNickname(name);
    if (userSd == -1)
    {
        sendMessage(send_rpl_err(441, serv, FIND_USER(sd), name, channel->getChannelname()), sd);
        return ;
    }
    channel->leftUser(userSd);
    if (mode[0] == '-')
        channel->addUser(userSd, FIND_USER(userSd));
    else
        channel->addChanops(userSd, FIND_USER(userSd));
    std::string user_answer = user_output(FIND_USER(sd));
    user_answer += buffer;
    sendEveryone(user_answer, channel);
}

void mode_v(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    if (!channel->isChanop(sd))
    {
        sendMessage(send_rpl_err(482, serv, FIND_USER(sd), channel->getChannelname(), ""), sd);
        return ;
    }
    int i = 0;
    for (int j = 0;buffer[i] && j < 3; i++)
        if (buffer[i] == ' ')
            j++;
    std::string name = buffer.substr(i, (buffer.length() - 2) - i);
    int userSd = channel->searchUserByNickname(name);
    if (userSd == -1)
    {
        sendMessage(send_rpl_err(441, serv, FIND_USER(sd), name, channel->getChannelname()), sd);
        return ;
    }
    if (channel->isChanop(userSd) == true)
        return ;
    channel->leftUser(userSd);
    if (mode[0] == '-')
        channel->addUser(userSd, FIND_USER(userSd));
    else
        channel->addVoices(userSd, FIND_USER(userSd));
    std::string user_answer = user_output(FIND_USER(sd));
    std::cout << mode << " " << buffer << std::endl;
    user_answer += buffer;
    sendEveryone(user_answer, channel);
}

void mode_b(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    (void)mode;
    int i = 0;
    for (int j = 0;buffer[i] && j < 3; i++)
        if (buffer[i] == ' ')
            j++;
    std::string name = buffer.substr(i, (buffer.length() - 2) - i);
    if (name.empty())
    {
        std::string banlist = channel->get_list_of_user_ban();
        if (!banlist.empty())
        {
            banlist.erase(banlist.length() - 1, 1);
            sendMessage(send_rpl_err(367, serv, FIND_USER(sd), channel->getChannelname(), banlist), sd);
        }
        sendMessage(send_rpl_err(368, serv, FIND_USER(sd), channel->getChannelname(), ""), sd);
    }
	else
    {
        std::string full_name = name;
        std::string nick = full_name.substr(0, full_name.find('!'));
	    std::string user = full_name.substr(full_name.find('!') + 1, full_name.find('@') - (full_name.find('!') + 1));
	    std::string host = full_name.substr(full_name.find('@') + 1, full_name.length() - (full_name.find('@') + 1));
        if (nick.length() > 1 || (nick.length() == 1 && nick[0] != '*'))
        {
            if (nick[0] == '*')
                name = &nick[1];
            else
                name = nick;
        }
        else if (user.length() > 1 || (user.length() == 1 && user[0] != '*'))
        {
            if (user[0] == '*')
                name = &user[1];
            else
                name = user;
        }
        else if (host.length() > 1 || (host.length() == 1 && host[0] != '*'))
        {
            if (host[0] == '*')
                name = &host[1];
            else
                name = host;
        }
        else
        {
            // aucun nick ni user ni hostname trouvé
        }
		std::cout << "name: " << name << std::endl;
        if (mode[0] == '-')
            channel->getBanList().erase(name);
        else
            channel->getBanList().insert(std::make_pair(name, full_name));
	}
}

void mode_k(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    (void)mode;
    (void)sd;
    (void)serv;
    int i = 0;
    for (int j = 0;buffer[i] && j < 3; i++)
        if (buffer[i] == ' ')
            j++;
    size_t ret1 = &buffer[i].find(' ');
    std::string key;
    if (ret1 < buffer.length() - 1)
    {
        key = buffer.substr(i, ret1 - i);
    }
    else
    {
        key = buffer.substr(i, (buffer.length() - 2) - i);
    }
    if (key.compare("x") == 0)
    {
        sendMessage(send_rpl_err(467, serv, FIND_USER(sd), channel->getKey(), ""), sd);
    }
    else
    {
        channel->setKey(key);
    }
}

void mode_l(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    (void)mode;
    (void)sd;
    (void)serv;
    int i = 0;
    for (int j = 0;buffer[i] && j < 3; i++)
        if (buffer[i] == ' ')
            j++;
    std::string name = buffer.substr(i, (buffer.length() - 2) - i);
    channel->setMaxUser(std::stoi(name));
}

bool availableMode(char c, std::string availableMode)
{
    return (!(availableMode.find(c) == std::string::npos));
}

void channelMode(Server *serv, Channel *channel, std::string mode, int sd, char *buffer)
{
    std::string channelMode = channel->getMode();
    int i;
    typedef void (*modes)(Server *, Channel *, std::string, std::string, int);

    std::map<char, modes> modehandler;

	modehandler.insert(std::make_pair('o', &mode_o));
	modehandler.insert(std::make_pair('v', &mode_v));
	modehandler.insert(std::make_pair('b', &mode_b));

	// modehandler.insert(std::make_pair('e', &mode_e));
	// modehandler.insert(std::make_pair('I', &mode_I));
    modehandler.insert(std::make_pair('k', &mode_k));
    modehandler.insert(std::make_pair('l', &mode_l));

    if (mode[0] == '-')
    {
        std::string deletedMode;
        for (i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], CHANNEL_MODE) == false)
                sendMessage(send_rpl_err(501, serv, FIND_USER(sd), "", ""), sd);
            else if (availableMode(mode[i], "ovbeI") == true)
                modehandler[mode[i]](serv, channel, mode, buffer, sd);
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
            if (availableMode(mode[i], CHANNEL_MODE) == false)
                sendMessage(send_rpl_err(501, serv, FIND_USER(sd), "", ""), sd);
            else if (availableMode(mode[i], "ovbeI") == true)
            {
                modehandler[mode[i]](serv, channel, mode, buffer, sd);
            }
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