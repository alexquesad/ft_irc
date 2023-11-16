#include "main.hpp"

// void leave_all_channels(Server *serv, int sd)
// {

// }

// void parse_join(char *buffer)
// {

// }

void join(Server *serv, char *buffer, int sd)
{
    int i = 0;
    int j = 0;
    int nb_of_keys = 0;
    std::string buf(buffer);
    for (; buf[5 + i] && buf[5 + i] != ' ' && buf[5 + i] != '\r' && buf[5 + i] != '\n'; i++);
    std::string channels_name(buf.substr(5, i));
    // if (channels_name.compare("0"))
    // {
    //     leave_all_channels(serv, sd);
    //     return;
    // }
    std::cout << "[" << channels_name << "]" << std::endl;
    std::string keys_for_channels = "";
    if (buf[5 + i] && buf[5 + i] == ' ')
    {
        for (; buf[6 + i + j] && buf[6 + i + j] != ' ' && buf[6 + i + j] != '\r' && buf[6 + i + j] != '\n'; j++);
        keys_for_channels = buf.substr(6 + i, j);
        std::cout << "{" << keys_for_channels << "}" << std::endl;
    }
    int nb_of_channels = 1 + std::count(channels_name.begin(), channels_name.end(), ',');
    //IMPORTANT FAIRE UN COMPTEUR DE CLES
    if (!keys_for_channels.empty())
        nb_of_keys = 1 + std::count(keys_for_channels.begin(), keys_for_channels.end(), ','); 
    std::cout << "(" << nb_of_channels << ")" << std::endl;
    if (nb_of_keys != 0)
        std::cout << "(" << nb_of_keys << ")" << std::endl;
    if (serv->getChannels().find(channels_name) == serv->getChannels().end()) // si le channel n'existe pas on le cree et on l'ajoute a notre map de channel
	{
		Channel *chan = new Channel(channels_name);
    	serv->setChannels(channels_name, chan);
	}
	//On ajoute le client a notre serveur
    if (serv->getChannels().find(channels_name)->second->getUsersnumber() == 0)
        serv->getChannels().find(channels_name)->second->addOper(sd, serv->getUsers().find(sd)->second);
    else
	    serv->getChannels().find(channels_name)->second->addUser(sd, serv->getUsers().find(sd)->second);
    serv->getUsers().find(sd)->second->add_channel(channels_name);
    std::string user_answer = user_output(serv->getUsers().find(sd)->second);
    user_answer += buffer;
    sendEveryone(user_answer, serv->getChannels().find(channels_name)->second);
    sendMessage(send_rpl_err(332, serv, serv->getUsers().find(sd)->second, channels_name, ""), sd);
    std::string list_of_user = serv->getChannels().find(channels_name)->second->get_list_of_user_in_chan();
    std::cout << list_of_user << std::endl;
    sendMessage(send_rpl_err(353, serv, serv->getUsers().find(sd)->second, channels_name, list_of_user), sd);
    sendMessage(send_rpl_err(366, serv, serv->getUsers().find(sd)->second, channels_name, ""), sd);
}