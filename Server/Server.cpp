#include "precomp.h"

#include <enet/enet.h>

#include "Core/Network/NetworkHandler.h"

int main(int argc, char** argv)
{
	//Initialise ENet before doing anything.
	if(enet_initialize() != 0) 
	{
		fprintf(stderr, "An error occured while initiating ENet.");
		return EXIT_FAILURE;
	}

	std::string ipAddress;
	int port;

	std::cout << "Enter IP address to host on: ";
	std::cin >> ipAddress;

	std::cout << "Enter port number: ";
	std::cin >> port;

	atexit(enet_deinitialize);

	NetworkHandler server = NetworkHandler::create_host(ipAddress.c_str(), port, 100);
	//NetworkHandler server = NetworkHandler::create_local_host();
	server.start_ticking();

	return EXIT_SUCCESS;
}

