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

	//std::string ipaddress;
	//int port;

	//std::cout << "enter ip address to host on: ";
	//std::cin >> ipaddress;

	//std::cout << "enter port number: ";
	//std::cin >> port;

	atexit(enet_deinitialize);

	//NetworkHandler server = NetworkHandler::create_host(ipaddress.c_str(), port, 100);
	NetworkHandler server = NetworkHandler::create_local_host();
	server.start_ticking();

	return EXIT_SUCCESS;
}

