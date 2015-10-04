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
	int initSocketClient(const char *serverAddress, int port);
	SocketClient();
	~SocketClient();
private:
	WSAData data;
	SOCKET sock;
	struct sockaddr_in si_other;
	//std::string address;
	//int port;
	int slen = sizeof(si_other);
};
