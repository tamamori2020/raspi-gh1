// List5_Ex04.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ		
#include <wiringPi.h>       //wiringPi

const int ledGpio[4] ={23,22,25,24};            //LED GPIOを配列で定義
const int swGpio[8] = {4,5,6,26,17,27,20,21};   //SW GPIOを配列で定義
/* グローバル変数　*/
static volatile int g_state; 	//SW操作の変数
/* プロトタイプ宣言 */
void IntSw0(void);         	//SW0の割込み関数
void IntSw1(void);         	//SW1の割込み関数

int main (void){
    int i;
    wiringPiSetupGpio();	//BCMのGPIO番号を使用
    for(i=0;i<4;i++){		//LED0-LED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}
    for(i=0;i<8;i++){		//SW0-SW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i=0;i<8;i++){		//SW0-SW7をプルダウン抵抗をつける
	pullUpDnControl(swGpio[i],PUD_DOWN);}
    //SW0の立上りエッジで割込みを発生する設定 
    wiringPiISR(swGpio[0], INT_EDGE_RISING, (void*)IntSw0);
    //SW1の立上りエッジで割込みを発生する設定 
    wiringPiISR(swGpio[1], INT_EDGE_RISING, (void*)IntSw1);    
    	    
    while(1){
        if(g_state){   	//if文の条件式は真偽値で判定、0以外が真、0が偽
            digitalWrite(ledGpio[0],HIGH);
            delay(1000);
            digitalWrite(ledGpio[0],LOW);
            delay(1000);
        }
    }
    return EXIT_SUCCESS;
}

void IntSw0(void)  //SW0の割込み関数
{
    g_state = 1;
}

void IntSw1(void)  //SW1の割込み関数
{
    g_state = 0;
}
