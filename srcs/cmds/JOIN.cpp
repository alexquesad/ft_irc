#include "irc.hpp"

bool checkInvalidCharacter(char c)
{
    return (c == '\0' || c == '\a' || c == '\r' || c == '\n' || c == ' ' || c == ',' || c == ':');
}

bool channelNameInvalid(std::string name)
{
    if (name.size() > 50)
        return false;
    std::string::iterator it = name.begin();
    std::string channelId = "#&+";
    if (!name.empty() && channelId.find(name[0]) == std::string::npos)
        return false;
    for (; it != name.end() && !checkInvalidCharacter(*it); it++);
    return (it == name.end());
}

// Function allowing a user to join one or more channels on the server.
void join(Server *serv, std::string buffer, int sd)
{
    // Declare and initialize loop counters and temporary variables for parsing.
    size_t i = 0, j = 0;
    std::string buf(buffer);
    std::string channelsName = "";

    // Extract the channel names from the buffer after skipping any leading separators.
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        channelsName = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));

    // Check if no channel name was provided and send an error message if that is the case.
    if (channelsName.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "JOIN", ""), sd);
        return;
    }

    // Calculate the number of channels to join by counting commas in the channel names string.
    int nbOfChannels = 1 + std::count(channelsName.begin(), channelsName.end(), ',');
    
    // Initialize a string to hold the corresponding keys for the channels.
    std::string keysForChannels = ""; 
    
    // Extract the keys for the channels from the buffer.
    if (buf.find_first_not_of(sep, j) != std::string::npos)
    {
        j = buf.find_first_not_of(sep, j);
        keysForChannels = buf.substr(j, (buf.find_first_of(sep, j) - j));
    }

    // Loop over the number of channels to process each one.
    for (int i = 0; i < nbOfChannels; i++)
    {
        // Parse out each channel name and key, then erase them from the strings.
        std::string channelName = channelsName.substr(0, channelsName.find(","));
        channelsName.erase(0, channelsName.find(",") + 1);
        
        // Check if the user has reached the maximum number of channels they can join.
        if (!(FIND_USER(sd)->getChannelsSize() < 10))
        {
            sendMessage(sendRplErr(405, serv, FIND_USER(sd), channelName, ""), sd);
            break;
        }
        
        // Validate the channel name and send an error message if it's invalid.
        if (!channelNameInvalid(channelName))
        {
            sendMessage(channelName + ":Erroneous Channel Name", sd);
            continue;
        }
        
        // Extract the key for this particular channel.
        std::string key = keysForChannels.substr(0, keysForChannels.find(","));
        keysForChannels.erase(0, keysForChannels.find(",") + 1);
        
        // Check if the channel exists on the server, if not, create it.
        if (serv->getChannels().find(channelName) == serv->getChannels().end())
        {
            Channel *chan = new Channel(channelName);
            serv->setChannels(channelName, chan);
        }
        
        // Check for various channel modes and send appropriate error messages if conditions are not met.
        // "b" mode: Check if the user is banned from the channel.
        if (FIND_CHANNEL(channelName)->getMode().find("b") != std::string::npos)
        {
            if (FIND_CHANNEL(channelName)->isBan(FIND_USER(sd)->getNickname()) == true)
            {
                sendMessage(sendRplErr(474, serv, FIND_USER(sd), channelName, ""), sd);
                continue;
            }
        }
        
        // "k" mode: Check if the channel has a key set and compare with the provided key.
        if (FIND_CHANNEL(channelName)->getMode().find("k") != std::string::npos)
        {
            if (key.empty())
            {
                sendMessage(sendRplErr(461, serv, FIND_USER(sd), "JOIN", ""), sd);
                return;
            }
            if (FIND_CHANNEL(channelName)->getKey().compare(key) != 0)
            {
                sendMessage(sendRplErr(475, serv, FIND_USER(sd), channelName, ""), sd);
                continue;
            }
        }
        
        // "l" mode: Check if the channel is full.
        if (FIND_CHANNEL(channelName)->getMode().find("l") != std::string::npos)
        {
            if (FIND_CHANNEL(channelName)->getMaxUser() <= FIND_CHANNEL(channelName)->getUsersNumber())
            {
                sendMessage(sendRplErr(471, serv, FIND_USER(sd), channelName, ""), sd);
                continue;
            }
        }

        if (FIND_CHANNEL(channelName)->getMode().find("i") != std::string::npos)
        {
            if (FIND_CHANNEL(channelName)->isUserInvited(FIND_USER(sd)->getNickname()) == false)
            {
                sendMessage(sendRplErr(473, serv, FIND_USER(sd), channelName, ""), sd);
                continue;
            }
        }

        // Add the client to the server, setting their role and notifying other users in the channel.
        // If the channel is empty, the user becomes a channel operator (chanop).
        if (FIND_CHANNEL(channelName)->getUsersNumber() == 0)
            FIND_CHANNEL(channelName)->addChanops(sd, FIND_USER(sd));
        // If the user is already in the channel, do nothing.
        else if (FIND_CHANNEL(channelName)->searchUserByNickname(FIND_USER(sd)->getNickname()) != -1)
            return;
        // Otherwise, add the user as a regular member.
        else
            FIND_CHANNEL(channelName)->addUser(sd, FIND_USER(sd));
        
        // Update the user's list of channels.
        FIND_USER(sd)->addChannel(channelName);
        
        // Notify all users in the channel about the new user joining.
        std::string userAnswer = userOutput(FIND_USER(sd));
        userAnswer += "JOIN " + channelName;
        
        // Send JOIN messages to everyone in the channel except if mode "a" is active.
        if (FIND_CHANNEL(channelName)->getMode().find("a") == std::string::npos)
            sendEveryoneInChan(userAnswer, FIND_CHANNEL(channelName));
        
        // Send the channel topic to the user, or indicate that there is no topic.
        if (FIND_CHANNEL(channelName)->getTopic() == "")
            sendMessage(sendRplErr(331, serv, FIND_USER(sd), channelName, ""), sd);
        else
            sendMessage(sendRplErr(332, serv, FIND_USER(sd), channelName, FIND_CHANNEL(channelName)->getTopic()), sd);
        
        // Send the list of users in the channel to the newly joined user.
        std::string listOfUser = FIND_CHANNEL(channelName)->getListOfUserInChan();
        
        // Again, if mode "a" is not active, provide the list of users and end of list message.
        if (FIND_CHANNEL(channelName)->getMode().find("a") == std::string::npos)
        {
            sendMessage(sendRplErr(353, serv, FIND_USER(sd), channelName, listOfUser), sd);
            sendMessage(sendRplErr(366, serv, FIND_USER(sd), channelName, ""), sd);
        }
        
        // If the channel has modes set, send that information to the user.
        if (!FIND_CHANNEL(channelName)->getMode().empty())
            sendMessage(sendRplErr(324, serv, FIND_USER(sd), channelName, FIND_CHANNEL(channelName)->getMode()), sd);
    }
}


