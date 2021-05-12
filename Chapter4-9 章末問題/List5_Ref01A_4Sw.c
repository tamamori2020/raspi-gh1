// List5_Ref01A_4Sw.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
// List5_Ref01A.cをSW0からSW3の4入力に拡張
// 修正：CPU使用率の抑制のためusleep(1000)を挿入

#include <stdio.h>
#include <stdlib.h>         //EXIT_SUCCESS
#include <unistd.h>         //usleep
#include <wiringPi.h>       //wiringPiSetupGpio,pinMode,etc

#define INTERVAL    50      //バウンシング対策時間(ms)

/* LEDとSWのGPIOを配列で定義 */
const int ledGpio[4] = {23,22,25,24};   //グローバル定数
const int swGpio[4] = {4,5,6,26};

/* グローバル変数　*/
static volatile int g_sw0; //SW0操作の変数
static volatile int g_sw1; //SW1操作の変数
static volatile int g_sw2; //SW2操作の変数
static volatile int g_sw3; //SW3操作の変数

/* プロトタイプ宣言 */
void IntSw0(void);          //SW0の割込み関数
void IntSw1(void);          //SW1の割込み関数
void IntSw2(void);          //SW2の割込み関数
void IntSw3(void);          //SW3の割込み関数

int main(void){
    int i;
    wiringPiSetupGpio();                //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){             //LED0からLED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}
    for(i = 0; i < 4; i++){             //SW0からSW3を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i = 0; i < 4; i++){             //SW0からSW3にプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}
    wiringPiISR(swGpio[0], INT_EDGE_BOTH, IntSw0); //両方のエッジでの割り込み設定
    wiringPiISR(swGpio[1], INT_EDGE_BOTH, IntSw1); //両方のエッジでの割り込み設定
    wiringPiISR(swGpio[2], INT_EDGE_BOTH, IntSw2); //両方のエッジでの割り込み設定
    wiringPiISR(swGpio[3], INT_EDGE_BOTH, IntSw3); //両方のエッジでの割り込み設定
    g_sw0 = 0;
    g_sw1 = 0;
    g_sw2 = 0;
    g_sw3 = 0;
    while(1){
        digitalWrite(ledGpio[0],g_sw0);
        digitalWrite(ledGpio[1],g_sw1);
        digitalWrite(ledGpio[2],g_sw2);
        digitalWrite(ledGpio[3],g_sw3);
        usleep(1000);       //CPU使用率の抑制のため
    }
    return EXIT_SUCCESS;
}

void IntSw0(void){
    unsigned int t0Now;         //現在の時刻
    static unsigned int t0Old;  //前回の時刻
    unsigned int t0Diff;        //時間間隔
    static int alt0;
    t0Now = millis();       //現在の時刻の取得
    t0Diff = t0Now -t0Old;  //前回の時刻より間隔の取得
    if(t0Diff > INTERVAL){
        if(alt0 == 0){
            g_sw0 = !g_sw0;
        }
        alt0 = !alt0;
    }
    t0Old = t0Now;
}

void IntSw1(void){
    unsigned int t1Now;         //現在の時刻
    static unsigned int t1Old;  //前回の時刻
    unsigned int t1Diff;        //時間間隔
    static int alt1;
    t1Now = millis();       //現在の時刻の取得
    t1Diff = t1Now -t1Old;  //前回の時刻より間隔の取得
    if(t1Diff > INTERVAL){
        if(alt1 == 0){
            g_sw1 = !g_sw1;
        }
        alt1 = !alt1;
    }
    t1Old = t1Now;
}

void IntSw2(void){
    unsigned int t2Now;         //現在の時刻
    static unsigned int t2Old;  //前回の時刻
    unsigned int t2Diff;        //時間間隔
    static int alt2;
    t2Now = millis();       //現在の時刻の取得
    t2Diff = t2Now -t2Old;  //前回の時刻より間隔の取得
    if(t2Diff > INTERVAL){
        if(alt2 == 0){
            g_sw2 = !g_sw2;
        }
        alt2 = !alt2;
    }
    t2Old = t2Now;
}

void IntSw3(void){
    unsigned int t3Now;         //現在の時刻
    static unsigned int t3Old;  //前回の時刻
    unsigned int t3Diff;        //時間間隔
    static int alt3;
    t3Now = millis();       //現在の時刻の取得
    t3Diff = t3Now -t3Old;  //前回の時刻より間隔の取得
    if(t3Diff > INTERVAL){
        if(alt3 == 0){
            g_sw3 = !g_sw3;
        }
        alt3 = !alt3;
    }
    t3Old = t3Now;
}
