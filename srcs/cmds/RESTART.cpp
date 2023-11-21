#include "main.hpp"

void restart(Server *serv, char *buffer, int sd)
{
	(void)buffer;
	if (FIND_USER(sd)->getMode().find('o') == std::string::npos)
        sendMessage(send_rpl_err(481, serv, FIND_USER(sd), "", ""), sd);
	else
		serv->setIsRestart();
}