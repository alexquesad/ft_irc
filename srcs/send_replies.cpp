#include "main.hpp"

std::string send_rpl_err(int code, Server *serv, User *user, std::string args, std::string args2)
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
        ret += ":" + serv->getServername() + " " + codestr + " " + user->getNickname() + " ";
    else
        ret += ":" + serv->getServername() + " " + codestr + " " + "*" + " ";
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
        // case 301:
        //     ret += RPL_AWAY(args);
        //     break;
        // case 315:
        //     ret += RPL_ENDOFWHO(user->getUsername());
		// 	break;
        // case 322:
        //     ret += RPL_LIST();
		// 	break;
        // case 323:
        //     ret += RPL_LISTEND;
		// 	break;
        // case 324:
        //     ret += RPL_CHANNELMODEIS;
		// 	break;
        case 331:
            ret += RPL_NOTOPIC(args);
			break;
        case 332:
            ret += RPL_TOPIC(args, args2);
			break;
        // case 341:
        //     ret += RPL_INVITING(user->getNickname());
		// 	break;
        // case 351:
        //     ret += RPL_VERSION;
		// 	break;
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
        // case 372:
        //     ret += RPL_MOTD;
		// 	break;
        // case 375:
        //     ret += RPL_MOTDSTART;
		// 	break;
        // case 376:
        //     ret += RPL_ENDOFMOTD;
		// 	break;
        // case 381:
        //     ret += RPL_YOUREOPER;
		// 	break;
        // case 401:
        //     ret += ERR_NOSUCHNICK(user->getNickname());
		// 	break;
        // case 402:
        //     ret += ERR_NOSUCHSERVER;
		// 	break;
        case 403:
            ret += ERR_NOSUCHCHANNEL(args);
			break;
        case 404:
            ret += ERR_CANNOTSENDTOCHAN(args);
			break;
        case 405:
            ret += ERR_TOOMANYCHANNELS(args);
            break;
        // case 411:
        //     ret += ERR_NORECIPIENT;
		// 	break;
        // case 412:
        //     ret += ERR_NOTEXTTOSEND;
		// 	break;
        // case 413:
        //     ret += ERR_NOTOPLEVEL;
		// 	break;
        // case 414:
        //     ret += ERR_WILDTOPLEVEL;
		// 	break;
        // case 421:
        //     ret += ERR_UNKNOWNCOMMAND;
		// 	break;
        // case 422:
        //     ret += ERR_NOMOTD;
		// 	break;
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
        // case 443:
        //     ret += ERR_USERONCHANNEL(user->getUsername());
		// 	break;
        // case 451:
        //     ret += ERR_NOTREGISTERED;
		// 	break;
        case 461:
            ret += ERR_NEEDMOREPARAMS(args);
			break;
        // case 462:
        //     ret += ERR_ALREADYREGISTRED;
		// 	break;
        // case 464:
        //     ret += ERR_PASSWDMISMATCH;
		// 	break;
        // case 467:
        //     ret += ERR_KEYSET;
		// 	break;
        // case 471:
        //     ret += ERR_CHANNELISFULL;
		// 	break;
        // case 472:
        //     ret += ERR_UNKNOWNMODE;
		// 	break;
        // case 473:
        //     ret += ERR_INVITEONLYCHAN;
		// 	break;
        // case 474:
        //     ret += ERR_BANNEDFROMCHAN;
		// 	break;
        // case 475:
        //     ret += ERR_BADCHANNELKEY;
		// 	break;
        // case 476:
        //     ret += ERR_BADCHANMASK;
		// 	break;
        case 482:
            ret += ERR_CHANOPRIVSNEEDED(args);
			break;
        case 501:
            ret += ERR_UMODEUNKNOWNFLAG;
			break;
        // case 502:
        //     ret += ERR_USERSDONTMATCH;
		// 	break;
		default:
			;
    }
    std::cout << ret << std::endl;
    return ret;
}