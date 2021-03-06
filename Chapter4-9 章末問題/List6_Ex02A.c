//List6_Ex02A.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
// 修正：CPU使用率の抑制のためusleep(1000)を挿入

#include <stdio.h>
#include <stdlib.h>         //EXIT_SUCCESS
#include <unistd.h>         //usleep
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#include <softPwm.h>        //softPwmCreate,softPwmWrite

#define BUZZER  18          //GPIO18をBUZZERと定義
const int swGpio[8] = {4,5,6,26,17,27,20,21};   //SW GPIOを配列で定義

int main (void){
    int i;
    int pwmRange = 23;      //440Hzの周期は約2.3ms、2.3ms/最小変化幅100us=23
    int initialValue = 0;   //softPwmCreate関数実行時には出力信号0V
    int value;              //softPwmWrite関数の引数
    wiringPiSetupGpio();    //BCMのGPIO番号を使用
    pinMode(BUZZER, OUTPUT);//BUZZERを出力に設定
    for(i = 0; i < 8; i++){             //SW0-SW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i = 0; i < 8; i++){             //SW0-SW7をプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}
    softPwmCreate(BUZZER,initialValue,pwmRange); //ソフトPWMの設定

     while(1){
        if(digitalRead(swGpio[0])==HIGH){    //SW0が押された
            value = pwmRange/2;         //デューティ比約50%
            softPwmWrite(BUZZER,value); //PWM信号の出力
        }
        else{                       //SW0が押されてない
            softPwmWrite(BUZZER,0); //PWM信号の停止
        }
        usleep(1000);           //CPU使用率の抑制のため
    }
    return EXIT_SUCCESS;
}
