#pragma once
// R-LlgmnForMouse.h: RLLgmnMouse クラスのインターフェイス
//
//----------------------------------------
//                              2004/11/09
//          R-LLGMNクラス
//          　マウスバージョン
//
//                               Ver.   0.22
// Ver. 0.22
//  修正完了（とりあえず）
// Ver. 0.21
//  教師信号作成部分修正（論文と等しくした）
// Ver. 0.2
//  学習不具合部分修正
//  変数など整理
// Ver. 0.1
//  1stVer.完成
//  とりあえず動作確認済み
// Ver. 0(2004.04.01)
//  作成
//----------------------------------------
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
//━クラス用インクルード━━━━━━━━━━━━━━━━━━
#include <stdio.h>
#include <random>
#include <cmath>
#include <time.h>
#include <iostream>
#include "MemoryClass.h"

using namespace std;


//━クラス用定義━━━━━━━━━━━━━━━━━━━━━━
#define RLLGMN_VER 0			//バージョン管理用
#define LEARNING_DATA_SIZE 20	//学習信号のデータ数のデフォルト
#define ADD_DATA_SIZE 20        //補正用データ数
#define TA_TIME 1.0				//ターミナルアトラクタ時間パラメータ
#define TA_SAMPLING_TIME 0.001	//ターミナルアトラクタサンプリング時間パラメータ
#define TA_BETA_R 0.95				//ターミナルアトラクタベータパラメータ
#define Min_J 10.0
#define	pi()	3.1415
//━ネットワーク全体を構成するパラメータ━━━━━━━━━━
struct NetworkFunction {
	int		data_length;					//学習の長さ
	int		data_length_e;					//++識別データ長
	int		component_number;				//コンポーネントの数
	int		sample_number;					//サンプルデータ数
	int		class_number;					//クラス数
	int		state_number;					//状態数
	int		channel_number;					//入力信号数（電極数）
	double	*****weight;					//重み用変数
	double	*****best_weight;				//最も誤差が少ないときの重みを保存
	double	**input_vector;					//線形化前の値（フィルタ後の値）
	double	**x1;							//一層の入力＆出力[データ数]×[チャンネル数]
	double	*****x2;						//二層の出力
											//	double	*****ix2;						//二層の内部変数
	double	****x3;							//三層の出力
	double	****ix3;						//三層の内部変数
	double	***x4;							//四層の出力
	double	***ix4;							//四層の内部変数
	double	**x4_in;
	//	double	**pre4;							//1時刻前の4層の出力
	double	*sum4_2;						//4層の出力の分母
	double	**x5;							//五層の出力
	double	*x5_in;
	double	*emg_aut_gain;					//各電極のゲイン([電極数])（各チャンネルの最大筋力の逆数）
	double	*motion_gain;					//ロボット前腕動作のゲイン([動作数])（動作時の最大推定筋力の逆数）
	char	*file_name_parameter;			//ネットワークを構成するパラメータ（コンポーネント数）のファイルネーム
};
//━学習を行う際に必要なパラメータ━━━━━━━━━━━━━
struct LearningFunction {
	int		ta_count;				//ターミナルアトラクタ学習回数
	double	ta_beta;				//ターミナルアトラクタのβ
	double	ta_gamma;				//ターミナルアトラクタのγ
	double	ta_eta;					//ターミナルアトラクタのΘ
	double	ta_time;				//ターミナルアトラクタ指定時間
	double	ta_sampling_time;		//データのサンプリング時間
									//	double	beta;
	double	rita;
	int		teacher_number;			//教師信号方向数
	int		sample_number;			//学習用サンプル数
	double	**emg_sample;			//学習用データ格納用([電極数][サンプル数×動作数])
	double	**teach;				//教師信号
	double	*teach_orig;			//学習角度
	double	******dw;				//重み誤差①
	double	*****Dw;				//重み誤差②
	double	*****O14;				//後ろ向き計算用
	double	***BPTT;				//後ろ向き計算用
									//	double	*jw;					//ターミナルアトラクタによる修正値(通常の誤差もこちらに代入される)
									//	double	*wdt;					//時間当たりの重みの変化量
									//	double	*wdt_pre;				//ひとつ前の時間当たりの重みの変化量
	double	energy;					//評価関数の値
	double	energy_ini;				//評価関数の初期値
	double	best_energy;			//評価関数の初期値
	double	ratio;					//評価関数の減少の割合
	double	min;					//評価関数の割合の最小値を代入するための変数
	int		difficult_flag;			//学習の難しいときのフラグ
	double	*input_vector;			//学習時の受け渡し用
	double	sum_dw;
	double	gamma_dw;

	int		counter;
};

class RLLgmnMouse
{
	bool f_channel_number;
	bool f_data_length;
	bool f_data_length_e;		//++
	bool f_sample_number;
	bool f_component_number;
	bool f_class_number;
	bool f_state_number;
	bool f_teacher_number;
	bool f_learning;				//学習用の場合true
	FILE *filecheck;
public:
	RLLgmnMouse();
	virtual ~RLLgmnMouse();
	virtual bool SetChannelNumber(int value);
	virtual bool SetClassNumber(int value);
	virtual bool SetComponentNumber(int value);
	virtual bool SetDataLength(int value);
	virtual bool SetDataLength_E(int value);		//++
	virtual bool SetTeacherNumber(int value);
	virtual bool SetSampleNumber(int value);
	virtual bool SetStateNumber(int value);
	virtual bool CreateTeachingSignal(void);							//教師信号生成関数
	virtual bool SetTerminalAttractorTime(double value);				//TAの時間パラメータ（デフォルト1.0）
	virtual bool SetTerminalAttractorSamplingTime(double value);		//TAのサンプリング時間パラメータ（デフォルト0.001）
	virtual bool SetTerminalAttractorBeta(double value);				//TAのベータパラメータ（デフォルト0.5）
	virtual bool InitializeRLLGMN(void);								//R-LLGMNの初期化
	virtual bool InitializeRLLGMNLearning(void);						//R-LLGMNの学習の初期化
	virtual bool InitializeOutput(void);
	virtual bool LearningNetWork(void);									//ネットワーク学習
	virtual bool FrontCalculation(double **input_vector);				//前向き計算(学習用)
	virtual bool HMN_FC(int number, double **input_vector);				//前向き計算(パターン識別用)
	virtual bool BackPropagationThroughTime(int counter);				//後ろ向き計算
	virtual bool CoeffChg(double J);
	virtual bool GetPosterioriProbability(double *value);				//事後確率出力
	virtual bool GetNetworkData(NetworkFunction *data);					//各パラメータ出力
	virtual bool GetLearningData(LearningFunction *data);				//各パラメータ出力
	virtual bool LoadLearningData(char *file_name);						//学習用信号読み込み
	virtual bool SaveWeight(char *file_name);							//重み保存(チャンネルゲインなどもこのファイルに保存)
	virtual bool LoadWeight(char *file_name);							//重み読み込み
	virtual void Finish(void);											//終了処理

	virtual bool CheckLearning(void);									//学習データを入力して学習できてるか確認する

protected:
	NetworkFunction		NF; //NetWork data				ネットワーク全般に必要な変数
	LearningFunction	LF; //Learning data			学習に必要な変数
	virtual bool CreateNetWork(void);					//ネットワークの動的確保
	virtual bool FileCheck(bool write, bool tab, double data);

};

