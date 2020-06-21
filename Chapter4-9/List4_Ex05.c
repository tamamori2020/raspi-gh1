// List4_Ex05.c
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
    int ledData;
    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    for(i=0;i<4;i++){           //LED0からLED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}

    while(1){                   //永久ループ
        printf("0から15までの値を入力してください>>");
        scanf("%d",&ledData);
        if(0 <= ledData && ledData <=15){   //入力データが0から15までの範囲かどうか判断
            Led4bit(ledData);
        }else{
            printf("入力の値が範囲外です\n\n");
            Led4bit(0);         //全てのLEDを消灯
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
    for (i=0; i<4; i++){        //LSBのLED0から順番に出力する
        digitalWrite(ledGpio[i], ledData&1);    //LSBだけの値にして出力
        ledData = ledData >> 1;                 //1bit 右シフト
    }
}
