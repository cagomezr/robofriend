// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include "Pkt_Def.h"

#include <stdio.h>
#include <tchar.h>
class TelemitryStruct {
private:
	
	uint16_t  sonar  ;
	uint16_t  position ;
	unsigned char Drive : 1;
	unsigned char ArmU : 1;
	unsigned char ArmD : 1;
	unsigned char ClawO : 1;
	unsigned char ClawC : 1;
	unsigned char Padding : 3;
public:
	TelemitryStruct();
	TelemitryStruct(char*);
	bool getArm();
	bool getDrive();
	int getSonar();
	bool getClaw();
	int getPosition();
};

class Adapter {
public:
	
	PktDef createForward(int, int);
	PktDef createDownward(int, int);
	PktDef createLeft(int, int);
	PktDef createRigth(int, int);
	PktDef createArmUp(int);
	PktDef createArmDwn(int);
	PktDef createClawOpen(int);
	PktDef createClawClosed(int);
	PktDef createSleep(int);
	
};

// TODO: reference additional headers your program requires here
