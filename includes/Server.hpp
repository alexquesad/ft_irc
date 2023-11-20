#ifndef SERVER_HPP
# define SERVER_HPP

#include "main.hpp"

class User;

class Server{
	public:

	typedef void (*command)(Server *, char *, int);

	private:

	int _sockserver;
	int _sockcom;
	const std::string _port;
	const std::string _password;
	std::map<int, User*> _users;
	std::map<std::string, command> _commandhandler;
	std::map<std::string, Channel*> _channels;
	struct sockaddr_in server;
	std::string _server_name;
	bool _isRestart;

	public:


	Server(const std::string &port, const std::string &password);
	~Server();
	void connectToServer();
	int newSocket();
	std::map<std::string, Channel*> & getChannels();
	std::map<int, User*> & getUsers() ;
	std::string receiveMessage() const;
	std::string getServername() const;
	std::string getPort() const;
	struct sockaddr_in getServer();
	void setChannels(std::string channel_name, Channel *chan);
	void setUsers(int sd, User *user);
	void setIsRestart();
	int searchUserByNickname(std::string nickname);
	void new_connection(void);
	void clearAll();

};

std::ostream	&operator<<(std::ostream &stdout, std::map<std::string, Channel*> &channels);
std::ostream	&operator<<(std::ostream &stdout, std::map<int, User*> &users);
std::ostream	&operator<<(std::ostream &stdout, User &user);


#endif
