#pragma once

#include "pch.h"
#include "networkClasses.h"

#define STATUS_OK 0 // no error
#define ALREADY_CONNECTED 1 // second call to ss.Open() without closing connection
#define NOT_CONNECTED 2 // call to ss.Send()/Close() without ss.Open()
#define INVALID_NAME 3 // ss.Open() with targetHost that has no DNS entry
#define FAILED_SEND 4 // sendto() failed in kernel
#define TIMEOUT 5 // timeout after all retx attempts are exhausted
#define FAILED_RECV 6 // recvfrom() failed in kernel

#define MAGIC_PORT 22345 // receiver listens on this port
#define MAX_PKT_SIZE (1500-28) // maximum UDP packet size accepted by receiver

class senderSocket {
	SOCKET sock;
	struct sockaddr_in remote;
	double RTO;
	bool connected;
	public:
		senderSocket();
		int open(char* targetHost, int port, int senderWindow, LinkProperties* lp);
		int send();
		int close();
};