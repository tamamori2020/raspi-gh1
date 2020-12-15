// List8_Ex03.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <string.h>         //文字列操作
#include <wiringPi.h>       //wiringPi
#include <wiringPiSPI.h>    //SPI用wiringPi
#include "MyPi.h"           //マイライブラリ

//SPIインタフェース
#define SPI_SS0     0       //スレーブセレクト ADコンバータ
#define SPI_SS1     1       //スレーブセレクト DAコンバータ
#define SPI_SPEED   500000  //SPIのSCKクロック信号の周波数

//D/Aコンバータ　MCP4922
#define MCP4922_DACA    0       //VOUTAの出力
#define MCP4922_DACB    1       //VOUTBの出力
#define MCP4922_GAx1    0x20    //ゲイン1倍
#define MCP4922_SHDN    0x10    //VOUTピン出力の有効

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
    unsigned short dacCode, adcCode;
    wiringPiSetupGpio();
    wiringPiSPISetup(SPI_SS0, SPI_SPEED);    //ADコンバータ
    wiringPiSPISetup(SPI_SS1, SPI_SPEED);    //DAコンバータ

    while(1){
        for(i=0;i<=0x1000;i+=0x100){
            dacCode=i;
            if(dacCode != 0){
                dacCode-=1;
            }
            Mcp4922Write(SPI_SS1,MCP4922_DACB,dacCode);
            printf("DA-VoutB=%3XH  ",dacCode);
            adcCode = Mcp3208RW(SPI_SS0,MCP3208_CH1);
            printf("AD-CH1=%3XH %5.3fV\n", adcCode,adcCode*(3.3/4096));
            delay(500);
        }
    }
    return EXIT_SUCCESS;
}
