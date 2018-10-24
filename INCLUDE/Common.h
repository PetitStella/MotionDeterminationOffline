#pragma once

#include <stdio.h>

#ifdef _WIN32
#pragma warning(disable:4996)
#endif

class CommonClass
{
public:
	int motionNum;
	int channelNum;
	int compNum;
	int dataLength;


	CommonClass();
	bool LoadStateData(char *fileName = "state.dat");
	bool SaveStateData(char *fileName = "state.dat");
};
