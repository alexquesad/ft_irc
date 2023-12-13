#include "irc.hpp"

// Handles setting or unsetting the operator ('o') mode for a user in a channel.
void mode_o(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    // Initialize an index counter to parse the buffer string.
    int i = 0;
    // Iterate through the buffer to separate command arguments based on spaces or tabs.
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        // Check if the current character is a space or a tab,
        // indicating a separation between arguments.
        if (buffer[i] == ' ' || buffer[i] == '\t')
        {
            // Skip consecutive space or tab characters to reach the next argument.
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            // Increment the argument counter.
            j++;
            // Move back one place in the buffer to point at the start of the next argument.
            i--;
        }
    }

    // Extract the nickname from the buffer using the separator character 'sep'.
    std::string name = buffer.substr(i, (buffer.find_first_of(sep, i) - i));

    // Search the channel for the user by their nickname and get their socket descriptor.
    int userSd = channel->searchUserByNickname(name);

    // If the user isn't found in the channel, send an error message to the initiator and return.
    if (userSd == -1)
    {
        sendMessage(sendRplErr(441, serv, FIND_USER(sd), name, channel->getChannelName()), sd);
        return; // Exit the function if the user is not found in the channel.
    }

    // Remove the target user from the channel's user list
    channel->leftUser(userSd);

    // Depending on whether the mode starts with '-', add the user as either a regular user or as a channel operator.
    if (mode[0] == '-')
        channel->addUser(userSd, FIND_USER(userSd)); // Re-add as a regular user without operator privileges.
    else
        channel->addChanops(userSd, FIND_USER(userSd)); // Add as a channel operator with operator privileges.

    // Prepare the response message for the user who requested the change.
    std::string userAnswer = userOutput(FIND_USER(sd));

    // Modify the response for anonymous channels by replacing it with an anonymous output.
    if (channel->getMode().find('a') != std::string::npos)
        userAnswer = anonymousOutput(); 

    // Append the mode change action details to the response message.
    userAnswer += "MODE " + channel->getChannelName() + " " + mode + " " + name;

    // Send the final composed message to the targeted user's socket descriptor.
    sendMessage(userAnswer, userSd);
}


// Handles setting or unsetting the voice mode for a user in a channel.
void mode_v(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    // Initialize an index counter for parsing the buffer.
    int i = 0;
    // Iterate through the buffer to find command arguments separated by spaces or tabs.
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t') // Check for space or tab characters.
        {
            // Skip all consecutive spaces or tab characters.
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            // Increment the argument counter.
            j++; 
            // Decrement the index to point to the beginning of the next word.
            i--; 
        }
    }
    
    // Extract the substring representing the username from the buffer, using the separator character.
    std::string name = buffer.substr(i, (buffer.find_first_of(sep, i) - i));
    
    // Search for the user in the channel by nickname and get their socket descriptor.
    int userSd = channel->searchUserByNickname(name);

    // If the nickname is not found in the channel, send an error message back to the requester.
    if (userSd == -1)
    {
        sendMessage(sendRplErr(441, serv, FIND_USER(sd), name, channel->getChannelName()), sd);
        return; // Exit function after sending the error message.
    }

    // Do not proceed if the user is already a channel operator.
    if (channel->isChanop(userSd) == true)
        return;

    // Remove the user with userSd from the channel's user list.
    channel->leftUser(userSd);

    // Depending on the mode, add the user to the regular list or the list of users with voice.
    if (mode[0] == '-')
        channel->addUser(userSd, FIND_USER(userSd)); // Add user back without voice privileges.
    else
        channel->addVoices(userSd, FIND_USER(userSd)); // Grant voice privileges to the user.

    // Prepare the response message to the user who initiated the command.
    std::string userAnswer = userOutput(FIND_USER(sd));
    
    // If the channel is in anonymous mode ('a'), use an anonymous output format.
    if (channel->getMode().find('a') != std::string::npos)
        userAnswer = anonymousOutput();
    
    // Append the mode change action to the user's response message.
    userAnswer += "MODE " + channel->getChannelName() + " " + mode + " " + name;

    // Send the constructed response message back to the initiating user.
    sendMessage(userAnswer, sd);
}


// Handles the addition or removal of a user from the channel's ban list based on mode and buffer input.
void mode_b(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    // Unused parameter is explicitly cast to void to prevent unused parameter warnings.
    (void)mode;

    // Initialize an index counter for parsing the buffer.
    int i = 0;
    // Iterate through the buffer to find command arguments separated by spaces or tabs.
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t') // Check for space or tab characters.
        {
            // Skip all consecutive spaces or tab characters.
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            // Increment the argument counter.
            j++; 
            // Correct the index to the start of the argument.
            i--; 
        }
    }
    
    // Extract the substring representing the username from the buffer.
    std::string name = buffer.substr(i, (buffer.find_first_of(sep, i) - i));

    // If no username is provided, process the request as a query for the current ban list.
    if (name.empty())
    {
        // Get a string containing the list of banned users for the channel.
        std::string banlist = channel->getListOfUserBan();

        // If the banlist is not empty, send the ban list to the user.
        if (!banlist.empty())
            sendMessage(sendRplErr(367, serv, FIND_USER(sd), channel->getChannelName(), banlist), sd);
        
        // Send end of ban list numeric reply.
        sendMessage(sendRplErr(368, serv, FIND_USER(sd), channel->getChannelName(), ""), sd);
    }
    else // If a username is provided, proceed to add or remove the user from the ban list.
    {
        // Extract the nickname from the given name (if it contains a '!' sign).
        std::string nick = name.substr(0, name.find('!'));
        
        // Process the nickname and ensure it's valid for adding/removing from the ban list.
        if (nick.length() > 1 || (nick.length() == 1 && nick[0] != '*'))
        {
            // If the first character is '*', remove it from the name.
            if (nick[0] == '*')
                name = &nick[1];
            else
                name = nick;
        }
        else
            return; // If the processed nickname is invalid, exit the function.
        
        // Check the mode to determine whether to add or remove the user to/from the ban list.
        if (mode[0] == '-')
            channel->getBanList().erase(name); // Remove the user from the ban list.
        else
            channel->getBanList().insert(std::make_pair(name, nick)); // Add the user to the ban list.
    }
}


// Sets or removes the access key (password) for a channel based on mode and buffer input.
void mode_k(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    // Unused parameters are explicitly cast to void to prevent unused parameter warnings.
    (void)mode;
    (void)serv;

    // Check if the mode starts with '-', which indicates removing the key.
    if (mode[0] == '-')
    {
        channel->setKey(""); // Clear the key for the channel.
        return; // Exit the function after removing the key.
    }

    // Initialize an index counter for parsing the buffer.
    int i = 0;
    // Iterate through the buffer to find command arguments separated by spaces or tabs.
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t') // Check for space or tab characters.
        {
            // Skip all consecutive spaces or tab characters.
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            // Increment the argument counter.
            j++;
            // Correct the index to the start of the argument.
            i--;
        }
    }
    // Extract the substring representing the key from the buffer.
    std::string key = buffer.substr(i, (buffer.find_first_of(sep, i) - i));

    // Check if the extracted key is "x", which means an error should be returned.
    if (key.compare("x") == 0)
    {
        // Send an error message using the server's response method, including current key info.
        sendMessage(sendRplErr(467, serv, FIND_USER(sd), channel->getKey(), ""), sd);
    }
    else
    {
        // If the key is not empty, set the new key for the channel.
        if (!key.empty())
            channel->setKey(key);
        else
        {
            // If the key is empty, send an error message to the user indicating a wrong key.
            std::string truc = userOutput(FIND_USER(sd)); // Get the string representation of the user.
            sendMessage(truc + ": Wrong key!", sd); // Send the error message to the user via socket descriptor.
        }
    }
}

// Changes the maximum number of users allowed in a channel based on mode and buffer input.
void mode_l(Server *serv, Channel *channel, std::string mode, std::string buffer, int sd)
{
    // Unused parameters are explicitly cast to void to prevent unused parameter warnings.
    (void)mode;
    (void)serv;

    // Check if the mode string starts with a '-' indicating removing the limit.
    if (mode[0] == '-')
    {
        channel->setMaxUser(10); // Removes the user limit from the channel.
        return; // Early exit from the function.
    }

    // Initialize an index counter for parsing the buffer.
    int i = 0;
    // Iterate over the buffer to find command arguments separated by spaces or tabs.
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t') // Check for space or tab characters.
        {
            // Skip all consecutive spaces or tab characters.
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            // Increment the argument counter.
            j++;
            // Correct the index to the start of the argument.
            i--;
        }
    }
    // Extract the substring that represents the maximum number of users for the channel.
    std::string name = buffer.substr(i, buffer.find('\r') != std::string::npos ? buffer.length() - 2 - i : buffer.length() - 1 - i);
    // Convert the extracted substring to an integer value representing maximum users.
    int maxUser = std::strtoul(name.c_str(), NULL, 0);
    
    // Check if the provided maximum user value is valid.
    if (maxUser < 0)
    {
        sendMessage("Max user key must be superior to 0.", sd); // Send error message back to the client.
        return; // Early exit from the function since maxUser is invalid.
    }
    
    // Set the maximum number of users to the channel if the value is valid.
    channel->setMaxUser(maxUser);
}

bool availableMode(char c, std::string availableMode)
{
    return (!(availableMode.find(c) == std::string::npos));
}

// Function to change the mode of a channel.
void channelMode(Server *serv, Channel *channel, std::string mode, int sd, std::string buffer)
{
    // Retrieve current modes of the channel
    std::string channelMode = channel->getMode();
    int i;  // Iterator variable for loops

    // Define a function pointer type that can be used to handle mode changes.
    typedef void (*modes)(Server *, Channel *, std::string, std::string, int);

    // Map each mode character to its respective handler function.
    std::map<char, modes> modehandler;
    modehandler.insert(std::make_pair('o', &mode_o));  // Operator privileges
    modehandler.insert(std::make_pair('v', &mode_v));  // Voice privileges
    modehandler.insert(std::make_pair('b', &mode_b));  // Ban from channel
    modehandler.insert(std::make_pair('k', &mode_k));  // Set a channel key (password)
    modehandler.insert(std::make_pair('l', &mode_l));  // User limit on a channel
    //modehandler.insert(std::make_pair('i', &mode_i));  // Channel invite only

    // Check if we are removing modes from the channel.
    if (mode[0] == '-')
    {
        std::string deletedMode;  // String to keep track of the modes being removed
        // Iterate over each mode character except the first one (the '-' sign).
        for (i = 1; mode[i]; i++)
        {
            // Check if the mode is available and valid.
            if (availableMode(mode[i], CHANNEL_MODE) == false)
            {
                std::string stringMode(1, mode[i]);  // Convert char to string for messaging
                // Send an error message back to the client if the mode is not available.
                sendMessage(sendRplErr(472, serv, FIND_USER(sd), stringMode, channel->getChannelName()), sd);
            }
            else if (availableMode(mode[i], "ovbkl") == true)
            {
                // Call the appropriate mode handler function for valid modes.
                modehandler[mode[i]](serv, channel, mode, buffer, sd);
        	    // For modes 'k' and 'l', we keep track which were successfully deleted.
                if (mode[i] == 'k' || mode[i] == 'l' || mode[i] == 'b')
                    deletedMode += mode[i];
            }
            // If the mode exists in the current modes, remove it.
            else if (channelMode.find(mode[i]) != std::string::npos)
            {
                deletedMode += mode[i];
                channelMode.erase(channelMode.find(mode[i]), 1);
            }
        }
        // Update channel modes after removal.
        channel->setMode(channelMode);

        // Prepare a user-specific answer string based on mode changes.
        std::string userAnswer = userOutput(FIND_USER(sd));
        // Adjust the message for anonymous channels.
        if (channel->getMode().find("a") != std::string::npos)
            userAnswer = anonymousOutput();

        // Append MODE command with the deleted modes if any were removed.
        if (!deletedMode.empty())
            userAnswer += "MODE " + channel->getChannelName() + " -" + deletedMode;

        // Broadcast the changes to everyone in the channel if modes were changed.
        if (userAnswer.find("MODE") != std::string::npos)
            sendEveryoneInChan(userAnswer, channel);
    }
    else  // Mode string does not start with '-', so we are adding modes.
    {
        std::string addedMode;  // Keep track of the modes being added

        // Iterate over each mode character (start at index 0 if no '+' sign, otherwise start at 1).
        for ((mode[0] != '+') ? i = 0 : i = 1; mode[i]; i++)
        {
            // Check if the mode is available and valid.
            if (availableMode(mode[i], CHANNEL_MODE) == false)
            {
                std::string stringMode(1, mode[i]);  // Convert char to string for messaging
                // Send an error message back to the client if the mode is not available.
                sendMessage(sendRplErr(472, serv, FIND_USER(sd), stringMode, channel->getChannelName()), sd);
            }
            else if (availableMode(mode[i], "ovbkl") == true)
            {
                // Call the appropriate mode handler function for valid modes.
                modehandler[mode[i]](serv, channel, mode, buffer, sd);
                // Track modes 'k' (when set) and 'l' as they are added.
                if ((mode[i] == 'k' && channel->getKey() != "") || mode[i] == 'l' || mode[i] == 'b')
                    addedMode += mode[i];
            }
            // Append mode only if it's not already present in the current modes.
            else if (channelMode.find(mode[i]) == std::string::npos)
                addedMode += mode[i];
        }
        // Update channel modes by appending new modes.
        channel->setMode(channelMode + addedMode);

        // Prepare a user-specific answer string based on mode changes.
        std::string userAnswer = userOutput(FIND_USER(sd));
        // Adjust the message for anonymous channels.
        if (channel->getMode().find("a") != std::string::npos)
            userAnswer = anonymousOutput();

        // Append MODE command with the added modes if any were added.
        if (!addedMode.empty())
            userAnswer += "MODE " + channel->getChannelName() + " +" + addedMode;

        // Broadcast the changes to everyone in the channel if modes were changed.
        if (userAnswer.find("MODE") != std::string::npos)
            sendEveryoneInChan(userAnswer, channel);
    }
}


// Function to change or display a user's mode.
void userMode(Server *serv, User *user, std::string mode, int sd)
{
    // Get current modes of the user.
    std::string userMode = user->getMode();
    int i;
    // Find socket descriptor for the user based on their nickname.
    int userSd = serv->searchUserByNickname(user->getNickname());

    // If the first character in the mode string is a '-', proceed to delete modes.
    if (mode[0] == '-')
    {
        std::string deletedMode; // String to keep track of modes that are being deleted.
        // Iterate over the mode string starting from the second character.
        for (i = 1; mode[i]; i++)
        {
            // Check if a given mode character is an available user mode.
            if (availableMode(mode[i], USER_MODE) == false)
                // Send an error message if mode is not available (Reply Code: 501).
                sendMessage(sendRplErr(501, serv, FIND_USER(sd), "", ""), sd);
            else if (userMode.find(mode[i]) != std::string::npos)
            {
                // Special condition check for 'o' (operator) mode.
                if (mode[i] == 'o' && user->getNickname() != FIND_USER(sd)->getNickname())
                    // Not allowed to deop other users (Reply Code: 481).
                    sendMessage(sendRplErr(481, serv, FIND_USER(sd), "", ""), sd);
                // Special condition check for 'r' (restricted) mode.
                else if (mode[i] == 'r' && ((FIND_USER(sd)->getMode().find('r') != std::string::npos) || (FIND_USER(sd)->getMode().find('o') == std::string::npos)))
                    // Not allowed to change 'r' mode under certain conditions (Reply Code: 481).
                    sendMessage(sendRplErr(481, serv, FIND_USER(sd), "", ""), sd);
                else
                {
                    // Add mode to the list of deleted modes and remove it from the user's current modes.
                    deletedMode += mode[i];
                    userMode.erase(userMode.find(mode[i]), 1);
                }
            }
        }
        // Update user's mode after deletion.
        user->setMode(userMode);
        // Generate user response string.
        std::string userAnswer = userOutput(FIND_USER(userSd));
        // If any modes were deleted, append this info to the response.
        if (!deletedMode.empty())
            userAnswer += "MODE " + user->getNickname() + " -" + deletedMode;
        // Only send a message if there was a mode change.
        if (userAnswer.find("MODE") != std::string::npos)
            sendMessage(userAnswer, userSd);
    }
    else // If the first character in the mode string is not a '-', proceed to add modes.
    {
        std::string addedMode; // String to keep track of modes that are being added.
        // Initialize iterator. Start at 0 if no '+' sign; otherwise start at 1.
        for ((mode[0] != '+') ? i = 0 : i = 1; mode[i]; i++)
        {
            // Check if a given mode character is an available user mode.
            if (availableMode(mode[i], USER_MODE) == false)
                // Send an error message if mode is not available (Reply Code: 501).
                sendMessage(sendRplErr(501, serv, FIND_USER(sd), "", ""), sd);
            else if (userMode.find(mode[i]) == std::string::npos)
            {
                // Special condition check for 'o' (operator) mode.
                if (mode[i] == 'o' && FIND_USER(sd)->getMode().find('o') == std::string::npos)
                    // Not allowed to op users when sender isn't an op (Reply Code: 481).
                    sendMessage(sendRplErr(481, serv, FIND_USER(sd), "", ""), sd);
                else if (mode[i] == 'r' && user->getNickname() == FIND_USER(sd)->getNickname())
                    // Special handling if adding 'r' mode to oneself.
                    addedMode += mode[i];
                else if (mode[i] == 'r' && ((FIND_USER(sd)->getMode().find('r') != std::string::npos) || (FIND_USER(sd)->getMode().find('o') == std::string::npos)))
                    // Not allowed to change 'r' mode under certain conditions (Reply Code: 481).
                    sendMessage(sendRplErr(481, serv, FIND_USER(sd), "", ""), sd);
                else
                    // Add mode to the list of added modes.
                    addedMode += mode[i];
            }
        }
        // Update user's mode after addition.
        user->setMode(userMode + addedMode);
        // Generate user response string.
        std::string userAnswer = userOutput(FIND_USER(userSd));
        // If any modes were added, append this info to the response.
        if (!addedMode.empty())
            userAnswer += "MODE " + user->getNickname() + " +" + addedMode;
        // Only send a message if there was a mode change.
        if (userAnswer.find("MODE") != std::string::npos)
            sendMessage(userAnswer, userSd);
    }
}


// Function to handle "MODE" command in the server context.
void mode(Server *serv, std::string buffer, int sd)
{
    // Copying input buffer for manipulation and processing.
    std::string buf(buffer);
    
    // Initialization of variables used to track string parsing positions.
    int j = 0;
    size_t i;
    
    // This will hold the target of the MODE command (channel or user).
    std::string msgtarget = "";
    
    // Parse out the target from the buffer starting after the "MODE" keyword which starts at index 5.
    if ((i = buf.find_first_not_of(sep, 5)) != std::string::npos)
        msgtarget = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));
    
    // If no valid target is found, send an error message with code 461 (ERR_NEEDMOREPARAMS).
    if (msgtarget.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "MODE", ""), sd);
        return;
    }
    
    // Valid prefixes for channel identifiers.
    std::string idOfChannel = "#&+";
    
    // Skip separators after the target to find the start of the next parameter.
    j = buf.find_first_not_of(sep, j);
    
    // This will store the mode changes requested.
    std::string mode = "";
    
    // Extract the mode changes from the buffer if they exist.
    if (buf.find_first_of(sep, j) != std::string::npos)
        mode = buf.substr(j, (buf.find_first_of(sep, j) - j));
    
    // Check if the target is a channel based on its first character.
    if (!msgtarget.empty() && idOfChannel.find(msgtarget[0]) != std::string::npos)
    {
        // If the channel does not exist, send an error message with code 403 (ERR_NOSUCHCHANNEL).
        if (serv->getChannels().find(msgtarget) == serv->getChannels().end())
            sendMessage(sendRplErr(403, serv, FIND_USER(sd), msgtarget, ""), sd);
        // If the issuing user has restricted mode 'r', send an error message with code 484 (ERR_RESTRICTED).
        else if (FIND_USER(sd)->getMode().find('r') != std::string::npos)
            sendMessage(sendRplErr(484, serv, FIND_USER(sd), "", ""), sd);
        // If the issuing user is not a channel operator, send an error message with code 482 (ERR_CHANOPRIVSNEEDED).
        else if (FIND_CHANNEL(msgtarget)->getChanops().find(sd) == FIND_CHANNEL(msgtarget)->getChanops().end())
            sendMessage(sendRplErr(482, serv, FIND_USER(sd), msgtarget, ""), sd);
        // If none of the above conditions are met, proceed to change the channel's mode.
        else
            channelMode(serv, FIND_CHANNEL(msgtarget), mode, sd, buffer);
    }
    else // The target is not a channel (hence it should be a user).
    {
        // Attempt to find user descriptor by nickname.
        int userSd;
        if ((userSd = serv->searchUserByNickname(msgtarget)) == -1)
            // If user is not found, send an error message with code 401 (ERR_NOSUCHNICK).
            sendMessage(sendRplErr(401, serv, FIND_USER(sd), msgtarget, ""), sd);
        // If no mode specified and the target user has modes, respond with code 221 (RPL_UMODEIS).
        else if (mode.empty() && !FIND_USER(userSd)->getMode().empty())
            sendMessage(sendRplErr(221, serv, FIND_USER(userSd), '+' + FIND_USER(userSd)->getMode(), ""), sd);
        // If no mode specified and the target user has no modes, send appropriate message.
        else if (mode.empty() && FIND_USER(userSd)->getMode().empty())
            sendMessage("This user don't have any modes!", sd);
        // If mode change is requested, proceed to change the user's mode.
        else
            userMode(serv, FIND_USER(userSd), mode, sd);
    }
}
