#include "precomp.h"

#include "Core/Network/Client/ClientWrapper.h"

#include <enet/enet.h>

int main(int argc, char** argv)
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occured while initializing ENet.\n");
		return EXIT_FAILURE;
	}

	bool valid_connection = false;


	Client c = Client::connect_localhost(valid_connection);

	if (!valid_connection) 
	{
		fprintf(stderr, "No valid connection could be established. \n");
		return EXIT_FAILURE;
	}

	c.start_ticking();
	c.quit();

	return 0;
}



