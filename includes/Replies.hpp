#ifndef RPL_ERR_HPP
# define RPL_ERR_HPP

#include "main.hpp"

std::string send_rpl_err(int code, Server *serv, User *user, std::string args, std::string args2);

//RPL

//001
# define RPL_WELCOME(nick, user, host) \
    (":Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host)

//002
# define RPL_YOURHOST \
    ("Your host is " SERVER_NAME ", running version " ver)

//003
# define RPL_CREATED \
    ("This server was created 03/04/02")

//004
# define RPL_MYINFO \
    (SERVER_NAME " " ver " iwoOr <available channel modes>")

//005
# define RPL_BOUNCE(port) \
    ("Try server " SERVER_NAME ", port " + port)

//221
# define RPL_UMODEIS(args) \
    (args)

//301
# define RPL_AWAY(nick) \
    (nick + " :Message has been sent !")

//315
# define RPL_ENDOFWHO(name) \
    (name + " :End of WHO list")

//322
# define RPL_LIST(channel) \
    (channel +" <# visible> :<topic>")

//323
# define RPL_LISTEND \
    (":End of LIST")

//324
# define RPL_CHANNELMODEIS(channel) \
    (channel + " <mode> <mode params>")

//331
# define RPL_NOTOPIC(channel) \
    (channel + " :No topic is set")

//332
# define RPL_TOPIC(channel, topic) \
    (channel + " :" + topic)

//341
# define RPL_INVITING(channel, nick) \
    (channel + " " + nick)

//351
# define RPL_VERSION \
    (ver ".<debuglevel> " SERVER_NAME " :<comments>")

//353
# define RPL_NAMREPLY(channel, list_of_users) \
    ("= " + channel + " :" + list_of_users)

//366
# define RPL_ENDOFNAMES(channel) \
    (channel + " :End of NAMES list")

//367
# define RPL_BANLIST(channel) \
    (channel + " <banmask>")

//368
# define RPL_ENDOFBANLIST(channel) \
    (channel + " :End of channel ban list")

//372
# define RPL_MOTD \
    (":- <text>")

//375
# define RPL_MOTDSTART \
    (":- " SERVER_NAME " Message of the day - ")

//376
# define RPL_ENDOFMOTD \
    (":End of MOTD command")

//381
# define RPL_YOUREOPER \
    (":You are now an IRC operator")

//ERR

//401
# define ERR_NOSUCHNICK(nick) \
    (nick + " :No such nick/channel")

//402
# define ERR_NOSUCHSERVER \
    (SERVER_NAME " :No such server")

//403
# define ERR_NOSUCHCHANNEL(channel) \
    (channel + " :No such channel")

//404
# define ERR_CANNOTSENDTOCHAN(channel) \
    (channel + " :Cannot send to channel")

//405
# define ERR_TOOMANYCHANNELS(channel) \
    (channel + " :You have joined too many channels")

//411
# define ERR_NORECIPIENT \
    (":No recipient given (<command>)")

//412
# define ERR_NOTEXTTOSEND \
    (":No text to send")

//413
# define ERR_NOTOPLEVEL \
    ("<mask> :No toplevel domain specified")

//414
# define ERR_WILDTOPLEVEL \
    ("<mask> :Wildcard in toplevel domain")

//421
# define ERR_UNKNOWNCOMMAND \
    ("<command> :Unknown command")

//422
# define ERR_NOMOTD \
    (":MOTD File is missing")

//431
# define ERR_NONICKNAMEGIVEN \
    (":No nickname given")

//432
# define ERR_ERRONEUSNICKNAME(nick) \
    (nick +" :Erroneous nickname")

//433
# define ERR_NICKNAMEINUSE(nick) \
    (nick + " :Nickname is already in use")

//441
# define ERR_USERNOTINCHANNEL(nick, channel) \
    (nick + " " + channel + " :They aren't on that channel")

//442
# define ERR_NOTONCHANNEL(channel) \
    (channel + " :You're not on that channel")

//443
# define ERR_USERONCHANNEL(user, channel) \
    (user + " " + channel " :is already on channel")

//451
# define ERR_NOTREGISTERED \
    (":You have not registered")

//461
# define ERR_NEEDMOREPARAMS(command) \
    (command + " :Not enough parameters")

//462
# define ERR_ALREADYREGISTRED \
    (":Unauthorized command (already registered)")

//464
# define ERR_PASSWDMISMATCH \
    (":Password incorrect")

//467
# define ERR_KEYSET(channel) \
    (channel + " :Channel key already set")

//471
# define ERR_CHANNELISFULL(channel) \
    (channel + " :Cannot join channel (+l)")

//472
# define ERR_UNKNOWNMODE(channel) \
    ("<char> :is unknown mode char to me for " + channel)

//473
# define ERR_INVITEONLYCHAN(channel) \
    (channel + " :Cannot join channel (+i)")

//474
# define ERR_BANNEDFROMCHAN(channel) \
    (channel + " :Cannot join channel (+b)")

//475
# define ERR_BADCHANNELKEY(channel) \
    (channel + " :Cannot join channel (+k)")

//476
# define ERR_BADCHANMASK(channel) \
    (channel + " :Bad Channel Mask")

//482
# define ERR_CHANOPRIVSNEEDED(channel) \
    (channel + " :You're not channel operator")

//501
# define ERR_UMODEUNKNOWNFLAG \
    (":Unknown MODE flag")

//502
# define ERR_USERSDONTMATCH \
    (":Cannot change mode for other users")

#endif