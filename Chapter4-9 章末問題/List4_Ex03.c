// List4_Ex03.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf
#include <stdlib.h>         //EXIT_SUCCESS
#include <wiringPi.h>       //wiringPiSetupGpio,etc

/* LEDのGPIOを配列で定義 */
const int ledGpio[4] = {23,22,25,24};   //グローバル定数
/* プロトタイプ宣言*/
void Led4bit(int ledData);

int main (void){
    int i;
    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){     //LED0からLED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}

    while(1){                   //永久ループ
        for(i = 1; i < 16; i<<= 1){ //変数iをfor文内で左シフトさせる
            Led4bit(i);         //変数iを4bit出力する
            delay(500);         //0.5秒時間待ち
        }
    }
    return EXIT_SUCCESS;        //main関数の戻り値、正常終了
}

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

