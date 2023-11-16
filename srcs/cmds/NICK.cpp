#include "main.hpp"

bool nickname_is_valid(std::string nick)
{
    std::string letter = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string special = "[]\\'_^{|}";
    std::string digit = "0123456789";
    std::string minus = "-";
    std::string first_char = letter + special;
    std::string all = letter + special + digit + minus;
    if (nick.size() > 9)
        return (false);
    if (first_char.find(nick[0]) == std::string::npos)
        return (false);
    for (int i = 1; nick[i] && i < 9; i++)
    {
        if (all.find(nick[i]) == std::string::npos)
            return (false);
    }
    return (true);
}

bool nickname_is_in_use(Server *serv, std::string nick)
{
    for (std::map<int, User *>::iterator it = serv->getUsers().begin(); it != serv->getUsers().end(); it++)
    {
        if (nick.compare(it->second->getNickname()) == 0)
            return (true);
    }
    return (false);
}

void nick(Server *serv, char *buffer, int sd)
{
    int i = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && buf[5 + i] != ' ' && buf[5 + i] != '\r' && buf[5 + i] != '\n';i++);
    std::string new_nickname(buf.substr(5, i));
    std::cout << "[ " << new_nickname << " ]" << std::endl;
    if (new_nickname.empty())
    {
        sendMessage(send_rpl_err(431, serv, serv->getUsers().find(sd)->second, "", ""), sd);
        return ;
    }
    if (!nickname_is_valid(new_nickname))
    {
        sendMessage(send_rpl_err(432, serv, serv->getUsers().find(sd)->second, new_nickname, ""), sd);
        return ;
    }
    if (nickname_is_in_use(serv, new_nickname))
    {
        sendMessage(send_rpl_err(433, serv, serv->getUsers().find(sd)->second, new_nickname, ""), sd);
        return ;
    }
    std::string user_answer = user_output(serv->getUsers().find(sd)->second);
    user_answer += buffer;
    std::cout << user_answer << std::endl;
    sendMessage(user_answer, sd);
    serv->getUsers().find(sd)->second->setNick(new_nickname);
}