
#ifndef USER_HPP
# define USER_HPP

#include "main.hpp"

class User{
	private:
	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _real_name;
	std::set<std::string> _channels;
	std::string _mode;


	public:
	User(std::string nick, std::string user, std::string hostname, std::string real_name);
	~User();
	std::string getNickname() const;
	std::string getUsername() const;
	std::string getHostname() const;
	std::string getReal_name() const;
	std::set<std::string> &getChannels();
	size_t getChannelsSize() const;
	std::string getMode() const;
	void add_channel(std::string channel_name);
	void setNick(std::string new_nickname);
	void setMode(std::string mode);

};

#endif