#include "irc.hpp"

int clientSocket[maxClients];
bool serverActive = true;

/**
 * @brief Constructs a Server object with the specified port and password.
 * 
 * This constructor initializes a Server object with the given port number and  password.
 * It also sets up the command handler by mapping IRC commands to their corresponding member functions.
 * The Server object is initialized with default values for server name and restart status.
 * 
 * @param port The port number on which the server will run.
 * @param password The password required for server access.
 * 
 * Command handler setup:
 * - Various IRC commands are mapped to their corresponding member functions for handling.
 *   (e.g., NICK, JOIN, PRIVMSG, PING, PONG, PART, TOPIC, KICK, MODE, OPER, KILL, QUIT, RESTART)
 *   The mapping allows for easy command processing during server operation.
 * 
 * Server object state:
 * - _serverName is initialized to an empty string.
 * - _isRestart is initialized to false.
 */
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
    this->_commandhandler.insert(std::pair<std::string, command>("INVITE", &invite));
	this->_commandhandler.insert(std::pair<std::string, command>("RESTART", &restart));
	this->_commandhandler.insert(std::pair<std::string, command>("restart", &restart));
}

Server::~Server()
{
	this->_commandhandler.clear();
}

void sendMOTD(int sd)
{
	sendMessage("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⢰⣿⣿⣿⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣀⣀⣾⣿⣿⣿⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⡏⠉⠛⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⣿⠀⠀⠀⠈⠛⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠛⠉⠁⠀⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⣿⣧⡀⠀⠀⠀⠀⠙⠿⠿⠿⠻⠿⠿⠟⠿⠛⠉⠀⠀⠀⠀⠀⣸⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⣿⣿⣷⣄⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⣿⣿⣿⣿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⣴⣿⣿⣿⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⠀⢰⣹⡆⠀⠀⠀⠀⠀⠀⣭⣷⠀⠀⠀⠸⣿⣿⣿⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⠈⠉⠀⠀⠤⠄⠀⠀⠀⠉⠁⠀⠀⠀⠀⢿⣿⣿⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⣿⣿⣿⢾⣿⣷⠀⠀⠀⠀⡠⠤⢄⠀⠀⠀⠠⣿⣿⣷⠀⢸⣿⣿⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⣿⣿⣿⡀⠉⠀⠀⠀⠀⠀⢄⠀⢀⠀⠀⠀⠀⠉⠉⠁⠀⠀⣿⣿⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣿", sd);
	sendMessage("⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿", sd);
	sendMessage("", sd);

}

void    handler(int signum)
{
	(void)signum;
	serverActive = false;
}

/**
 * @brief NewServerSocket
 *
 * Configures the server socket with necessary options and binds it to the server address.
 * Sets the socket to non-blocking mode and listens for incoming connections.
 *
 * @return The configured server socket.
 * @throws std::runtime_error if an error occurs during configuration or binding.
 */

int Server::NewServerSocket()
{
    // Creating a new socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // Checking for errors in socket creation
    if (sock < 0)
        throw std::runtime_error("Error creating socket.\n");

    // Setting socket options to allow reuse of the address
    int tmp = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp)))
        throw std::runtime_error("Error while setting up socket.\n");

    // Setting the socket to non-blocking mode
    if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("Error while setting socket NON-BLOCKING mode.\n");

    // Configuring the server address structure
    _server.sin_addr.s_addr = INADDR_ANY;
    _server.sin_port = htons(static_cast<unsigned short>(std::strtoul(this->_port.c_str(), NULL, 0)));
    _server.sin_family = AF_INET;

    // Binding the socket to the server address
    if (bind(sock, (const struct sockaddr*)&_server, sizeof(_server)) < 0)
        throw std::runtime_error("Error binding socket.\n");

    // Setting the socket to listen for incoming connections
    if (listen(sock, 10) < 0)
        throw std::runtime_error("Error listening on socket.\n");

    // Returning the created socket
    return sock;
}

/**
 * @brief HandleNewConnection
 *
 * Accepts a new connection, processes the initial messages from the client, and establishes a new user session.
 * Validates the received password, nickname, and user information, and adds the new user to the server.
 *
 * This function performs the following steps:
 * 1. Accepts a new connection and obtains a new socket descriptor.
 * 2. Displays information about the new connection.
 * 3. Initializes variables for processing incoming messages.
 * 4. Receives the first message from the new connection.
 * 5. Processes the received message based on specific conditions.
 * 6. Validates the received password and sets the 'isPassGood' flag.
 * 7. Validates the received nickname and sets the 'isNickGood' flag.
 * 8. Validates the received user information and sets the 'isUserGood' flag.
 * 9. Continues processing based on the validity of the password, nickname, user, and server flags.
 * 10. If conditions are met, sets the server name, creates a new user, and adds the user to the server.
 * 11. Sends initial messages to the new user.
 * 12. Adds the new socket to the array of client sockets.
 * 13. Handles cases where the server is full or errors occur during the process.
 *
 * @throws std::runtime_error if an error occurs during the acceptance of the new connection or message reception.
 */

void Server::handleNewConnection(void)
{
    // Set up variables for handling the new connection
    socklen_t csize = sizeof(_server);

    // Accept the incoming connection, obtain a new socket descriptor
    if ((this->_newClientSocket = accept(this->_sockserver, (struct sockaddr *)&_server, &csize)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    // Display information about the new connection
    std::cout << "New connection , socket fd is " << this->_newClientSocket << " , ip is : " << inet_ntoa(_server.sin_addr) << " , port : " << ntohs(_server.sin_port) << std::endl;

    // Initialize variables for processing incoming messages
    std::string ret;
    size_t occ;
    size_t firstOcc;
    bool isPassGood = false, isNickGood = false, isUserGood = false;
    std::string nick = "", user = "", host = "", serverName = "", realName = "", pass = "";

    // Receive the first message from the new connection
    ret = this->receiveMessage(this->_newClientSocket);

    // Process the received message based on specific conditions
    if (((ret.find("CAP LS") != std::string::npos && ret.find("PASS ") == std::string::npos) || (ret.find("CAP LS") != std::string::npos && ret.find("PASS ") == std::string::npos && ret.find("NICK ") != std::string::npos)) && ret.find("USER ") == std::string::npos)
        ret = this->receiveMessage(this->_newClientSocket);

    // Check for the presence of "PASS" in the received message
    if ((occ = ret.find("PASS ")) != std::string::npos)
    {
        // Extract the password from the message
        if ((firstOcc = ret.find_first_not_of(sep, occ + 5)) == std::string::npos)
        {
            // Handle the case where the password is missing
            sendMessage(sendRplErr(461, this, NULL, "PASS", ""), this->_newClientSocket);
            close(this->_newClientSocket);
        }
        else
        {
            // Construct the password by iterating through the characters
            for (int i = 0; ret[firstOcc + i] && sep.find(ret[firstOcc + i]) == std::string::npos; i++)
                pass += ret[firstOcc + i];
            // Check the validity of the password
            if (pass.empty())
            {
                // Handle the case where the password is empty
                sendMessage(sendRplErr(461, this, NULL, "PASS", ""), this->_newClientSocket);
                close(this->_newClientSocket);
            }
            else if (pass.compare(this->_password) != 0)
            {
                // Handle the case of an incorrect password
                sendMessage("WRONG PASSWORD", this->_newClientSocket);
                close(this->_newClientSocket);
            }
            else
                // Set the flag indicating a good password
                isPassGood = true;
        }
    }
    else
    {
        // Handle the case where "PASS" is not found in the message
        sendMessage("You need to enter a pass!", this->_newClientSocket);
        close(this->_newClientSocket);
    }
    // Continue processing based on the validity of the password
    if (isPassGood == true)
    {
        // Check for the presence of "NICK" in the received message
        if (ret.find("NICK ") == std::string::npos)
            ret = this->receiveMessage(this->_newClientSocket);
        // Process the message if "NICK" is found
        if ((occ = ret.find("NICK ")) != std::string::npos)
        {
            // Extract the nickname from the message
            if ((firstOcc = ret.find_first_not_of(sep, occ + 5)) == std::string::npos)
            {
                // Handle the case where the nickname is missing
                sendMessage(sendRplErr(432, this, NULL, nick, ""), this->_newClientSocket);
                close(this->_newClientSocket);
            }
            else
            {
                // Construct the nickname and ensure its validity
                nick = ret.substr(firstOcc, ret.find_first_of(endBuf, firstOcc) - firstOcc);
                nick = nick.substr(0, nick.find_last_not_of(sep, nick.size()) + 1);
                if (!nicknameIsValid(nick))
                {
                    // Handle the case of an invalid nickname
                    sendMessage(sendRplErr(432, this, NULL, nick, ""), this->_newClientSocket);
                    close(this->_newClientSocket);
                }
                else if (nicknameIsInUse(this, nick))
                {
                    // Handle the case of a nickname already in use
                    sendMessage(sendRplErr(433, this, NULL, nick, ""), this->_newClientSocket);
                    sendMessage("Please try reconnecting with an available nickname.", this->_newClientSocket);
                    close(this->_newClientSocket);
                }
                else
                    // Set the flag indicating a good nickname
                    isNickGood = true;
            }
        }
        else
        {
            // Handle the case where "NICK" is not found in the message
            sendMessage("You have to enter a nickname\nUsage: NICK [nickname]", this->_newClientSocket);
            close(this->_newClientSocket);
        }
        // Continue processing based on the validity of the nickname and user flag
        if (isUserGood == false && isNickGood == true)
        {
            // Check for the presence of "USER" in the received message
            if (ret.find("USER ") == std::string::npos)
                ret = this->receiveMessage(this->_newClientSocket);
            // Process the message if "USER" is found
            if ((occ = ret.find("USER ")) != std::string::npos)
            {
                int i = 0;
                // Extract the username from the message
                if ((firstOcc = ret.find_first_not_of(sep, occ + 5)) == std::string::npos)
                    sendMessage(sendRplErr(461, this, NULL, "USER", ""), this->_newClientSocket);
                else
                {
                    user = ret.substr(firstOcc, (i = ret.find_first_of(sep, firstOcc)) - firstOcc);

                    // Extract the hostname from the message
                    if ((firstOcc = ret.find_first_not_of(sep, i)) == std::string::npos)
                        sendMessage(sendRplErr(461, this, NULL, "USER", ""), this->_newClientSocket);
                    else
                    {
                        host = ret.substr(firstOcc, (i = ret.find_first_of(sep, firstOcc)) - firstOcc);

                        // Extract the serverName from the message
                        if ((firstOcc = ret.find_first_not_of(sep, i)) == std::string::npos)
                            sendMessage(sendRplErr(461, this, NULL, "USER", ""), this->_newClientSocket);
                        else
                        {
                            serverName = ret.substr(firstOcc, (i = ret.find_first_of(sep, firstOcc)) - firstOcc);

                            // Extract the realName from the message
                            if ((firstOcc = ret.find_first_not_of(sep, i)) == std::string::npos)
                                sendMessage(sendRplErr(461, this, NULL, "USER", ""), this->_newClientSocket);
                            else
                            {
                                realName = ret.substr(firstOcc, (i = ret.find_first_of(sep, firstOcc)) - firstOcc);
                                realName = realName.substr(0, realName.find_last_not_of(sep, realName.size()) + 1);
                            }
                        }
                    }
                }
                // Set the user flag based on the validity of extracted information
                if (!(user.empty() || host.empty() || serverName.empty() || realName.empty()))
                    isUserGood = true;
            }
        }
        // Handle the case where "USER" is not found and user flag is not set
        if (isUserGood == false && isNickGood == true)
        {
            sendMessage("Usage: USER [username] [hostname] [serverName] [realName]", this->_newClientSocket);
            close(this->_newClientSocket);
        }
    }
    // Continue processing based on the validity of the password, nickname, user, and server flags
    if (isPassGood == true && _users.size() < 10 && isNickGood == true && isUserGood == true && serverActive == true)
    {
        // Set the server name, create a new user, and add the user to the server
        this->_serverName = serverName;
        User *newUser = new User(nick, user, host, realName);
        this->setUsers(this->_newClientSocket, newUser);
        std::cout << "Number of users connected on the server: " << this->_users.size() << std::endl;
        
        // Send initial messages to the new user
        sendMessage(sendRplErr(001, this, newUser, "", ""), this->_newClientSocket);
        sendMessage(sendRplErr(002, this, newUser, "", ""), this->_newClientSocket);
        sendMessage(sendRplErr(003, this, newUser, "", ""), this->_newClientSocket);
        sendMessage(sendRplErr(004, this, newUser, "", ""), this->_newClientSocket);
        sendMOTD(this->_newClientSocket);

        // Add the new socket to the array of client sockets
        for (int i = 0; i < maxClients; i++)
        {
            if (clientSocket[i] == 0)
            {
                clientSocket[i] = this->_newClientSocket;
                break;
            }
        }
    }
    // Handle the case where the server is full
    else if (isPassGood == true && isNickGood == true && serverActive == true && isUserGood == true)
        sendMessage(sendRplErr(005, this, NULL, nick, ""), this->_newClientSocket);
}

/**
 * @brief Run the IRC server, managing new and existing client connections.
 *
 * This function initializes and monitors sockets for both server and client connections.
 * It uses the select system call to efficiently handle multiple sockets concurrently.
 * The server continuously checks for new connections and processes commands from existing clients.
 * The main loop runs until a server restart is requested or the server is deactivated.
 *
 * @details
 * - Creates a new server socket and sets up variables for socket activity monitoring.
 * - Initializes an array to store client sockets.
 * - Displays a message indicating the server is listening.
 * - Sets up a signal handler for interrupt signals.
 * - In the main loop:
 *   - Clears the socket set and adds the master socket.
 *   - Adds child sockets to the set.
 *   - Waits for activity on one of the sockets using the select system call.
 *   - Handles a new connection if the server socket is set.
 *   - Processes commands from existing client sockets.
 * - The loop continues until a server restart is requested or the server is deactivated.
 *
 * @see handleNewConnection(), receiveMessage(), _commandhandler
 *
 * @remark
 * This function is crucial for the continuous operation of the IRC server, managing
 * both new connections and existing client commands efficiently.
 */

void Server::runServer()
{
	// Create a new server socket
	this->_sockserver = NewServerSocket();

	// Initialize variables for monitoring socket activity
	fd_set readfds;
	int sd, activity, maxSd;

	// Initialize an array to store client sockets
	for (int i = 0; i < maxClients; i++)
    clientSocket[i] = 0;

	// Display a message indicating the server is listening
	std::cout << "listening..." << std::endl;

	// Main loop for server operation
	while (this->_isRestart == false && serverActive == true)
	{
    	// Set up a signal handler for interrupt signals
    	std::signal(SIGINT, handler);

    	// Clear the socket set
    	FD_ZERO(&readfds);

    	// Add the master socket to the set
    	FD_SET(this->_sockserver, &readfds);
    	maxSd = this->_sockserver;

    	// Add child sockets to the set
    	for (int i = 0; i < maxClients; i++)
    	{
        	// Get the socket descriptor
        	sd = clientSocket[i];

        	// If it's a valid socket descriptor, add it to the read list
        	if (sd > 0)
            	FD_SET(sd, &readfds);

        	// Update the highest file descriptor number
        	if (sd > maxSd)
            	maxSd = sd;
    	}

    	// Wait for activity on one of the sockets, with an indefinite timeout
    	activity = select(maxSd + 1, &readfds, NULL, NULL, NULL);

    	// Check for select errors
    	if ((activity < 0) && (errno != EINTR))
        	std::cerr << ("select error") << std::endl;

    	// Handle new connection if the server socket is set
    	if (FD_ISSET(this->_sockserver, &readfds) && serverActive == true)
        	handleNewConnection();
    	// Handle existing client sockets
    	else if (serverActive == true)
    	{
        	for (int i = 0; i < maxClients; i++)
        	{
            	sd = clientSocket[i];

            	// Check if the socket is set in the readfds set
            	if (FD_ISSET(sd, &readfds))
            	{
                	std::string buf;

                	// Check if it's for closing, and read the incoming message
                	buf = receiveMessage(sd);

                	// Process the received command using a command handler
                	if (!buf.empty())
                	{
                    	std::cout << "\033[1;34mCOMMAND RECEIVED :\033[0m " << buf;
                    	std::string command(buf);
						// Extract the command from the message
                    	int occ = buf.find_first_not_of(sep, 0);
                    	buf = command.substr(occ, buf.length() - occ);
                    	command = buf.substr(0, buf.find_first_of(sep, 0));

						//If the command is registered in the command handler, execute it
                    	if (_commandhandler.find(command) != _commandhandler.end())
                        	(_commandhandler[command])(this, buf, sd);

                    	break;
                	}
         		}
        	}
    	}
	}

	// Clean up resources after exiting the main loop
	clearAll();

	// Close all client sockets
	for (int i = 0; i < maxClients; i++)
	{
    	if (clientSocket[i] != 0)
    	{
        	close(clientSocket[i]);
        	clientSocket[i] = 0;
    	}
	}

	// Close the server socket
	close(this->_sockserver);

	// Restart the server if requested
	if (this->_isRestart == true && serverActive == true)
	{
    	this->_isRestart = false;
    	std::cout << "SERVER RESTARTING..." << std::endl;
    	runServer();
	}
}

/**
 * @brief Receive a complete message from a client socket.
 *
 * This function reads data from the provided socket descriptor until it forms a complete
 * message terminated by a newline character ('\n'). It ensures that the server is active
 * to avoid unnecessary processing when shutting down.
 *
 * @param sd The socket descriptor for the client.
 * @return A string containing the received message.
 *
 * @throws std::runtime_error if an error occurs during the reception of the message.
 *
 * @remark
 * The function uses a buffer to accumulate data until a newline character is encountered,
 * indicating the end of the message. It handles the serverActive flag to avoid processing
 * messages when the server is in the process of shutting down.
 */

std::string Server::receiveMessage(int sd) const
{
	char buffer[1024];
	std::string buf = "";
	memset(buffer, 0, 1024);
	while ((buf += buffer).find('\n') == std::string::npos && serverActive == true)
		if (recv(sd, buffer, 1024, 0) < 0)
			throw std::runtime_error("Error receiving message");
	return buf;
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

void Server::setChannels(std::string channelName, Channel *chan)
{
	this->_channels.insert(std::make_pair(channelName, chan));
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