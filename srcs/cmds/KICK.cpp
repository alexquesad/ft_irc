#include "irc.hpp"

// Function to process a KICK command, which removes users from a channel.
void kick(Server *serv, std::string buffer, int sd)
{
    // Store the buffer and count the number of kick commands by newline characters.
    std::string buf(buffer);
    int kickCount = std::count(buf.begin(), buf.end(), '\n');
    int k = 0; // Variable to keep track of the next user's nickname start position.
    size_t i, j; // Variables for string indexing.
    std::string channelsName = "";

    // Extracting the channel names from the buffer.
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        channelsName = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));

    // Check if no channel name was provided and send an error message if that is the case.
    if (channelsName.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "KICK", ""), sd);
        return;
    }

    // Calculate the number of channels from which the user will be kicked.
    int nbOfChannels = 1 + std::count(channelsName.begin(), channelsName.end(), ',');

    // Move past the channel name(s) to get to the nickname(s) in the buffer.
    j = buf.find_first_not_of(sep, j);
    k = j;

    // Iterate over each KICK command present in the buffer.
    for (int l = 0; l < kickCount; l++)
    {
        // Parse out the nicknames of the users to kick.
        std::string usersNick = buf.substr(k, (buf.find_first_of(sep, k) - k));
        int nbUsers = 1 + std::count(usersNick.begin(), usersNick.end(), ',');

        // If no user nickname is provided, send an error message.
        if (usersNick.empty())
        {
            sendMessage(sendRplErr(461, serv, FIND_USER(sd), "KICK", ""), sd);
            return;
        }

        // Process each user nickname.
        for (int j = 0; j < nbUsers; j++)
        {
            // Parse out the individual nickname and update the usersNick string.
            std::string userNick = usersNick.substr(0, usersNick.find(","));
            usersNick.erase(0, usersNick.find(",") + 1);

            // Process each channel name.
            for (int i = 0; i < nbOfChannels; i++)
            {
                std::string tmp = channelsName;
                std::string channelName = tmp.substr(0, tmp.find(","));
                tmp.erase(0, tmp.find(",") + 1);

                // Check if the channel exists in the server's list.
                if (serv->getChannels().find(channelName) == serv->getChannels().end())
                {
                    // Send an error message if this is the first user being processed.
                    if (j == 0)
                        sendMessage(sendRplErr(403, serv, FIND_USER(sd), channelName, ""), sd);
                    continue;
                }

                // Skip processing if the anonymous flag 'a' is set in the channel mode.
                if (FIND_CHANNEL(channelName)->getMode().find("a") != std::string::npos)
                    continue;

                // Check if the kicking user is in the channel.
                if (FIND_CHANNEL(channelName)->searchUserByNickname(FIND_USER(sd)->getNickname()) == -1)
                {
                    sendMessage(sendRplErr(442, serv, FIND_USER(sd), channelName, ""), sd);
                    continue;
                }

                // Check if the kicking user has 'restricted' mode set.
                if (FIND_USER(sd)->getMode().find('r') != std::string::npos)
                {
                    sendMessage(sendRplErr(484, serv, FIND_USER(sd), "", ""), sd);
                    continue;
                }

                // Verify that the kicking user has channel operator privileges.
                if (!FIND_CHANNEL(channelName)->isChanop(sd))
                {
                    sendMessage(sendRplErr(482, serv, FIND_USER(sd), channelName, ""), sd);
                    continue;
                }

                // Determine if the user to be kicked is present in the channel.
                int userToKickSd;
                if ((userToKickSd = FIND_CHANNEL(channelName)->searchUserByNickname(userNick)) == -1)
                {
                    sendMessage(sendRplErr(441, serv, FIND_USER(sd), userNick, channelName), sd);
                    continue;
                }

                // Notify all users in the channel about the kick event.
                std::string userAnswer = userOutput(FIND_USER(userToKickSd));
                userAnswer += "PART " + channelName;
                sendEveryoneInChan(userAnswer, FIND_CHANNEL(channelName));

                // Remove the kicked user from the channel.
                FIND_CHANNEL(channelName)->leftUser(userToKickSd);

                // If the channel is empty after the kick, delete the channel.
                if (FIND_CHANNEL(channelName)->getUsersNumber() == 0)
                {
                    delete serv->getChannels().find(channelName)->second;
                    serv->getChannels().erase(channelName);
                }

                // Update the kicked user's list of channels.
                FIND_USER(userToKickSd)->getChannels().erase(channelName);
            }
        }

        // Prepare the buffer for the next KICK command by removing the current line.
        if (buf.find('\r') != std::string::npos)
            buf.erase(0, buf.find("\r\n") + 2); // For Windows-style end-of-line.
        else
            buf.erase(0, buf.find('\n') + 1); // For Unix-style end-of-line.
    }
}

