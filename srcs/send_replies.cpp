#include "irc.hpp"

/**
 * @brief Constructs an IRC server response based on the provided code, server, and user information.
 * 
 * This function takes an integer code, a pointer to a Server object (serv), a pointer to a User object (user),
 * and two strings (args and args2) to generate an appropriate IRC server response. It dynamically builds the response
 * by considering the code, server information, and user details if available.
 * 
 * If a user is provided, the response includes the user's nickname; otherwise, it uses a "*" placeholder.
 * The constructed IRC response is returned as a string.
 * 
 * @param code The numeric code representing the IRC response type.
 * @param serv Pointer to the Server object providing server information.
 * @param user Pointer to the User object providing user information (can be nullptr if not applicable).
 * @param args Additional string argument used in constructing certain responses.
 * @param args2 Additional string argument used in constructing certain responses.
 * @return The generated IRC server response as a string.
 */

std::string sendRplErr(int code, Server *serv, User *user, std::string args, std::string args2)
{
    std::stringstream ss;
    ss << code;
    std::string codestr = ss.str();
    if (code < 10)
        codestr.insert(0, 2, '0');
    else if (code < 100)
        codestr.insert(0, 1, '0');

    std::string ret;

    if (user)
        ret += ":" + serv->getServerName() + " " + codestr + " " + user->getNickname() + " ";
    else
        ret += ":" + serv->getServerName() + " " + codestr + " " + "*" + " ";
    switch (code)
    {
        case 001:
            ret += RPL_WELCOME(user->getNickname(), user->getUsername(), user->getHostname());
			break;
		case 002:
            ret += RPL_YOURHOST;
			break;
        case 003:
            ret += RPL_CREATED;
			break;
        case 004:
            ret += RPL_MYINFO;
			break;
        case 005:
            ret += RPL_BOUNCE(serv->getPort());
			break;
        case 221:
            ret += RPL_UMODEIS(args);
			break;
        case 324:
            ret += RPL_CHANNELMODEIS(args, args2);
			break;
        case 331:
            ret += RPL_NOTOPIC(args);
			break;
        case 332:
            ret += RPL_TOPIC(args, args2);
			break;
        case 341:
            ret += RPL_INVITING(args, args2);
		 	break;
        case 353:
            ret += RPL_NAMREPLY(args, args2);
			break;
        case 366:
            ret += RPL_ENDOFNAMES(args);
			break;
        case 367:
            ret += RPL_BANLIST(args, args2);
			break;
        case 368:
            ret += RPL_ENDOFBANLIST(args);
			break;
        case 381:
            ret += RPL_YOUREOPER;
			break;
        case 401:
            ret += ERR_NOSUCHNICK(args);
			break;
        case 403:
            ret += ERR_NOSUCHCHANNEL(args);
			break;
        case 404:
            ret += ERR_CANNOTSENDTOCHAN(args);
			break;
        case 405:
            ret += ERR_TOOMANYCHANNELS(args);
            break;
        case 431:
            ret += ERR_NONICKNAMEGIVEN;
			break;
        case 432:
            ret += ERR_ERRONEUSNICKNAME(args);
			break;
        case 433:
            ret += ERR_NICKNAMEINUSE(args);
			break;
        case 441:
            ret += ERR_USERNOTINCHANNEL(args, args2);
			break;
        case 442:
            ret += ERR_NOTONCHANNEL(args);
			break;
        case 443:
            ret += ERR_USERALREADYINCHANNEL(args, args2);
			break;
        case 461:
            ret += ERR_NEEDMOREPARAMS(args);
			break;
        case 464:
            ret += ERR_PASSWDMISMATCH;
			break;
        case 471:
            ret += ERR_CHANNELISFULL(args);
			break;
        case 472:
            ret += ERR_UNKNOWNMODE(args, args2);
            break;
        case 473:
            ret += ERR_INVITEONLYCHAN(args);
			break;
        case 474:
            ret += ERR_BANNEDFROMCHAN(args);
			break;
        case 475:
            ret += ERR_BADCHANNELKEY(args);
			break;
        case 481:
            ret += ERR_NOPRIVILEGES;
            break;
        case 482:
            ret += ERR_CHANOPRIVSNEEDED(args);
			break;
        case 484:
            ret += ERR_RESTRICTED;
			break;
        case 501:
            ret += ERR_UMODEUNKNOWNFLAG;
			break;
		default:
			;
    }
    //std::cout << ret << std::endl;
    return ret;
}