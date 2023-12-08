#include "irc.hpp"

// Define maximum number of clients that can connect to the server.
extern int clientSocket[maxClients];

// Function to disconnect a user from the server.
void disconnectUser(Server* serv, int sd)
{
    // Retrieve all channels that the user is currently in.
    std::set<std::string> userChannels = FIND_USER(sd)->getChannels();

    // Iterate through each channel and remove the user from them.
    for (std::set<std::string>::iterator it = userChannels.begin(); it != userChannels.end(); ++it)
    {
        // Create a PART message for the user leaving the channel.
        std::string userAnswer = userOutput(FIND_USER(sd));
        userAnswer += "PART " + *it;

        // Send the PART message to everyone in the channel.
        sendEveryoneInChan(userAnswer, FIND_CHANNEL(*it));

        // Remove the user from the channel's list of users.
        FIND_CHANNEL(*it)->leftUser(sd);

        // Check if the channel is empty after user left; delete it and erase from the server's channels if so.
        if (FIND_CHANNEL(*it)->getUsersNumber() == 0)
        {
            delete serv->getChannels().find(*it)->second;
            serv->getChannels().erase(*it);
        }
    }

    // Clear the user's list of channels.
    FIND_USER(sd)->getChannels().clear();

    // Delete the user object and remove user from the server's user list.
    delete FIND_USER(sd);
    serv->getUsers().erase(sd);

    // Output disconnection details to standard output.
    std::cout << "Host disconnected , ip " << inet_ntoa(serv->getServer().sin_addr) 
              << " , port " << ntohs(serv->getServer().sin_port) << " , number of users: " 
              << serv->getUsers().size() << std::endl;

    // Reset the client socket descriptor array entry.
    for (int i = 0; i < maxClients; i++)
    {
        if (clientSocket[i] == sd)
            clientSocket[i] = 0;
    }

    // Close the socket descriptor.
    close(sd);
}

// Function to handle when a user wants to quit.
void quit(Server *serv, std::string buffer, int sd)
{
    // Copy the passed buffer to local variable for manipulation.
    std::string buf(buffer);
    
    // Declare variables for indexing and message storage.
    size_t i;
    std::string message;

    // Extract and store any message that comes after the QUIT command.
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        message = buf.substr(i, (buf.find_first_of(sep, i) - i));

    // Prepare the quit message based on whether the user provided a message or not.
    std::string userAnswer;
    if (!message.empty())
        // Include the user's custom quit message if available.
        userAnswer = userOutput(FIND_USER(sd)) + buffer;
    else
        // Default quit message if the user did not provide one.
        userAnswer = userOutput(FIND_USER(sd)) + "QUIT :leaving";

    // Disconnect the user from the server.
    disconnectUser(serv, sd);

    // Send the user's quit message to all connected users.
    sendEveryone(userAnswer, serv->getUsers());
}
