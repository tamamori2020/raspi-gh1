// List4_Ex01.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <wiringPi.h>       //wiringPi

/* LEDのGPIOを配列で定義 */
const int ledGpio[4] ={23,22,25,24};    //グローバル変数
/* プロトタイプの定義 */
void Led4bit(int ledData);

int main (void){
    int i;
    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    for(i=0;i<4;i++){           //LED0からLED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}

    for(i=0;i<10;i++){
        Led4bit(0b0101);        //点灯パターン 0101
        delay(500);             //0.5秒待つ
        Led4bit(0b1010);        //点灯パターン 1010
        delay(500);             //0.5秒待つ
    }
        Led4bit(0);             //消灯
    return EXIT_SUCCESS;        //main関数の戻り値、正常終了
}

//関数名  void Led4bit(int ledData)
//引数    ledData  4bitデータ　0から15
//戻り値  なし
//概要    ledDataをLED0からLED３へ出力
void Led4bit(int ledData)
{
    int i;
    for (i=0; i<4; i++){        //LSBのLED0から順番に出力する
        digitalWrite(ledGpio[i], ledData&1);    //LSBだけの値にして出力
        ledData = ledData >> 1;                 //1bit 右シフト
    }
}
