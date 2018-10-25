//=======================================だけのプログラム
// 学習に必要なパラメータ（動作数，チャネル数など）は外部ファイル sta=========================
////  - 3Dプリンタ義手用筋電パターン学習プログラム -
//// フォルダ「MotionData」に保存した学習データを使って学習するte.dat から設定する
// 2017/09/22	作成
// Created by Akira Furui
//================================================================

// TODO: Make development variables for Mac and Win
// TODO: Refactoring

#include <stdio.h>
#include <math.h>

#define _CRT_SECURE_NO_WARNINGS

#include "./INCLUDE/R-LlgmnClass.h"
#include "./INCLUDE/MemoryClass.h"
#include "./INCLUDE/Common.h"
#include "./INCLUDE/EnvSelect.h"

#pragma warning(disable:4996)

#define CONTROL_COUNT 0
#define CONTROL_COUNT_COMBINE 20
#define TOKEN_COUNT 20


bool Config(void);

bool Training(void);

bool Testing(void);

int main(void) {
    char select = NULL;

    while (select != 0x1b) {
        ClearConsole;
        printf("\n");
        printf("---------------------------------------------------\n");
        printf("         LEARNING EMG PATTERN with R-LLGMN         \n");
        printf("---------------------------------------------------\n");
        printf("\n");
        printf(" 0: Setting state parameters\n");
        printf(" 1: Training\n");
        printf(" 2: Testing (Motion determination)");
        printf("\n");
        printf("---------------------------------------------------\n");

        cin >> select;

        switch (select) {
            case '0':
                ClearConsole;
                Config();
                break;
            case '1':
                ClearConsole;
                Training();
                break;
            case '2':
                ClearConsole;
                Testing();
                break;
            default:
                ClearConsole;
                break;
        }

    }

    return 0;
}


// 学習に必要なパラメータを変更する
bool Config(void) {
    CommonClass Com;


    if (Com.LoadStateData("./state.dat") == false) return false;
    printf("\nSet number of single motions\tnow: %d  changed to: ", Com.singleMotionNum);
    scanf("%d", &Com.singleMotionNum);
    printf("\nSet number of combined motions\tnow: %d  changed to: ", Com.combineMotionNum);
    scanf("%d", &Com.combineMotionNum);
    printf("\nSet number of channel\tnow: %d  changed to: ", Com.channelNum);
    scanf("%d", &Com.channelNum);
    printf("\nSet number of component\tnow: %d  changed to: ", Com.compNum);
    scanf("%d", &Com.compNum);
    printf("\nSet length of train data\tn0ow: %d  changed to: ", Com.trainDataLength);
    scanf("%d", &Com.trainDataLength);
    printf("\nSet length of time series\tn0ow: %d  changed to: ", Com.timeLength);
    scanf("%d", &Com.timeLength);

    // パラメータを保存
    if (Com.SaveStateData("./state.dat") == false) return false;
    return true;
}


bool Training(void) {
    MemoryClass<double> Mem;
    CommonClass Com;
    RLLgmnMouse RLL;
    LearningFunction lerningdata;

    FILE *rfp;
    FILE *wfp;

    // Load parameters for learning
    if (!Com.LoadStateData("./state.dat")) return false;

    char fName[100];
    bool loopFlag = true;
    int counter;
    double tmpData;

    // Create learning data
    wfp = fopen("./Data/LearningData.dat", "w");
    printf("\n-----------------------------\n");
    printf(" [Generate Train data]\n");
    for (int m = 0; m < Com.singleMotionNum; m++) {
        printf("\tLoad: MOTION%d.TXT\n", m + 1);
        counter = 0;
        sprintf(fName, "./MotionData/MOTION%d.TXT", m + 1);
        if ((rfp = fopen(fName, "r")) == NULL) {
            printf("Can not open file: MOTION%d.TXT", m + 1);
            return false;
        }
        while (loopFlag) {
            if (counter < 500) {
                for (int c = 0; c < Com.channelNum; c++) {
                    fscanf(rfp, "%lf", &tmpData);
                }
            } else if ((counter >= 500) && (counter < (500 + Com.trainDataLength))) {
                for (int c = 0; c < Com.channelNum; c++) {
                    fscanf(rfp, "%lf", &tmpData);
                    fprintf(wfp, "%lf\t", tmpData);
                }
                fprintf(wfp, "\n");
            } else {
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
    RLL.SetClassNumber(Com.singleMotionNum);
    RLL.SetComponentNumber(Com.compNum);
    RLL.SetDataLength(Com.trainDataLength);
    RLL.SetDataLength_E(dataLength_e);
    RLL.SetSampleNumber(1);
    RLL.SetStateNumber(1);
    RLL.SetTeacherNumber(Com.singleMotionNum);


    if (!RLL.InitializeRLLGMNLearning()) {
        printf("Error! Ini\r\n");
        return false;
    }

    if (!RLL.LoadLearningData("./Data/LearningData.dat")) {
        printf("Error! -> Load [LearningData.dat]\n");
        return false;
    }

    if (!RLL.CreateTeachingSignal()) {
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

bool Testing(void) {
    MemoryClass<double> Mem;
    CommonClass Com;
    RLLgmnMouse RLL;
    LearningFunction lerningdata;

    FILE *rfp;
    FILE *wfp;

    char fName[100];

    double *inputData;
    double *testData;
    double **ADdata;
    double *vector;
    int allMotionNum = 0;

    double force = 0.0;
    int estimate_motion = 0;
    int pre_estimate_motion = 0;
    int decide_motion = 0;
    int pre_decide_motion = 0;
    int motion_count = 0;

    // Load parameters for learning
    if (!Com.LoadStateData("./state.dat")) return false;

    allMotionNum = Com.singleMotionNum + Com.combineMotionNum;
    inputData = Mem.matrix1_alloc(Com.channelNum);
    testData = Mem.matrix1_alloc(Com.channelNum);
    ADdata = Mem.matrix2_alloc(Com.timeLength, Com.channelNum);
    vector = Mem.matrix1_alloc(Com.singleMotionNum);

    // Give initial values for time series data
    for (int i = 0; i < Com.timeLength; i++) {
        for (int j = 0; j < Com.channelNum; j++) {
            ADdata[i][j] = 0.0001;
        }
    }

    for (int i = 0; i < Com.channelNum; i++) {
        inputData[i] = 0.0;
        testData[i] = 0.0;
    }
    for (int i = 0; i < Com.singleMotionNum; i++) {
        vector[i] = 0.0;
    }

    RLL.SetChannelNumber(Com.channelNum);
    RLL.SetClassNumber(Com.singleMotionNum);
    RLL.SetComponentNumber(Com.compNum);
    RLL.SetDataLength(Com.trainDataLength);
    RLL.SetDataLength_E(Com.timeLength);
    RLL.SetSampleNumber(1);
    RLL.SetStateNumber(1);
    RLL.SetTeacherNumber(Com.singleMotionNum);

    if (!RLL.InitializeRLLGMN()) {
        printf("Error! in initializing R-LLGMN\n");
        return false;
    }

    if (!RLL.LoadWeight("./Data/Weight")) {
        printf("Error! in loading weights\n");
        return false;
    }
    if (!RLL.InitializeOutput()) {
        printf("Error! in initializing R-LLGMN outputs\n");
        return false;
    }

    // Settings for motion generation model-----------------
    double **T_vector;
    double *output_vector = NULL;

    int classified_combined_motion = 0;
    int decided_combined_motion = 0;
    int pre_classified_combined_motion = 0;
    int pre_decided_combined_motion = 0;
    int motion_count_comb = 0;

    double **modifying_vector;
    int token = 0;
    int pre_token = 0;
    int token_count = 0;
    double *similarity = NULL;
    double *modified_similarity = NULL;
    double max_modified_similarity = 0;

    T_vector = Mem.matrix2_alloc(allMotionNum, Com.singleMotionNum);
    output_vector = Mem.matrix1_alloc(allMotionNum);
    modifying_vector = Mem.matrix2_alloc(allMotionNum + 1, allMotionNum);
    similarity = Mem.matrix1_alloc(allMotionNum);
    modified_similarity = Mem.matrix1_alloc(allMotionNum);

    //-------------------------------------------------------

    printf("Load files...\n\n");
    // File loading
    for (int i = 0; i < allMotionNum; i++) {
        for (int j = 0; j < Com.singleMotionNum; j++) {
            T_vector[i][j] = 0.0;
        }
    }

    if ((rfp = fopen("./Data/Teach", "rt")) == NULL) {
        printf("Error in loading [Teach]\n");
        return false;
    }
    for (int i = 0; i < allMotionNum; i++) {
        for (int j = 0; j < Com.singleMotionNum; j++) {
            fscanf(rfp, "%lf", &T_vector[i][j]);
        }
    }
    fclose(rfp);
    printf("./Data/Teach  Loaded!\n");

    for (int i = 0; i < allMotionNum + 1; i++) {
        for (int j = 0; j < allMotionNum; j++) {
            modifying_vector[i][j] = 0.0;
        }
    }

    if ((rfp = fopen("./Data/Modify", "rt")) == NULL) {
        printf("Error in loading [Modify]\n");
        return false;
    }
    for (int i = 0; i < allMotionNum + 1; i++) {
        for (int j = 0; j < allMotionNum; j++) {
            fscanf(rfp, "%lf\t", &modifying_vector[i][j]);
        }
    }
    fclose(rfp);
    printf("./Data/Modify  Loaded!\n");



    int read_motion_num;
    int read_trial_num;
    printf("Motion number: ");
    scanf("%d", &read_motion_num);
    printf("Trial number: ");
    scanf("%d", &read_trial_num);

    sprintf(fName, "./Data/Testing/識別%d回目/動作%d/Check/alpha.dat", read_trial_num, read_motion_num);
    if((rfp = fopen(fName, "r")) == NULL){
        printf("No input file\n");
        return false;
    }
    sprintf(fName, "./Results/m%d_%d.dat", read_motion_num,read_trial_num);
    wfp = fopen(fName, "w");
    fprintf(wfp, "combined_motion\tsingle_motion\tforce");
    for (int i = 0; i < Com.singleMotionNum; i++) {
        fprintf(wfp, "\tPostProb_M%d", i + 1);
    }
    fprintf(wfp, "\n");
    // Start testing
    printf("Start motion determination!\n");
    while (fscanf(rfp, "%lf", &inputData[0]) != EOF) {
        for (int i = 1; i < Com.channelNum; i++) {
            fscanf(rfp, "%lf", &inputData[i]);
        }

        for (int i = 1; i < Com.timeLength; i++) {
            for (int j = 0; j < Com.channelNum; j++) {
                ADdata[Com.timeLength - i][j] = ADdata[Com.timeLength - 1 - i][j];
            }
        }

        double sum_inputData = 0.0;
        for (int i = 0; i < Com.channelNum; i++) {
            sum_inputData += inputData[i];
            force = sum_inputData / (double) Com.channelNum;
        }
        for (int i = 0; i < Com.channelNum; i++) {
            if (sum_inputData == 0.0) {
                testData[i] = 0.0;
            } else {
                testData[i] = inputData[i] / sum_inputData;
            }
        }

        for (int i = 0; i < Com.channelNum; i++) {
            ADdata[0][i] = testData[i];
        }

        for (int i = 0; i < Com.timeLength; i++) {
            RLL.HMN_FC(i, ADdata);
        }
        RLL.GetPosterioriProbability(vector);
        RLL.InitializeOutput();
        // Estimate motion
        if (force > Com.forceThreshold) {
            double P_max = 0.0;
            for (int i = 0; i < Com.singleMotionNum; i++) {
                if (P_max < vector[i]) {
                    P_max = vector[i];
                    estimate_motion = i + 1;
                }
            }
            if (estimate_motion == pre_estimate_motion) {
                motion_count++;
                if (motion_count < CONTROL_COUNT) {
                    decide_motion = pre_decide_motion;  // suspend
                } else {
                    decide_motion = estimate_motion;    // decide
                }
            } else {
                decide_motion = pre_decide_motion;  // suspend
            }
        } else {
            estimate_motion = 0;
            decide_motion = 0;
            motion_count = 0;
        }
        pre_decide_motion = decide_motion;
        pre_estimate_motion = estimate_motion;

        // Motion determination based on Motion generation model
        if (force > Com.forceThreshold) {
            // Calculate: inter vector distance
            for (int i = 0; i < allMotionNum; i++) {
                double tmp = 0.0;
                for (int j = 0; j < Com.singleMotionNum; j++) {
                    tmp += (T_vector[i][j] - vector[j]) * (T_vector[i][j] - vector[j]);
                }
                output_vector[i] = sqrt(tmp);
            }
            // Calculate: similarity & modified similarity
            for (int i = 0; i < allMotionNum; i++) {
                similarity[i] = 1.0 - output_vector[i] / sqrt(2.0);
                modified_similarity[i] = similarity[i] * modifying_vector[token][i];
            }
            max_modified_similarity = 0;        // Initialized
            classified_combined_motion = 0;     // Initialized
            for (int i = 0; i < allMotionNum; i++) {
                if (modified_similarity[i] > max_modified_similarity) {
                    max_modified_similarity = modified_similarity[i];
                    classified_combined_motion = i + 1;
                }
            }
            // Move: token
            if (classified_combined_motion == pre_classified_combined_motion) {
                token_count++;
                if (token_count < TOKEN_COUNT) {
                    token = pre_token;
                } else {
                    token = classified_combined_motion;
                }
            } else {
                token = pre_token;
                token_count = 0;
            }
            if (token == pre_token) {
                motion_count_comb++;
                if (motion_count_comb < CONTROL_COUNT_COMBINE)
                    decided_combined_motion = pre_decided_combined_motion;  // stay
                else
                    decided_combined_motion = token;                        // transit
            } else {
                decided_combined_motion = pre_decided_combined_motion;      // stay
                motion_count_comb = 0;
            }
        } else {
            token = 0;
            motion_count_comb = 0;
            token_count = 0;
            classified_combined_motion = 0;
            decided_combined_motion = 0;
        }

        pre_decided_combined_motion = decided_combined_motion;
        pre_classified_combined_motion = classified_combined_motion;
        pre_token = token;

        fprintf(wfp, "%d\t%d\t%lf", decided_combined_motion, decide_motion, force);
        for (int i = 0; i < Com.singleMotionNum; i++) {
            fprintf(wfp, "\t%lf", vector[i]);
        }
        fprintf(wfp, "\n");
    }
    fclose(rfp);
    fclose(wfp);

    Mem.matrix1_free(inputData);
    Mem.matrix1_free(testData);
    Mem.matrix2_free(ADdata);
    Mem.matrix1_free(vector);
    Mem.matrix2_free(T_vector);
    Mem.matrix1_free(output_vector);
    Mem.matrix2_free(modifying_vector);
    Mem.matrix1_free(similarity);
    Mem.matrix1_free(modified_similarity);

    return true;
}
