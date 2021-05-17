// List15_Ex04.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_FAILURE,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //usleep
#include <math.h>           //pow
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#include "SilentNight.h"    //楽譜
#define BUZZER  18          //GPIO18をBUZZERと定義
#define PI4B                //本行をコメントにすると、CLKはPi3/3B+用が設定される。
#ifdef  PI4B
    #define CLK  54000000   //Pi4Bの内部クロック Hz
#else
    #define CLK  19200000   //Pi3/3B+の内部クロック Hz
#endif
#define RANGE       100     //PWM range
#define LED0        23      //LEDのGPIO番号の定義
#define DELAYTIME   20      //バウンシングの収束時間ms
#define NUM_THREADS 3       //スレッドの生成数

/* グローバル宣言　*/
static volatile int g_alt[3];   //g_alt[0](SW0用),g_alt[1](SW1用),g_alt[2](SW7用)
const int ledGpio[4] ={23,22,25,24}; //LED0からLED3
const int swGpio[3] = {4,5,21}; //swGpio[0](SW0用),swGpio[1](SW1用),swGpio[2](SW7用)
/* プロトタイプ宣言 */
void *ThreadAlt017(void *t);        //SWのオルタネート動作の関数
void *ThreadLED4(void *arg);        //LED点灯の左シフトと右シフト
void *ThreadOrgel(void *arg);       //オルゴールの再生
void g(int oct, int solfa, int length); //音名または休符の発生
void Led4bit(int ledData);          //LED0~LED3へ出力

int main (void){
    int i;
    pthread_t   thread[3];      //スレッドの宣言
    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){     //LED0からLED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}
    for(i = 0; i < 3; i++){     //SW0,SW1,SW7を入力に設定
        pinMode(swGpio[i], INPUT);
        pullUpDnControl(swGpio[i],PUD_DOWN); //プルダウン抵抗をつける
        g_alt[i] = LOW;             //g_altの初期化
    }
    printf("CLK %d\n",CLK);
    
    if(pthread_create(&thread[0], NULL, ThreadAlt017, (void *)DELAYTIME) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&thread[1], NULL, ThreadLED4, NULL) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&thread[2], NULL, ThreadOrgel, NULL) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < NUM_THREADS; i++){           //スレッドの終了待ち
        if(pthread_join(thread[i], NULL) != 0){
            fprintf(stderr,"Error pthread_join.\n");
            exit(EXIT_FAILURE);
        }
    }

    g(OC,0,0);                  //音の停止
    Led4bit(0);                 //LEDの消灯
    printf("End of main.\n");
    return EXIT_SUCCESS;
}

//関数名  void *ThreadAlt017(void *t)
//引数    t バウンシングの収束時間ms
//戻り値  なし
//グローバル宣言 g_alt[3],swGpio[3]
//概要    SW0とSW1についてバウンシング対策したオルタネート動作する。
//       SW7がHIGHのとき、スレッドを終了する。
void *ThreadAlt017(void *t){
    int i;
    int sNow[3];
    int sOld[3];
    for(i = 0; i < 3; i++){sOld[i] = LOW;}      //sOldの初期化
    while(g_alt[2] != HIGH){                    //g_alt[2]=SW7
        for(i = 0; i < 3; i++){
            sNow[i] = digitalRead(swGpio[i]);
            if((sOld[i] == LOW)&&(sNow[i] == HIGH)){    //SWの立ち上がりを検出
                g_alt[i] = !g_alt[i];
            }
            sOld[i] = sNow[i];
        }
        for(i = 2; i >= 0; i--){        //デバッグ表示用
            printf("%d ",g_alt[i]);
            if(i == 0){printf("\n");}   //改行
        }
        delay((int)t);       //バウンシング対策の時間待ち
    }
    printf("End of ThreadAlt017.\n");
    pthread_exit(NULL);
}

//関数名  void *ThreadLED4(void *arg)
//引数    なし
//戻り値  なし
//グローバル変数 g_alt[0],g_alt[2]
//概要    4個のLEDが奇数と偶数で交互に0.5秒間隔で点滅する。
void *ThreadLED4(void *arg){
    while(g_alt[2] != HIGH){        //g_alt[2]=SW7
        if(g_alt[0] == HIGH){
            Led4bit(0b0101);
            delay(500);
            Led4bit(0b1010);
            delay(500);
        }else{
            usleep(1000);           //CPU使用率の抑制のため
        }
    }
    printf("End of ThreadLED4.\n");
    pthread_exit(NULL);
}

//関数名  void *ThreadOrgel(void *arg)
//引数    なし
//戻り値  なし
//概要    楽譜のヘッダーファイルの配列musicを再生する
void *ThreadOrgel(void *arg){
    int i;
    int oct;        //オクターブ
    int solfa;      //階名
    int length;     //音の長さ
    int musicNo;    //音符・休符の数
    pinMode(BUZZER, PWM_OUTPUT);    //BUZZERをPWM出力に設定
    pwmSetRange(RANGE);             //階調の設定
    pwmSetMode(PWM_MODE_MS);        //信号波形モードの設定

    musicNo = sizeof music/((sizeof music[0][0])*3); //配列の要素数(音符・休符)の計算
    while(1){
        for(i=0; i<musicNo; i++) {   //音符の最初から最後まで再生
            if(g_alt[2] == HIGH){
                printf("End of ThreadOrgel.\n");
                pthread_exit(NULL);
            }else if(g_alt[1] == LOW){
                g(OC,0,0);           //音の停止
                break;
            }
            oct = music[i][0];      //オクターブの取得
            solfa =  music[i][1];   //階名の取得
            length = music[i][2];   //音の長さの取得
            g(oct,solfa,length);    //1音を再生
        }
        usleep(1000);               //CPU使用率の抑制のため
    }
}

//関数名  void g(int oct, int solfa, int length)
//引数    oct     3:1オクターブ下,4:中央,5:1オクターブ上
//         solfa   0:休符,1:ド,2:ド#,3:レ,4:レ#,5:ミ,6:ファ,
//                7:ファ#,8:ソ,9:ソ#,10:ラ,11:ラ#,12:シ
//         length　時間の長さ 単位ms
//戻り値  なし
//概要   引数で指定された音名または休符を発生する。
void g(int oct, int solfa, int length){
    int d = 0;      //指数 式6-101
    int f = 0;      //周波数
    int divisor;    //分周値 pwmSetClock関数の引数
    int value;      //tON時間 pwmWrite関数の引数
        if(solfa !=0){         //休符以外の場合、音名の周波数を計算
            d = solfa - 10;    //引数solfaから式6-101の変数dに変換する
            if (oct == OL){    //1オクターブ下のとき
                d -= 12;
            }
            if(oct == OH){     //1オクターブ上のとき
                d+=12;
            }
            f = 440 * pow(2,(double)d/12)+0.5;  //べき乗計算のpow関数
            divisor = CLK / (f*RANGE);          //divisorを求める。
            pwmSetClock(divisor);               //内部クロックの分周の設定
            value = RANGE/2;    //デューティ比50%
        }else{
            value = 0;          //休符、valueを0にして、音を止めます
        }
    pwmWrite(BUZZER, value); //length(ms) だけ音を出力
    delay(length);
}

//関数名  void Led4bit(int ledData)
//引数    ledData  4bitデータ　0から15
//戻り値  なし
//グローバル宣言 ledGpio[4]
//概要    ledDataをLED0からLED３へ出力
void Led4bit(int ledData){
    int i;
    for (i=0; i<4; i++){        //LSBのLED0から順番に出力する
        digitalWrite(ledGpio[i], ledData&1);    //LSBだけの値にして出力
        ledData = ledData >> 1;	                //1bit 右シフト
    }
}
