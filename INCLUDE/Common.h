#pragma once

#include <stdio.h>

#ifdef _WIN32
#pragma warning(disable:4996)
#endif

class CommonClass
{
public:
	int singleMotionNum;	// # of single motions
	int combineMotionNum;	// # of combined motions
	int channelNum;			// # of channels
	int compNum;			// # of components of R-LLGMN
	int trainDataLength;	// # of train samples
	int timeLength;			// # of time series samples for R-LLGMN
	double entropyThreshold;
	double forceThreshold;


	CommonClass();
	bool LoadStateData(char *fileName = "state.dat");
	bool SaveStateData(char *fileName = "state.dat");
};
