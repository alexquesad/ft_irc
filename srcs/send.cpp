#include "main.hpp"

void sendMessage(std::string message, int sd)
{
	message += "\r\n";
	if (send(sd, message.c_str(), message.length(), 0) < 0)
		throw std::runtime_error("Error sending message.");
}

void sendEveryone(std::string message, Channel *chan)
{
    for (std::map<int, User *>::iterator it = chan->getUsers().begin(); it != chan->getUsers().end(); it++)
    {
        sendMessage(message, it->first);
    }
    for (std::map<int, User *>::iterator it = chan->getOpers().begin(); it != chan->getOpers().end(); it++)
    {
        sendMessage(message, it->first);
    }
}