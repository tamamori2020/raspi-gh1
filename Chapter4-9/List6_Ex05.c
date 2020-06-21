// List6_Ex05.c 
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm
// gccコマンドに数学ライブラリのリンクオプション-lmを追記します
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <math.h>           //数学
#include <wiringPi.h>       //wiringPi

#define BUZZER  18          //GPIO18をBUZZERと定義
#define PI4_CLK 54000000    //Pi4Bの内部クロック Hz
#define PI3_CLK 19200000    //Pi3B/3B+の内部クロック Hz
#define RANGE   100         //PWM range

#define OL  3   //1 オクターブ下
#define OC  4   //中央のオクターブ
#define OH  5   //1 オクターブ上

#define REST    0   //休符
#define DO      1   //ド
#define DOS     2   //ド#
#define RE      3   //レ	
#define RES     4   //レ#
#define MI      5   //ミ	
#define FA      6   //ファ
#define FAS     7   //ファ#
#define SO      8   //ソ	
#define SOS     9   //ソ#
#define RA      10  //ラ
#define RAS     11  //ラ#
#define SI      12  //シ	

//#define  DEBUG    // マクロ定義により#ifdef #endifで挟まれた命令が有効になります。
                    // 命令を無効にする場合は、このマクロ定義をコメント文にしてビルドします。
const int swGpio[8] = {4,5,6,26,17,27,20,21};   //SW GPIOを配列で定義
/* グローバル変数　*/
static volatile int g_state;    //SW操作の変数
/* プロトタイプの定義 */
void g(int oct, int solfa, int length); //1音を発生する関数
void IntSw1(void);      //SW1の割込み関数

int main (void){
    int i;
    int oct,solfa;
    int bpm = 72;       //楽譜のテンポ(Beats Per Minute) 
    int qN = 60000/bpm; //4分音符(quarter note) の長さ(ミリ秒)
    int musicNo;
    int music[][3] = {  //さくらさくらの曲
        {OC,RA,qN},
        {OC,RA,qN},
        {OC,SI,qN*2},
        {OC,RA,qN},
        {OC,RA,qN},
        {OC,SI,qN*2},
        {OC,RA,qN},
        {OC,SI,qN},
        {OH,DO,qN},
        {OC,SI,qN},
        {OC,RA,qN},
        {OC,SI,qN/2},
        {OC,RA,qN/2},
        {OC,FA,qN*2},
    
        {OC,MI,qN},
        {OC,DO,qN},
        {OC,MI,qN},
        {OC,FA,qN},
        {OC,MI,qN},
        {OC,MI,qN/2},
        {OC,DO,qN/2},
        {OL,SI,qN*2},
        {OC,RA,qN},
        {OC,SI,qN},
        {OH,DO,qN},
        {OC,SI,qN},

        {OC,RA,qN},
        {OC,SI,qN/2},
        {OC,RA,qN/2},
        {OC,FA,qN*2},
        {OC,MI,qN},
        {OC,DO,qN},
        {OC,MI,qN},
        {OC,FA,qN},
        {OC,MI,qN},
        {OC,MI,qN/2},
        {OC,DO,qN/2},
        {OL,SI,qN*2},

        {OC,RA,qN},
        {OC,RA,qN},
        {OC,SI,qN*2},
        {OC,RA,qN},
        {OC,RA,qN},
        {OC,SI,qN*2},
        {OC,MI,qN},
        {OC,FA,qN},
        {OC,SI,qN/2},
        {OC,RA,qN/2},
        {OC,FA,qN},
        {OC,MI,qN*2},

        {OC,REST,qN},    //曲の最後は休止符を挿入して音を止める
    };

    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    for(i=0;i<8;i++){           //SW0-SW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i=0;i<8;i++){           //SW0-SW7をプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}
        
    pinMode(BUZZER, PWM_OUTPUT);    //BUZZERをPWM出力に設定
    pwmSetRange(RANGE);             //階調の設定
    pwmSetMode(PWM_MODE_MS);        //信号波形モードの設定
    wiringPiISR(swGpio[1], INT_EDGE_RISING, (void*)IntSw1);
    musicNo = sizeof music/((sizeof music[0][0])*3); //配列の要素数(音符)の計算
    #ifdef DEBUG
        printf("%d \n",musicNo);
    #endif
    
    while(1){
        if(digitalRead(swGpio[0]) == HIGH){ //オルゴールスタート
            g_state = 0;
            for(i=0; i<musicNo; i++){   //音符の最初から最後まで再生
    #ifdef DEBUG
                printf("%d  ",i+1);
    #endif
                oct = music[i][0];      //オクターブの取得
                solfa =  music[i][1];   //階名の取得
                qN = music[i][2];       //音の長さの取得
                g(oct,solfa,qN);        //1音を再生
                if(g_state == 1){       //SW1が押されたら再生停止
                    g(OC,REST,qN);      //音の出力を停止
                    break;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

/*****************************************************
 関数名  void g(int oct, int solfa, int length)
 引数    oct     0:1オクターブ下,1:中央,2:1オクターブ上
        solfa   0:休符,1:ド,2:ド#,3:レ,4:レ#,5:ミ,6:ファ,
                7:ファ#,8:ソ,9:ソ#,10:ラ,11:ラ#,12:シ
        length　時間の長さ 単位ms
 戻り値  なし
 概要   引数で指定された音名または休符を発生する。
 *****************************************************/
void g(int oct, int solfa, int length){
    int d = 0;      //整数
    int f = 0;      //周波数
    int divisor;    //分周値 pwmSetClock関数の引数
    int value;      //tON時間 pwmWrite関数の引数
        if(solfa !=0){         //休符以外の場合、音名の周波数を計算
            d = solfa - 10;    //引数solfaからテキスト表7-10のdに変換する
            if (oct == OL){    //1オクターブ下のとき
                d -= 12;
            }
            if(oct == OH){     //1オクターブ上のとき
                d+=12;
            }
            f = 440 * pow(2,(double)d/12)+0.5;  //べき乗計算のpow関数

//          Pi3B/3B+とPi4Bのどちらかを選択。
            divisor = PI4_CLK / (f*RANGE);      //Pi4Bのとき
//            divisor = PI3_CLK / (f*RANGE);      //Pi3B/3B+のとき

            pwmSetClock(divisor);               //内部クロックの分周の設定
            value = RANGE/2;    //デューティ比50%
        }else{
            value = 0;          //休符、valueを0にして、音を止めます
        }
    #ifdef DEBUG
        printf("d=%d  f = %d\n",d,f);
    #endif
    pwmWrite(BUZZER, value); //length(ms) だけ音を出力
    delay(length);
}

void IntSw1(void)  //SW1の割込み関数
{g_state = 1;}

