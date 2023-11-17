#include "main.hpp"

void oper(Server *serv, char *buffer, int sd)
{
    int i = 0;
    std::string buf(buffer);

    for (; buf[5 + i] && buf[5 + i] != ' ' && buf[5 + i] != '\r' && buf[5 + i] != '\n'; i++);
    std::string names(buf.substr(5, i));
    int nb_of_users = 1 + std::count(names.begin(), names.end(), ',');
    for (int i = 0; i < nb_of_users; i++)
    {
        std::string names = names.substr(0, names.find(","));
        names.erase(0, names.find(",") + 1);
        // if ()
    }
}