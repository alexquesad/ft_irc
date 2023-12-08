#include "irc.hpp"

// Defines a function to handle the "TOPIC" command in an IRC Server context.
void topic(Server *serv, std::string buffer, int sd)
{
    // Initialize a copy of the incoming message buffer.
    std::string buf(buffer);

    // Declare variables to hold positions within a string.
    size_t i, j = 0;

    // Variable to hold the name of the channel.
    std::string channelName = "";

    // Extract the channel name from the buffer after the 'TOPIC' keyword.
    if ((i = buf.find_first_not_of(sep, 6)) != std::string::npos)
        channelName = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));

    // Check if the channel name is empty and respond with an error if it is.
    if (channelName.empty())
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "TOPIC", ""), sd);
    // Check if the specified channel does not exist and respond with an error.
    else if (serv->getChannels().find(channelName) == serv->getChannels().end())
        sendMessage(sendRplErr(403, serv, FIND_USER(sd), channelName, ""), sd);
    // Check if the user is not part of the channel and respond with an error.
    else if (FIND_USER(sd)->getChannels().find(channelName) == FIND_USER(sd)->getChannels().end())
        sendMessage(sendRplErr(442, serv, FIND_USER(sd), channelName, ""), sd);
    // Check if the channel has 'topic lock' mode set and the user is restricted and respond with an error.
    else if ((FIND_CHANNEL(channelName)->getMode().find("t") != std::string::npos) && (FIND_USER(sd)->getMode().find('r') != std::string::npos))
        sendMessage(sendRplErr(484, serv, FIND_USER(sd), "", ""), sd);
    // Check if the channel has 'topic lock' mode set and the user is not a channel operator and respond with an error.
    else if ((FIND_CHANNEL(channelName)->getMode().find("t") != std::string::npos) && (FIND_CHANNEL(channelName)->getChanops().find(sd) == FIND_CHANNEL(channelName)->getChanops().end()))
        sendMessage(sendRplErr(482, serv, FIND_USER(sd), channelName, ""), sd);
    else
    {
        // Variable to store the new topic if provided by the buffer.
        std::string topic = "";
        // Attempt to extract a new topic from the buffer.
        if ((j = buf.find_first_not_of(sep, j)) != std::string::npos)
        {
            if (buf[j] == ':')
                j++;
            topic = buf.substr(j, (buf.find_first_of(endBuf, j) - j));
        }
        // If no topic was provided, send back the current topic.
        if (topic.empty())
            sendMessage(sendRplErr(332, serv, FIND_USER(sd), channelName, FIND_CHANNEL(channelName)->getTopic()), sd);
        else
        {
            // Set the new topic for the channel.
            FIND_CHANNEL(channelName)->setTopic(topic);
            // Announce the change to the channel members.
            sendMessage(sendRplErr(332, serv, FIND_USER(sd), channelName, FIND_CHANNEL(channelName)->getTopic()), sd);
            // Prepare the message to be sent to users.
            std::string userAnswer = userOutput(FIND_USER(sd));
            // Handle anonymous mode for the channel if set.
            if (FIND_CHANNEL(channelName)->getMode().find("a") != std::string::npos)
                userAnswer = anonymousOutput();
            userAnswer += "TOPIC " + channelName + " " + FIND_CHANNEL(channelName)->getTopic();
            // Broadcast the topic change to all users in the channel.
            sendEveryoneInChan(userAnswer, FIND_CHANNEL(channelName));
        }
    }
}
