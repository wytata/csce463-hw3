// hw3.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Name: Wyatt Cade
// Course: CSCE 463
// Semester: Spring 2024

#include "pch.h"
#pragma comment (lib, "Ws2_32.lib")


void usage() {
	printf("Usage: hw3.exe <server> <buffer size 2^n> <sender window> <RTT delay> <loss probability forward> <loss probability return> <link speed>\n");
	exit(1);
}

int main(int argc, char** argv) {
	if (argc != 8) {
		usage();
	}

	int bufferSizePower, senderWindow ;
	double roundTripDelay, lossForward, lossReturn, linkSpeed;

	bufferSizePower = atoi(argv[2]);
	senderWindow = atoi(argv[3]);

	linkSpeed = std::stod(argv[7]);
	roundTripDelay = std::stod(argv[4]);
	lossForward = std::stod(argv[5]);
	lossReturn = std::stod(argv[6]);

	char* targetHost = argv[1];

	printf("Main:\tsender W = %d, RTT %.3f sec, loss %g / %g, link %.3f Mbps\n", senderWindow, roundTripDelay, lossForward, lossReturn, linkSpeed);

	clock_t startTime = clock();
	printf("Main:\tinitializing DWORD array with 2^%d elements... ", bufferSizePower);
	 
	UINT64 dwordBufSize = (UINT64)1 << bufferSizePower;
	DWORD* dwordBuf = new DWORD[dwordBufSize]; 
	for (UINT64 i = 0; i < dwordBufSize; i++) {
		dwordBuf[i] = i;
	}

	int initTime = (int)(1000 * ((clock() - startTime) / CLOCKS_PER_SEC));

	printf("done in %d ms\n", initTime);

	char* server = argv[1];

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("Winsock initialization failed with return code %d\n", WSAGetLastError());
		delete[] dwordBuf;
		return 1;
	}

	LinkProperties lp;
	lp.RTT = roundTripDelay;
	lp.speed = 1e6 * linkSpeed;
	lp.pLoss[FORWARD_PATH] = lossForward;
	lp.pLoss[RETURN_PATH] = lossReturn;
	lp.bufferSize = senderWindow + 3; // W + R

	int sendResult;
	senderSocket sender;
	clock_t connectionStart = clock();
	if ((sendResult = sender.open(targetHost, MAGIC_PORT, senderWindow, &lp)) != STATUS_OK) {
		delete[] dwordBuf;
		printf("Main:\tconnect failed with status %d\n", sendResult);
		return 1;
	}

	clock_t connectionFinish = clock();
	printf("Main:\tconnected to %s in %.3f sec, pkt size %d bytes\n", targetHost, ((double)(connectionFinish - connectionStart)/CLOCKS_PER_SEC), MAX_PKT_SIZE);
	
	int closeResult;
	clock_t terminationStart = clock();
	if ((closeResult = sender.close()) != STATUS_OK) {
		printf("Main:\tterminate failed with status %d\n", closeResult);
		delete[] dwordBuf;
		return 1;
	}

	printf("Main:\ttransfer finished in %.3f sec\n", ((double)(terminationStart - connectionFinish)/CLOCKS_PER_SEC));

	delete[] dwordBuf;
	WSACleanup();
	return 0;
}