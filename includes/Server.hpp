#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <map>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cstdio>

#include "Client.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include "Replies.hpp"
#include "utils.hpp"

# define SERVER_NAME "localhost"
# define ver "1.0"

class Channel;

class Server{
	public:

	typedef void (*command)(Server *, char *, int);

	private:

	int _sockserver;
	int _sockcom;
	const std::string _port;
	const std::string _password;
	std::map<int, Client*> _users;
	std::map<std::string, command> _commandhandler;
	std::map<std::string, Channel *> _channels;
	struct sockaddr_in server;
	std::string _server_name;

	public:


	Server(const std::string &port, const std::string &password);
	~Server();
	void connectToServer();
	int newSocket();
	void sendMessage(std::string message, int sd) const;
	std::map<std::string, Channel *> getChannels();
	std::map<int, Client*> getUsers();
	std::string receiveMessage() const;
	std::string getServername() const;
	std::string getPort() const;

};

#endif