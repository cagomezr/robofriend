// stdafx.cpp : source file that includes just the standard includes
// ConsoleApplication2.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file



PktDef Adapter::createForward(int duration, int PktCount) {
	MotorBody DriveCmd;
	DriveCmd.Direction = FORWARD;
	DriveCmd.Duration = duration;
	ActuatorBody myAction;
	PktDef TestPkt;
	TestPkt.SetCmd(DRIVE);
	TestPkt.SetBodyData((char *)&DriveCmd, 2);
	TestPkt.SetPktCount(PktCount);
	TestPkt.CalcCRC();

	return TestPkt;
}
PktDef Adapter::createDownward(int duration, int PktCount) {
	MotorBody DriveCmd;
	DriveCmd.Direction = BACKWARD;
	DriveCmd.Duration = duration;
	ActuatorBody myAction;
	PktDef TestPkt;
	TestPkt.SetCmd(DRIVE);
	TestPkt.SetBodyData((char *)&DriveCmd, 2);
	TestPkt.SetPktCount(PktCount);
	TestPkt.CalcCRC();

	return TestPkt;
}
PktDef Adapter::createLeft(int duration, int PktCount) {
	MotorBody DriveCmd;
	DriveCmd.Direction = LEFT;
	DriveCmd.Duration = duration;
	ActuatorBody myAction;
	PktDef TestPkt;
	TestPkt.SetCmd(DRIVE);
	TestPkt.SetBodyData((char *)&DriveCmd, 2);
	TestPkt.SetPktCount(1);
	TestPkt.CalcCRC();

	return TestPkt;
}
PktDef Adapter::createRigth(int duration, int PktCount) {
	MotorBody DriveCmd;
	DriveCmd.Direction = RIGHT;
	DriveCmd.Duration = duration;
	ActuatorBody myAction;
	PktDef TestPkt;
	TestPkt.SetCmd(DRIVE);
	TestPkt.SetBodyData((char *)&DriveCmd, 2);
	TestPkt.SetPktCount(1);
	TestPkt.CalcCRC();

	return TestPkt;

}
PktDef Adapter::createArmUp(int PktCount) {
	PktDef TestPkt;
	ActuatorBody myAction;
	TestPkt.SetCmd(ARM);
	myAction.Action = UP;
	TestPkt.SetBodyData((char *)&myAction, 1);
	TestPkt.SetPktCount(PktCount);
	TestPkt.CalcCRC();
	return TestPkt;
}
PktDef Adapter::createArmDwn(int PktCount) {
	PktDef TestPkt;
	ActuatorBody myAction;
	TestPkt.SetCmd(ARM);
	myAction.Action = DOWN;
	TestPkt.SetBodyData((char *)&myAction, 1);
	TestPkt.SetPktCount(PktCount);
	TestPkt.CalcCRC();
	return TestPkt;
}
PktDef Adapter::createClawOpen(int PktCount) {
	PktDef TestPkt;
	ActuatorBody myAction;
	TestPkt.SetCmd(CLAW);
	myAction.Action = OPEN;
	TestPkt.SetBodyData((char *)&myAction, 1);
	TestPkt.SetPktCount(PktCount);
	TestPkt.CalcCRC();
	return TestPkt;
}
PktDef Adapter::createClawClosed(int PktCount) {
	PktDef TestPkt;
	ActuatorBody myAction;
	TestPkt.SetCmd(CLAW);
	myAction.Action = CLOSE;
	TestPkt.SetBodyData((char *)&myAction, 1);
	TestPkt.SetPktCount(PktCount);
	TestPkt.CalcCRC();
	return TestPkt;
}
PktDef Adapter::createSleep(int PktCount) {
	PktDef TestPkt;
	TestPkt.SetCmd(SLEEP);
	TestPkt.SetPktCount(PktCount);
	TestPkt.CalcCRC();

	return TestPkt;
}
TelemitryStruct::TelemitryStruct() {
	this->sonar = 0;
	this->position = 0;
	this->Drive = 0;
	this->ArmU = 0;
	this->ArmD = 0;
	this->ClawO = 0;
	this->ClawC = 0;
	this->Padding = 0;
}
TelemitryStruct::TelemitryStruct(char * raw) {
	char * temp = raw;
	
	memcpy(&sonar, temp, sizeof(uint16_t));
	temp += sizeof(uint16_t);
	memcpy(&position, temp, sizeof(uint16_t));
	temp += sizeof(uint16_t);


	this->Drive = *temp & 1;
	this->ArmU = (*temp >> 1) & 1;
	this->ArmD = (*temp >> 2) & 1;
	this->ClawO = (*temp >> 3) & 1;
	this->ClawC = (*temp >> 4) & 1;
	this->Padding = (*temp >> 5) & 3;
}
bool TelemitryStruct::getArm(){
	bool retval = false;
	if (this->ArmU == 1)
		retval = true;
	return retval;
}
bool TelemitryStruct::getClaw(){
	bool retval = false;
	if (this->ClawO == 1)
		retval = true;
	return retval;
}
bool TelemitryStruct::getDrive(){
	bool retval = false;
	if (this->Drive == 1)
		retval = true;

	return retval;
}
int TelemitryStruct::getSonar() {
	return sonar;
}
int TelemitryStruct::getPosition() {
	return position;
}
