#ifndef MAIN_HPP
# define MAIN_HPP

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "User.hpp"
class User;
#include "Command.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Replies.hpp"

# define SERVER_NAME "localhost"
# define ver "1.0"

class Server;
class User;
class Channel;

std::string user_output(User *user);
void sendMessage(std::string message, int sd);
void sendEveryone(std::string message, Channel *chan);
void sendEveryone(std::string message, Channel *chan, int sd);
bool nickname_is_in_use(Server *serv, std::string nick);

#endif