// List7_Ex01.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf
#include <stdlib.h>
#include <wiringPi.h>       //delay
#include <wiringPiI2C.h>    //wiringPiI2CSetup
#include "MyPi.h"           //マイライブラリ

#define ADT7410_ADR	0x48    //温度センサのスレーブアドレス

int main(void){
    int fd;                             //ファイルディスクリプタ
    double temp;                         //温度 実数値℃
    fd = wiringPiI2CSetup(ADT7410_ADR); //温度センサのI2Cセットアップ
    while(1){
        temp=Adt7410Read(fd);
        printf("%4.1f C\n",temp);
        delay(500);	
    }
}
