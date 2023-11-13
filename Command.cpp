#include "Command.hpp"
#include "Server.hpp"


void nick(Server *serv, char *buffer)
{
    (void)serv;
    (void) buffer;
    std::cout << "test1" << std::endl;
}

void ping(Server *serv, char *buffer)
{
    buffer[1] = 'O';
    serv->sendMessage(buffer);
    std::cout << "test2 " << buffer << std::endl;
}

void pong(Server *serv, char *buffer)
{
    buffer[1] = 'I';
    serv->sendMessage(buffer);
    std::cout << "test3" << std::endl;
}

// void user()
// {
//     std::cout << "test2" << std::endl;
// }