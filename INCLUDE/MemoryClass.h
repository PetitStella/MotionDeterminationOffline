#pragma once
// MemoryClass.h: MemoryClass クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////
//          メモリー関連クラス
//
//  このクラスはバージョンチェッカーや、出力
//  関数などすべてのクラスに共通な関数を定義
//  しています。
//                                  Ver.1.0
//  Ver.1.0(2001.8.16)
//  基底クラスから分離．
//-----------------------------------------
//  BASEclass<double> c;    ←＜＞内で型を指定
//  double ***p;
//  if((p=MEM.matrix3_alloc(XX,YY,ZZ))==0)return;
//  MEM.matrix3_free(p);
//-----------------------------------------
#pragma warning(disable:4996)

template <class variable_type>
class MemoryClass
{
public:
    //メモリ関連
    variable_type * matrix1_alloc(int);             //一次元配列を動的確保する
    variable_type ** matrix2_alloc(int,int);        //二次元配列を動的確保する
    variable_type *** matrix3_alloc(int,int,int);   //三次元配列を動的確保する
    variable_type **** matrix4_alloc(int,int,int,int);              //四次元配列を動的確保する
    variable_type ***** matrix5_alloc(int,int,int,int,int);         //五次元配列を動的確保する
    variable_type ****** matrix6_alloc(int,int,int,int,int,int);    //六次元配列を動的確保する
    void matrix1_free(variable_type *);             //一次元配列を解放する
    void matrix2_free(variable_type **);            //二次元配列を解放する
    void matrix3_free(variable_type ***);           //三次元配列を解放する
    void matrix4_free(variable_type ****);                          //四次元配列を解放する
    void matrix5_free(variable_type *****);                         //五次元配列を解放する
    void matrix6_free(variable_type ******);                        //六次元配列を解放する

};

//-----------------------------------------
//　一次元配列確保
//-----------------------------------------
template <class variable_type>
variable_type * MemoryClass<variable_type>::matrix1_alloc(int l)
{
    variable_type *p;
    //エラーチェック
    if(l<=0)return 0;
    p=(variable_type *)calloc(l,sizeof(variable_type));
    if (p==NULL) {
        printf("Error!!\r\n");
//        textOut("BASEclass::一次元の配列が確保できませんでした\n");
        return 0;
    }
    return p;
}

//-----------------------------------------
//　二次元配列確保
//-----------------------------------------
template <class variable_type>
variable_type ** MemoryClass<variable_type>::matrix2_alloc(int l, int m)
{
    variable_type *p_tmp;
    variable_type **p;
    int i;
    //エラーチェック
    if((l<=0)||(m<=0))return 0;
    //ｌ×ｍのメモリ領域を確保
    p_tmp = (variable_type *)calloc(l*m,sizeof(variable_type));
    //先頭ポインタ格納用メモリ領域を確保
    p = (variable_type **)calloc(l,sizeof(variable_type *));
    if ((p==NULL)||(p_tmp==NULL)) {
        printf("Error!!\r\n");
//        textOut("BASEclass::二次元の配列が確保できませんでした\n");
        return 0;
    }
    //ポインタの割り当て
    for(i=0; i<l; i++)
        p[i] = p_tmp+i*m;
    return p;
}

//-----------------------------------------
//　三次元配列確保
//-----------------------------------------
template <class variable_type>
variable_type *** MemoryClass<variable_type>::matrix3_alloc(int l, int m, int n)
{
    variable_type *p_tmp1;
    variable_type **p_tmp2;
    variable_type ***p;
    int i;
    //エラーチェック
    if((l<=0)||(m<=0)||(n<=0))return 0;
    //ｌ×ｍ×nのメモリ領域を確保
    p_tmp1 = (variable_type *)calloc(l*m*n,sizeof(variable_type));
    //ｌ×ｍ個の先頭ポインタ格納用メモリ領域を確保
    p_tmp2 = (variable_type **)calloc(l*m,sizeof(variable_type *));
    //先頭ポインタ格納用メモリ領域を確保
    p = (variable_type ***)calloc(l,sizeof(variable_type **));

    if ((p==NULL)||(p_tmp1==NULL)||(p_tmp2==NULL)) {
        printf("Error!!\r\n");
//        textOut("BASEclass::三次元の配列が確保できませんでした\n");
        return 0;
    }
    //ポインタの割り当て
    for(i=0; i<m*l; i++)
        p_tmp2[i] = p_tmp1+i*n;
    for(i=0; i<l; i++)
        p[i] = p_tmp2+i*m;
    return p;
}


//-----------------------------------------
//  四次元配列確保
//-----------------------------------------
template <class variable_type>
variable_type **** MemoryClass<variable_type>::matrix4_alloc(int l, int m, int n, int o)
{
    variable_type *p_tmp1;
    variable_type **p_tmp2;
    variable_type ***p_tmp3;
    variable_type ****p;
    int i;
    //エラーチェック
    if((l<=0)||(m<=0)||(n<=0)||(o<=0))return 0;
    //ｌ×ｍ×n×oのメモリ領域を確保
    p_tmp1 = (variable_type *)calloc(l*m*n*o,sizeof(variable_type));
    //ｌ×ｍ×n個の先頭ポインタ格納用メモリ領域を確保
    p_tmp2 = (variable_type **)calloc(l*m*n,sizeof(variable_type*));
    //ｌ×ｍ個の先頭ポインタ格納用メモリ領域を確保
    p_tmp3 = (variable_type ***)calloc(l*m,sizeof(variable_type **));
    //先頭ポインタ格納用メモリ領域を確保
    p = (variable_type ****)calloc(l,sizeof(variable_type ***));

    if ((p==NULL)||(p_tmp1==NULL)||(p_tmp2==NULL)||(p_tmp3==NULL)) {
        printf("Error!!\r\n");
//        textOut("BASEclass::四次元の配列が確保できませんでした\n");
        return 0;
    }
    //ポインタの割り当て
    for(i=0; i<n*m*l; i++)
        p_tmp2[i] = p_tmp1+i*o;
    for(i=0; i<m*l; i++)
        p_tmp3[i] = p_tmp2+i*n;
    for(i=0; i<l; i++)
        p[i] = p_tmp3+i*m;
    return p;
}

//-----------------------------------------
//  五次元配列確保
//-----------------------------------------
template <class variable_type>
variable_type ***** MemoryClass<variable_type>::matrix5_alloc(int l, int m, int n, int o, int q)
{
    variable_type *p_tmp1;
    variable_type **p_tmp2;
    variable_type ***p_tmp3;
    variable_type ****p_tmp4;
    variable_type *****p;
    int i;

    //エラーチェック
    if((l<=0)||(m<=0)||(n<=0)||(o<=0)||(q<=0))return 0;
    //ｌ×ｍ×n×o×qのメモリ領域を確保
    p_tmp1 = (variable_type *)calloc(l*m*n*o*q,sizeof(variable_type));
    //ｌ×ｍ×n×oのメモリ領域を確保
    p_tmp2 = (variable_type **)calloc(l*m*n*o,sizeof(variable_type*));
    //ｌ×ｍ×n個の先頭ポインタ格納用メモリ領域を確保
    p_tmp3 = (variable_type ***)calloc(l*m*n,sizeof(variable_type**));
    //ｌ×ｍ個の先頭ポインタ格納用メモリ領域を確保
    p_tmp4 = (variable_type ****)calloc(l*m,sizeof(variable_type ***));
    //先頭ポインタ格納用メモリ領域を確保
    p = (variable_type *****)calloc(l,sizeof(variable_type ****));

    if ((p==NULL)||(p_tmp1==NULL)||(p_tmp2==NULL)||(p_tmp3==NULL)||(p_tmp4==NULL)) {
        printf("Error!!\r\n");
//        textOut("BASEclass::五次元の配列が確保できませんでした\n");
        return 0;
    }
    //ポインタの割り当て
    for(i=0; i<o*n*m*l; i++)
        p_tmp2[i] = p_tmp1+i*q;
    for(i=0; i<n*m*l; i++)
        p_tmp3[i] = p_tmp2+i*o;
    for(i=0; i<m*l; i++)
        p_tmp4[i] = p_tmp3+i*n;
    for(i=0; i<l; i++)
        p[i] = p_tmp4+i*m;
    return p;
}

//-----------------------------------------
//  六次元配列確保
//-----------------------------------------
template <class variable_type>
variable_type ****** MemoryClass<variable_type>::matrix6_alloc(int l, int m, int n, int o, int q, int r)
{
    variable_type *p_tmp1;
    variable_type **p_tmp2;
    variable_type ***p_tmp3;
    variable_type ****p_tmp4;
    variable_type *****p_tmp5;
    variable_type ******p;
    int i;

    //エラーチェック
    if((l<=0)||(m<=0)||(n<=0)||(o<=0)||(q<=0)||(r<=0))return 0;
    //ｌ×ｍ×n×o×q×rのメモリ領域を確保
    p_tmp1 = (variable_type *)calloc(l*m*n*o*q*r,sizeof(variable_type));
    //ｌ×ｍ×n×o×qのメモリ領域を確保
    p_tmp2 = (variable_type **)calloc(l*m*n*o*q,sizeof(variable_type*));
    //ｌ×ｍ×n×oのメモリ領域を確保
    p_tmp3 = (variable_type ***)calloc(l*m*n*o,sizeof(variable_type**));
    //ｌ×ｍ×n個の先頭ポインタ格納用メモリ領域を確保
    p_tmp4 = (variable_type ****)calloc(l*m*n,sizeof(variable_type***));
    //ｌ×ｍ個の先頭ポインタ格納用メモリ領域を確保
    p_tmp5 = (variable_type *****)calloc(l*m,sizeof(variable_type ****));
    //先頭ポインタ格納用メモリ領域を確保
    p = (variable_type ******)calloc(l,sizeof(variable_type *****));

    if ((p==NULL)||(p_tmp1==NULL)||(p_tmp2==NULL)||(p_tmp3==NULL)||(p_tmp4==NULL)||(p_tmp5==NULL)) {
        printf("Error!!\r\n");
//        textOut("BASEclass::六次元の配列が確保できませんでした\n");
        return 0;
    }
    //ポインタの割り当て
    for(i=0; i<q*o*n*m*l; i++)
        p_tmp2[i] = p_tmp1+i*r;
    for(i=0; i<o*n*m*l; i++)
        p_tmp3[i] = p_tmp2+i*q;
    for(i=0; i<n*m*l; i++)
        p_tmp4[i] = p_tmp3+i*o;
    for(i=0; i<m*l; i++)
        p_tmp5[i] = p_tmp4+i*n;
    for(i=0; i<l; i++)
        p[i] = p_tmp5+i*m;
    return p;
}

//-----------------------------------------
//　一次元配列解放
//-----------------------------------------
template <class variable_type>
void  MemoryClass<variable_type>::matrix1_free(variable_type *p)
{
    if(p==0)return;
    free(p);
    p=0;
    return;
}

//-----------------------------------------
//　二次元配列解放
//-----------------------------------------
template <class variable_type>
void  MemoryClass<variable_type>::matrix2_free(variable_type **p)
{
    if(p==0)return;
    free(p[0]);
    free(p);
    p=0;
    return;
}

//-----------------------------------------
//　三次元配列解放
//-----------------------------------------
template <class variable_type>
void  MemoryClass<variable_type>::matrix3_free(variable_type ***p)
{
    if(p==0)return;
    free(p[0][0]);
    free(p[0]);
    free(p);
    p=0;
    return;
}

//-----------------------------------------
//  四次元配列解放
//-----------------------------------------
template <class variable_type>
void  MemoryClass<variable_type>::matrix4_free(variable_type ****p)
{
    if(p==0)return;
    free(p[0][0][0]);
    free(p[0][0]);
    free(p[0]);
    free(p);
    p=0;
    return;
}

//-----------------------------------------
//　五次元配列解放
//-----------------------------------------
template <class variable_type>
void  MemoryClass<variable_type>::matrix5_free(variable_type *****p)
{
    if(p==0)return;
    free(p[0][0][0][0]);
    free(p[0][0][0]);
    free(p[0][0]);
    free(p[0]);
    free(p);
    p=0;
    return;
}

//-----------------------------------------
//　六次元配列解放
//-----------------------------------------
template <class variable_type>
void  MemoryClass<variable_type>::matrix6_free(variable_type ******p)
{
    if(p==0)return;
    free(p[0][0][0][0][0]);
    free(p[0][0][0][0]);
    free(p[0][0][0]);
    free(p[0][0]);
    free(p[0]);
    free(p);
    p=0;
    return;
}


