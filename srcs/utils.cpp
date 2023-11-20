#include "main.hpp"

extern int client_socket[max_clients];

std::string user_output(User *user)
{
    std::string output = ":";

    output += user->getNickname();
    output += "!";
    output += user->getUsername();
    output += "@";
    output += user->getHostname();
    output += " ";
    return (output);
}

std::string anonymous_output()
{
    return (":anonymous!anonymous@anonymous ");
}

void disconnectUser(Server *serv, int sd)
{
    std::set<std::string> userChannels = FIND_USER(sd)->getChannels();
    std::string userAnswer = user_output(FIND_USER(sd));
    // pour chaque channel dans user effacer ce user dans le channel;
    for (std::set<std::string>::iterator it = userChannels.begin(); it != userChannels.end(); it++)
    {
        userAnswer += "PART " + *it;
        sendEveryone(userAnswer, FIND_CHANNEL(*it));
        FIND_CHANNEL(*it)->leftUser(sd);
        if (FIND_CHANNEL(*it)->getUsersnumber() == 0)
            serv->getChannels().erase(*it);
        FIND_USER(sd)->getChannels().clear();
    }
    serv->getUsers().erase(sd);
    std::cout << "Host disconnected , ip " << inet_ntoa(serv->getServer().sin_addr) << " , port " << ntohs(serv->getServer().sin_port) << " , number of users: " <<  serv->getUsers().size() << std::endl;
    for (int i = 0; i < max_clients; i++)
        if (client_socket[i] == sd)
            client_socket[i] = 0;
    close(sd);
}