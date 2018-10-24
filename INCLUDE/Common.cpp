#include "Common.h"

CommonClass::CommonClass()
{
	motionNum = 0;
	channelNum = 0;
	compNum = 0;
	dataLength = 0;

}


bool CommonClass::LoadStateData(char *fileName)
{
	FILE *fp;

	fp = fopen(fileName, "r");
	if (fp == NULL) {
		printf("%s can not open.\n", fileName);
		return false;
	}
	fscanf(fp, "%d", &motionNum);
	fscanf(fp, "%d", &channelNum);
	fscanf(fp, "%d", &compNum);
	fscanf(fp, "%d", &dataLength);

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
	fprintf(fp, "%d\n", motionNum);
	fprintf(fp, "%d\n", channelNum);
	fprintf(fp, "%d\n", compNum);
	fprintf(fp, "%d\n", dataLength);


	fclose(fp);

	return true;
}