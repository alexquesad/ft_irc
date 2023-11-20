#include "main.hpp"

int main(int ac, char **av)
{
	try
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		Server server(av[1], av[2]);
		server.connectToServer();
	}
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		return 1;
	}
	return (0);
}