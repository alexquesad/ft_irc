#include "irc.hpp"

// Function to handle a PART command in an IRC-like server, allowing a user to leave channels.
void part(Server *serv, std::string buffer, int sd)
{
    // Create a string copy of the input buffer for manipulation.
    std::string buf(buffer);
    size_t i; // Variable to store string index.
    std::string channelsName = ""; // To keep track of channel names.

    // Finds and extracts the first channel's name from the buffer.
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        channelsName = buf.substr(i, (buf.find_first_of(sep, i) - i));

    // If no channel name is supplied, send an error message and return from the function.
    if (channelsName.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "PART", ""), sd);
        return;
    }

    // Initialize a message string to possibly send with the PART command.
    std::string message = "";
    size_t j = i;
    // Find the next separator after the channel name.
    j = buf.find_first_of(sep, j);
    // Then find the first non-separator character, which is the start of the message.
    j = buf.find_first_not_of(sep, j);
    int u = j; // Store position to extract the message.

    // Look for the end marker of the buffer and if there's a message, extract it.
    if ((j = buf.find_first_of(endBuf, j)) != std::string::npos)
        message = buf.substr(u, (buf.find_first_of(endBuf, u) - u));

    // Count the number of channels provided in the command.
    int nbOfChannels = 1 + std::count(channelsName.begin(), channelsName.end(), ',');

    // Loop through all provided channels.
    for (int i = 0; i < nbOfChannels; i++)
    {
        // Extracts each individual channel name.
        std::string channelName = channelsName.substr(0, channelsName.find(","));
        // Erase the processed channel name from the list.
        channelsName.erase(0, channelsName.find(",") + 1);

        // If the channel does not exist, send an error message.
        if (serv->getChannels().find(channelName) == serv->getChannels().end())
            sendMessage(sendRplErr(403, serv, FIND_USER(sd), channelName, ""), sd);
        // If the user is not in the channel, send an error message.
        else if (FIND_USER(sd)->getChannels().find(channelName) == FIND_USER(sd)->getChannels().end())
            sendMessage(sendRplErr(442, serv, FIND_USER(sd), channelName, ""), sd);
        else
        {
            // Construct the PART command to notify other users.
            std::string userAnswer = userOutput(FIND_USER(sd));
            userAnswer += "PART " + channelName + " " + message;

            // Check if the channel mode allows announcing departures, and act accordingly.
            if (FIND_CHANNEL(channelName)->getMode().find("a") == std::string::npos)
                // Send message to all users in the channel.
                sendEveryoneInChan(userAnswer, FIND_CHANNEL(channelName));
            else
                // Else send message only to the user who is parting.
                sendMessage(userAnswer, sd);

            // Removing the user from the channel.
            FIND_CHANNEL(channelName)->leftUser(sd);

            // If there are no more users in the channel, delete the channel.
            if (FIND_CHANNEL(channelName)->getUsersNumber() == 0)
            {
                delete serv->getChannels().find(channelName)->second; // Delete the channel object.
                serv->getChannels().erase(channelName); // Erase the channel from the server's map.
            }

            // Remove the channel from the user's list of channels.
            FIND_USER(sd)->getChannels().erase(channelName);
        }
    }
}
