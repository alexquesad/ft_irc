#include "Client.hpp"
#include <errno.h>
#include <stdio.h>
#include <string.h>

int max_clients = 10;

Client::Client(std::string nick, std::string user, std::string hostname, std::string real_name) : _nick(nick), _user(user), _hostname(hostname), _real_name(real_name){
}

Client::~Client(){}

std::string Client::getNick() const
{
    return this->_nick;
}

std::string Client::getUser() const
{ 
    return this->_user;
}

std::string Client::getHostname() const
{
    return this->_hostname;
}

std::string Client::getReal_name() const
{
    return this->_real_name;
}

bool Client::getIs_oper() const
{
    return this->_is_oper;
}