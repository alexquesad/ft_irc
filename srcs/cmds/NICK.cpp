#include "main.hpp"

// bool nickname_is_valid(std::string nick)
// {
//     if (nick.size() > 9)
//         return (false);
//     if (nick[1] != )
// }

void nick(Server *serv, char *buffer, int sd)
{
    int i = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && buf[5 + i] != ' ' && buf[5 + i] != '\r' && buf[5 + i] != '\n';i++);
    std::string new_nickname(buf.substr(5, i));
    // if (!nickname_is_valid(new_nickname))
    // {
    //     sendMessage(send_rpl_err(432, serv, serv->getUsers().find(sd)->second, new_nickname, ""), sd);
    //     return ;
    // }
    std::string user_answer = user_output(serv->getUsers().find(sd)->second);
    user_answer += buffer;
    std::cout << user_answer << std::endl;
    sendMessage(user_answer, sd);
    serv->getUsers().find(sd)->second->setNick(new_nickname);
}