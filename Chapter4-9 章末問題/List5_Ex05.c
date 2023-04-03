// List5_Ex05.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
// 2021-05-12 CPU使用率の抑制のためusleep(1000)を挿入

#include <stdio.h>
#include <stdlib.h>         //EXIT_SUCCESS
#include <unistd.h>         //usleep
#include <wiringPi.h>       //wiringPiSetupGpio,pinMode,etc

/* LEDとSWのGPIOを配列で定義 */
const int ledGpio[4] = {23,22,25,24};           //グローバル定数
const int swGpio[8] = {4,5,6,26,17,27,20,21};
/* グローバル変数　*/
static volatile int g_state; //SW操作の変数
/* プロトタイプ宣言 */
void IntSw0(void);           //SW0の割込み関数
void IntSw1(void);           //SW1の割込み関数
void IntSw2(void);           //SW2の割込み関数
void IntSw3(void);           //SW3の割込み関数
void IntSw4(void);           //SW4の割込み関数
void IntSw5(void);           //SW5の割込み関数
void IntSw6(void);           //SW6の割込み関数
void IntSw7(void);           //SW7の割込み関数
void Led4bit(int ledData);   //LED0からLED3までの4bitの点灯

int main (void){
    int i;
    wiringPiSetupGpio();                //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){             //LED0からLED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}
    for(i = 0; i < 8; i++){             //SW0からSW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i = 0; i < 8; i++){             //SW0からSW7にプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}
    
    //各SWの立上りエッジで割込みを発生する設定 
    wiringPiISR(swGpio[0], INT_EDGE_RISING, IntSw0);
    wiringPiISR(swGpio[1], INT_EDGE_RISING, IntSw1);
    wiringPiISR(swGpio[2], INT_EDGE_RISING, IntSw2);
    wiringPiISR(swGpio[3], INT_EDGE_RISING, IntSw3);
    wiringPiISR(swGpio[4], INT_EDGE_RISING, IntSw4);
    wiringPiISR(swGpio[5], INT_EDGE_RISING, IntSw5);
    wiringPiISR(swGpio[6], INT_EDGE_RISING, IntSw6);
    wiringPiISR(swGpio[7], INT_EDGE_RISING, IntSw7);

    while(1){
        Led4bit(g_state);    //LEDの点灯
        usleep(1000);       //CPU使用率の抑制のため
    }
    return EXIT_SUCCESS;
}

void IntSw0(void)  //SW0の割込み関数
{g_state = 0;}
void IntSw1(void)  //SW1の割込み関数
{g_state = 1;}
void IntSw2(void)  //SW2の割込み関数
{g_state = 2;}
void IntSw3(void)  //SW3の割込み関数
{g_state = 3;}
void IntSw4(void)  //SW4の割込み関数
{g_state = 4;}
void IntSw5(void)  //SW5の割込み関数
{g_state = 5;}
void IntSw6(void)  //SW6の割込み関数
{g_state = 6;}
void IntSw7(void)  //SW7の割込み関数
{g_state = 7;}

//関数名  void Led4bit(int ledData)
//引数    ledData  4bitデータ　0から15
//戻り値  なし
//概要    ledDataをLED0からLED３へ出力
void Led4bit(int ledData)
{
    int i;
    for (i = 0; i < 4; i++){        //LSBのLED0から順番に出力する
        digitalWrite(ledGpio[i], ledData&1);    //LSBだけの値にして1bitの出力
        ledData = ledData >> 1;                 //1bit 右シフト
    }
}
