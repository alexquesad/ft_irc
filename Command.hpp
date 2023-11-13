#ifndef COMMAND_HPP
# define COMMAND_HPP

# include "Server.hpp"

class Server;

void nick(Server *serv, char *buffer);
void ping(Server *serv, char *buffer);
void pong(Server *serv, char *buffer);

#endif