#include "main.hpp"

int client_socket[max_clients];

Server::Server(const std::string &port, const std::string &password) : _port(port), _password(password), _server_name(){
	this->_commandhandler.insert(std::make_pair("NICK", &nick));
	this->_commandhandler.insert(std::make_pair("JOIN", &join));
	this->_commandhandler.insert(std::make_pair("PRIVMSG", &privmsg));
	this->_commandhandler.insert(std::make_pair("NOTICE", &privmsg));
	this->_commandhandler.insert(std::make_pair("PING", &ping));
	this->_commandhandler.insert(std::make_pair("PONG", &pong));
	this->_commandhandler.insert(std::make_pair("PART", &part));
	this->_commandhandler.insert(std::make_pair("TOPIC", &topic));
	this->_commandhandler.insert(std::make_pair("KICK", &kick));
	this->_commandhandler.insert(std::make_pair("MODE", &mode));
	this->_commandhandler.insert(std::make_pair("OPER", &oper));
	this->_commandhandler.insert(std::make_pair("kill", &kill));
}

Server::~Server(){close(this->_sockserver);}

int Server::newSocket()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		throw std::runtime_error("Error creating socket.\n");
	int tmp = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp)))
		throw std::runtime_error("Error while setting up socket.\n");
	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error while setting socket NON-BLOCKING mode.\n");
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(static_cast<unsigned short>(std::strtoul(this->_port.c_str(), NULL, 0)));
	server.sin_family = AF_INET;
	if (bind(sock, (const struct sockaddr*)&server, sizeof(server)) < 0)
		throw std::runtime_error("Error binding socket.\n");
	if (listen(sock, 10) < 0)
		throw std::runtime_error("Error listening on socket.\n");
	std::cout << "accept" << std::endl;
	return sock;
}

void Server::new_connection(void)
{
	socklen_t csize = sizeof(server);
	if ((this->_sockcom = accept(this->_sockserver, (struct sockaddr *)&server, &csize)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	//inform user of socket number - used in send and receive commands
	std::cout << "New connection , socket fd is " << this->_sockcom << " , ip is : " << inet_ntoa(server.sin_addr) << " , port : " <<  ntohs(server.sin_port) << std::endl;
	std::string ret;
	//send new connection greeting message
	size_t occ;
	bool is_pass_good = false;
	bool is_nick_good = true;
	std::string nick, user, host, server_name, real_name, pass, buffer;
	do
	{
		ret = this->receiveMessage();
		if ((occ = ret.find("PASS")) != std::string::npos)
		{
			for (int i = 0;ret[occ + 5 + i] && ret[occ + 5 + i] != ' ' && ret[occ + 5 + i] != '\n' && ret[occ + 5 + i] != '\r'; i++)
			{
				pass += ret[occ + 5 + i];
			}
			if (pass.compare(this->_password) != 0)
			{
				is_pass_good = false;
				sendMessage("WRONG PASSWORD", this->_sockcom);
				break;
			}
			is_pass_good = true;
		}
		if ((ret.find("CAP LS") == std::string::npos && is_pass_good == false) || (ret.find("CAP LS") != std::string::npos && ret.find("NICK") != std::string::npos && is_pass_good == false))
		{
			sendMessage("PASS :Not enough parameters", this->_sockcom);
			break;
		}
		// split buffer to stock informations : std::string ret(split(buffer.c_str(), " "))
		if ((occ = ret.find("NICK")) != std::string::npos)
		{
			//nickname
			for (int i = 0;ret[occ + 5 + i] && ret[occ + 5 + i] != ' ' && ret[occ + 5 + i] != '\n' && ret[occ + 5 + i] != '\r'; i++)
				nick += ret[occ + 5 + i];
			if (nickname_is_in_use(this, nick))
			{
				sendMessage(nick + " :Nickname is already in use", this->_sockcom);
				is_nick_good = false;
				break;
			}		
		}
		if ((occ = ret.find("USER")) != std::string::npos)
		{
			int i = 0;
			//username
			for (;ret[occ + 5 + i] && ret[occ + 5 + i] != ' ' && ret[occ + 5 + i] != '\n' && ret[occ + 5 + i] != '\r'; i++)
				user += ret[occ + 5 + i];
			//hostname
			for (;ret[occ + 6 + i] && ret[occ + 6 + i] != ' ' && ret[occ + 6 + i] != '\n' && ret[occ + 6 + i] != '\r'; i++)
				host += ret[occ + 6 + i];
			//server_name
			for (;ret[occ + 7 + i] && ret[occ + 7 + i] != ' ' && ret[occ + 7 + i] != '\n' && ret[occ + 7 + i] != '\r'; i++)
				server_name += ret[occ + 7 + i];

			//real_name
			for (;ret[occ + 9 + i] && ret[occ + 9 + i] != '\n' && ret[occ + 9 + i] != '\r'; i++)
				real_name += ret[occ + 9 + i];
		}
	} while (ret.find("USER") == std::string::npos);
	if (is_pass_good == true && _users.size() < 10 && is_nick_good == true)
	{
		this->_server_name = server_name;
		User *new_user = new User(nick, user, host, real_name);
		this->setUsers(this->_sockcom, new_user);
		std::cout << "number of user connected to the server: " << this->_users.size() << std::endl;
		// sendMessage("001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host);
		sendMessage(send_rpl_err(001, this, new_user, "", ""), this->_sockcom);
		sendMessage(send_rpl_err(002, this, new_user, "", ""), this->_sockcom);
		sendMessage(send_rpl_err(003, this, new_user, "", ""), this->_sockcom);
		sendMessage(send_rpl_err(004, this, new_user, "", ""), this->_sockcom);
		//add new socket to array of sockets
		for (int i = 0; i < max_clients; i++)
		{
			//if position is empty
			if( client_socket[i] == 0 )
			{
				client_socket[i] = this->_sockcom;
				std::cout << "Adding to list of sockets as " << i << std::endl;
				break;
			}
		}
	}
	else if (is_pass_good == true && is_nick_good == true)
		sendMessage(send_rpl_err(005, this, NULL, nick, ""), this->_sockcom);
}

void Server::connectToServer()
{
	this->_sockserver = newSocket();
	fd_set readfds;
	int sd, activity, max_sd;
	int loop = 1;
	bool is_restart = false;
	for (int i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }
	socklen_t csize = sizeof(server);
	std::cout << "listening..." << std::endl;
	while (loop == 1)
	{
		//clear the socket set
        FD_ZERO(&readfds);
        //add master socket to set
        FD_SET(this->_sockserver, &readfds);
        max_sd = this->_sockserver;
        //add child sockets to set
        for ( int i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = client_socket[i];
            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);
            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }
        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
        if ((activity < 0) && (errno!=EINTR))
            std::cerr << ("select error") << std::endl;
		if (FD_ISSET(this->_sockserver, &readfds)) // connect new user
        {
            new_connection();
		}
		else
		{
			for (int i = 0; i < max_clients; i++)
			{
				sd = client_socket[i];
				if (FD_ISSET( sd , &readfds))
				{
					char buffer[1025];
					memset(buffer, 0, 1025);
					//Check if it was for closing , and also read the
					//incoming message
					if (recv( sd , buffer, 1024, 0) > 0)
					{
						std::cout << "\033[1;34mRECV RETOUR :\033[0m " << buffer;
						if (strcmp(buffer, "QUIT :leaving\r\n") == 0)
						{
							//Somebody disconnected , get his details and print
							getpeername(sd , (struct sockaddr*)&server, &csize);
							disconnectUser(this, sd);
							//Close the socket and mark as 0 in list for reuse
							// close(sd);
							client_socket[i] = 0;
						}
						//Echo back the message that came in
						else
						{
							std::string command(buffer);
							command = command.substr(0, command.find(' '));
							if (command.compare(0, 7, "restart") == 0)
							{
								if (this->_users.find(sd)->second->getMode().find('o') == std::string::npos)
									sendMessage(send_rpl_err(481, this, this->_users.find(sd)->second, "", ""), sd);
								else
								{
									loop = 0;
									is_restart = true;
									for (int u = 0; u < max_clients;u++)
									{
										if (client_socket[u] != 0)
										{
											close(client_socket[u]);
											client_socket[u] = 0;
										}

									}
									clearAll();
								}
							}
							else if (_commandhandler.find(command) != _commandhandler.end())
							{
								(_commandhandler[command])(this, buffer, sd);
							}
							break;
						}
					}
				}
			}
		}
	}
	close(this->_sockserver);
	if (is_restart == true)
	{
		is_restart = false;
		std::cout << "SERVER RESTARTING..." << std::endl;
		connectToServer();
	}
}

std::string Server::receiveMessage() const
{
	char buffer[1024];
	std::string message;
	memset(buffer, 0, 1024);
	if (recv(this->_sockcom, buffer, 1024, 0) < 0){
		std::cout << strerror(errno) << std::endl;
		throw std::runtime_error("Error receiving message");
	}
	// std::cout << "BUFFER : " << buffer << "]\n";
	message = buffer;
	return message;
}

std::string Server::getServername() const
{
	return this->_server_name;
}

std::string Server::getPort() const
{
	return this->_port;
}

std::map<std::string, Channel*> & Server::getChannels()
{
	return this->_channels;
}

std::map<int, User*> & Server::getUsers()
{
	return this->_users;
}

struct sockaddr_in Server::getServer()
{
	return this->server;
}

void Server::setChannels(std::string channel_name, Channel *chan)
{
	this->_channels.insert(std::make_pair(channel_name, chan));
}

void Server::setUsers(int sd, User *user)
{
	this->_users.insert(std::make_pair(sd, user));
}

int Server::searchUserByNickname(std::string nickname)
{
    for (std::map<int, User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
    {
        if (nickname.compare(it->second->getNickname()) == 0)
            return it->first;
    }
    return (-1);
}

std::ostream	&operator<<(std::ostream &stdout, std::map<std::string, Channel*> &channels)
{
	int i = 0;
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); it++, i++)
	{
		stdout << "Channel " << i << " name is " << it->first << "." << std::endl;
	}
	return (stdout);
}

std::ostream	&operator<<(std::ostream &stdout, std::map<int, User*> &users)
{
	int i = 0;
	for (std::map<int, User*>::iterator it = users.begin(); it != users.end(); it++, i++)
	{
		stdout << "User " << i << " nick is " << it->second->getNickname() << "." << std::endl;
	}
	return (stdout);
}

std::ostream	&operator<<(std::ostream &stdout, User &user)
{
    int i = 0;
    std::set<std::string> channels = user.getChannels();
    for (std::set<std::string>::iterator it = channels.begin(); it != channels.end(); it++, i++)
    {
        stdout << "Channel " << i << " of User " << user.getNickname() << " is called " << *it << std::endl;
    }
    return (stdout);
}

void Server::clearAll()
{
	for (std::map<int, User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
		delete it->second;
	this->_users.clear();
	for (std::map<std::string, Channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++)
		delete it->second;
	this->_channels.clear();
}