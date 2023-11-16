#include <string>
#include "User.hpp"

std::string user_output(User *user)
{
    std::string output = ":";

    output += user->getNickname();
    output += "!";
    output += user->getUsername();
    output += "@";
    output += user->getHostname();
    output += " ";
    return (output);
}