#pragma once

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "User.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Replies.hpp"

#define SERVER_NAME "localhost"
#define ver "1.0"
#define USER_MODE "ior"
#define CHANNEL_MODE "ovamntlkb"
#define OPER_PW "1234"

#define FIND_USER(sd) \
    serv->getUsers().find(sd)->second

#define FIND_CHANNEL(channel_name) \
    serv->getChannels().find(channel_name)->second

#define CHECKUSERMODE(sd, mode) \
    FIND_USER(sd)->getMode().find(mode) == std::string::npos ? true : false

#define CHECKCHANNELMODE(channel_name, mode) \
    FIND_CHANNEL(channel_name)->getMode().find(mode) == std::string::npos ? true : false

#define max_clients 10

#define endBuf std::string("\r\n")
#define sep std::string(" \r\n")

class Server;
class User;
class Channel;

std::string userOutput(User *user);
std::string anonymousOutput();
void disconnectUser(Server *serv, int sd);
void sendMessage(std::string message, int sd);
void sendEveryoneInChan(std::string message, Channel *chan);
void sendEveryoneInChanExceptUser(std::string message, Channel *chan, int sd);
void sendEveryone(std::string message, std::map<int, User *> users);
bool nicknameIsInUse(Server *serv, std::string nick);
bool nickname_is_valid(std::string nick);