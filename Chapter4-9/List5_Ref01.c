// List5_Ref01.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <string.h>         //文字列操作
#include <stdbool.h>        //論理型および論理値
#include <limits.h>         //整数型の大きさ
#include <wiringPi.h>       //wiringPi

#define LED0    23          //GPIO23をLED0と定義
#define SW0     4           //GPIO4をSW0と定義
#define INTERVAL    300     //バウンシング対策時間ms

/* グローバル変数　*/
static volatile bool g_sw0;             //SW0操作の変数
static volatile unsigned int g_millis;  //経過時間ms
/* プロトタイプ宣言 */
void IntSw0(void);         	//SW0の割込み関数

int main(void){
    wiringPiSetupGpio();	//BCMのGPIO番号を使用
    pinMode(LED0, OUTPUT);	//LED0を出力に設定	
    pinMode(SW0, INPUT);	//SW0を入力に設定
    pullUpDnControl(SW0,PUD_DOWN); //SW0にプルダウン抵抗をつける
    	
    wiringPiISR(SW0, INT_EDGE_RISING, (void*)IntSw0); //立ち上がりエッジ
    g_millis = millis();
    g_sw0 = false;
    while(1){
	digitalWrite(LED0,g_sw0);
    }
    return EXIT_SUCCESS;
}

void IntSw0(void){
    unsigned int diff,now;
    now = millis();
    diff = now - g_millis;

    if(diff > INTERVAL){
	g_sw0 = !g_sw0;
//	printf("O now=%d, g_millis=%d, diff=%d, g_sw0=%d\n",now,g_millis,diff,g_sw0); //デバッグ用
	g_millis = now;
    }
}
