#ifndef USER_HPP
# define USER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <netinet/in.h>

class User{
	private:
	std::string _nick;
	std::string _user;
	std::string _hostname;
	std::string _real_name;
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
	// bool getIs_oper() const;

};

#endif