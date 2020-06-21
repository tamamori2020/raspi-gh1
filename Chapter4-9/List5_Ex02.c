// List5_Ex02.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <wiringPi.h>       //wiringPi

const int ledGpio[4] ={23,22,25,24};            //LED GPIOを配列で定義
const int swGpio[8] = {4,5,6,26,17,27,20,21};   //SW GPIOを配列で定義

int main (void){
    int i;
    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    for(i=0;i<4;i++){           //LED0-LED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}
    for(i=0;i<8;i++){           //SW0-SW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i=0;i<8;i++){           //SW0-SW7をプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}
 
    while(1){
        for (i= 0;i<4;i++){     //SW0とSW4の状態から順番に調べてループ処理する
            if((digitalRead(swGpio[i]) | digitalRead(swGpio[i+4]))==HIGH) //ビット毎のORビット演算子
            digitalWrite(ledGpio[i], HIGH); //HIGHを出力する
            else 
            digitalWrite(ledGpio[i], LOW);  //LOWを出力する
        }
    }
    return EXIT_SUCCESS;
}
