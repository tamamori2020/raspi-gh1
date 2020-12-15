//List6_Ex03.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <wiringPi.h>       //wiringPi

#define BUZZER  18          //GPIO18をBUZZERと定義
const int swGpio[8] = {4,5,6,26,17,27,20,21};   //SW GPIOを配列で定義

int main (void){
    int i;
//  Pi3B/3B+とPi4Bのどちらかを選択。
    int divisor = 1227;      //Pi4Bの場合、内部クロック(54MHz)÷(440*100)=1227
//    int divisor = 436;       //Pi3B/3B+の場合、内部クロック(19.2MHz)÷(440*100)=436

    unsigned int range = 100; //PWMのレンジ値100分割
    wiringPiSetupGpio();    //BCMのGPIO番号を使用
    for(i=0;i<8;i++){       //SW0-SW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i=0;i<8;i++){       //SW0-SW7をプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}	
    pinMode(BUZZER, PWM_OUTPUT);    //BUZZERをPWM出力に設定
    pwmSetClock(divisor);           //内部クロックの分周の設定
    pwmSetRange(range);             //階調の設定	
    pwmSetMode(PWM_MODE_MS);        //信号波形モードの設定

    while(1){
        if(digitalRead(swGpio[0])==HIGH){ //SW0が押された
            pwmWrite(BUZZER,range/2);  //デューティ比50%
        }
        else{                           //SW0が押されてない
            pwmWrite(BUZZER,0);         //PWM信号出力停止
        }
    }
    return EXIT_SUCCESS;
}
