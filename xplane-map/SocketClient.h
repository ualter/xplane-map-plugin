#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

using namespace std;

class SocketClient
{
public:
	int sendTo(char *message);
	SocketClient(char *serverAddress);
	~SocketClient();

private:
	WSAData data;
	SOCKET sock;
	struct sockaddr_in si_other;
	std::string address;
	int slen = sizeof(si_other);
};
