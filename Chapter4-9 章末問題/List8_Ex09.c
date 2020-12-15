// List8_Ex09.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <string.h>         //文字列操作
#include <math.h>           //数学
#include <wiringPi.h>       //wiringPi
#include <wiringPiSPI.h>    //SPI用wiringPi
#include "MyPi.h"           //マイライブラリ  追加

//SPIインタフェース
#define SPI_SS1     1       //スレーブセレクト
#define SPI_SPEED   500000  //SPIのSCKクロック信号の周波数

//D/Aコンバータ　MCP4922
#define MCP4922_DACA  0     //VOUTAの出力
#define MCP4922_DACB  1     //VOUTBの出力
#define MCP4922_GAx1  0x20  //ゲイン1倍
#define MCP4922_SHDN  0x10  //VOUTピン出力の有効

//円周率
#define PI      3.141593F   //円周率(単精度)
#define DIV     360         //正弦波信号の360等分

//#define DEBUG             //#ifdef DEBUGから#endifの有効
int main (void){
    int i;
    unsigned int dacCode;
    const short daDelta=806; //DACの最小電圧　806uV=3.3V÷4096
    long sinWave;            //sin波電圧 uV 0Vから3300000uV
    wiringPiSetupGpio();
    wiringPiSPISetup(SPI_SS1, SPI_SPEED);	//SPIのセットアップ

    while(1){
        for (i=0;i<DIV;i++){
            sinWave = 1650000*sin(2*PI*i/DIV)+1650000;  //uVの単位として、整数で計算
            dacCode = sinWave/daDelta;                  //12bit(4096)の刻み値からDA変換データを求める
            Mcp4922Write(SPI_SS1,MCP4922_DACA,dacCode);
            #ifdef DEBUG
                printf("%ld %3x\n",sinWave,dacCode);
            #endif
        }
    }
    return EXIT_SUCCESS;
}

