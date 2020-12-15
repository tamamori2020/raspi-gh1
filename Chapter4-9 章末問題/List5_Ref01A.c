// List5_Ref01A.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <string.h>         //文字列操作
#include <limits.h>         //整数型の大きさ
#include <wiringPi.h>       //wiringPi

#define LED0        23      //GPIO23をLED0と定義
#define SW0         4       //GPIO4をSW0と定義
#define INTERVAL    50      //バウンシング対策時間(ms)

/* グローバル変数　*/
static volatile int g_sw0; //SW0操作の変数

/* プロトタイプ宣言 */
void IntSw0(void);          //SW0の割込み関数

int main(void){
    wiringPiSetupGpio();    //BCMのGPIO番号を使用
    pinMode(LED0, OUTPUT);  //LED0を出力に設定
    pinMode(SW0, INPUT);    //SW0を入力に設定
    pullUpDnControl(SW0,PUD_DOWN); //SW0にプルダウン抵抗をつける
    
    wiringPiISR(SW0, INT_EDGE_BOTH, IntSw0); //両方のエッジでの割り込み設定
    g_sw0 = 0;
    while(1){
        digitalWrite(LED0,g_sw0);
        delay(1);           //CPU使用率の低減用
    }
    return EXIT_SUCCESS;
}

void IntSw0(void){
    unsigned int t0Now;         //現在の時刻
    static unsigned int t0Old;  //前回の時刻
    unsigned int t0Diff;        //時間間隔
    static int alt0;
//    static int i;           //デバック用
    t0Now = millis();       //現在の時刻の取得
//    printf("t0Now = %u \n",t0Now);  //デバック用
    t0Diff = t0Now -t0Old;  //前回の時刻より間隔の取得
    if(t0Diff > INTERVAL){
        if(alt0 == 0){
            g_sw0 = !g_sw0;
        }
        alt0 = !alt0;
//        i++;    //デバック用
//        printf("t0Old = %u, t0Diff=%u, g_sw0=%d edge0 = %d **%d\n",t0Old,t0Diff,g_sw0,alt0,i); //デバック用
    }
    t0Old = t0Now;
}
