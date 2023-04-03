// List7_Ex03.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
// 2021-05-12 修正:bool から　_Bool

#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_SUCCESS
#include <stdbool.h>        //true
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#include <wiringPiI2C.h>    //wiringPiI2CSetup
#include "MyPi.h"           //マイライブラリ

#define SW0         4       //GPIO4をSW0と定義
#define INTERVAL    300     //バウンシング対策時間ms
#define LCD_ADR     0x3e    //LCD スレーブアドレス
#define ADT7410_ADR 0x48    //温度センサのスレーブアドレス

/* グローバル変数　*/
static volatile _Bool g_sw0; //SW0操作の変数
static volatile unsigned int g_millis;  //経過時間ms
/* プロトタイプ宣言 */
void IntSw0(void);          //SW0の割込み関数

int main(void){
    int i;
    int fd1;            //LCD
    int fd2;            //ADT7410
    double temp;        //温度 実数値℃
    char s1[17];        //LCD16文字+null
 
    wiringPiSetupGpio();                //GPIOの初期化命令
    pinMode(SW0, INPUT);                //SW0を入力に設定
    pullUpDnControl(SW0,PUD_DOWN);      //プルダウン抵抗を有効にする
    
    fd1 = wiringPiI2CSetup(LCD_ADR);    //LCDのI2Cセットアップ
    i = LcdSetup(fd1);
    if(i<0){                            //初期化に失敗した時
        printf("LCD setup error.\n");
        return EXIT_FAILURE;
    }
    fd2 = wiringPiI2CSetup(ADT7410_ADR);//温度センサのI2Cセットアップ

    wiringPiISR(SW0, INT_EDGE_RISING, IntSw0);
    g_millis = millis();
    g_sw0 = true;
    while(1){
        temp=Adt7410Read(fd2);          //温度データの取得
        if(g_sw0 == true){
            sprintf(s1,"%4.1f C",temp); //摂氏度を文字列
        }else{
            temp = temp*9/5+32;         //摂氏度から華氏度に変換
             sprintf(s1,"%4.1f F",temp); //華氏度を文字列
        }
        LcdClear(fd1);                  //画面クリア
        LcdWriteString(fd1, s1);        //LCDに温度を表示
        printf("%f  %s\n",temp,s1);     //ターミナルにデバック表示
        delay(500);                     //0.5秒待ち時間
    }
    return EXIT_SUCCESS;
}

void IntSw0(void){
    unsigned int diff,now;
    now = millis();
    diff = now - g_millis;
    if(diff > INTERVAL){
        g_sw0 = !g_sw0;
        g_millis = now;
    }
}
