#include "main.hpp"

void mode_o(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
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
    sendEveryoneInChan(user_answer, channel);
}

void mode_v(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
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
    user_answer += buffer;
    sendEveryoneInChan(user_answer, channel);
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
        if (nick.length() > 1 || (nick.length() == 1 && nick[0] != '*'))
        {
            if (nick[0] == '*')
                name = &nick[1];
            else
                name = nick;
        }
        else
            return ;
        if (mode[0] == '-')
            channel->getBanList().erase(name);
        else
            channel->getBanList().insert(std::make_pair(name, full_name)); // is it working ?
	}
}

void mode_k(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    (void)mode;
    (void)sd;
    (void)serv;
    if (mode[0] == '-')
    {
        channel->setKey("");
	return ;
    }
    int i = 0;
    for (int j = 0;buffer[i] && j < 3; i++)
        if (buffer[i] == ' ')
            j++;
    std::string key(&buffer[i]);
    size_t ret1 = key.find(' ');
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
    if (mode[0] == '-')
    {
	    channel->setMaxUser(-1);
	    return ;
    }
    int i = 0;
    for (int j = 0;buffer[i] && j < 3; i++)
        if (buffer[i] == ' ')
            j++;
    std::string name = buffer.substr(i, (buffer.length() - 2) - i);
    int maxUser = std::strtoul(name.c_str(), NULL, 0);
    if (maxUser < 0)
    {
	    sendMessage("Max user key must be superior to 0.", sd);
	    return ;
    }
    channel->setMaxUser(maxUser);
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
    modehandler.insert(std::make_pair('k', &mode_k));
    modehandler.insert(std::make_pair('l', &mode_l));

    if (mode[0] == '-')
    {
        std::string deletedMode;
        for (i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], CHANNEL_MODE) == false)
	        {
		        std::string stringMode(1, mode[i]);
                sendMessage(send_rpl_err(472, serv, FIND_USER(sd), stringMode, channel->getChannelname()), sd);
	        }
            else if (availableMode(mode[i], "ovbkl") == true)
	        {
		        modehandler[mode[i]](serv, channel, mode, buffer, sd);
	    	    if (mode[i] == 'k' || mode[i] == 'l')
			        deletedMode += mode[i];
	        }
	        else if (channelMode.find(mode[i]) != std::string::npos)
            {
                deletedMode += mode[i];
                channelMode.erase(channelMode.find(mode[i]), 1);
            }
        }
        channel->setMode(channelMode);
        std::string user_answer = user_output(FIND_USER(sd));
        if (channel->getMode().find("a") != std::string::npos)
            user_answer = anonymous_output();
        if (!deletedMode.empty())
            user_answer += "MODE " + channel->getChannelname() + " -" + deletedMode;
        sendEveryoneInChan(user_answer, channel);
    }
    else
    {
        std::string addedMode;
        for ((mode[0] != '+') ? i = 0: i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], CHANNEL_MODE) == false)
            {
                std::string stringMode(1, mode[i]);
                sendMessage(send_rpl_err(472, serv, FIND_USER(sd), stringMode, channel->getChannelname()), sd);
            }
            else if (availableMode(mode[i], "ovbkl") == true)
            {
                modehandler[mode[i]](serv, channel, mode, buffer, sd);
                if (mode[i] == 'k' || mode[i] == 'l')
                    addedMode += mode[i];
            }
            else if (channelMode.find(mode[i]) == std::string::npos)
                addedMode += mode[i];
        }
        channel->setMode(channelMode + addedMode);
        std::string user_answer = user_output(FIND_USER(sd));
        if (channel->getMode().find("a") != std::string::npos)
            user_answer = anonymous_output();
        if (!addedMode.empty())
            user_answer += "MODE " + channel->getChannelname() + " +" + addedMode;
        sendEveryoneInChan(user_answer, channel);
    }
}

void userMode(Server *serv, User *user, std::string mode, int sd)
{
    std::string userMode = user->getMode();
    int i;
    int userSd = serv->searchUserByNickname(user->getNickname());

    if (mode[0] == '-')
    {
        std::string deletedMode;
        for (i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], USER_MODE) == false)
                sendMessage(send_rpl_err(472, serv, FIND_USER(sd), "", ""), sd);
            else if (userMode.find(mode[i]) != std::string::npos)
            {
                if (mode[i] == 'o' && user->getNickname() != FIND_USER(sd)->getNickname())
                    sendMessage(send_rpl_err(481, serv, FIND_USER(sd), "", ""), sd);
                else if (mode[i] == 'r' && ((FIND_USER(sd)->getMode().find('r') != std::string::npos) || (FIND_USER(sd)->getMode().find('o') == std::string::npos )))
                    sendMessage(send_rpl_err(481, serv, FIND_USER(sd), "", ""), sd);
                else
                {
                    deletedMode += mode[i];
                    userMode.erase(userMode.find(mode[i]), 1);
                }
            }
        }
        user->setMode(userMode);
        std::string user_answer = user_output(FIND_USER(userSd));
        if (!deletedMode.empty())
            user_answer += "MODE " + user->getNickname() + " -" + deletedMode;
        sendMessage(user_answer, userSd);
    }
    else
    {
        std::string addedMode;
        for ((mode[0] != '+') ? i = 0: i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i], USER_MODE) == false)
                sendMessage(send_rpl_err(472, serv, FIND_USER(sd), "", ""), sd);
            else if (userMode.find(mode[i]) == std::string::npos)
            {
                if (mode[i] == 'o' && FIND_USER(sd)->getMode().find('o') == std::string::npos)
                    sendMessage(send_rpl_err(481, serv, FIND_USER(sd), "", ""), sd);
                else if (mode[i] == 'r' && user->getNickname() == FIND_USER(sd)->getNickname())
                    addedMode += mode[i];
                else if (mode[i] == 'r' && ((FIND_USER(sd)->getMode().find('r') != std::string::npos) || (FIND_USER(sd)->getMode().find('o') == std::string::npos)))
                    sendMessage(send_rpl_err(481, serv, FIND_USER(sd), "", ""), sd);
                else
                    addedMode += mode[i];
            }
        }
        user->setMode(userMode + addedMode);
        std::string user_answer = user_output(FIND_USER(userSd));
        if (!addedMode.empty())
            user_answer += "MODE " + user->getNickname() + " +" + addedMode;
        sendMessage(user_answer, userSd);
    }
}

void mode(Server *serv, char *buffer, int sd)
{
    int i = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && sep.find(buf[5 + i]) == std::string::npos;i++);
    std::string msgtarget(buf.substr(5, i));
    if (msgtarget.empty())
    {
        sendMessage(send_rpl_err(461, serv, FIND_USER(sd), "MODE", ""), sd);
        return;
    }
    std::string idOfChannel = "#&+";

    std::string mode(&buf[i + 6]);
    for (i = 0; mode[i] && sep.find(mode[i]) == std::string::npos;i++);
    mode.erase(i, mode.length());
    if (mode.empty() && !FIND_USER(sd)->getMode().empty())
        sendMessage(send_rpl_err(221, serv, FIND_USER(sd), '+' + FIND_USER(sd)->getMode(), ""), sd);
    else if (mode.empty() && FIND_USER(sd)->getMode().empty())
        sendMessage("You don't have any mode!", sd);
    else if (!msgtarget.empty() && idOfChannel.find(msgtarget[0]) != std::string::npos)
    {
        if (serv->getChannels().find(msgtarget) == serv->getChannels().end())
            sendMessage(send_rpl_err(403, serv, FIND_USER(sd), msgtarget, ""), sd);
        else if (FIND_USER(sd)->getMode().find('r') != std::string::npos)
            sendMessage(send_rpl_err(484, serv, FIND_USER(sd), "", ""), sd);
        else if (FIND_CHANNEL(msgtarget)->getChanops().find(sd) == FIND_CHANNEL(msgtarget)->getChanops().end())
            sendMessage(send_rpl_err(482, serv, FIND_USER(sd), msgtarget, ""), sd);
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