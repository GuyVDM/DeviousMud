#pragma once
#include <cstdint>
#include <memory>

typedef struct _ENetHost ENetHost;
typedef struct _ENetPeer ENetPeer;

class Client
{
public:

	static Client connect_localhost(bool& succeeded);

	static Client connect_host(bool& succeeeded, const char* ip, uint16_t port);

	static constexpr const char* TITLE = "DeviousMud";

	void start_ticking();

	void quit();


public:
	~Client() = default;

protected:
	Client& operator=(Client const&) = delete;
	Client(ENetHost* c);

private:
	void init();

private:
	ENetHost* m_host;
	ENetPeer* m_peer;
};