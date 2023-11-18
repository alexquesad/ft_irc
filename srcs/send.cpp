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
    for (std::map<int, User *>::iterator it = chan->getChanops().begin(); it != chan->getChanops().end(); it++)
    {
        sendMessage(message, it->first);
    }
    for (std::map<int, User *>::iterator it = chan->getVoices().begin(); it != chan->getVoices().end(); it++)
    {
        sendMessage(message, it->first);
    }
}

void sendEveryone(std::string message, Channel *chan, int sd)
{
    for (std::map<int, User *>::iterator it = chan->getUsers().begin(); it != chan->getUsers().end(); it++)
    {
        if (sd != it->first)
            sendMessage(message, it->first);
    }
    for (std::map<int, User *>::iterator it = chan->getChanops().begin(); it != chan->getChanops().end(); it++)
    {
        if (sd != it->first)
            sendMessage(message, it->first);
    }
    for (std::map<int, User *>::iterator it = chan->getVoices().begin(); it != chan->getVoices().end(); it++)
    {
        if (sd != it->first)
            sendMessage(message, it->first);
    }
}