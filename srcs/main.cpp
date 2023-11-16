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
	// Send login information
	// sendMessage("NICK mynick\r\n");
	// sendMessage("USER myuser 0 * :My Name\r\n");

	// // Join a channel
	// sendMessage("JOIN #mychannel\r\n");

	// // Send a message to the channel
	// sendMessage("PRIVMSG #mychannel :Hello, world!\r\n");
	// while(true)
	// {
	// 	std::cout << server.receiveMessage() << "\n" << std::endl;
	// }
	//receiveMessage not working atm
	return (0);
}