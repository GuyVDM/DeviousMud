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

	atexit(enet_deinitialize);

	NetworkHandler server = NetworkHandler::create_local_host();
	server.start_ticking();

	return EXIT_SUCCESS;
}

