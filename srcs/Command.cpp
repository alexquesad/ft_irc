#include "Command.hpp"
#include "Server.hpp"


void nick(Server *serv, char *buffer, int sd)
{
    (void)serv;
    (void)buffer;
    (void)sd;
    std::cout << "test1" << std::endl;
}

void join(Server *serv, char *buffer, int sd)
{
    int i = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && buf[5 + i] != ' ' && buf[5 + i] != '\r' && buf[5 + i] != '\n';i++);
    std::string channel_name(buf.substr(5, i));
    if (serv->getChannels().find(channel_name) == serv->getChannels().end()) // si le channel n'existe pas on le cree et on l'ajoute a notre map de channel
	{
		Channel *chan = new Channel(channel_name);
    	serv->setChannels(channel_name, chan);
	}
	//On ajoute le client a notre serveur
    std::cout << serv->getChannels() << std::endl;
	serv->getChannels().find(channel_name)->second->addUser(sd, serv->getUsers().find(sd)->second);
    if (serv.getChannels().find(channel_name)->second->getUsersnumber() == 0)
        serv->getChannels().find(channel_name)->second->addOper(sd, serv->getUsers().find(sd)->second);
    serv->getUsers().find(sd)->second->add_channel(channel_name);
    std::cout << serv->getUsers() << std::endl;
    std::cout << serv->getChannels().find(channel_name)->second->getUsers() << std::endl;
}

void privmsg(Server *serv, char *buffer, int sd)
{
    std::cout << buffer << std::endl;
    int i = 0;
    std::string buf(buffer);
    for (; buf[8 + i] && buf[8 + i] != ' ' && buf[8 + i] != '\r' && buf[8 + i] != '\n';i++);
    std::string channel_name(buf.substr(8, i));
    std::cout << "channel_name : " << channel_name << "lol" << std::endl;
    std::map<std::string, Channel *> channels = serv->getChannels();
    std::map<std::string, Channel *>::iterator test =  channels.find(channel_name);
    std::cout << "hiiiiiiiiii : " << test->first << std::endl; //si on retire ce print ca segfault (sur mac en tout cas)
    Channel *channel = test->second;
    std::map<int, User *> users = channel->getUsers();
    std::cout << channel->getUsersnumber() << std::endl;
    std::map<int, User*>::iterator end = users.end();
    for (std::map<int, User*>::iterator it = users.begin(); it != end; it++)
    {   
        //std::cout << "hi" << std::endl;
        if (sd != it->first)
            serv->sendMessage(buffer, it->first);
    }
}

// void ping(Server *serv, char *buffer, int sd)
// {
//     buffer[1] = 'O';
//     serv->sendMessage(buffer);
//     std::cout << "test2 " << buffer << std::endl;
// }

// void pong(Server *serv, char *buffer, int sd)
// {
//     buffer[1] = 'I';
//     serv->sendMessage(buffer);
//     std::cout << "test3" << std::endl;
// }

// void user()
// {
//     std::cout << "test2" << std::endl;
// }