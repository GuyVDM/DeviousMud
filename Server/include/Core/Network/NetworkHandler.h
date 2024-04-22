#pragma once
#include <vector>
#include <cinttypes>

typedef struct _ENetHost ENetHost;

/// <summary>
/// This class takes care of managing the initialization & establishment of the server.
/// </summary>
class NetworkHandler 
{
public:
	/// <summary>
	/// Creates a default listening server listening on 127.0.0.1:1234
	/// </summary>
	/// <param name="maxconnections"></param>
	/// <param name="channels"></param>
	/// <param name="inc_bandwith"></param>
	/// <param name="outg_bandwidth"></param>
	/// <returns></returns>
	static NetworkHandler create_local_host(int32_t maxconnections = 10, int32_t channels = 2, int32_t inc_bandwith = 0, int32_t outg_bandwidth = 0);

	/// <summary>
	/// Create a listening server with the specified parameters.
	/// </summary>
	/// <param name="adress"></param>
	/// <param name="port"></param>
	/// <param name="maxconnections"></param>
	/// <param name="channels"></param>
	/// <param name="inc_bandwith"></param>
	/// <param name="outg_bandwidth"></param>
	/// <returns></returns>
	static NetworkHandler create_host(const char* adress,
		                              int32_t port, 
		                              int32_t maxconnections, 
		                              int32_t channels = 2, 
		                              int32_t inc_bandwith = 0,
		                              int32_t outg_bandwidth = 0);

public:
	ENetHost* get_server_host();

	void start_ticking();


public:
	~NetworkHandler();

protected:
	NetworkHandler(ENetHost* m_server);
	void destroy();

private:
	ENetHost* m_server;

	const float m_tickDuration = 0.6f;
};