#include "irc.hpp"

bool nicknameIsValid(std::string nick)
{
    std::string letter = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string special = "[]\\`_^{|}";
    std::string digit = "0123456789";
    std::string minus = "-";
    std::string firstChar = letter + special;
    std::string all = letter + special + digit + minus;
    if (nick.size() > 9)
        return (false);
    if (firstChar.find(nick[0]) == std::string::npos)
        return (false);
    for (int i = 1; nick[i] && i < 9; i++)
    {
        if (all.find(nick[i]) == std::string::npos)
            return (false);
    }
    return (true);
}

bool nicknameIsInUse(Server *serv, std::string nick)
{
    for (std::map<int, User *>::iterator it = serv->getUsers().begin(); it != serv->getUsers().end(); it++)
    {
        if (nick.compare(it->second->getNickname()) == 0)
            return (true);
    }
    return (false);
}

// Define a function to handle the "NICK" command in an IRC server context.
void nick(Server *serv, std::string buffer, int sd)
{
    // Initialize index for parsing and a string to store the new nickname.
    size_t i = 0;
    std::string newNickname = "";
    // Create a copy of the buffer for manipulation.
    std::string buf(buffer);

    // Try to find the first character of the new nickname after "NICK ".
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        // If found, extract the new nickname from the buffer.
        newNickname = buf.substr(i, (buf.find_first_of(sep, i) - i));

    // Check if the new nickname is empty.
    if (newNickname.empty())
    {
        // Send error code 431 (No nickname given) to the client.
        sendMessage(sendRplErr(431, serv, FIND_USER(sd), "", ""), sd);
        return; // Exit the function.
    }

    // Check if the user mode includes 'r' (restricted).
    if (FIND_USER(sd)->getMode().find('r') != std::string::npos)
    {
        // Send error code 484 (Restricted) to the client.
        sendMessage(sendRplErr(484, serv, FIND_USER(sd), "", ""), sd);
        return; // Exit the function.
    }

    // Check if the new nickname is not valid according to server rules.
    if (!nicknameIsValid(newNickname))
    {
        // Send error code 432 (Erroneous nickname) to the client with the invalid nickname.
        sendMessage(sendRplErr(432, serv, FIND_USER(sd), newNickname, ""), sd);
        return; // Exit the function.
    }

    // Check if the new nickname is already in use on the server.
    if (nicknameIsInUse(serv, newNickname))
    {
        // Send error code 433 (Nickname is already in use) to the client with the attempted nickname.
        sendMessage(sendRplErr(433, serv, FIND_USER(sd), newNickname, ""), sd);
        return; // Exit the function.
    }

    // Prepare a message indicating the successful change of the nickname.
    std::string userAnswer = userOutput(FIND_USER(sd)); // Get user's current state.
    userAnswer += "NICK " + newNickname; // Append the NICK command with the new nickname.

    // Send the successful nickname change message to the client.
    sendMessage(userAnswer, sd);

    // Update the user's nickname in the server records.
    FIND_USER(sd)->setNick(newNickname);
}
