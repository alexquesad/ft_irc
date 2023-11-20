#ifndef COMMAND_HPP
# define COMMAND_HPP

#include "main.hpp"

class Server;

void nick(Server *serv, char *buffer, int sd);
void join(Server *serv, char *buffer, int sd);
void privmsg(Server *serv, char *buffer, int sd);
void ping(Server *serv, char *buffer, int sd);
void pong(Server *serv, char *buffer, int sd);
void part(Server *serv, char *buffer, int sd);
void topic(Server *serv, char *buffer, int sd);
void kick(Server *serv, char *buffer, int sd);
void mode(Server *serv, char *buffer, int sd);
void oper(Server *serv, char *buffer, int sd);

#endif