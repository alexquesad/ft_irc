#pragma once

#include "irc.hpp"


class Server{
	public:
		typedef void (*command)(Server *, std::string, int);

	private:
		int _sockserver;
		int _newClientSocket;
		const std::string _port;
		const std::string _password;
		std::map<int, User*> _users;
		std::map<std::string, command> _commandhandler;
		std::map<std::string, Channel*> _channels;
		struct sockaddr_in _server;
		std::string _serverName;
		bool _isRestart;
		int NewServerSocket();
		void handleNewConnection(void);

	public:
		Server(const std::string &port, const std::string &password);
		~Server();
		void runServer();
		std::map<std::string, Channel*> & getChannels();
		std::map<int, User*> & getUsers() ;
		std::string receiveMessage(int sd) const;
		std::string getServerName() const;
		std::string getPort() const;
		struct sockaddr_in getServer();
		void setChannels(std::string channelName, Channel *chan);
		void setUsers(int sd, User *user);
		void setIsRestart();
		int searchUserByNickname(std::string nickname);
		void clearAll();
};

std::ostream	&operator<<(std::ostream &stdout, std::map<std::string, Channel*> &channels);
std::ostream	&operator<<(std::ostream &stdout, std::map<int, User*> &users);
std::ostream	&operator<<(std::ostream &stdout, User &user);