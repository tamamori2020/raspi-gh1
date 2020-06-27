// List5_Ex03.c
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
 
    for(;;){                    //永久ループ
        if(digitalRead(swGpio[0])==HIGH){ //SW0がHならLED０を点滅させます
            while(1){
                digitalWrite(ledGpio[0],HIGH);
                delay(1000);
                digitalWrite(ledGpio[0],LOW);
                delay(1000);
                if(digitalRead(swGpio[1])==HIGH){ //SW1がHならwhileループ文から抜けます
                    break;}
            }
        }
    }
    return EXIT_SUCCESS;
}