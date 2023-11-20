#include "main.hpp"

void oper(Server *serv, char *buffer, int sd)
{
    (void)serv;
    (void)sd;
    std::string buf(buffer);
    std::string password;
    std::string user;
    int i = 0;
    for (int j = 0;buffer[i] && j < 1; i++)
        if (buffer[i] == ' ')
            j++;
    int k = 0;
    for (int j = 0;buffer[k] && j < 2; k++)
        if (buffer[k] == ' ')
            j++;
    user = buf.substr(i, k - i - 1);
    if (serv->searchUserByNickname(user) == -1)
    {
        sendMessage(send_rpl_err(401, serv, FIND_USER(sd), user, ""), sd);
        return ;
    }
    password = buf.substr(k, buf.length() - 2 - k);
    // std::cout << "user: " << user << " password: " << password << std::endl;
    if (password.compare(OPER_PW) == 0)
    {
        std:: string user_answer = user_output(FIND_USER(serv->searchUserByNickname(user)));
        user_answer += "MODE " + FIND_USER(serv->searchUserByNickname(user))->getNickname() + " +o";
        FIND_USER(serv->searchUserByNickname(user))->setMode(FIND_USER(serv->searchUserByNickname(user))->getMode() + "o");
        sendMessage(user_answer, serv->searchUserByNickname(user));
        sendMessage(send_rpl_err(381, serv, FIND_USER(serv->searchUserByNickname(user)), "", ""), serv->searchUserByNickname(user));
    }
    else
        sendMessage(send_rpl_err(464, serv, FIND_USER(serv->searchUserByNickname(user)), "", ""), sd);
}