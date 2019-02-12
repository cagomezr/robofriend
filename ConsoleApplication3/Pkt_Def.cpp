#include "Pkt_Def.h"
#include <bitset>
#include <string>
#include <vector>

using namespace std;


/*
PktDef() – A default constructor that places the PktDef object in a safe state
*/
PktDef::PktDef()
{
	this->cmdpacket.hdr.PktCount = 0;
	this->cmdpacket.hdr.Sleep = 0;
	this->cmdpacket.hdr.Status = 0;
	this->cmdpacket.hdr.Drive = 0;
	this->cmdpacket.hdr.Claw = 0;
	this->cmdpacket.hdr.Arm = 0;
	this->cmdpacket.hdr.Ack = 0;
	this->cmdpacket.hdr.Padding = 0;
	this->cmdpacket.hdr.Length = 0;
	this->cmdpacket.Data = nullptr;
	this->cmdpacket.CRC = 0;
}



/*
PktDef(char *) – An overloaded constructor that takes a RAW data buffer,
parses the data and populates the Header, Body, and CRC contents of the PktDef object.
*/
PktDef::PktDef(char * raw) {

	memcpy(&cmdpacket.hdr.PktCount, raw, sizeof(unsigned int));
	raw += sizeof(unsigned int);

	cmdpacket.hdr.Sleep = *raw & 1;
	cmdpacket.hdr.Status = (*raw >> 1) & 1;
	cmdpacket.hdr.Drive = (*raw >> 2) & 1;
	cmdpacket.hdr.Claw = (*raw >> 3) & 1;
	cmdpacket.hdr.Arm = (*raw >> 4) & 1;
	cmdpacket.hdr.Ack = (*raw >> 5) & 1;
	cmdpacket.hdr.Padding = (*raw >> 6) & 1;
	cmdpacket.hdr.Padding = (*raw >> 7) & 1;


	raw += 1;//move the raw ptr 1 byte (the size of all flags combined) further to the end 
	memcpy(&cmdpacket.hdr.Length, raw, sizeof(unsigned char));

	raw += sizeof(unsigned char);

	unsigned int dataSize = 0;
	dataSize = (this->cmdpacket.hdr.Length) - (this->hdrsize) - 1;

	if (dataSize > 0) {
		char * tmp = new char[dataSize];
		memcpy(tmp, raw, sizeof(unsigned char) * dataSize);
		this->cmdpacket.Data = tmp;
		raw += sizeof(unsigned char) * dataSize;
	}

	memcpy(&cmdpacket.CRC, raw, sizeof(cmdpacket.CRC));
}


/*
void SetCmd(CmdType) – A set function that sets the packets command flag based on the CmdType
Drive, Arm, Claw and Sleep flags should never be set at the same time.
*/
void PktDef::SetCmd(CmdType cmd) {

	if (cmd == ACK) {
		this->cmdpacket.hdr.Ack = 1;
	}
	else {
		this->cmdpacket.hdr.Sleep = 0;
		this->cmdpacket.hdr.Status = 0;
		this->cmdpacket.hdr.Drive = 0;
		this->cmdpacket.hdr.Claw = 0;
		this->cmdpacket.hdr.Arm = 0;
		this->cmdpacket.hdr.Ack = 0;

		switch (cmd) {
		case SLEEP:
			this->cmdpacket.hdr.Sleep = 1;
			this->cmdpacket.hdr.Length = this->hdrsize + sizeof(this->cmdpacket.CRC);
			break;
		case DRIVE:
			this->cmdpacket.hdr.Drive = 1;
			break;
		case CLAW:
			this->cmdpacket.hdr.Claw = 1;
			break;
		case ARM:
			this->cmdpacket.hdr.Arm = 1;
			break;
		}
	}
}


/*
void SetBodyData(char *, int) – a set function that takes a pointer to a RAW data buffer and the size of the buffer in bytes.
This function will allocate the packets Body field and copies the provided data into the objects buffer
*/
void PktDef::SetBodyData(char * rawDataBuffer, int size = 0)
{
	this->cmdpacket.Data = new char[size];
	memcpy(cmdpacket.Data, rawDataBuffer, size);

	this->cmdpacket.hdr.Length = this->hdrsize + size + sizeof(this->cmdpacket.CRC);
}


/*
void SetPktCount(int) – a set function that sets the objects PktCount header variable
*/
void PktDef::SetPktCount(int count)
{
	this->cmdpacket.hdr.PktCount = count;
}


/*
CmdType GetCmd() – a query function that returns the CmdType based on the set command flag bit

Question: According to the requirement, if the CMD is ACK, there will be another CMD which is set to be 1.
However, the return type of the function will return a single value.
How to solve it?

*/
CmdType PktDef::GetCmd()
{
	CmdType cmd1;

	if (cmdpacket.hdr.Sleep == 1) {
		cmd1 = SLEEP;
	}
	else if (cmdpacket.hdr.Drive == 1) {
		cmd1 = DRIVE;
	}
	else if (cmdpacket.hdr.Claw == 1) {
		cmd1 = CLAW;
	}
	else if (cmdpacket.hdr.Arm == 1) {
		cmd1 = ARM;
	}
	else
		cmd1 = NACK;

	return cmd1;
}


std::vector<CmdType> PktDef::GetFailedCmds()
{
	std::vector<CmdType>  failedCmds;

	if (cmdpacket.hdr.Sleep == 1) {
		failedCmds.push_back(SLEEP);
	}

	if (cmdpacket.hdr.Drive == 1) {
		failedCmds.push_back(DRIVE);
	}

	if (cmdpacket.hdr.Claw == 1) {
		failedCmds.push_back(CLAW);
	}

	if (cmdpacket.hdr.Arm == 1) {
		failedCmds.push_back(ARM);
	}

	/*if (cmdpacket.hdr.N == 1) {
	cmd1 = NACK;
	}*/

	return failedCmds;
}


bool PktDef::checkStatus() {
	bool isSet = false;
	if (this->cmdpacket.hdr.Status == 1) {
		isSet = true;
	}
	return isSet;
}

/*
bool GetAck() – a query function that returns True/False based on the Ack flag in the header
Question: need conversion between unsigned char to bool?
*/
bool PktDef::GetAck()
{
	bool isAck = false;

	if (this->cmdpacket.hdr.Ack == 1) {
		isAck = true;
	}

	return isAck;
}


/*
int GetLength() – a query function that returns the packet Length in bytes
*/
int PktDef::GetLength()
{
	return this->cmdpacket.hdr.Length;
}


/*
char *GetBodyData() – a query function that returns a pointer to the objects Body field
*/
char* PktDef::GetBodyData()
{
	return cmdpacket.Data;
}


/*
int GetPktCount() – a query function that returns the PktCount value
*/
int PktDef::GetPktCount()
{
	return cmdpacket.hdr.PktCount;
}


/*
bool CheckCRC(char *, int) – a function that takes a pointer to a RAW data buffer,
the size of the packet in bytes located in the buffer, and calculates the CRC.
If the calculated CRC matches the CRC of the packet in the buffer the function returns TRUE,
otherwise FALSE.
*/
bool PktDef::CheckCRC(char * raw, int s)
{
	bool isSame = false;
	unsigned int cnt = 0;

	//retreive everything before temp., CRC, and count the set bits
	char * tmp = new char[s - 1];
	memcpy(tmp, &raw[0], 8);

	for (int i = 0; i < s - 1; i++) {
		cnt += countSetBits(tmp[i]);
	}

	//retreive CRC from the char * raw 
	unsigned char tmpCRC = 0;
	memcpy(&tmpCRC, &raw[8], 1);

	if (cnt == tmpCRC) {
		isSame = true;
	}

	return isSame;
}


/*
void CalcCRC() – a function that calculates the CRC and sets the objects packet CRC parameter.
https://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
*/
void PktDef::CalcCRC()
{
	Header tmp = cmdpacket.hdr;

	unsigned int count = 0;

	count += countSetBits(tmp.PktCount);
	count += countSetBits(tmp.Sleep);
	count += countSetBits(tmp.Status);
	count += countSetBits(tmp.Drive);
	count += countSetBits(tmp.Claw);
	count += countSetBits(tmp.Arm);
	count += countSetBits(tmp.Ack);
	count += countSetBits(tmp.Length);

	if ((tmp.Arm == 1) || (tmp.Claw == 1)) {
		char * tmpdata = new char[1];
		memcpy(&tmpdata[0], &cmdpacket.Data[0], sizeof(unsigned char));
		count += countSetBits(tmpdata[0]);

	}
	else if (tmp.Drive == 1) {
		char * tmpdata = new char[2];
		memcpy(&tmpdata[0], &cmdpacket.Data[0], sizeof(unsigned char));
		memcpy(&tmpdata[1], &cmdpacket.Data[1], sizeof(unsigned char));
		count += countSetBits(tmpdata[0]);
		count += countSetBits(tmpdata[1]);

	}

	this->cmdpacket.CRC = count;

}

void PktDef::setCRC(int num) {
	this->cmdpacket.CRC = num;
}

void PktDef::setLength(int num) {
	this->cmdpacket.hdr.Length = num;
}

void PktDef::setWrongCMDFlag(CmdType cType) {
	switch (cType) {
	case ACK:
		this->cmdpacket.hdr.Ack = 1;
		break;
	case SLEEP:
		this->cmdpacket.hdr.Sleep = 1;
		break;
	case DRIVE:
		this->cmdpacket.hdr.Drive = 1;
		break;
	case CLAW:
		this->cmdpacket.hdr.Claw = 1;
		break;
	case ARM:
		this->cmdpacket.hdr.Arm = 1;
		break;
	}
}
/*
char *GenPacket() – a function that allocates the private RawBuffer and transfers the contents from the objects member variables into a RAW data packet (RawBuffer) for transmission.
The address of the allocated RawBuffer is returned.
*/
char * PktDef::GenPacket()
{
	int packetSize = this->GetLength();
	int dataSize = 0;

	char * tmpPakt = new char[packetSize];
	char * tmp = tmpPakt;

	memcpy(tmpPakt, &this->cmdpacket.hdr, hdrsize);
	tmpPakt += hdrsize;

	//The Data body is empty for Sleep Command.
	dataSize = packetSize - hdrsize - sizeof(unsigned char) * 1;
	if (dataSize) {
		memcpy(tmpPakt, cmdpacket.Data, dataSize);
		tmpPakt += dataSize;
	}

	memcpy(tmpPakt, &cmdpacket.CRC, sizeof(unsigned char) * 1);
	char*meh = tmp;
	PktDef gundam = PktDef(tmp);
	return tmp;
}



