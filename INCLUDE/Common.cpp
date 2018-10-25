#include "Common.h"

CommonClass::CommonClass()
{
	singleMotionNum = 0;
	combineMotionNum = 0;
	channelNum = 0;
	compNum = 0;
	trainDataLength = 0;
	timeLength = 0;
	entropyThreshold = 0;
	forceThreshold = 0;
}


bool CommonClass::LoadStateData(char *fileName)
{
	FILE *fp;

	fp = fopen(fileName, "r");
	if (fp == NULL) {
		printf("%s can not open.\n", fileName);
		return false;
	}
	fscanf(fp, "%d", &singleMotionNum);
	fscanf(fp, "%d", &combineMotionNum);
	fscanf(fp, "%d", &channelNum);
	fscanf(fp, "%d", &compNum);
	fscanf(fp, "%d", &trainDataLength);
	fscanf(fp, "%d", &timeLength);
	fscanf(fp, "%lf", &entropyThreshold);
	fscanf(fp, "%lf", &forceThreshold);

	fclose(fp);

	return true;
}


bool CommonClass::SaveStateData(char *fileName)
{
	FILE *fp;

	fp = fopen(fileName, "w");
	if (fp == NULL) {
		printf("%s can not open.\n", fileName);
		return false;
	}
	fprintf(fp, "%d\n", singleMotionNum);
	fprintf(fp, "%d\n", combineMotionNum);
	fprintf(fp, "%d\n", channelNum);
	fprintf(fp, "%d\n", compNum);
	fprintf(fp, "%d\n", trainDataLength);
	fprintf(fp, "%d\n", timeLength);
	fprintf(fp, "%lf\n", entropyThreshold);
	fprintf(fp, "%lf\n", forceThreshold);

	fclose(fp);

	return true;
}