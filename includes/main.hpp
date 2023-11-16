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
#include "Command.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "Replies.hpp"

# define SERVER_NAME "localhost"
# define ver "1.0"

class Server;
class User;
class Channel;


#endif