// List7_Ex02.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_SUCCESS
#include <wiringPi.h>       //delay
#include <wiringPiI2C.h>    //wiringPiI2CSetup
#include "MyPi.h"           //マイライブラリ

#define LCD_ADR     0x3e    //LCD スレーブアドレス
#define ADT7410_ADR 0x48    //温度センサのスレーブアドレス

int main(void){
    int i;
    int fd1;        //LCD
    int fd2;        //ADT7410
    float temp;                         //温度 実数値℃
    char s1[16];                        //LCD16文字分のバッファの確保

    fd1 = wiringPiI2CSetup(LCD_ADR);    //LCDのI2Cセットアップ
    i = LcdSetup(fd1);
    if(i<0){                            //初期化に失敗した時
        printf("LCD setup error.\n");
        return EXIT_FAILURE;
    }
    fd2 = wiringPiI2CSetup(ADT7410_ADR); //温度センサのI2Cセットアップ

    while(1){
        temp=Adt7410Read(fd2);          //温度データの取得
        sprintf(s1,"%4.1f C",temp);     //実数を文字列に変換
        LcdClear(fd1);                  //画面クリア
        LcdWriteString(fd1, s1);        //LCDに温度を表示
        printf("%f  %s\n",temp,s1);     //ターミナルにデバック表示
        delay(500);                     //0.5秒待ち時間
    }
    return EXIT_SUCCESS;
}
