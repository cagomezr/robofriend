#pragma once
#ifndef PKTDEF_H
#define PKTDEF_H

#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>



const int FORWARD = 1;
const int BACKWARD = 2;
const int LEFT = 3;
const int RIGHT = 4;
const int UP = 5;
const int DOWN = 6;
const int OPEN = 7;
const int CLOSE = 8;

struct telPktFlags {
	unsigned char DriveFlag : 1;
	unsigned char ArmUpFlag : 1;
	unsigned char ArmDownFlag : 1;
	unsigned char ClawOpenFlag : 1;
	unsigned char ClawClosedFlag : 1;
	unsigned char Pad : 3;
};


//HEADERSIZE represents the size of the Header in bytes (must be calculated by hand)
//HEADERSIZE = Header(6 bytes) + Data(2 bytes) + CRC (1 byte); 
const int HEADERSIZE = 9;

enum CmdType {
	SLEEP,
	DRIVE,
	CLAW,
	ARM,
	ACK,
	NACK
};

//size of Header is 6 bytes: PktCount(4 bytes) + flags(1 byte) + Length(1 byte)
//however, due to memory padding, sizeof() actually indicates 8 bytes.
struct Header {
	unsigned int PktCount;
	unsigned char Sleep : 1;
	unsigned char Status : 1;
	unsigned char Drive : 1;
	unsigned char Claw : 1;
	unsigned char Arm : 1;
	unsigned char Ack : 1;
	unsigned char Padding : 2;
	unsigned char Length;
};

struct MotorBody {
	unsigned char Direction;
	unsigned char Duration;//need to check the bit overflow
};

struct ActuatorBody {
	unsigned char Action;
};

class PktDef {
private:
	//fixed number of bytes for Header instance to avoid using sizeof operator
	//since the program will return 8 as the size due to memory padding.
	unsigned int hdrsize = 6;

	struct CmdPacket {
		Header hdr;
		char * Data;
		unsigned char CRC;
	}cmdpacket;

public:
	PktDef();
	PktDef(char *);
	void SetCmd(CmdType);
	void SetBodyData(char *, int);
	void SetPktCount(int);
	CmdType GetCmd();
	std::vector<CmdType> GetFailedCmds();
	bool checkStatus();
	bool GetAck();
	int GetLength();
	char * GetBodyData();
	int GetPktCount();
	bool CheckCRC(char *, int);
	void CalcCRC();
	void setCRC(int num);
	void setLength(int num);
	void setWrongCMDFlag(CmdType cType);
	char *GenPacket();
};



//utility function for PktDef::CalcCRC()
template<typename T>
unsigned int countSetBits(T t) {
	unsigned int count = 0;

	while (t) {
		count += t & 1;
		t >>= 1;
	}
	return count;
}

//As per the assignment doc, the data type for Motorbody's Duration is unsigned char.
//If the input value for this variable is greater than 255 or less than 0, it will cause bit overflow.
//The compiler will throw away the overflowed bit and cause unpredictable resultant value.
//In order to handle this exception, the isWithinUnSignedCharRange() is coded.
template<typename T>
bool isWithinUnSignedCharRange(T & t) {
	bool isWithin = false;

	if ((t > 0) && (t < 255)) {
		isWithin = true;
	}
	return isWithin;
}


#endif PKTDEF_H
#pragma once


