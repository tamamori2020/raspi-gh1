// List8_Ex07.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_SUCCESS
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#include <wiringPiSPI.h>    //wiringPiSPISetup
#include <wiringPiI2C.h>    //wiringPiI2CSetup
#include "MyPi.h"           //マイライブラリ

//BUZZER&SW
#define BUZZER      18      //GPIO18をBUZZERと定義
#define SW0         4       //GPIO4をSW0と定義

//LCD
#define LCD_ADR     0x3e    //LCD スレーブアドレス

//SPIインタフェース
#define SPI_SS0     0       //スレーブセレクト ADコンバータ
#define SPI_SS1     1       //スレーブセレクト DAコンバータ
#define SPI_SPEED   500000  //SPIのSCKクロック信号の周波数

//A/Dコンバータ MCP3208
#define MCP3208_CH0 0       // CH0入力
#define MCP3208_CH1 1       // CH1入力
#define MCP3208_CH2 2       // CH2入力
#define MCP3208_CH3 3       // CH3入力
#define MCP3208_CH4 4       // CH4入力
#define MCP3208_CH5 5       // CH5入力
#define MCP3208_CH6 6       // CH6入力
#define MCP3208_CH7 7       // CH7入力

#define DEBUG               //#ifdef DEBUGから#endifの有効
/* プロトタイプ宣言 */
long map(long value, long fromLow, long fromHigh, long toLow, long toHigh);

int main(void)
{
    int i;
    int fd1;                //LCD用
    char s1[17];            //LCD16文字+null
    int fpwm;
    unsigned short adcCode = 0;
    
//  Pi3B/3B+とPi4Bのどちらかを選択。
    int divisor = 540;          //Pi4Bの場合、内部クロック(54MHz)の分周して最小変化幅を10usにする設定値
//    int divisor = 192;          //Pi3B/3B+の場合、内部クロック(19.2MHz)の分周して最小変化幅を10usにする設定値

    unsigned int range;         //PWMのレンジ値
    wiringPiSetupGpio();
    wiringPiSPISetup(SPI_SS0, SPI_SPEED);  //ADコンバータ
    pinMode(SW0,INPUT);             //SW0を入力に設定
    pullUpDnControl(SW0,PUD_DOWN);  //SW0にプルダウン抵抗をつける

    pinMode(BUZZER, PWM_OUTPUT);    //BUZZERをPWM信号の出力端子に設定
    pwmSetClock(divisor);           //内部クロックの分周の設定
    pwmSetMode(PWM_MODE_MS);        //信号波形モードの設定

    fd1 = wiringPiI2CSetup(LCD_ADR);  //LCDのI2Cセットアップ
    i = LcdSetup(fd1);                //LCDの初期化
    if(i<0){                          //初期化に失敗した時
        printf("LCD setup error.\n");
        return EXIT_FAILURE;
    }
    LcdWriteString(fd1,"Push SW0");

    while(1){
        adcCode = Mcp3208RW(SPI_SS0,MCP3208_CH7); //VR1の電圧を0-FFFHへA/D変換
        range = map(adcCode,0,0xfff,1000,2);      //A/D変換した値を1000~2へ
        if(digitalRead(SW0)==HIGH){               //SW0が押された
            pwmSetRange(range);                   //階調の設定
            pwmWrite(BUZZER,range/2);             //デューティ比50%
            fpwm = 1/(range*(float)0.00001);      //周期から周波数を計算, 10us=0.00001
            sprintf(s1,"%d Hz",fpwm);             //文字列に変換
            LcdClear(fd1);                        //画面クリア
            LcdWriteString(fd1,s1);               //LCDに電圧を表示
            #ifdef DEBUG
                printf("%3XH %d %dHz\n",adcCode,range,fpwm);//デバック用
            #endif
        }else{                          //SW0が押されてない
            pwmWrite(BUZZER,0);         //PWM信号出力停止
        }
        delay(100);
    }
    return EXIT_SUCCESS;
}

//関数名  long map(long value, long fromLow, long fromHigh, long toLow, long toHigh)
//引数    元の数値value、元の範囲の最小値fromLowと最大値fromHigh、新しい範囲の最小値toLowと最大値toHigh
//戻り値  新しい範囲に変換された数値
//概要    数値を元の範囲から新しい範囲に変換する。
long map(long value, long fromLow, long fromHigh, long toLow, long toHigh)
{
    return(value -fromLow)*(toHigh-toLow)/(fromHigh-fromLow)+toLow;
}

