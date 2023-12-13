#include "irc.hpp"

int checkPort(char *av1)
{
	if (strlen(av1) != 4)
		return 1;
	int port = static_cast<int>(std::strtoul(av1, NULL, 0));
	if ((port >= 6660 && port <= 6669) || port == 6679 || port == 6697)
		return 0;
	return 1;
}

int main(int ac, char **av)
{
	try
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		if (checkPort(av[1]) == 1)
			throw std::runtime_error("Wrong port: Only (6660-6669), 6679 and 6697 accepted");
		Server server(av[1], av[2]);
		server.runServer();
	}
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		return 1;
	}
	return (0);
}