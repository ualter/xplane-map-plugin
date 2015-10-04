
#include "SocketClient.h"
#include <sstream>

using namespace std;

SocketClient::SocketClient() {
}
int SocketClient::initSocketClient(const char *serverAddress, int portNumber) {
	//address = serverAddress;
	//port = portNumber;
					  	
	// Initialize Socket
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret != 0) {
		printf("Error WSAStartup failed: " + WSAGetLastError());
		WSACleanup();
	}

	// Open Socket with the Server
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) {
		printf("Error opening socket: " + WSAGetLastError());
	}

	// Setting connection
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(portNumber);
	si_other.sin_addr.S_un.S_addr = inet_addr(serverAddress);

	return 1;
}

int SocketClient::sendTo(const char *message) {
	int ret = sendto(sock, message, strlen(message), 0, (struct sockaddr *) &si_other, slen);
	if (ret < 0) {
		printf("Failed Sending:" + WSAGetLastError());
		return 0;
	}
	return 1;
}

SocketClient::~SocketClient() {
	closesocket(sock);
	WSACleanup();
}