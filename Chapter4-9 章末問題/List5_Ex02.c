// List5_Ex02.c
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

int main (void){
    int i;
    wiringPiSetupGpio();                //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){             //LED0からLED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}
    for(i = 0; i < 8; i++){             //SW0からSW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i = 0; i < 8; i++){             //SW0からSW7にプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}
 
    while(1){
        for (i = 0; i < 4; i++){        //SW0とSW4の状態から順番に調べてループ処理する
            if((digitalRead(swGpio[i]) | digitalRead(swGpio[i+4])) == HIGH){ //ビット毎のORビット演算子
                digitalWrite(ledGpio[i], HIGH); //HIGHを出力する
            }else{ 
                digitalWrite(ledGpio[i], LOW);  //LOWを出力する
            }
        }
        usleep(1000);       //CPU使用率の抑制のため
    }
    return EXIT_SUCCESS;
}
