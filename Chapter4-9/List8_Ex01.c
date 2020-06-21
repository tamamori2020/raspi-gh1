// List8_Ex01.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <string.h>         //文字列操作
#include <wiringPi.h>       //wiringPi
#include <wiringPiSPI.h>    //SPI用wiringPi
#include "MyPi.h"           //マイライブラリ

//SPIインタフェース
#define SPI_SS1     1       //スレーブセレクト
#define SPI_SPEED   500000  //SPIのSCKクロック信号の周波数

//D/Aコンバータ　MCP4922
#define MCP4922_DACA    0       //VOUTAの出力
#define MCP4922_DACB    1       //VOUTBの出力
#define MCP4922_GAx1    0x20    //ゲイン1倍
#define MCP4922_SHDN    0x10    //VOUTピン出力の有効

int main (void){
    unsigned int dacCode;
    wiringPiSetupGpio();
    wiringPiSPISetup(SPI_SS1, SPI_SPEED);	//SPIのセットアップ
    while(1){
        printf("16進数で0からfffまでの値を入力してください >>>");
        scanf("%x",&dacCode);
        if(0<=dacCode && dacCode <=0xfff){
            printf("VoutA,VoutB = %5.3f V\n\n", ((3.3/4096) * dacCode));
            Mcp4922Write(SPI_SS1,MCP4922_DACA,dacCode);
            Mcp4922Write(SPI_SS1,MCP4922_DACB,dacCode);
        }else{
            printf("値が範囲外です\n\n");
            Mcp4922Write(SPI_SS1,MCP4922_DACA,0);
            Mcp4922Write(SPI_SS1,MCP4922_DACB,0);
        }
    }
    return EXIT_SUCCESS;
}

