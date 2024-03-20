#include "pch.h"

senderSocket::senderSocket() {
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in local;
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(0);
	if (bind(sock, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR) {
		printf("Error calling bind(). Got return code %d\n", WSAGetLastError());
		return;
	}
}

int senderSocket::open(char* targetHost, int port, int senderWindow, LinkProperties* lp) {
	return 0;
}

int senderSocket::send() {
	return 0;
}

int senderSocket::close() {
	return 0;
}