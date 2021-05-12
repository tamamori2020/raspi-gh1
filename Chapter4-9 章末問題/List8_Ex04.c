// List8_Ex04.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf
#include <stdlib.h>         //EXIT_SUCCESS
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#include <wiringPiSPI.h>    //wiringPiSPISetup
#include <wiringPiI2C.h>    //wiringPiI2CSetup
#include "MyPi.h"           //マイライブラリ

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

int main(void)
{
    int i;
    int fd1;                //LCD用
    char s1[16];            //LCD16文字分のバッファの確保
    unsigned short adcCode = 0;
    unsigned short adcCodeOld;
    wiringPiSetupGpio();
    wiringPiSPISetup(SPI_SS0, SPI_SPEED);   //ADコンバータ
    fd1 = wiringPiI2CSetup(LCD_ADR);        //LCDのI2Cセットアップ
    i = LcdSetup(fd1);                      //LCDの初期化
    if(i<0){                                //初期化に失敗した時
        printf("LCD setup error.\n");
        return EXIT_FAILURE;
    }

    while(1){               //AD変換処理
        adcCode = Mcp3208RW(SPI_SS0,MCP3208_CH7);
        if (adcCode != adcCodeOld){
            sprintf(s1,"CH7=%3XH %5.3fV",adcCode, ((3.3/4096) * adcCode));  //実数を文字列に変換
            LcdClear(fd1);              //画面クリア
            LcdWriteString(fd1,s1);     //LCDに電圧を表示
            printf("%s\n",s1);          //ターミナルにデバック表示
            adcCodeOld = adcCode;
        }
        delay(500);
    }
    return EXIT_SUCCESS;
}
