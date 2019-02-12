#pragma once
#ifndef WINSOCK_H
#define WINSOCK_H

#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

enum SocketType { CLIENT, SERVER };
enum ConnectionType { TCP, UDP, Nul=-1 };
const int DEFAULT_SIZE = 100;

class MySocket {
private:
	//char *Buffer to dynamically allocate RAW buffer space for communication activities
	char * Buffer;


	//A WelcomeSocket used by a MySocket object configured as a TCP/IP Server
	SOCKET WelcomeSocket;

	//A ConnectionSocket used for client / server communications(both TCP and UDP)
	SOCKET ConnectionSocket;

	//struct sockaddr_in SvrAddr to store connection information
	sockaddr_in SvrAddr;

	//struct sockaddr_in RespAddr to store the reply address for a UDP connection
	sockaddr_in RespAddr;

	//SocketType mySocket to hold the type of socket the MySocket object is initialized to
	SocketType mySocket;

	std::string IPAddr;

	int Port;

	//ConnectionType connectionType to define the Transport Layer protocol being used(TCP / UDP)
	ConnectionType connectionType;

	//A bool bConnect flag to determine if a socket has been initialized or not
	bool bConnect;

	//int MaxSize to store the maximum number of bytes the buffer is allocated to. 
	//This will help prevent overflows and synchronization issues.
	int MaxSize;

	bool connection_Established = false;
	bool tcpSocketInitialized = false;
	bool udpSocketInitialized = false;
	bool weclomeSocketInitialized = false;
	bool connectionSocketInitialized = false;

	int version_num1, version_num2;

public:
	MySocket(SocketType, std::string, unsigned int, ConnectionType, unsigned int);
	~MySocket();
	bool StartWSA();
	SOCKET initialize_tcp_socket();
	SOCKET initialize_udp_socket();
	bool ConnectTCP();
	bool DisconnectTCP();
	bool SetupUDP();
	bool TerminateUDP();
	int SendData(const char*, int);
	int GetData(char*);
	std::string GetIPAddr();
	bool SetIPAddr(std::string);
	bool SetPortNum(int);
	int GetPort();
	SocketType GetType();
	bool SetType(SocketType);
};

#endif WINSOCK_H
#pragma once
