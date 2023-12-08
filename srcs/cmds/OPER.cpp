#include "irc.hpp"

// Function to process an OPER command in an IRC-like server context.
void oper(Server *serv, std::string buffer, int sd)
{
    // Copy of the input buffer for manipulation.
    std::string buf(buffer);
    int j = 0; // Index variable for parsing.
    size_t i; // Variable to store string index.
    std::string user; // Variable to store extracted username.

    // Extracts the username from the buffer, bypassing the prefix and separator characters.
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        user = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));

    // If no user is specified, send an error message to the socket descriptor (sd).
    if (user.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "OPER", ""), sd);
        return; // Exit function early as the necessary parameter is missing.
    }

    // Searches for the user by their nickname, sends an error if not found.
    if (serv->searchUserByNickname(user) == -1)
    {
        sendMessage(sendRplErr(401, serv, FIND_USER(sd), user, ""), sd);
        return; // Exit function after sending the error message.
    }

    // Moves past the username to parse the password.
    j = buf.find_first_not_of(sep, j);
    // Extracts the password from the buffer.
    std::string password = buf.substr(j, (buf.find_first_of(sep, j) - j));

    // If no password is provided, send an error message to the sd.
    if (password.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "OPER", ""), sd);
        return; // Exit function as a required parameter is missing.
    }

    // Checks if the provided password matches the operator's password (OPER_PW).
    if (password.compare(OPER_PW) == 0)
    {
        // Constructs a user output string and sets user mode to operator ('+o').
        std:: string userAnswer = userOutput(FIND_USER(serv->searchUserByNickname(user)));
        userAnswer += "MODE " + FIND_USER(serv->searchUserByNickname(user))->getNickname() + " +o";
        // Updates the user's mode to include the 'o' (operator) flag.
        FIND_USER(serv->searchUserByNickname(user))->setMode(FIND_USER(serv->searchUserByNickname(user))->getMode() + "o");
        // Sends the mode change message to the user.
        sendMessage(userAnswer, serv->searchUserByNickname(user));
        // Sends a reply confirming successful operator status to the user.
        sendMessage(sendRplErr(381, serv, FIND_USER(serv->searchUserByNickname(user)), "", ""), serv->searchUserByNickname(user));
    }
    else
    {
        // If the password does not match, send an error regarding authentication failure.
        sendMessage(sendRplErr(464, serv, FIND_USER(serv->searchUserByNickname(user)), "", ""), sd);
    }
}
