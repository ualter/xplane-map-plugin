#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

using namespace std;

class SocketClient
{
public:
	int sendTo(const char *message);
	SocketClient(const char *serverAddress, int port);
	~SocketClient();

private:
	WSAData data;
	SOCKET sock;
	struct sockaddr_in si_other;
	std::string address;
	int slen = sizeof(si_other);
};
