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

	struct sockaddr_in remote;
	struct hostent* host;

	host = gethostbyname(targetHost);
	if (host == NULL) {
		printf("Failed to get IP for host. Got error %d\n", WSAGetLastError());
		return INVALID_NAME;
	}

	memset(&remote, 0, sizeof(remote));
	remote.sin_family = AF_INET;
	memcpy((char*)&(remote.sin_addr), host->h_addr, host->h_length);
	remote.sin_port = htons(port);
	int remoteLen = sizeof(remote);

	SenderDataHeader sdh;
	SenderSynHeader syn;

	sdh.flags.SYN = 1;
	syn.lp = *lp;
	syn.sdh = sdh;

	ReceiverHeader rh;
	int bytes;

	clock_t connectionStart = clock();
	for (int i = 0; i < 3; i++) {

		if (sendto(sock, (char*)&syn, sizeof(SenderSynHeader), 0, (struct sockaddr*)&remote, remoteLen) == SOCKET_ERROR) {
			printf("sendto() failed with return code %d\n", WSAGetLastError());
			return FAILED_SEND;
		};


		if ((bytes = recvfrom(sock, (char*)&rh, sizeof(rh), 0, (struct sockaddr*)&remote, &remoteLen) == SOCKET_ERROR)) {
			printf("recvfrom() failed with return code %d\n", WSAGetLastError());
			return FAILED_RECV;
		}
	}

			
	return STATUS_OK;
}

int senderSocket::send() {
	return 0;
}

int senderSocket::close() {
	return 0;
}