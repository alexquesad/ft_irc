#include "main.hpp"

bool nicknameIsValid(std::string nick)
{
    std::string letter = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string special = "[]\\`_^{|}";
    std::string digit = "0123456789";
    std::string minus = "-";
    std::string firstChar = letter + special;
    std::string all = letter + special + digit + minus;
    if (nick.size() > 9)
        return (false);
    if (firstChar.find(nick[0]) == std::string::npos)
        return (false);
    for (int i = 1; nick[i] && i < 9; i++)
    {
        if (all.find(nick[i]) == std::string::npos)
            return (false);
    }
    return (true);
}

bool nicknameIsInUse(Server *serv, std::string nick)
{
    for (std::map<int, User *>::iterator it = serv->getUsers().begin(); it != serv->getUsers().end(); it++)
    {
        if (nick.compare(it->second->getNickname()) == 0)
            return (true);
    }
    return (false);
}

void nick(Server *serv, std::string buffer, int sd)
{
    int i = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && sep.find(buf[5 + i]) == std::string::npos; i++);
    std::string newNickname(buf.substr(5, i));
    if (newNickname.empty())
    {
        sendMessage(sendRplErr(431, serv, FIND_USER(sd), "", ""), sd);
        return ;
    }
    if (FIND_USER(sd)->getMode().find('r') != std::string::npos)
    {
        sendMessage(sendRplErr(484, serv, FIND_USER(sd), "", ""), sd);
        return ;
    }
    if (!nicknameIsValid(newNickname))
    {
        sendMessage(sendRplErr(432, serv, FIND_USER(sd), newNickname, ""), sd);
        return ;
    }
    if (nicknameIsInUse(serv, newNickname))
    {
        sendMessage(sendRplErr(433, serv, FIND_USER(sd), newNickname, ""), sd);
        return ;
    }
    std::string userAnswer = userOutput(FIND_USER(sd));
    userAnswer += buffer;
    sendMessage(userAnswer, sd);
    FIND_USER(sd)->setNick(newNickname);
}