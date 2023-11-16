#ifndef COMMAND_HPP
# define COMMAND_HPP

#include "main.hpp"

class Server;

void nick(Server *serv, char *buffer, int sd);
void join(Server *serv, char *buffer, int sd);
void privmsg(Server *serv, char *buffer, int sd);
// void ping(Server *serv, char *buffer, int sd);
// void pong(Server *serv, char *buffer, int sd);

#endif