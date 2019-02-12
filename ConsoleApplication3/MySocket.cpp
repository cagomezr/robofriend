#include "MySocket.h"
#include <string>
#include <iostream>
#include <cstdio>

MySocket::MySocket(SocketType sType, std::string ip, unsigned int port = 0, ConnectionType cType = TCP, unsigned int size = 0)
{

	this->Port = port;
	this->IPAddr = ip;
	this->mySocket = sType;
	this->connectionType = cType;
	this->MaxSize = size;

	//dynamically allocates memory for the Buffer
	if (size < 0 || size>256) {
		Buffer = new char[DEFAULT_SIZE];
	}
	else {
		Buffer = new char[size];
	}

	StartWSA();

	//initialize sockets
	if (cType == UDP) {
		this->ConnectionSocket = this->initialize_udp_socket();
		this->connectionSocketInitialized = true;
	}
	else if (cType == TCP) {
		if (sType == CLIENT) {
			this->ConnectionSocket = this->initialize_tcp_socket();
			this->connectionSocketInitialized = true;
		}
		else if (sType == SERVER) {
			this->WelcomeSocket = this->initialize_tcp_socket();
			this->weclomeSocketInitialized = true;
		}
	}
}


/*
A destructor that cleans up all dynamically allocated memory space
*/
MySocket::~MySocket()
{
	if (Buffer != nullptr)
		delete[] Buffer;

	/*if(connectionSocketInitialized)closesocket(this->ConnectionSocket);
	if(weclomeSocketInitialized)closesocket(this->WelcomeSocket);
	if (tcpSocketInitialized)closesocket(tcpSocketInitialized);
	if (udpSocketInitialized)closesocket(udpSocketInitialized);*/


	WSACleanup();
}


/*
A private member function that initializes the Winsock DLL.
This will return ture/false based on successful Winsock initialization
*/
bool MySocket::StartWSA()
{
	WSADATA wsaData;
	bool testWsaData = true;

	this->version_num1 = 2;
	this->version_num2 = 2;

	if ((WSAStartup(MAKEWORD(this->version_num1, this->version_num2), &wsaData)) != 0) {
		testWsaData = false;
	}

	return testWsaData;
}


SOCKET MySocket::initialize_tcp_socket() {
	SOCKET LocalSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (LocalSocket == INVALID_SOCKET) {
		WSACleanup();
		exit(0);
	}
	else {
		this->tcpSocketInitialized = true;
	}

	return LocalSocket;
}


SOCKET MySocket::initialize_udp_socket() {
	SOCKET LocalSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (LocalSocket == INVALID_SOCKET) {
		WSACleanup();
		exit(0);
	}
	else {
		this->udpSocketInitialized = true;
	}

	return LocalSocket;
}

/*
Used to establish a TCP/IP socket connection (3-way handshake).
This will return true if a successful connection is made
*/
bool MySocket::ConnectTCP()
{
	bool testConnectTCP = false;

	//Check if the connection is already established to avoid re-assigning a new socekt to an existing connection  
	if (!this->connection_Established) {

		if ((this->connectionType == TCP) && (this->mySocket == CLIENT)) {

			if (!this->connectionSocketInitialized) {
				this->ConnectionSocket = this->initialize_tcp_socket();
				this->connectionSocketInitialized = true;
			}

			SvrAddr.sin_family = AF_INET;
			SvrAddr.sin_port = htons(this->Port);
			SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str());

			if ((connect(this->ConnectionSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
				closesocket(this->ConnectionSocket);
			}
			else
			{
				this->connection_Established = true;
				testConnectTCP = true;
			}
		}
		else if ((this->connectionType == TCP) && (this->mySocket == SERVER)) {

			if (!this->weclomeSocketInitialized) {
				this->WelcomeSocket = this->initialize_tcp_socket();
				this->weclomeSocketInitialized = true;
			}


			SvrAddr.sin_family = AF_INET;
			SvrAddr.sin_port = htons(this->Port);
			//SvrAddr.sin_addr.s_addr = INADDR_ANY;
			SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str());
			//SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str()); //For a server, it can NOT be set to listen ONE specific address. 
			//It should listen to any addresses. 


			//binding
			if ((bind(this->WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
				closesocket(this->WelcomeSocket);
			}

			//listening
			if (listen(this->WelcomeSocket, 1) == SOCKET_ERROR) {
				closesocket(this->WelcomeSocket);
			}


			//keep accepting until exit 
			//while (true) {}// the reason why a while loop can NOT be used here is 
			//because the thread assigned to the server is detached
			//thus the server is in fact is waiting for incoming connection.
			if ((this->ConnectionSocket = accept(this->WelcomeSocket, NULL, NULL)) == SOCKET_ERROR) {
				closesocket(this->WelcomeSocket);
			}
			else {
				this->connectionSocketInitialized = true;
				this->connection_Established = true;
				testConnectTCP = true;
			}
		}
	}
	/*else {
	if (this->DisconnectTCP()) {
	testConnectTCP = this->ConnectTCP();
	}
	}*/
	else {
		if (this->GetPort() > 0) {//On top of checking the status if a ConnectionSocket is establised,
								  //the connection is considered as established when it has been assigned a port number
			testConnectTCP = true;
		}
		else {
			testConnectTCP = false;
		}
	}


	return testConnectTCP;
}


/*
Used to disconnect an established TCP/IP socket connection (4-way handshake).
Returns true/false depending on success
*/
bool MySocket::DisconnectTCP()
{
	bool isSuccess = false;

	if (this->connection_Established) {//check if  the function is trying to close is a live connection
		if (connectionType == TCP) {
			if (this->mySocket == CLIENT) {
				if (closesocket(this->ConnectionSocket) == 0) {
					this->connection_Established = false;
					this->connectionSocketInitialized = false;
					this->tcpSocketInitialized = false;
					isSuccess = true;
				}
			}
			else {
				//To close sockets on the server, first close the ConnectionSocket for data comm, then close the WelcomeSocekt for handshakes
				// If success, closesocket() returns 0
				if (closesocket(this->ConnectionSocket) == 0) {
					this->connectionSocketInitialized = false;

					if (closesocket(this->WelcomeSocket) == 0) {
						this->weclomeSocketInitialized = false;
						this->connection_Established = false;
						this->tcpSocketInitialized = false;
						isSuccess = true;
					}
				}
			}
		}
	}
	else {
		isSuccess = false;
	}

	return isSuccess;
}


/*
Configures the UDP connection sockets for communication.
This will return true if the sockets are successfully configured
*/
bool MySocket::SetupUDP()
{
	bool testSetupUDP = false;

	if (!this->connection_Established) {

		if (this->connectionType == UDP) {

			SvrAddr.sin_family = AF_INET;
			SvrAddr.sin_port = htons(this->Port);
			SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str());

			if (this->mySocket == CLIENT) {
				//which to use: if or while? Just try once to initialize the socket or keep trying to initialize the socket?
				if (!this->connectionSocketInitialized) {
					this->ConnectionSocket = this->initialize_udp_socket();
					this->connectionSocketInitialized = true;
				}


				this->connection_Established = true;
				testSetupUDP = true;
			}
			else {
				//which to use: if or while? Just try once to initialize the socket or keep trying to initialize the socket?
				if (!this->connectionSocketInitialized) {
					this->ConnectionSocket = this->initialize_udp_socket();
					this->connectionSocketInitialized = true;
				}


				if (bind(this->ConnectionSocket, (struct sockaddr *) &SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
					closesocket(this->ConnectionSocket);
					testSetupUDP = false;
				}
				else {
					this->connection_Established = true;
					testSetupUDP = true;
				}
			}
		}
		/*else {
		if (this->TerminateUDP()) {
		testSetupUDP = this->SetupUDP();
		}
		}*/
		else {
			if ((this->udpSocketInitialized) && (this->connectionSocketInitialized)) {
				testSetupUDP = true;
			}
			else {
				testSetupUDP = false;
			}

		}

		return testSetupUDP;
	}
}


/*
Used to close configured UDP sockets. Returns true/false depending on success
*/
bool MySocket::TerminateUDP()
{
	bool isSuccess = false;

	if (this->connection_Established == true) {
		if (this->connectionType == UDP) {
			closesocket(this->ConnectionSocket);
			this->connection_Established = false;
			this->connectionSocketInitialized = false;
			this->udpSocketInitialized = false;
			isSuccess = true;
		}
	}
	else {
		isSuccess = false;
	}

	return isSuccess;
}


/*
Used to transmit a block of RAW data,
specified by the starting memory address and number of bytes over the socket and return the number of bytes transmitted.
This function should work with both TCP and UDP.
*/
int MySocket::SendData(const char* tMsg, int size)
{
	int sSize;

	if (connectionType == UDP) {
		if (this->mySocket == CLIENT) {
			/*RespAddr.sin_family = AF_INET;
			RespAddr.sin_port = htons(this->Port);
			RespAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str());*/

			sSize = sendto(this->ConnectionSocket, tMsg, size, 0, (struct sockaddr*)&this->SvrAddr, sizeof(this->SvrAddr));
		}
		else {
			sSize = sendto(this->ConnectionSocket, tMsg, size, 0, (struct sockaddr*)&this->RespAddr, sizeof(this->RespAddr));
		}
	}
	else {
		sSize = send(this->ConnectionSocket, tMsg, size, 0);
	}

	memset(this->Buffer, 0, sizeof(char) * this->MaxSize);

	return sSize;
}


/*
Used to receive the last block of RAW data stored in the internal MySocket Buffer.
After getting the received message into Buffer,
this function will transfer its contents to the provided memory address
and return the total number of bytes written.
This function should work with both TCP and UDP
*/
int MySocket::GetData(char* rMsg)
{
	int rSize = 0;
	int rSizeTCP = 0;
	int rSizeUDP = 0;
	int addr;

	if (this->connectionType == UDP) {
		if (this->mySocket == SERVER) {
			//the UDP server gets data from Buffer 
			addr = sizeof(this->RespAddr);
			rSizeUDP = recvfrom(this->ConnectionSocket, this->Buffer, sizeof(char)*this->MaxSize, 0, (struct sockaddr*)&RespAddr, &addr);
		}
		else {
			//the UDP client gets data from Buffer
			addr = sizeof(this->SvrAddr);
			rSizeUDP = recvfrom(this->ConnectionSocket, this->Buffer, sizeof(char)*this->MaxSize, 0, (struct sockaddr*)&SvrAddr, &addr);
		}
	}
	else if (this->connectionType == TCP) {
		rSizeTCP = recv(this->ConnectionSocket, this->Buffer, sizeof(char)*this->MaxSize, 0);
	}

	if (rSizeTCP > 0) {
		rSize = rSizeTCP;
	}
	else if (rSizeUDP > 0) {
		rSize = rSizeUDP;
	}

	//memcpy(rMsg, this->Buffer, rSize);
	memcpy(rMsg, this->Buffer, sizeof(char)*this->MaxSize);
	memset(this->Buffer, 0, sizeof(char) * this->MaxSize);

	return rSize;
}


/*
Returns the IP address configured within the MySocket object
*/
std::string MySocket::GetIPAddr() {
	return this->IPAddr;
}


/*
Changes the default IP address within the MySocket object and returns true/false depending on if successful
*/
bool MySocket::SetIPAddr(std::string newIP) {

	if ((!this->connection_Established) && (!connectionSocketInitialized) && (!weclomeSocketInitialized)) {
		this->IPAddr = newIP;
		return true;
	}
	else {
		return false;
	}
}


/*
Changes the default Port number within the MySocket object and returns true/false depending on if successful
• This method should return an error if a connection has already been established
*/
bool MySocket::SetPortNum(int newPort) {

	if (!this->connection_Established) {
		this->Port = newPort;
		return true;
	}
	else {
		return false;
	}

}


/*
Returns the Port number configured within the MySocket object
*/
int MySocket::GetPort()
{
	return this->Port;
}


/*
Returns the default SocketType the MySocket object is configured as
*/
SocketType MySocket::GetType()
{
	return this->mySocket;
}

/*
Changes the default SocketType within the MySocket object only if the sockets are disconnected/uninitialized.
Returns true/false depending on success
*/
bool MySocket::SetType(SocketType newSocketType) {
	if (!this->connection_Established) {
		this->mySocket = newSocketType;
		return true;
	}
	else { return false; }
}



















