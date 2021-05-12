// List8_Ex02.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf
#include <stdlib.h>         //EXIT_SUCCESS
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#include <wiringPiSPI.h>    //wiringPiSPISetup
#include "MyPi.h"           //マイライブラリ

//SPIインタフェース
#define SPI_SS0     0       //スレーブセレクト ADコンバータ
#define SPI_SPEED   500000  //500kHz

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
    unsigned short adcCode = 0;    //ADCコード
    wiringPiSetupGpio();
    wiringPiSPISetup(SPI_SS0, SPI_SPEED);  //SPIのセットアップ
    while(1){   //A/D変換処理
        adcCode = Mcp3208RW(SPI_SS0,MCP3208_CH7);
        printf("CH7 = %3XH", adcCode);
        printf("\t電圧 = %5.3f V\n", ((3.3/4096) * adcCode));
        delay(500);
    }
    return EXIT_SUCCESS;
}

