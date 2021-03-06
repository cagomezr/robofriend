// ConsoleApplication2.cpp : Defines the entry point for the console application.
//


#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <future>
#include <mutex>
#include <iomanip>
#include "stdafx.h"
#include "MySocket.h"
#include "Pkt_Def.h"

using namespace std;

bool ExeComplete = false;
void PrintA(std::string msg, std::ofstream * printstream) {
	*printstream << msg << std::endl;
}
void PrintB(std::string msg, std::ofstream * printstream) {
	std::cout << msg << std::endl;
	*printstream << msg << std::endl;
}

void roboClient(bool &aDone, std::string server, int port) {
	Adapter packetFunction;
	std::ofstream orfs("Robot_Log_Output.txt");
	if (!orfs.is_open())
		std::cout << "ERROR:  Failed to open Client_Output.txt file" << std::endl;
	//start socket defaults
	MySocket ClientSocket(SocketType::CLIENT, "192.168.0.1", 27000, ConnectionType::TCP, 100);
	ClientSocket.SetIPAddr(server);
	ClientSocket.SetPortNum(port);
	PktDef sendPacket;
	bool maintainConection = true;
	bool systemcheck =false;
	int PktCount = 1;
	if (!ClientSocket.ConnectTCP()) {
		maintainConection = false;
		aDone = true;
		//return;
	}
	else {
		while (maintainConection) {
			std::string itemdats, direction;
			int duration = 0;
			bool  successcommand = true;
			PrintA("", &orfs);
			PrintB("Please tell me what should the robot do", &orfs);
			PrintB(" DRIVE (D),CLAW (C), ARM  (A), SLEEP (S)", &orfs);
			std::cin >> itemdats;
			
			if ((itemdats == "D") || (itemdats == "d")) {
				
				PrintB("Drive Menu", &orfs);
				PrintB(" Please tell me for how long", &orfs);
				std::cin >> duration;
				PrintA(" " + std::to_string(duration)  , &orfs);
				
				PrintB( "Please tell me what direction to go", &orfs);
				PrintB( " Forward(F), Backward(B), Left(L), Right(R)", &orfs);
				std::cin >> direction;
				if ((direction == "F") || (direction == "f")) {
					sendPacket = packetFunction.createForward(duration, PktCount);
					PrintA(" Forward(F)", &orfs);
				}
				else if ((direction == "B") || (direction == "b")) {
					sendPacket = packetFunction.createDownward(duration, PktCount);
					PrintA("Backward(B)", &orfs);
				}
				else if ((direction == "L") || (direction == "l")) {
					sendPacket = packetFunction.createLeft(duration, PktCount);
					PrintA("Left(L)", &orfs);
				}
				else if ((direction == "R") || (direction == "r")) {
					sendPacket = packetFunction.createRigth(duration, PktCount);
					PrintA("Right(R)", &orfs);
				}
				else {
					PrintB( "Error please try again", &orfs);
					successcommand = false;
				}


			}
			else if ((itemdats == "C") || (itemdats == "c")) {
				PrintB("Claw Menu",&orfs);
				PrintB( "Please tell me should I Close(C) or Open(O)", &orfs);
				std::cin >> direction;
				if ((direction == "C") || (direction == "c"))
				{
					sendPacket = packetFunction.createClawClosed(PktCount);
					PrintA("Close(C)", &orfs);
				}
				else if ((direction == "O") || (direction == "o")) {

					sendPacket = packetFunction.createClawOpen(PktCount);
					PrintA("Open(O)", &orfs);
				}
				else {
					successcommand = false;
				}

			}
			else if ((itemdats == "A") || (itemdats == "a")) {
				PrintB("Arm Menu", &orfs);
				PrintB( "Please tell me should I go Up(U) or Down(D)", &orfs);
				std::cin >> direction;
				if ((direction == "U") || (itemdats == "u")) {
					sendPacket = packetFunction.createArmUp(PktCount);
					PrintA("Up(U)", &orfs);
				}
				else if ((direction == "D") || (itemdats == "d")) {
					sendPacket = packetFunction.createArmDwn(PktCount);
					PrintA("Down(D)", &orfs);
				}
				else {
					successcommand = false;
				}
			}
			else  if ((itemdats == "S") || (itemdats == "s"))
			{
				PrintB( "Ready to quit Y/N", &orfs);
				std::cin >> direction;
				if ((direction == "Y") || (direction == "y")) {
					PrintB( " Yes Robot will Sleep now", &orfs);
					sendPacket = packetFunction.createSleep(PktCount);
					systemcheck = true;

				}								
			}
			else {
				PrintB( "Please tell me what should the robot do based on option", &orfs);
				successcommand = false;

			}


			if (successcommand) {
				char *ptr;
				ptr = sendPacket.GenPacket();				
				int PacketLength = sendPacket.GetLength();
				ClientSocket.SendData(ptr, PacketLength);
				bool serveractive = true;
				char *buffer = new char[100];
				PktDef ReceptionPkt;
				while (serveractive) {
					int size = ClientSocket.GetData(buffer);
					if (size > 0) {
						ReceptionPkt = PktDef(buffer);
		
						if (ReceptionPkt.GetAck()) {
							serveractive = false;
							PktCount += 1;
							PrintB( "Command send  succesfully and acknowledged", &orfs);
							if (systemcheck) {
								maintainConection = false;
								aDone = true;
							}
						}
						else {
							PrintB( "Packet error has  occured error try sending command again", &orfs);
							serveractive = false;
						}
					}
					else {
						PrintB( "Packet seems not been send please send command again", &orfs);
						serveractive = false;
					}
				}

			}
		}

	}
	ClientSocket.DisconnectTCP();

}
void telemitryClient(bool &bDone, std::string server, int port) {
	std::ofstream otfs("Telemetry_Log_Output.txt");
	if (!otfs.is_open())
		std:cout << "ERROR:  Failed to open Client_Output.txt file" << std::endl;
	//start socket

	MySocket TelemetrySocket(SocketType::CLIENT, "192.168.0.1", 27501, ConnectionType::TCP, 100);
	TelemetrySocket.SetIPAddr(server);
	TelemetrySocket.SetPortNum(port);
	char *buffer = new char[100];
	if (TelemetrySocket.ConnectTCP()) {
		while (bDone == false) {
			PktDef ReceptionPkt;
			int size = TelemetrySocket.GetData(buffer);
			if (size > 0) {
				ReceptionPkt = PktDef(buffer);
				TelemitryStruct data = TelemitryStruct(ReceptionPkt.GetBodyData());

				
				otfs << "Data for Telemitry Packet" << std::endl << std::endl;
				otfs << "Sensor indicator reading " << data.getSonar() << std::endl;
				otfs << "Arm Position reading " << data.getPosition() << std::endl;

				if (data.getDrive()) 
					otfs << "Robot is Driving!" << std::endl;				
				else 
					otfs << "Robot is Stationary" << std::endl;
				
				if (data.getArm())
					otfs << "Arm is UP" << std::endl;
				else
					otfs << "Arm is Down" << std::endl;
				
				if (data.getClaw())
					otfs << "Claw is Open" << std::endl;
				else
					otfs << "claw is closed" << std::endl << std::endl;
				

			}
		}
	}
	TelemetrySocket.DisconnectTCP();

}
int main(int argc, char *argv[])
{

	std::ofstream ofs("Client_Output.txt");
	if (!ofs.is_open())
		std::cout << "ERROR:  Failed to open Client_Output.txt file" << std::endl;
	//start socket

	std::string serverAdress;
	int adrPort = 0;
	std::string telemetryAdress;
	int telPort = 0;
	bool ExeComplete = false;
	PrintB("Connect to Robot Adress", &ofs);
	std::cin >> serverAdress;
	PrintA(serverAdress, &ofs);

	PrintB("Connect to Robot Port", &ofs);
	std::cin >> adrPort;
	PrintA(std::to_string(adrPort) , &ofs);
	
	PrintB("Connect to Robot Telemitry  Port", &ofs);
	std::cin >> telPort;
	PrintA(std::to_string(telPort), &ofs);

	PrintB("Running TCP RobotFriend Comunications ", &ofs);

	PrintA("Start thread 1 ", &ofs);
	std::thread T1(roboClient, std::ref(ExeComplete), serverAdress, adrPort);
	T1.detach();
	PrintA("Thread one  detatched", &ofs);

	PrintA("Start Thread 2 ", &ofs);
	std::thread T2(telemitryClient, std::ref(ExeComplete), serverAdress, telPort);
	T2.detach();
	PrintA("Detatch thread 2 ", &ofs);
	PrintA("begin EXEComplete  sequence ", &ofs);

	while (!ExeComplete) {}
	PrintB("Stopping TCP RobotFriend Comunications ", &ofs);
	PrintB("Goodbye ", &ofs);


}