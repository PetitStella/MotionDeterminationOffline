//=======================================だけのプログラム
// 学習に必要なパラメータ（動作数，チャネル数など）は外部ファイル sta=========================
////  - 3Dプリンタ義手用筋電パターン学習プログラム -
//// フォルダ「MotionData」に保存した学習データを使って学習するte.dat から設定する
// 2017/09/22	作成
// Created by Akira Furui
//================================================================

#include <stdio.h>
#include <math.h>

#define _CRT_SECURE_NO_WARNINGS

#include "./INCLUDE/R-LlgmnClass.h"
#include "./INCLUDE/MemoryClass.h"
#include "./INCLUDE/Common.h"

#pragma warning(disable:4996)

bool Config(void);
bool Training(void);

int main(void)
{
    char select = NULL;

    while (select != 0x1b)
    {
        system("cls");
        printf("\n");
        printf("---------------------------------------------------\n");
        printf("         LEARNING EMG PATTERN with R-LLGMN         \n");
        printf("---------------------------------------------------\n");
        printf("\n");
        printf(" 0: Setting state parameters\n");
        printf(" 1: Training\n");
        printf("\n");
        printf("---------------------------------------------------\n");

        cin >> select;

        switch (select)
        {
            case '0':
                system("cls");
                Config();
                break;
            case '1':
                system("cls");
                Training();
                break;
            default:
                system("cls");
                break;
        }

    }

    return 0;
}


// 学習に必要なパラメータを変更する
bool Config(void)
{
    CommonClass Com;


    if (Com.LoadStateData("./state.dat") == false) return false;
    printf("\nSet number of motion\tnow: %d  changed to: ", Com.motionNum);
    scanf("%d", &Com.motionNum);
    printf("\nSet number of channel\tnow: %d  changed to: ", Com.channelNum);
    scanf("%d", &Com.channelNum);
    printf("\nSet number of component\tnow: %d  changed to: ", Com.compNum);
    scanf("%d", &Com.compNum);
    printf("\nSet length of train data\tn0ow: %d  changed to: ", Com.dataLength);
    scanf("%d", &Com.dataLength);

    // パラメータを保存
    if (Com.SaveStateData("./state.dat") == false) return false;
    return true;
}


bool Training(void){
    MemoryClass<double> Mem;
    CommonClass Com;
    RLLgmnMouse RLL;
    LearningFunction lerningdata;

    FILE *rfp;
    FILE *wfp;

    // Load parameters for learning
    if(!Com.LoadStateData("./state.dat")) return false;

    char fName[100];
    bool loopFlag = true;
    int counter;
    double tmpData;

    wfp = fopen("./Data/LearningData.dat", "w");
    printf("\n-----------------------------\n");
    printf(" [Generate Train data]\n");
    for (int m=0; m < Com.motionNum; m++) {
        printf("\tLoad: MOTION%d.TXT\n", m + 1);
        counter = 0;
        sprintf(fName, "./MotionData/MOTION%d.TXT", m+1);
        if ((rfp = fopen(fName, "r")) == NULL) {
            printf("Can not open file: MOTION%d.TXT", m+1);
            return false;
        }
        while (loopFlag) {
            if (counter < 500) {
                for (int c = 0; c < Com.channelNum; c++) {
                    fscanf(rfp, "%lf", &tmpData);
                }
            }
            else if ((counter >= 500)&&(counter < (500+Com.dataLength))) {
                for (int c = 0; c < Com.channelNum; c++) {
                    fscanf(rfp, "%lf", &tmpData);
                    fprintf(wfp, "%lf\t", tmpData);
                }
                fprintf(wfp, "\n");
            }
            else {
                loopFlag = false;
            }
            counter++;
        }
        loopFlag = true;
        fclose(rfp);
    }
    fclose(wfp);
    printf("\tFinish!\n\n");
    printf("-----------------------------\n");

    int dataLength_e = 4;
    RLL.SetChannelNumber(Com.channelNum);
    RLL.SetClassNumber(Com.motionNum);
    RLL.SetComponentNumber(Com.compNum);
    RLL.SetDataLength(Com.dataLength);
    RLL.SetDataLength_E(dataLength_e);
    RLL.SetSampleNumber(1);
    RLL.SetStateNumber(1);
    RLL.SetTeacherNumber(Com.motionNum);


    if (RLL.InitializeRLLGMNLearning() == false) {
        printf("Error! Ini\r\n");
        return false;
    }

    if (RLL.LoadLearningData("./Data/LearningData.dat") == false) {
        printf("Error! -> Load [LearningData.dat]\n");
        return false;
    }

    if (RLL.CreateTeachingSignal() == false) {
        printf("Error! -> CreateTeachingSignal");
        return false;
    }


    printf(" [Start Training]\n");
    printf("\tSteps        Evaluate function\n");
    while (!RLL.LearningNetWork()) {
        RLL.GetLearningData(&lerningdata);
        printf("\t %.1lf %%          J=%e\r", 100 * double(lerningdata.ta_count) / 1000.0, lerningdata.energy);
    }
    printf("\n");

    // Save weight
    RLL.SaveWeight("./data/Weight");

    RLL.CheckLearning();

    RLL.Finish();

    return true;
}
