#include "main.hpp"

bool availableMode(char c)
{
    std::string availableMode = "iwoOr";
    return (!(availableMode.find(c) == std::string::npos));

}

void userMode(Server *serv, User *user, std::string mode, int sd)
{
    std::string userMode = user->getMode();

    if (mode[0] == '-')
    {
        for (int i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i]) == false)
                sendMessage(send_rpl_err(501, serv, user, "", ""), sd);
            else if (userMode.find(mode[i]) != std::string::npos)
                userMode.erase(userMode.find(mode[i]));
        }
    }
    else
    {
        for (int i = 1; mode[i]; i++)
        {
            if (availableMode(mode[i]) == false)
                sendMessage(send_rpl_err(501, serv, user, "", ""), sd);
            else if (userMode.find(mode[i]) == std::string::npos)
            {
                std::cout << "test" << std::endl;
                userMode += mode[i];
            }
        }
    }
    user->setMode(userMode);
    sendMessage(send_rpl_err(221, serv, user, userMode, ""), sd);
    std::cout << "userMode: " << userMode << std::endl;
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

    // std::string user_answer = user_output(FIND_USER(sd));
    // user_answer += buffer;
    std::string mode(&buf[i + 6]);
    for (i = 0; mode[i] && mode[i] != ' ' && mode[i] != '\r' && mode[i] != '\n';i++);
    mode.erase(i, mode.length());
    if (!msgtarget.empty() && idOfChannel.find(msgtarget[0]) != std::string::npos)
    {
        //
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
