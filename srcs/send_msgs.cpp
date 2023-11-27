#include "irc.hpp"

/**
 * @brief Sends a message to a specified socket descriptor (sd).
 * 
 * This function appends a newline ("\r\n") to the provided message and sends it to the specified socket descriptor.
 * If the send operation encounters an error, a runtime_error is thrown.
 * 
 * @param message The message to be sent.
 * @param sd The socket descriptor to which the message will be sent.
 * @throws std::runtime_error if there is an error sending the message.
 */
void sendMessage(std::string message, int sd)
{
	message += "\r\n";
	if (send(sd, message.c_str(), message.length(), 0) < 0)
		throw std::runtime_error("Error sending message.");
}

/**
 * @brief Sends a message to everyone in a channel, including channel operators and voices.
 * 
 * This function iterates through the users, channel operators, and voices in the specified channel
 * and sends the given message to each of their respective socket descriptors.
 * 
 * @param message The message to be sent.
 * @param chan Pointer to the Channel object representing the channel.
 */
void sendEveryoneInChan(std::string message, Channel *chan)
{
    for (std::map<int, User *>::iterator it = chan->getUsers().begin(); it != chan->getUsers().end(); it++)
        sendMessage(message, it->first);
    for (std::map<int, User *>::iterator it = chan->getChanops().begin(); it != chan->getChanops().end(); it++)
        sendMessage(message, it->first);
    for (std::map<int, User *>::iterator it = chan->getVoices().begin(); it != chan->getVoices().end(); it++)
        sendMessage(message, it->first);
}

/**
 * @brief Sends a message to everyone in a channel, excluding a specific user.
 * 
 * This function iterates through the users, channel operators, and voices in the specified channel,
 * excluding the user with the provided socket descriptor, and sends the given message to the remaining users.
 * 
 * @param message The message to be sent.
 * @param chan Pointer to the Channel object representing the channel.
 * @param sd The socket descriptor of the user to be excluded.
 */
void sendEveryoneInChanExceptUser(std::string message, Channel *chan, int sd)
{
    for (std::map<int, User *>::iterator it = chan->getUsers().begin(); it != chan->getUsers().end(); it++)
        if (sd != it->first)
            sendMessage(message, it->first);
    for (std::map<int, User *>::iterator it = chan->getChanops().begin(); it != chan->getChanops().end(); it++)
        if (sd != it->first)
            sendMessage(message, it->first);
    for (std::map<int, User *>::iterator it = chan->getVoices().begin(); it != chan->getVoices().end(); it++)
        if (sd != it->first)
            sendMessage(message, it->first);
}

/**
 * @brief Sends a message to everyone in a map of users.
 * 
 * This function iterates through a map of users and sends the given message to each user's socket descriptor.
 * 
 * @param message The message to be sent.
 * @param users The map of user socket descriptors.
 */
void sendEveryone(std::string message, std::map<int, User *> users)
{
    for (std::map<int, User *>::iterator it = users.begin(); it != users.end(); it++)
        sendMessage(message, it->first);
}

/**
 * @brief Generates a formatted output string for a user.
 * 
 * This function constructs and returns a string in the format ":nickname!username@hostname ".
 * 
 * @param user Pointer to the User object for which the output is generated.
 * @return The formatted output string for the user.
 */
std::string userOutput(User *user)
{
    return (":" + user->getNickname() + "!" + user->getUsername() + "@" + user->getHostname() + " ");
}

/**
 * @brief Generates a formatted output string for an anonymous user.
 * 
 * This function constructs and returns a string in the format ":anonymous!anonymous@anonymous ".
 * 
 * @return The formatted output string for an anonymous user.
 */
std::string anonymousOutput()
{
    return (":anonymous!anonymous@anonymous ");
}
