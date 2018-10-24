// R-LlgmnForMouse.cpp: RLLgmnMouse クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "R-LlgmnClass.h"
//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
//-----------------------------------------
//　ユニット、重みなどの領域を動的に確保
//　＜内部関数＞
//-----------------------------------------
bool RLLgmnMouse::CreateNetWork(void)
{
	MemoryClass<double> Mem;

	//重み用変数
	NF.weight =
		Mem.matrix5_alloc(NF.class_number, NF.state_number, NF.state_number, NF.component_number,
			1 + NF.channel_number*(NF.channel_number + 3) / 2);
	//最もよい重み用変数
	NF.best_weight =
		Mem.matrix5_alloc(NF.class_number, NF.state_number, NF.state_number, NF.component_number,
			1 + NF.channel_number*(NF.channel_number + 3) / 2);
	//入出力変数
	NF.x1 =
		Mem.matrix2_alloc(NF.data_length, 1 + NF.channel_number*(NF.channel_number + 3) / 2);
	NF.x2 =
		Mem.matrix5_alloc(NF.data_length, NF.class_number, NF.state_number, NF.state_number, NF.component_number);
	NF.x3 =
		Mem.matrix4_alloc(NF.data_length, NF.class_number, NF.state_number, NF.state_number);
	NF.ix3 =
		Mem.matrix4_alloc(NF.data_length, NF.class_number, NF.state_number, NF.state_number);
	NF.x4 =
		Mem.matrix3_alloc(NF.data_length, NF.class_number, NF.state_number);
	NF.ix4 =
		Mem.matrix3_alloc(NF.data_length, NF.class_number, NF.state_number);
	NF.x4_in =
		Mem.matrix2_alloc(NF.class_number, NF.state_number);
	NF.x5 =
		Mem.matrix2_alloc(NF.data_length, NF.class_number);
	NF.x5_in =
		Mem.matrix1_alloc(NF.class_number);
	NF.sum4_2 =
		Mem.matrix1_alloc(NF.data_length);
	NF.emg_aut_gain =
		Mem.matrix1_alloc(NF.channel_number);		//各電極のゲイン([電極数])
	NF.motion_gain =
		Mem.matrix1_alloc(LF.teacher_number);			//ロボット前腕動作のゲイン([動作数])
	NF.input_vector =
		Mem.matrix2_alloc(NF.data_length, NF.channel_number);		//フィルタ後の入力
	if (f_learning == true)//学習用データ数が0ならば学習しない
	{
		LF.emg_sample =
			Mem.matrix2_alloc(NF.sample_number*LF.teacher_number*NF.data_length, NF.channel_number);
		LF.teach =
			Mem.matrix2_alloc(NF.sample_number*LF.teacher_number, NF.class_number + 1);				//教師信号
		LF.teach_orig =
			Mem.matrix1_alloc(NF.sample_number*LF.teacher_number*NF.data_length);//NF.sample_number*LF.teacher_number);                            //学習角度
		LF.Dw =
			Mem.matrix5_alloc(NF.class_number, NF.state_number, NF.state_number, NF.component_number,
			(1 + NF.channel_number*(NF.channel_number + 3) / 2));
		LF.dw =
			Mem.matrix6_alloc(NF.sample_number*LF.teacher_number, NF.class_number, NF.state_number, NF.state_number, NF.component_number,
				1 + NF.channel_number*(NF.channel_number + 3) / 2); //重み誤差
		LF.O14 =
			Mem.matrix5_alloc(NF.data_length, NF.class_number, NF.state_number, NF.class_number, NF.state_number);
		LF.BPTT =
			Mem.matrix3_alloc(NF.data_length, NF.class_number, NF.state_number);
	}
	else {
		printf("No Learning\n");
	}
	return true;
}

//-----------------------------------------
//　コンストラクタ
//　各パラメータのの初期化
//　＜内部関数＞
//-----------------------------------------
RLLgmnMouse::RLLgmnMouse()
{
	f_channel_number = false;
	f_class_number = false;
	f_component_number = false;
	f_data_length = false;
	f_learning = false;
	f_sample_number = false;
	f_state_number = false;
	f_teacher_number = false;

	NF.weight = NULL;
	NF.best_weight = NULL;
	NF.x1 = NULL;
	NF.x2 = NULL;
	NF.x3 = NULL;
	NF.x4 = NULL;
	NF.x5 = NULL;
	NF.ix3 = NULL;
	NF.ix4 = NULL;
	NF.emg_aut_gain = NULL;
	NF.motion_gain = NULL;
	NF.input_vector = NULL;

	LF.energy = 0.0;
	LF.energy_ini = 1.0;
	LF.best_energy = 0.0;
	LF.ratio = 1.0;
	LF.min = 10.0;
	LF.emg_sample = NULL;
	LF.teach = NULL;
	LF.teach_orig = NULL;
	LF.dw = NULL;

	LF.ta_count = 0;
	LF.ta_time = TA_TIME;
	LF.ta_sampling_time = TA_SAMPLING_TIME;
	LF.ta_beta = TA_BETA_R;

	LF.counter = 0;

}

//-----------------------------------------
//　デストラクタ
//　動的メモリーを解放
//　＜内部関数＞
//-----------------------------------------
RLLgmnMouse::~RLLgmnMouse()
{
	Finish();
}

//-----------------------------------------
//  チャンネル数設定
//-----------------------------------------
bool RLLgmnMouse::SetChannelNumber(int value)
{
	NF.channel_number = value;
	f_channel_number = true;
	return true;
}
bool RLLgmnMouse::SetClassNumber(int value)
{
	NF.class_number = value;
	f_class_number = true;
	return true;
}
bool RLLgmnMouse::SetComponentNumber(int value)
{
	NF.component_number = value;
	f_component_number = true;
	return true;
}
bool RLLgmnMouse::SetDataLength(int value)
{
	NF.data_length = value;
	f_data_length = true;
	return true;
}
bool RLLgmnMouse::SetDataLength_E(int value)
{
	NF.data_length_e = value;
	f_data_length_e = true;
	return true;
}
bool RLLgmnMouse::SetSampleNumber(int value)
{
	NF.sample_number = value;
	f_sample_number = true;
	return true;
}
bool RLLgmnMouse::SetStateNumber(int value)
{
	NF.state_number = value;
	f_state_number = true;
	return true;
}
bool RLLgmnMouse::SetTeacherNumber(int value)
{
	LF.teacher_number = value;
	f_teacher_number = true;
	return true;
}
bool RLLgmnMouse::SetTerminalAttractorTime(double value)
{
	LF.ta_time = value;
	return true;
}
bool RLLgmnMouse::SetTerminalAttractorSamplingTime(double value)
{
	LF.ta_sampling_time = value;
	return true;
}
bool RLLgmnMouse::SetTerminalAttractorBeta(double value)
{
	LF.ta_beta = value;
	return true;
}

//-----------------------------------------
//　①R-LLGMNMの初期化関数(識別用)
//　識別を行うときに必ず実行する。
//　事前に各パラメータに必要事項を代入しておかないと怒られます．
//-----------------------------------------
bool RLLgmnMouse::InitializeRLLGMN(void)
{
	int i;
	//必要なパラメータのチェック
	if (f_channel_number == false) {
		//		cout<<"Error!! -> f_channel_number is false"<<endl;
		return false;
	}
	if (f_class_number == false) {
		//		cout<<"Error!! -> f_class_number is false"<<endl;
		return false;
	}
	if (f_component_number == false) {
		//		cout<<"Error!! -> f_component_number is false"<<endl;
		return false;
	}
	if (f_data_length == false) {
		//		cout<<"Error!! -> f_data_length is false"<<endl;
		return false;
	}
	if (f_sample_number == false) {
		//		cout<<"Error!! -> f_sample_number is false"<<endl;
		return false;
	}
	if (f_state_number == false) {
		//		cout<<"Error!! -> f_state_number is false"<<endl;
		return false;
	}
	f_learning = false;

	//ネットワークに必要なメモリーを確保
	if (CreateNetWork() == false) {
		//		cout<<"Error!! -> CreateNetwork() is false"<<endl;
		return false;
	}

	for (i = 0; i<NF.class_number; i++)
		NF.x5_in[i] = 0.0;

	return true;
}

//-----------------------------------------
//　①R-LLGMNの初期化関数(学習用)
//　学習を行う時に初期化を行う関数
//　事前に各パラメータに必要事項を代入しておかないと怒られます．
//-----------------------------------------
bool RLLgmnMouse::InitializeRLLGMNLearning(void)
{
	int i, j, k, l, m;

	//必要なパラメータのチェック
	if (f_channel_number == false)		return false;
	if (f_class_number == false)		return false;
	if (f_component_number == false)	return false;
	if (f_data_length == false)		return false;
	if (f_data_length_e == false)		return false;
	//	if(f_sample_number==false)		return false;
	//	if(f_state_number==false)		return false;
	//	if(f_teacher_number==false)		return false;
	f_learning = true;

	//ネットワークに必要なメモリーを確保
	if (CreateNetWork() == false)return false;

	if ((filecheck = fopen("./Data/Learning/CheckData.xls", "wt")) == NULL)
		return false;

	//重みの初期値を乱数で発生
	srand((unsigned)time(NULL));
	for (i = 0; i<NF.class_number; i++) {
		for (j = 0; j<NF.state_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				for (l = 0; l<NF.component_number; l++) {
					for (m = 0; m<(1 + NF.channel_number*(NF.channel_number + 3) / 2); m++) {
						NF.weight[i][j][k][l][m]
							= ((double)rand() / 32768.0 - 0.5) / 50.0;
						NF.best_weight[i][j][k][l][m]
							= ((double)rand() / 32768.0 - 0.5) / 50.0;
					}
				}
			}
		}
	}
	return true;
}

//-----------------------------------------
//  状態の初期化
//-----------------------------------------
bool RLLgmnMouse::InitializeOutput(void)
{
	int	i, j;
	for (i = 0; i<NF.class_number; i++) {
		for (j = 0; j<NF.state_number; j++) {
			NF.x4_in[i][j] = 1.0;
		}
	}
	return true;
}

//-----------------------------------------
//　教師信号生成関数
//　＜内部関数＞
//-----------------------------------------
bool RLLgmnMouse::CreateTeachingSignal(void)
{
	int		i, j, k, mtn;

	double	fai_c;
	double	fai_c1;
	double	fai_c2;
	//FILE	*fp;

	//if ((fp = fopen("./Data/Learning/TeachingSignal3.txt", "rt")) != NULL) {
		//教師信号の作成(5指動作用)
		for (i = 0; i<(NF.sample_number*NF.class_number); i++) {
			for (mtn = 0; mtn < NF.class_number; mtn++) {
				//教師信号の初期化
				//LF.teach[i][mtn] = i + mtn;//0.0;
										   //教師信号読み込み
				//fscanf(fp, "%lf", &LF.teach[i][mtn]);
				if (i == mtn) {
					LF.teach[i][mtn] = 1;
				}
				else {
					LF.teach[i][mtn] = 0;
				}
			}

			/*
			//教師信号の作成(マウス用)
			for(mtn=1;mtn<=(NF.class_number+1);mtn++){
			fai_c	=2*pi()*mtn/NF.class_number;
			fai_c1	=2*pi()*(mtn-1)/NF.class_number;
			fai_c2	=2*pi()*(mtn-2)/NF.class_number;

			if(fai_c1<=LF.teach_orig[i+(i*NF.data_length)] && LF.teach_orig[i+(i*NF.data_length)]<=fai_c)
			LF.teach[i][mtn-1]=(cos(fabs(LF.teach_orig[i+(i*NF.data_length)]-fai_c1))-cos(fabs(fai_c-LF.teach_orig[i+(i*NF.data_length)]))*cos(2*pi()/NF.class_number))/
			((cos(fabs(LF.teach_orig[i+(i*NF.data_length)]-fai_c1))+cos(fabs(fai_c-LF.teach_orig[i+(i*NF.data_length)])))*(1-cos(2*pi()/NF.class_number)));
			else if(fai_c2<=LF.teach_orig[i+(i*NF.data_length)] && LF.teach_orig[i+(i*NF.data_length)]<=fai_c1)
			LF.teach[i][mtn-1]=(cos(fabs(LF.teach_orig[i+(i*NF.data_length)]-fai_c1))-cos(fabs(fai_c2-LF.teach_orig[i+(i*NF.data_length)]))*cos(2*pi()/NF.class_number))/
			((cos(fabs(LF.teach_orig[i+(i*NF.data_length)]-fai_c2))+cos(fabs(fai_c1-LF.teach_orig[i+(i*NF.data_length)])))*(1-cos(2*pi()/NF.class_number)));
			else
			LF.teach[i][mtn-1]=0;
			}
			*/
			//小改良部分（教師信号作成方法上必須）//++?????
			//			if(LF.teach[i][NF.class_number]!=0)
			//				LF.teach[i][0]=LF.teach[i][NF.class_number];
		}
		//fclose(fp);
	//}


	return true;
}

//-----------------------------------------
//　学習用データ＆パラメータロード関数
//　＜内部関数＞
//-----------------------------------------
bool RLLgmnMouse::LoadLearningData(char *file_name)
{
	int		i, j;
	int		counter = 0;
	FILE	*fp;

	// 学習用サンプルデータファイルオープン 
	if ((fp = fopen(file_name, "rt")) != NULL) {
		// 学習サンプルデータの読み込み
		for (i = 0; i<(NF.sample_number*LF.teacher_number*NF.data_length); i++) {
			for (j = 0; j<NF.channel_number; j++) {
				fscanf(fp, "%lf", &LF.emg_sample[i][j]);
			}
			//++fscanf(fp,"%lf",&LF.teach_orig[i]);
		}
		// 各電極のゲイン読み込み
		for (i = 0; i<NF.channel_number; i++) {
			fscanf(fp, "%lf", &NF.emg_aut_gain[i]);
		}
		// 各方向に対する動作のゲイン読み込み
		for (i = 0; i<LF.teacher_number; i++) {
			fscanf(fp, "%lf", &NF.motion_gain[i]);
		}
		fclose(fp);
	}
	else {
		printf("学習サンプルデータファイルをオープンできません RLLgmnMouse::loadLearningData()\n");
		return false;
	}

	return true;
}

//-----------------------------------------
//　重みのロード関数
//　＜内部関数＞
//-----------------------------------------
bool RLLgmnMouse::LoadWeight(char *file_name)
{
	int		i, j, k;
	int		l, m;//追加
	FILE	*fp;

	MemoryClass<double> Mem;

	// 重みファイルのオープン
	if ((fp = fopen(file_name, "rt")) != NULL) {
		for (i = 0; i<NF.channel_number; i++) {
			fscanf(fp, "%lf", &NF.emg_aut_gain[i]);//チャンネルゲインを呼び出し
		}
		for (i = 0; i<LF.teacher_number; i++) {
			fscanf(fp, "%lf", &NF.motion_gain[i]);//モーションゲインを呼び出し
		}
		//重みの読み込み(R-LLGMN)
		for (i = 0; i<NF.class_number; i++) {
			for (j = 0; j<NF.state_number; j++) {
				for (k = 0; k<NF.state_number; k++) {
					for (l = 0; l<NF.component_number; l++) {
						for (m = 0; m<(1 + NF.channel_number*(NF.channel_number + 3) / 2); m++) {
							fscanf(fp, "%lf", &NF.weight[i][j][k][l][m]);
						}
					}
				}
			}
		}
		// 重みファイルのクローズ 
		fclose(fp);
	}
	else {
		printf("重みファイルがオープンできません．\n");
		//重みを1とする(R-LLGMN)
		for (i = 0; i<NF.class_number; i++) {
			for (j = 0; j<NF.state_number; j++) {
				for (k = 0; k<NF.state_number; k++) {
					for (l = 0; l<NF.component_number; l++) {
						for (m = 0; m<(1 + NF.channel_number*(NF.channel_number + 3) / 2); m++) {
							NF.weight[i][j][k][l][m] = 1.0;
						}
					}
				}
			}
		}
	}
	return true;
}

//-----------------------------------------
//　②学習用関数(返り値がtrueとなったら終了)
//　外部でまわして（whileなど）利用してください．
//　例
//		while(!learningNetWork());
//-----------------------------------------
bool RLLgmnMouse::LearningNetWork(void)
{
	int		ptn;
	int		result = 0;
	int		i, j;//,k,tn,ii;
	double	J;				//評価関数値
							//	double	sum;
							//	double	w_temp;
	static int number = 0;

	FILE	*fp;

	// ターミナルアトラクタ用変数の初期化
	/*	for(i=0; i<(1+NF.channel_number*(NF.channel_number+3)/2)*NF.component_number*NF.motion_number; i++)
	{
	LF.jw[i] = 0.0;
	}
	*/
	//評価関数値の初期化
	J = 0.0;
	for (ptn = 0; ptn<(NF.sample_number*NF.class_number); ptn++) {		//teacher→class??

																		//++学習用筋電データ作成（ptnのデータのみ切り出し）
		for (i = 0; i<NF.data_length; i++) {
			for (j = 0; j<NF.channel_number; j++) {
				NF.input_vector[i][j] = LF.emg_sample[i + (ptn)*(NF.data_length)][j];
			}
		}
		//前向き計算開始
		FrontCalculation(NF.input_vector);
		//評価関数値の計算
		for (i = 0; i<NF.class_number; i++) {
			if (NF.x5[NF.data_length - 1][i] >= 0.0001)
				J += -LF.teach[ptn][i] * log(NF.x5[NF.data_length - 1][i]);
		}

		//++Check
		fp = fopen("./Data/Learning/CheckData.txt", "wt");
		fprintf(fp, "%d \n%lf \n", ptn, J);
		for (i = 0; i<NF.class_number; i++) {
			fprintf(fp, "%lf \t", LF.teach[ptn][i]);
		}
		fprintf(fp, "\n");
		for (i = 0; i<NF.class_number; i++) {
			fprintf(fp, "%lf \t", NF.x5[NF.data_length - 1][i]);
		}
		fprintf(fp, "\n");
		fclose(fp);


		/*		//評価関数値の計算[KL情報量使用]
		for(i=0;i<NF.class_number;i++){
		if(NF.x5[NF.data_length-1][i]>=0.0001){
		if(LF.teach[ptn][i]!=0)
		J+=LF.teach[ptn][i]*log(LF.teach[ptn][i])-LF.teach[ptn][i]*log(NF.x5[NF.data_length-1][i]);
		if(LF.teach[ptn][i]==0)
		J+=-LF.teach[ptn][i]*log(NF.x5[NF.data_length-1][i]);
		}
		}
		*/
		//後ろ向き計算開始
		BackPropagationThroughTime(ptn);

	}
	//ターミナルアトラクタの計算
	if (LF.ta_count == 0) {
		LF.rita = pow(J, 1 - LF.ta_beta) / (LF.ta_time*(1.0 - LF.ta_beta));
		LF.energy_ini = J;
		LF.energy = J;
	}
	LF.ratio = (J - LF.energy) / LF.energy_ini;
	LF.energy = J;
	CoeffChg(J);

	//終了処理
	if (LF.ta_count == (int)(LF.ta_time / LF.ta_sampling_time))
	{
		return true;//正常終了した場合は-1を出力する
	}
	else
	{
		LF.ta_count++;
		LF.counter++;
	}
	return false;
}

//-----------------------------------------
//　③重み保存
//　学習時に重み保存を行う場合実行してください．
//-----------------------------------------
bool RLLgmnMouse::SaveWeight(char *file_name)
{
	FILE *fp;
	int i, j, k;
	int l, m;//n;
	int result = 0;
	MemoryClass<double> Mem;
	// 学習済みの重み記録用ファイルオープン
	if ((fp = fopen(file_name, "wt")) != NULL) {

		for (i = 0; i<NF.channel_number; i++)
			fprintf(fp, "%lf \t", NF.emg_aut_gain[i]); 			// 各電極のゲイン
		fprintf(fp, "\n");

		for (i = 0; i<LF.teacher_number; i++)
			fprintf(fp, "%lf \t", NF.motion_gain[i]); 		// ロボット前腕動作のゲイン
		fprintf(fp, "\n");

		// 学習後の重みの保存（互換を保つ為に封印）
		for (i = 0; i<NF.class_number; i++) {
			for (j = 0; j<NF.state_number; j++) {
				for (k = 0; k<NF.state_number; k++) {
					for (l = 0; l<NF.component_number; l++) {
						for (m = 0; m<(1 + NF.channel_number*(NF.channel_number + 3) / 2); m++) {
							fprintf(fp, "%lf\t", NF.best_weight[i][j][k][l][m]);
						}
					}
				}
			}
		}
		fclose(fp);
	}
	else {
		printf("重みファイルがオープンできません．RLLgmnMouse::saveWeight()\n");
		return false;
	}
	return true;
}

//-----------------------------------------
//　②識別用関数（前向き計算：学習用）
//　入力信号をいれると事後確率を計算する
//　事後確率の取り出しは，
//   char vector[];
//   GetPsterioriProbability(vector);
//　こんな感じで取り出す．
//-----------------------------------------
bool RLLgmnMouse::FrontCalculation(double **input_vector)
{
	int		count;
	int		i, j, k;
	int		h, l, m, n;		//追加
							//	double	g_s_sum;
	double	sum2;		//追加:2層の総和用
	double	sum3;		//追加:3層の総和用
	double	sum4_1;		//追加:4層の総和用
	double	sum5;		//追加:5層の総和用

	FILE *fp;

	fp = fopen("./Data/Learning/LearningCheck2.txt", "wt");


	//R-LLGMNの前向き計算開始(学習用)
	//①For:1st Layer----->2nd Layer
	//R-LLGMNの入力ベクトル変換
	for (h = 0; h<NF.data_length; h++) {
		NF.x1[h][0] = 1.0;
		for (j = 0; j<NF.channel_number; j++)
			NF.x1[h][j + 1] = input_vector[h][j];
		//Simple Ver.
		count = NF.channel_number;
		for (i = 0; i<NF.channel_number; i++) {
			for (j = i; j<NF.channel_number; j++) {
				count++;
				NF.x1[h][count] = NF.x1[h][j + 1] * NF.x1[h][i + 1];
			}
		}
		/*		count=0;
		for(i=0;i<NF.channel_number+1;i++){
		for(j=i;j<NF.channel_number+1;j++){
		//j++;
		NF.x1[h][count]=NF.x1[h][j]*NF.x1[h][i];
		count++;
		//				NF.x1[i][j]=input_vector[i][k]*input_vector[i][l];
		}
		}
		*/
	}

	for (i = 0; i<NF.data_length; i++)
	{
		//②For:2nd Layer ----->3rd Layer
		for (j = 0; j<NF.class_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				for (l = 0; l<NF.state_number; l++) {
					for (m = 0; m<NF.component_number; m++) {
						sum2 = 0.0;
						for (n = 0; n<(1 + NF.channel_number*(NF.channel_number + 3) / 2); n++) {
							sum2 += NF.x1[i][n] * NF.weight[j][k][l][m][n];
							if (sum2>15)
								sum2 = 15;
							NF.x2[i][j][k][l][m] = exp(sum2);
						}
					}
				}
			}
		}

		//③For:3rd Layer----->4th Layer
		for (j = 0; j<NF.class_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				for (l = 0; l<NF.state_number; l++) {
					sum3 = 0.0;
					for (m = 0; m<NF.component_number; m++)
						sum3 += NF.x2[i][j][k][l][m];
					NF.ix3[i][j][k][l] = sum3;
					if (i == 0)
						NF.x3[i][j][k][l] = sum3;
					else
						NF.x3[i][j][k][l] = NF.x4[i - 1][j][l] * sum3;
				}
			}
		}

		//④For:4th Layer----->5th Layer
		sum4_1 = 0.0;
		NF.sum4_2[i] = 0.0;
		for (j = 0; j<NF.class_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				sum4_1 = 0.0;
				for (l = 0; l<NF.state_number; l++)
					sum4_1 += NF.x3[i][j][k][l];
				NF.ix4[i][j][k] = sum4_1;
				NF.sum4_2[i] += sum4_1;
			}
		}
		for (j = 0; j<NF.class_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				NF.x4[i][j][k] = NF.ix4[i][j][k] / NF.sum4_2[i];
			}
		}
		//⑤For:5th Layer----->Output
		for (j = 0; j<NF.class_number; j++) {
			sum5 = 0.0;
			for (k = 0; k<NF.state_number; k++)
				sum5 += NF.x4[i][j][k];
			NF.x5[i][j] = sum5;
			fprintf(fp, "%lf  ", NF.x5[i][j]);
		}
		fprintf(fp, "\n");
	}

	fclose(fp);

	return true;
}

//-----------------------------------------
//  前向き計算(パターン識別用)
//-----------------------------------------
bool RLLgmnMouse::HMN_FC(int number, double **input_vector)
{
	MemoryClass<double> Mem;

	int		i, j, k, l, m;
	int		count;
	double	a;
	double	Tmp;
	//各層の入出力用配列
	double	*x1_t;				//第1層
	double	****x2_t;			//第2層
	double	***x3_t;			//第3層
	double	***ix3_t;			//第3層
	double	**ix4_t;			//第4層

	double	*data;				//入力信号
	double	sum;

	//動的メモリ確保
	x1_t = Mem.matrix1_alloc(1 + NF.channel_number*(NF.channel_number + 3) / 2);
	x2_t = Mem.matrix4_alloc(NF.class_number, NF.state_number, NF.state_number, NF.component_number);
	x3_t = Mem.matrix3_alloc(NF.class_number, NF.state_number, NF.state_number);
	ix3_t = Mem.matrix3_alloc(NF.class_number, NF.state_number, NF.state_number);
	ix4_t = Mem.matrix2_alloc(NF.class_number, NF.state_number);
	data = Mem.matrix1_alloc(NF.channel_number);
	//入力信号獲得
	for (i = 0; i<NF.channel_number; i++)
		data[i] = input_vector[NF.data_length_e - number - 1][i];
	/*	sum=0;
	for(i=0;i<NF.channel_number;i++)
	sum+=data[i];
	for(i=0;i<NF.channel_number;i++)
	data[i]=data[i]/sum;
	*/	//入力ベクトル変換
	x1_t[0] = 1.0;
	for (i = 0; i<NF.channel_number; i++)
		x1_t[i + 1] = data[i];
	count = 0;
	for (i = 0; i<NF.channel_number + 1; i++) {
		for (j = i; j<NF.channel_number + 1; j++) {
			x1_t[count] = x1_t[j] * x1_t[i];
			count++;
		}
	}
	//①For:2nd Layer----->3rd Layer
	for (i = 0; i<NF.class_number; i++) {
		for (j = 0; j<NF.state_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				for (l = 0; l<NF.component_number; l++) {
					Tmp = 0.0;
					for (m = 0; m<(1 + NF.channel_number*(NF.channel_number + 3) / 2); m++)
						Tmp += x1_t[m] * NF.weight[i][j][k][l][m];
					if (Tmp>15)
						Tmp = 15;
					x2_t[i][j][k][l] = exp(Tmp);
				}
			}
		}
	}
	//②For:3rd Layer----->4th Layer
	for (i = 0; i<NF.class_number; i++) {
		for (j = 0; j<NF.state_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				a = 0.0;
				for (l = 0; l<NF.component_number; l++)
					a += x2_t[i][j][k][l];
				x3_t[i][j][k] = NF.x4_in[i][k] * a;
			}
		}
	}
	//③For:4th Layer----->5th Layer
	sum = 0.0;
	for (i = 0; i<NF.class_number; i++) {
		for (j = 0; j<NF.state_number; j++) {
			a = 0.0;
			for (k = 0; k<NF.state_number; k++)
				a += x3_t[i][j][k];
			ix4_t[i][j] = a;
			sum += a;
		}
	}
	for (i = 0; i<NF.class_number; i++) {
		for (j = 0; j<NF.state_number; j++) {
			NF.x4_in[i][j] = ix4_t[i][j] / sum;
		}
	}
	//④For:5th Layer----->Output
	for (i = 0; i<NF.class_number; i++) {
		a = 0.0;
		for (j = 0; j<NF.state_number; j++)
			a += NF.x4_in[i][j];
		NF.x5_in[i] = a;
	}

	//動的メモリ解放
	Mem.matrix1_free(x1_t);
	Mem.matrix4_free(x2_t);
	Mem.matrix3_free(x3_t);
	Mem.matrix3_free(ix3_t);
	Mem.matrix2_free(ix4_t);
	Mem.matrix1_free(data);
	x1_t = NULL;
	x2_t = NULL;
	x3_t = NULL;
	ix3_t = NULL;
	ix4_t = NULL;
	data = NULL;

	return true;
}


//-----------------------------------------
//  後ろ向き計算(BPTT)
//-----------------------------------------
bool RLLgmnMouse::BackPropagationThroughTime(int counter)
{
	int		i, j, k, l, m, n, o, p;
	double	a, b;

	//Calculate for Δ
	for (i = 0; i<NF.data_length; i++) {
		for (j = 0; j<NF.class_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				for (l = 0; l<NF.class_number; l++) {
					for (m = 0; m<NF.state_number; m++) {
						if (j == l && k == m)
							LF.O14[i][j][k][l][m] = ((NF.sum4_2[i] - NF.ix4[i][j][k]) / (NF.sum4_2[i] * NF.sum4_2[i]));
						else
							LF.O14[i][j][k][l][m] = (-NF.ix4[i][j][k] / (NF.sum4_2[i] * NF.sum4_2[i]));
					}
				}
			}
		}
	}
	//Calculate for BackPropagationThroughTime
	for (i = 0; i<NF.data_length; i++) {
		for (j = 0; j<NF.class_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				if (i == 0) {
					if (NF.x5[NF.data_length - 1][j]<1.0E-40)
						//					if(NF.x5[NF.data_length-1][j]<pow(10,-10))
						a = 1.0E-40;
					//						a=pow(10,-10);
					else
						a = NF.x5[NF.data_length - 1][j];
					LF.BPTT[i][j][k] = (LF.teach[counter][j] * 1.0) / a;
				}
				else {
					a = 0.0;
					for (l = 0; l<NF.class_number; l++) {
						for (m = 0; m<NF.state_number; m++) {
							b = 0.0;
							for (n = 0; n<NF.state_number; n++) {
								b += (LF.O14[NF.data_length - i][l][m][j][n] * NF.ix3[NF.data_length - i][j][n][k]);
							}
							a += (LF.BPTT[i - 1][l][m] * b);
						}
					}
					LF.BPTT[i][j][k] = a;
				}
			}
		}
	}
	//dwの計算
	//************ Caluculate the dw at Time=0 **********
	for (i = 0; i<NF.class_number; i++) {
		for (j = 0; j<NF.state_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				for (l = 0; l<NF.component_number; l++) {
					for (m = 0; m<(1 + NF.channel_number*(NF.channel_number + 3) / 2); m++) {
						a = 0.0;
						for (n = 0; n<NF.class_number; n++) {
							for (o = 0; o<NF.state_number; o++) {
								a += (LF.BPTT[NF.data_length - 1][n][o] * LF.O14[0][n][o][i][j]
									* NF.x2[0][i][j][k][l] * NF.x1[0][m]);
							}
						}
						LF.dw[counter][i][j][k][l][m] = (-a);
					}
				}
			}
		}
	}
	//************ Caluculate the dw at the  other Time **********
	for (i = 0; i<NF.class_number; i++) {
		for (j = 0; j<NF.state_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				for (l = 0; l<NF.component_number; l++) {
					for (m = 0; m<(1 + NF.channel_number*(NF.channel_number + 3) / 2); m++) {
						a = 0.0;
						for (n = 0; n<(NF.data_length - 1); n++) {
							for (o = 0; o<NF.class_number; o++) {
								for (p = 0; p<NF.state_number; p++) {//2004/11/02修正完了
									a += (LF.BPTT[n][o][p] * LF.O14[NF.data_length - n - 1][o][p][i][j] * NF.x4[NF.data_length - n - 2][i][k]
										* NF.x2[NF.data_length - n - 1][i][j][k][l] * NF.x1[NF.data_length - n - 1][m]);
								}
							}
							//LF.dw[counter][i][j][k][l][m]-=a;
						}
						LF.dw[counter][i][j][k][l][m] -= a;
					}
				}
			}
		}
	}
	for (m = 0; m<(1 + NF.channel_number*(NF.channel_number + 3) / 2); m++)
		LF.dw[counter][NF.class_number - 1][NF.state_number - 1][NF.state_number - 1][NF.component_number - 1][m] = 0.0;
	return true;
}

//-----------------------------------------
//	???
//-----------------------------------------
bool RLLgmnMouse::CoeffChg(double J)
{
	int		h, i, j, k, l, m;
	double	CoTmp;
	LF.sum_dw = 0.0;   				   // Summery the square of all dw

									   //*********************************
	for (i = 0; i<NF.class_number; i++) {
		for (j = 0; j<NF.state_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				for (l = 0; l<NF.component_number; l++) {
					for (m = 0; m<(1 + NF.channel_number*(NF.channel_number + 3) / 2); m++) {
						CoTmp = 0.0;
						for (h = 0; h<(NF.sample_number*LF.teacher_number); h++) {//*NF.class_number);h++){
							CoTmp += LF.dw[h][i][j][k][l][m];
						}
						LF.Dw[i][j][k][l][m] = CoTmp / ((NF.sample_number*LF.teacher_number)*1.0);//修正完了(2004/11/02)
																								  //                        LF.Dw[i][j][k][l][m]=CoTmp/((NF.class_number*NF.sample_number*LF.teacher_number)*1.0);
						LF.sum_dw += (LF.Dw[i][j][k][l][m] * LF.Dw[i][j][k][l][m]);
					}
				}
			}
		}
	}

	if (LF.sum_dw<1.0E-10)
		LF.sum_dw = 1.0E-10;
	//if(J<0)	J=0;
	LF.gamma_dw = pow(J, LF.ta_beta) / LF.sum_dw;

	//*********************************
	// Change the W  (Terminal Attractor)
	for (i = 0; i<NF.class_number; i++) {
		for (j = 0; j<NF.state_number; j++) {
			for (k = 0; k<NF.state_number; k++) {
				for (l = 0; l<NF.component_number; l++) {
					for (m = 0; m<(1 + NF.channel_number*(NF.channel_number + 3) / 2); m++) {
						CoTmp = (-LF.rita*LF.gamma_dw*LF.ta_sampling_time*LF.Dw[i][j][k][l][m]);
						NF.weight[i][j][k][l][m] += CoTmp;
						if (J<LF.min)
							NF.best_weight[i][j][k][l][m] = NF.weight[i][j][k][l][m];
					}
				}
			}
		}
	}
	if (J<LF.min) {
		LF.min = J;
		//		fprintf(fp_wt,"\n\n%lf",J);
		//  	fclose(fp_wt);
	}
	return true;
}

//-----------------------------------------
//　③事後確率取りだし関数
//-----------------------------------------
bool RLLgmnMouse::GetPosterioriProbability(double *vector)
{
	int i;
	for (i = 0; i<NF.class_number; i++)
		vector[i] = NF.x5_in[i];
	return true;
}

//-----------------------------------------
//　内部データコピー
//　ここからでも，事後確率などが取り出せる．
//-----------------------------------------
bool RLLgmnMouse::GetNetworkData(NetworkFunction *data)
{
	*data = NF;
	return true;
}

//-----------------------------------------
//　内部データコピー
//　ここから学習誤差のパラメータを取り出せる．
//-----------------------------------------
bool RLLgmnMouse::GetLearningData(LearningFunction *data)
{
	*data = LF;
	return true;
}

//-----------------------------------------
//  Checkの為急遽
//-----------------------------------------
bool RLLgmnMouse::FileCheck(bool write, bool tab, double data)
{
	if (write == true)
		fprintf(filecheck, "%lf", data);
	if (tab == true)
		fprintf(filecheck, "\t");
	else
		fprintf(filecheck, "\n");
	return true;
}

//-----------------------------------------
//終了処理
//-----------------------------------------
void RLLgmnMouse::Finish(void)
{

	if (NF.weight != NULL)
	{

		MemoryClass<double> Mem;
		Mem.matrix5_free(NF.weight);
		Mem.matrix2_free(NF.x1);
		Mem.matrix5_free(NF.x2);
		Mem.matrix4_free(NF.x3);
		Mem.matrix4_free(NF.ix3);
		Mem.matrix3_free(NF.x4);
		Mem.matrix3_free(NF.ix4);
		Mem.matrix2_free(NF.x5);
		//		Mem.matrix5_free(NF.ix2);
		Mem.matrix1_free(NF.sum4_2);
		Mem.matrix1_free(NF.emg_aut_gain);
		Mem.matrix1_free(NF.motion_gain);
		Mem.matrix2_free(NF.input_vector);

		NF.weight = NULL;
		NF.x1 = NULL;
		NF.x2 = NULL;
		NF.x3 = NULL;
		NF.ix3 = NULL;
		NF.x4 = NULL;
		NF.ix4 = NULL;
		NF.x5 = NULL;
		//		NF.ix2= NULL;
		NF.sum4_2 = NULL;
		NF.emg_aut_gain = NULL;
		NF.motion_gain = NULL;
		NF.input_vector = NULL;

		if (f_learning == true)//学習用
		{
			fclose(filecheck);

			Mem.matrix2_free(LF.emg_sample);
			//			cout<<"free(LF.emg_sample)"<<endl;
			Mem.matrix2_free(LF.teach);
			//			cout<<"free(LF.teach)"<<endl;
			Mem.matrix1_free(LF.teach_orig);
			//			cout<<"free(LF.teach_orig)"<<endl;
			//			Mem.matrix1_free(LF.jw);
			//			cout<<"free(LF.jw)"<<endl;
			Mem.matrix6_free(LF.dw);
			//			cout<<"free(LF.dw)"<<endl;
			Mem.matrix5_free(LF.Dw);
			//			cout<<"free(LF.DW)"<<endl;
			Mem.matrix5_free(LF.O14);
			//			cout<<"free(LF.O14)"<<endl;
			Mem.matrix3_free(LF.BPTT);
			//			cout<<"free(LF.BPTT)"<<endl;

			LF.emg_sample = NULL;
			LF.teach = NULL;
			LF.teach_orig = NULL;
			LF.dw = NULL;
			LF.Dw = NULL;
			LF.O14 = NULL;
			LF.BPTT = NULL;

		}
	}
}



//◇◆++++++++++++++++++++++++++++++++++++++++++++
//		学習できとるか，学習データを入力してCheck
//◇◆++++++++++++++++++++++++++++++++++++++++++++
bool RLLgmnMouse::CheckLearning(void)
{
	MemoryClass<double> Mem;

	double **LearnData;
	double **InputData;
	double **ResultData;
	int ptn, i, j;
	FILE *fp;

	int aaa;		//識別データ長を一時保存用

	fp = fopen("./Data/Learning/LearningCheck.txt", "wt");

	//領域確保
	LearnData = Mem.matrix2_alloc(NF.data_length*LF.teacher_number, NF.channel_number);
	InputData = Mem.matrix2_alloc(NF.data_length, NF.channel_number);
	ResultData = Mem.matrix2_alloc(LF.teacher_number, NF.class_number);


	//最適な重みをロード
	if (LoadWeight("./Data/Weight") == false) {
		//		cout<<"Error!! -> LoadWeight"<<endl;
		return false;
	}
	//状態の初期化
	if (InitializeOutput() == false) {
		//		cout<<"Error!! -> InitializeOutput"<<endl;
		return false;
	}

	//識別データ長を学習データ長と同じに
	aaa = NF.data_length_e;
	NF.data_length_e = NF.data_length;

	//各パターン毎にcheck開始
	for (ptn = 0; ptn < (LF.teacher_number); ptn++) {
		fprintf(fp, "■動作%d：\n", ptn + 1);

		//状態の初期化(時系列のクリア)
		if (InitializeOutput() == false) {
			//			cout<<"Error!! -> InitializeOutput"<<endl;
			return false;
		}

		//学習データを準備
		fprintf(fp, "学習データ\n");
		for (i = 0; i < NF.data_length; i++) {
			for (j = 0; j < NF.channel_number; j++) {
				InputData[i][j] = LF.emg_sample[(ptn + 1)*(NF.data_length) - (i + 1)][j];		//時系列と逆順に代入
				fprintf(fp, "%lf \t", InputData[i][j]);
			}
			fprintf(fp, "\n");
		}

		//前向き計算
		for (i = 0; i < NF.data_length; i++) {
			HMN_FC(i, InputData);
			for (j = 0; j < NF.class_number; j++)
				fprintf(fp, "%lf  ", NF.x5_in[j]);
			fprintf(fp, "\n");
		}

		//事後確率保存
		for (i = 0; i < NF.class_number; i++)
			ResultData[ptn][i] = NF.x5_in[i];
	}

	//出力テスト
	for (i = 0; i < LF.teacher_number; i++){
		fprintf(fp, "動作%d：", i + 1);
		for (j = 0; j < NF.class_number; j++) {
			fprintf(fp, "%lf  ", ResultData[i][j]);
		}
		fprintf(fp, "\n");
	}

	NF.data_length_e = aaa;
	free(LearnData);
	free(InputData);
	free(ResultData);
	fclose(fp);

	return true;
}