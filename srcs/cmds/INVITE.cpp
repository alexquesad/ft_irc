#include "irc.hpp"

void invite(Server *serv, std::string buffer, int sd)
{   
    // 1. Parse out who is to be invited and to which channel from the buffer.
     size_t i = 0, j = 0;
    std::string buf(buffer);
    std::string inviteeNick = "";
    std::string channelName = "";

    // Find the target user's nickname
    if ((i = buf.find_first_not_of(sep, 7)) != std::string::npos)
        inviteeNick = buf.substr(i, ((j = buf.find_first_of(sep, i)) - i));

    // Find the channel name
    if (buf.find_first_not_of(sep, j) != std::string::npos)
    {
        j = buf.find_first_not_of(sep, j);
        channelName = buf.substr(j, (buf.find_first_of(sep, j) - j));
    }

    // Ensure both invitee and channel values have been parsed successfully.
    if (inviteeNick.empty() || channelName.empty())
    {
        sendMessage(sendRplErr(461, serv, FIND_USER(sd), "INVITE", ""), sd);
        return;
    }

    // 2. Find invitee user by nickname.
    if (serv->searchUserByNickname(inviteeNick) == -1)
    {
        sendMessage(sendRplErr(401, serv, FIND_USER(sd), inviteeNick, ""), sd);
        return;
    }

    // 3. Validate the channel existence and the inviter's membership in it.
   if (serv->getChannels().find(channelName) == serv->getChannels().end())
   {
        sendMessage(sendRplErr(403, serv, FIND_USER(sd), channelName, ""), sd);
        return;
   }
    // If the user is not in the channel, send an error message.
    if (FIND_USER(sd)->getChannels().find(channelName) == FIND_USER(sd)->getChannels().end())
    {
        sendMessage(sendRplErr(442, serv, FIND_USER(sd), channelName, ""), sd);
        return;
    }
            
    Channel* channel = FIND_CHANNEL(channelName);

    // 4. Check if the channel is invite-only and if the inviter is an operator.
    if (channel->getMode().find('i') != std::string::npos && !FIND_CHANNEL(channelName)->isChanop(sd))
    {
        // Only channel operators can invite if channel is invite-only.
        sendMessage(sendRplErr(482, serv, FIND_USER(sd), channelName, ""), sd);
        return;
    }

    // 5. Check if the user invited is already on the channel.
    int inviteeUser = channel->searchUserByNickname(inviteeNick);
    if (inviteeUser != -1)
    {
        sendMessage(sendRplErr(443, serv, FIND_USER(sd), channelName, inviteeNick), sd);
        return;
    }

    // Add the invitee to the invited users list if the channel is invite-only.
    if (channel->getMode().find('i') != std::string::npos) 
    {
        channel->addInvitee(inviteeNick);
    }

    // 6. Send invite notice to invitee.
    int inviteeUserSd = serv->searchUserByNickname(inviteeNick);
    std::string inviteMsg = ":" + FIND_USER(sd)->getNickname() + "! INVITE " + inviteeNick + " :" + channelName;
    sendMessage(inviteMsg, inviteeUserSd);

    // 7. Notify inviter that the invitation was sent successfully.
    sendMessage(sendRplErr(341, serv, FIND_USER(sd), inviteeNick, channelName), sd);
}