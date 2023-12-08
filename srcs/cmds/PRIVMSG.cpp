#include "irc.hpp"

// Function for handling the PRIVMSG (private message) command in an IRC server.
void privmsg(Server *serv, std::string buffer, int sd)
{
    // Declaration of variables used in function scope.
    int occAfterCmd;
    std::string buf(buffer);
    
    // If the command is NOTICE, set occAfterCmd to 7; otherwise, it's a PRIVMSG and set to 8.
    if (buf.compare(0, 6, "NOTICE") == 0)
        occAfterCmd = 7;
    else
        occAfterCmd = 8;
    
    // Extract the target recipient of the message from the buffer.
    size_t i;
    std::string msgtarget = "";
    if ((i = buf.find_first_not_of(sep, occAfterCmd)) != std::string::npos)
        msgtarget = buf.substr(i, (buf.find_first_of(sep, i) - i));
    
    // Extract the actual message to be sent from the buffer.
    std::string msg = "";
    size_t j;
    j = buf.find_first_of(sep, i);
    if ((j = buf.find_first_not_of(sep, j)) != std::string::npos)
        msg = buf.substr(j, (buf.find_first_of(endBuf, j) - j));
    
    // String containing channel identifier characters.
    std::string idOfChannel = "#&+";
    
    // Prepare for checking if the message is to a user or a channel.
    int userToSendSd;
    std::string userAnswer;
    
    // Format the message for sending.
    userAnswer = userOutput(FIND_USER(sd));
    userAnswer += "PRIVMSG " + msgtarget + " " + msg;

    // If the target is a channel (starts with #, &, or +), perform channel-specific checks.
    if (!msgtarget.empty() && idOfChannel.find(msgtarget[0]) != std::string::npos)
    {
        // If the target channel does not exist, send an error message to the sender.
        if (serv->getChannels().find(msgtarget) == serv->getChannels().end())
            sendMessage(sendRplErr(401, serv, FIND_USER(sd), msgtarget, ""), sd);
        // If the 'no messages to/from non-members' mode is on and the user is not in the channel, send an error message.
        else if ((FIND_CHANNEL(msgtarget)->getMode().find("n") != std::string::npos) &&
                 (FIND_CHANNEL(msgtarget)->searchUserByNickname(FIND_USER(sd)->getNickname()) == -1))
            sendMessage(sendRplErr(404, serv, FIND_USER(sd), msgtarget, ""), sd);
        // If the 'moderated channel' mode is on and the user is neither channel op nor voiced, send an error message.
        else if ((FIND_CHANNEL(msgtarget)->getMode().find("m") != std::string::npos) &&
                 (!FIND_CHANNEL(msgtarget)->isChanop(sd)) &&
                 (!FIND_CHANNEL(msgtarget)->isVoices(sd)))
            sendMessage(sendRplErr(404, serv, FIND_USER(sd), msgtarget, ""), sd);
        // If the sender is banned from the channel, send an error message.
        else if (FIND_CHANNEL(msgtarget)->isBan(FIND_USER(sd)->getNickname()) == true)
            sendMessage(sendRplErr(404, serv, FIND_USER(sd), msgtarget, ""), sd);
        // If the 'anonymous channel' mode is on, format the message using anonymous output and send to everyone in the channel except the sender.
        else if ((FIND_CHANNEL(msgtarget)->getMode().find("a") != std::string::npos))
        {
            userAnswer = anonymousOutput() + "PRIVMSG " + msgtarget + " " + msg;
            sendEveryoneInChanExceptUser(userAnswer, FIND_CHANNEL(msgtarget), sd);
        }
        // Otherwise, send the message to everyone in the channel except the sender.
        else
            sendEveryoneInChanExceptUser(userAnswer, FIND_CHANNEL(msgtarget), sd);
    }
    // If the message target is not a channel, it must be a private message to another user.
    else
    {
        // Search for the user by nickname. If not found, send an error message to the sender.
        if ((userToSendSd = serv->searchUserByNickname(msgtarget)) == -1)
            sendMessage(sendRplErr(401, serv, FIND_USER(sd), msgtarget, ""), sd);
        // If the target user is found, send them the formatted message.
        else
            sendMessage(userAnswer, userToSendSd);
    }
}
