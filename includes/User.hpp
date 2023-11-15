#ifndef USER_HPP
# define USER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <netinet/in.h>
#include <vector>
#include <algorithm>

class User{
	private:
	std::string _nick;
	std::string _user;
	std::string _hostname;
	std::string _real_name;
	std::vector<std::string> _channels;
	// bool _is_connect;
	// bool _is_oper;

	public:
	User(std::string nick, std::string user, std::string hostname, std::string real_name);
	User(const User &T);
    User &operator=(const User &T);
	~User();
	std::string getNick() const;
	std::string getUser() const;
	std::string getHostname() const;
	std::string getReal_name() const;
	std::vector<std::string> getChannels() const;
	void add_channel(std::string channel_name);
	// bool getIs_oper() const;

};

// std::ostream	&operator<<(std::ostream &stdout, User const &user);
#endif