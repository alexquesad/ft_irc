#include "main.hpp"

extern int client_socket[max_clients];

void disconnectUser(Server* serv, int sd)
{
    std::set<std::string> userChannels = FIND_USER(sd)->getChannels();
    std::string userAnswer = userOutput(FIND_USER(sd));
    // erase user from each channel;
    for (std::set<std::string>::iterator it = userChannels.begin(); it != userChannels.end(); it++)
    {
        userAnswer += "PART " + *it;
        sendEveryoneInChan(userAnswer, FIND_CHANNEL(*it));
        FIND_CHANNEL(*it)->leftUser(sd);
        if (FIND_CHANNEL(*it)->getUsersnumber() == 0)
        {
            delete serv->getChannels().find(*it)->second;
            serv->getChannels().erase(*it);
        }
    }
    FIND_USER(sd)->getChannels().clear();
    delete FIND_USER(sd);
    serv->getUsers().erase(sd);
    std::cout << "Host disconnected , ip " << inet_ntoa(serv->getServer().sin_addr) << " , port " << ntohs(serv->getServer().sin_port) << " , number of users: " <<  serv->getUsers().size() << std::endl;
    for (int i = 0; i < max_clients; i++)
        if (client_socket[i] == sd)
            client_socket[i] = 0;
    close(sd);
}

void quit(Server *serv, std::string buffer, int sd)
{
    std::string buf(buffer);
    std::cout << "enter" << std::endl;
    int i = 0;
    for (;buf[i + 5] && sep.find(buf[i + 5]) == std::string::npos; i++);
    std::string message = buf.substr(5, i);
    std::cout << "message : " << message << std::endl;
    std::string userAnswer;
    if (!message.empty())
        userAnswer = userOutput(FIND_USER(sd)) + buffer;
    else
        userAnswer = userOutput(FIND_USER(sd)) + "QUIT :leaving";
    disconnectUser(serv, sd);
    sendEveryone(userAnswer, serv->getUsers());
}