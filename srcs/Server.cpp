#include "Server.hpp"

#define max_clients 10

Server::Server(const std::string &port, const std::string &password) : _port(port), _password(password), _server_name(){
	this->_sockserver = newSocket();
	this->_commandhandler.insert(std::make_pair("NICK", &nick));
	this->_commandhandler.insert(std::make_pair("JOIN", &join));
	this->_commandhandler.insert(std::make_pair("PRIVMSG", &privmsg));
	// this->_commandhandler.insert(std::make_pair("PING", &ping));
	// this->_commandhandler.insert(std::make_pair("PONG", &pong));
	// this->_commandhandler.insert(std::make_pair("userhost", &user));
}

Server::~Server(){close(this->_sockserver);}

int Server::newSocket()
{   
    // Create a new socket using IPv4 addressing and TCP protocol
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		throw std::runtime_error("Error creating socket.\n");
	int tmp = 1;
    // Set socket options to allow reuse of the address
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp)))
		throw std::runtime_error("Error while setting up socket.\n");
    // Set the socket to non-blocking mode
	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error while setting socket NON-BLOCKING mode.\n");
    // Set up the server address structure
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(static_cast<unsigned short>(std::strtoul(this->_port.c_str(), NULL, 0)));
	server.sin_family = AF_INET;
    // Bind the socket to the specified address and port
	if (bind(sock, (const struct sockaddr*)&server, sizeof(server)) < 0)
		throw std::runtime_error("Error binding socket.\n");
    // Start listening on the socket with a backlog queue of 10 connections
	if (listen(sock, 10) < 0)
		throw std::runtime_error("Error listening on socket.\n");
    // Print a message indicating that the server is ready to accept connections
	std::cout << "Ready to accept connections..." << std::endl;
	return sock;
}

void Server::connectToServer()
{
	fd_set readfds;
	int client_socket[max_clients], sd, activity, max_sd;
	for (int i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }
	socklen_t csize = sizeof(server);
	std::cout << "listening..." << std::endl;
	while (1)
	{
		//clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(this->_sockserver, &readfds);
        max_sd = this->_sockserver;

        //add client sockets to set
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
            if ((this->_sockcom = accept(this->_sockserver, (struct sockaddr *)&server, &csize)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            //inform user of socket number - used in send and receive commands
            std::cout << "New connection , socket fd is" << this->_sockcom << " , ip is : " << inet_ntoa(server.sin_addr) << " , port : " <<  ntohs(server.sin_port) << std::endl;
            //send new connection greeting message

            // Initialize variables for handling user authentication
			std::string ret;
			size_t occ;
			bool is_pass_good = false;
			std::string nick, user, host, server_name, real_name, pass, buffer;
            // Loop to handle user authentication 
			do
			{   
                // Receive a message from the new connection
				ret = this->receiveMessage();
                // Check for the presence of the "PASS" command in the received message
				if ((occ = ret.find("PASS")) != std::string::npos)
				{   
                    // Extract the password from the message
					for (int i = 0;ret[occ + 5 + i] && ret[occ + 5 + i] != ' ' && ret[occ + 5 + i] != '\n' && ret[occ + 5 + i] != '\r'; i++)
					{
						pass += ret[occ + 5 + i];
					}
                    // Check if the password is correct
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
					// Extract the Nickname from the message
					for (int i = 0;ret[occ + 5 + i] && ret[occ + 5 + i] != ' ' && ret[occ + 5 + i] != '\n' && ret[occ + 5 + i] != '\r'; i++)
					{
						nick += ret[occ + 5 + i];
					}
					std::cout << "nickname: " << nick << std::endl;
				}
				if ((occ = ret.find("USER")) != std::string::npos)
				{
					int i = 0;
					// Extract the username from the message
					for (;ret[occ + 5 + i] && ret[occ + 5 + i] != ' ' && ret[occ + 5 + i] != '\n' && ret[occ + 5 + i] != '\r'; i++)
					{
						user += ret[occ + 5 + i];
					}
					std::cout << "username: " << user << std::endl;
					//hostname
					for (;ret[occ + 6 + i] && ret[occ + 6 + i] != ' ' && ret[occ + 6 + i] != '\n' && ret[occ + 6 + i] != '\r'; i++)
					{
						host += ret[occ + 6 + i];
					}
					std::cout << "hostname: " << host << std::endl;
					//server_name
					for (;ret[occ + 7 + i] && ret[occ + 7 + i] != ' ' && ret[occ + 7 + i] != '\n' && ret[occ + 7 + i] != '\r'; i++)
					{
						server_name += ret[occ + 7 + i];
					}
					std::cout << "server_name: " << server_name << std::endl;

					//real_name
					for (;ret[occ + 9 + i] && ret[occ + 9 + i] != '\n' && ret[occ + 9 + i] != '\r'; i++)
					{
						real_name += ret[occ + 9 + i];
					}
					std::cout << "real_name: " << real_name << std::endl;
				}
			}
			while (ret.find("USER") == std::string::npos);
			if (is_pass_good == true && _users.size() < 10)
			{   
                // Set the server name and create a new user
				this->_server_name = server_name;
				Client new_user(nick, user, host, real_name);
				this->_users.insert(std::make_pair(this->_sockcom, &new_user));
                // Display the number of users connected to the server
				std::cout << "number of user connected to the server: " << this->_users.size() << std::endl;
                // Send welcome messages to the new user
				// sendMessage("001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host);
				sendMessage(send_rpl_err(this, new_user, 001), this->_sockcom);
				sendMessage(send_rpl_err(this, new_user, 002), this->_sockcom);
				sendMessage(send_rpl_err(this, new_user, 003), this->_sockcom);
				sendMessage(send_rpl_err(this, new_user, 004), this->_sockcom);

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
			else if (is_pass_good == true)
				sendMessage("005 " + nick + " :Try server " + server_name + ", port 6667", this->_sockcom);
		}
		else
		{   
            // Loop through existing clients to check for activity
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
                        // Check if the message is a QUIT command
						if (strcmp(buffer, "QUIT :leaving\r\n") == 0)
						{
							//Somebody disconnected , get his details and print
							getpeername(sd , (struct sockaddr*)&server, &csize);
							//remove user to the map but how to find the nick ? Maybe replace the nick by the fd in the map
							for (std::map<int, Client*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
							{
								if ((*it).first == sd)
								{
									this->_users.erase(it);
									break;
								}
							}
							std::cout << "Host disconnected , ip " << inet_ntoa(server.sin_addr) << " , port " << ntohs(server.sin_port) << " , number of users: " <<  this->_users.size() << std::endl;
							//Close the socket and mark as 0 in list for reuse
							close( sd );
							
							client_socket[i] = 0;
						}

						//Echo back the message that came in
						else
						{
							std::string command(buffer);
							command = command.substr(0, command.find(' '));
							std::cout << "MY NICK: " << command << std::endl;
							if (_commandhandler.find(command) != _commandhandler.end())
								(_commandhandler[command])(this, buffer,sd);
							break;
						}
					}
				}
			}
		}
	}
    // Close the master socket
	close(this->_sockserver);
}

void Server::sendMessage(std::string message, int sd) const
{
	message += "\r\n";
	if (send(sd, message.c_str(), message.length(), 0) < 0)
		throw std::runtime_error("Error sending message.");
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

	std::cout << "BUFFER : " << buffer << "]\n";
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

std::map<std::string, Channel *> Server::getChannels() const
{
	return this->_channels;
}

std::map<int, Client *> Server::getUsers() const
{
	return this->_users;
}