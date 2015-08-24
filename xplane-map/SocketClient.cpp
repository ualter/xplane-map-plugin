
#include "SocketClient.h"
#include <sstream>

using namespace std;

SocketClient::SocketClient(char *serverAddress) {

	address = serverAddress;

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
	si_other.sin_port = htons(8881);
	si_other.sin_addr.S_un.S_addr = inet_addr(address.c_str());
}

int SocketClient::sendTo(char *message) {
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