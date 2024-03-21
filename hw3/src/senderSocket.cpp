#include "pch.h"

senderSocket::senderSocket() {
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	RTO = 1;
	memset(&remote, 0, sizeof(remote));

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
	clock_t connectionStart = clock();

	struct hostent* host;

	host = gethostbyname(targetHost);
	if (host == NULL) {
		printf("[%.3f] --> target %s is invalid\n", ((double)(clock() - connectionStart) / CLOCKS_PER_SEC), targetHost);
		return INVALID_NAME;
	}

	remote.sin_family = AF_INET;
	memcpy((char*)&(remote.sin_addr), host->h_addr, host->h_length);
	remote.sin_port = htons(port);
	int remoteLen = sizeof(remote);

	SenderDataHeader sdh;
	sdh.seq = 0;
	SenderSynHeader syn;

	sdh.flags.SYN = 1;
	syn.lp = *lp;
	syn.sdh = sdh;

	ReceiverHeader rh;
	int bytes;

	double sendTime, recvTime;
	for (int i = 0; i < 3; i++) {

		if (sendto(sock, (char*)&syn, sizeof(SenderSynHeader), 0, (struct sockaddr*)&remote, remoteLen) == SOCKET_ERROR) {
			printf("sendto() failed with return code %d\n", WSAGetLastError());
			return FAILED_SEND;
		};
		sendTime = ((double)(clock() - connectionStart)) / CLOCKS_PER_SEC;
		printf("[%.3f] --> SYN %d (attempt %d of 3, RTO %.3f) to %s\n", sendTime, sdh.seq, i+1, 1.000, inet_ntoa(remote.sin_addr));

		fd_set readfds;
		const timeval timeout = { RTO, 0 };
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		int available = select(0, &readfds, NULL, NULL, &timeout);

		if (available > 0) {
			if ((bytes = recvfrom(sock, (char*)&rh, sizeof(rh), 0, (struct sockaddr*)&remote, &remoteLen) == SOCKET_ERROR)) {
				printf("recvfrom() failed with return code %d\n", WSAGetLastError());
				return FAILED_RECV;
			}
			else {
				recvTime = ((double)(clock() - connectionStart)) / CLOCKS_PER_SEC;
				RTO = 3 * (recvTime - sendTime);
				printf("[%.3f] <-- SYN-ACK %d window %d; setting initial RTO to %.3f\n", recvTime, rh.ackSeq, rh.recvWnd, RTO);
				break;
			}
		}
		else if (available != 0) { 
			printf("select() failed with error code %d\n", WSAGetLastError());
			return FAILED_RECV;
		}
		else if (i == 2 && available == 0) { // timeout after max attempts
			return TIMEOUT;
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