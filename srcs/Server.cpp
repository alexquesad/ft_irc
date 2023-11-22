#include "main.hpp"

int client_socket[max_clients];
bool isAlive = true;

Server::Server(const std::string &port, const std::string &password) : _port(port), _password(password), _serverName(), _isRestart(false){
	this->_commandhandler.insert(std::pair<std::string, command>("NICK", &nick));
	this->_commandhandler.insert(std::pair<std::string, command>("JOIN", &join));
	this->_commandhandler.insert(std::pair<std::string, command>("PRIVMSG", &privmsg));
	this->_commandhandler.insert(std::pair<std::string, command>("NOTICE", &privmsg));
	this->_commandhandler.insert(std::pair<std::string, command>("PING", &ping));
	this->_commandhandler.insert(std::pair<std::string, command>("PONG", &pong));
	this->_commandhandler.insert(std::pair<std::string, command>("PART", &part));
	this->_commandhandler.insert(std::pair<std::string, command>("TOPIC", &topic));
	this->_commandhandler.insert(std::pair<std::string, command>("KICK", &kick));
	this->_commandhandler.insert(std::pair<std::string, command>("MODE", &mode));
	this->_commandhandler.insert(std::pair<std::string, command>("OPER", &oper));
	this->_commandhandler.insert(std::pair<std::string, command>("kill", &kill));
	this->_commandhandler.insert(std::pair<std::string, command>("KILL", &kill));
	this->_commandhandler.insert(std::pair<std::string, command>("QUIT", &quit));
	// this->_commandhandler.insert(std::make_pair("RESTART", &restart));
	// this->_commandhandler.insert(std::make_pair("restart", &restart));
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
	_server.sin_addr.s_addr = INADDR_ANY;
	_server.sin_port = htons(static_cast<unsigned short>(std::strtoul(this->_port.c_str(), NULL, 0)));
	_server.sin_family = AF_INET;
	if (bind(sock, (const struct sockaddr*)&_server, sizeof(_server)) < 0)
		throw std::runtime_error("Error binding socket.\n");
	if (listen(sock, 10) < 0)
		throw std::runtime_error("Error listening on socket.\n");
	return sock;
}

void Server::newConnection(void)
{
	socklen_t csize = sizeof(_server);
	if ((this->_sockcom = accept(this->_sockserver, (struct sockaddr *)&_server, &csize)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	//inform user of socket number - used in send and receive commands
	std::cout << "New connection , socket fd is " << this->_sockcom << " , ip is : " << inet_ntoa(_server.sin_addr) << " , port : " <<  ntohs(_server.sin_port) << std::endl;
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
			for (int i = 0;ret[occ + 5 + i] && sep.find(ret[occ + 5 + i]) == std::string::npos; i++)
			{
				pass += ret[occ + 5 + i];
			}
			if (pass.empty())
			{
				sendMessage(send_rpl_err(461, this, NULL, "PASS", ""), this->_sockcom);
				break;
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
			sendMessage(send_rpl_err(461, this, NULL, "PASS", ""), this->_sockcom);
			break;
		}
		if ((occ = ret.find("NICK")) != std::string::npos)
		{
			//nickname
			for (int i = 0;ret[occ + 5 + i] && sep.find(ret[occ + 5 + i]) == std::string::npos; i++)
				nick += ret[occ + 5 + i];
			if (nicknameIsInUse(this, nick))
			{
				sendMessage(send_rpl_err(433, this, NULL, nick, ""), this->_sockcom);
				is_nick_good = false;
				break;
			}		
		}
		if ((occ = ret.find("USER")) != std::string::npos)
		{
			int i = 0;
			//username
			for (;ret[occ + 5 + i] && sep.find(ret[occ + 5 + i]) == std::string::npos; i++)
				user += ret[occ + 5 + i];
			//hostname
			for (;ret[occ + 6 + i] && sep.find(ret[occ + 6 + i]) == std::string::npos; i++)
				host += ret[occ + 6 + i];
			//server_name
			for (;ret[occ + 7 + i] && sep.find(ret[occ + 7 + i]) == std::string::npos; i++)
				server_name += ret[occ + 7 + i];
			//real_name
			for (;ret[occ + 9 + i] && endBuf.find(ret[occ + 9 + i]) == std::string::npos; i++)
				real_name += ret[occ + 9 + i];
		}
	} while (ret.find("USER") == std::string::npos);
	if (is_pass_good == true && _users.size() < 10 && is_nick_good == true)
	{
		this->_serverName = server_name;
		User *new_user = new User(nick, user, host, real_name);
		this->setUsers(this->_sockcom, new_user);
		std::cout << "number of user connected to the server: " << this->_users.size() << std::endl;
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

void    handler(int signum)
{
	(void)signum;
	std::cout << "sig received." << std::endl;
	isAlive = false;
}

void Server::connectToServer()
{
	this->_sockserver = newSocket();
	fd_set readfds;
	int sd, activity, max_sd;
	for (int i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }
	std::cout << "listening..." << std::endl;
	while (this->_isRestart == false && isAlive == true)
	{
    	std::signal(SIGINT, handler);
		// signal(SIGQUIT, SIG_IGN);
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
		//connect new user
		if (FD_ISSET(this->_sockserver, &readfds))
            newConnection();
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
						std::cout << "\033[1;34mRECV RETOUR :\033[0m " << buffer << "]";
						std::string command(buffer);
						std::string buf(buffer);
						int occ = 0;
						for (; command[occ] && sep.find(command[occ]) == std::string::npos; occ++);
						command = command.substr(0, occ);
						std::cout << command << "]" << std::endl;
						if (_commandhandler.find(command) != _commandhandler.end())
							// (_commandhandler[command])(this, buffer, sd);
							(_commandhandler[command])(this, buf, sd);
						break;
					}
				}
			}
		}
	}
	clearAll();
	for (int i = 0; i < max_clients; i++)
	{
		if (client_socket[i] != 0)
		{
			close(client_socket[i]);
			client_socket[i] = 0;
		}
	}
	close(this->_sockserver);
	if (this->_isRestart == true && isAlive == true)
	{
		this->_isRestart = false;
		std::cout << "SERVER RESTARTING..." << std::endl;
		connectToServer();
	}

}

std::string Server::receiveMessage() const
{
	char buffer[1024];
	std::string message;
	memset(buffer, 0, 1024);
	if (recv(this->_sockcom, buffer, 1024, 0) < 0)
		throw std::runtime_error("Error receiving message");
	message = buffer;
	return message;
}

std::string Server::getServerName() const
{
	return this->_serverName;
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
	return this->_server;
}

void Server::setChannels(std::string channel_name, Channel *chan)
{
	this->_channels.insert(std::make_pair(channel_name, chan));
}

void Server::setUsers(int sd, User *user)
{
	this->_users.insert(std::make_pair(sd, user));
}

void Server::setIsRestart()
{
	this->_isRestart = !this->_isRestart;
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