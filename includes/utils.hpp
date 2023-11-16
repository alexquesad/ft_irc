#ifndef UTILS_HPP
# define UTILS_HPP

#include "main.hpp"

std::string user_output(User *user);
void sendMessage(std::string message, int sd);
void sendEveryone(std::string message, Channel *chan);
void sendEveryone(std::string message, Channel *chan, int sd);


#endif