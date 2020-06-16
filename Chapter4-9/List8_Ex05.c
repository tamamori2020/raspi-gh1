// List8_Ex05.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>			//入出力
#include <stdlib.h>			//一般ユーティリティ
#include <string.h>			//文字列操作
#include <wiringPi.h>		//wiringPi
#include <wiringPiI2C.h>	//I2C用wiringPi
#include <wiringPiSPI.h>	//SPI用wiringPi
#include "MyPi.h"			//マイライブラリ

//LCD
#define LCD_ADR	0x3e		//LCD スレーブアドレス

//SPIインタフェース
#define SPI_SS0  0			//スレーブセレクト ADコンバータ
#define SPI_SS1  1			//スレーブセレクト DAコンバータ
#define SPI_SPEED   500000  //SPIのSCKクロック信号の周波数

//A/Dコンバータ MCP3208
#define MCP3208_CH0 0		// CH0入力
#define MCP3208_CH1 1		// CH1入力
#define MCP3208_CH2 2		// CH2入力
#define MCP3208_CH3 3		// CH3入力
#define MCP3208_CH4 4		// CH4入力
#define MCP3208_CH5 5		// CH5入力
#define MCP3208_CH6 6		// CH6入力
#define MCP3208_CH7 7		// CH7入力

#define DEBUG				//#ifdef DEBUGから#endifの有効

long map(long value, long fromLow, long fromHigh, long toLow, long toHigh);

int main(void)
{
	int i,k;
	int fd1;				//LCD用
	unsigned short adcCode;
	unsigned short adcCodeOld=0;
	wiringPiSetupGpio();
	wiringPiSPISetup(SPI_SS0, SPI_SPEED);	//ADコンバータ
	fd1 = wiringPiI2CSetup(LCD_ADR);		//LCDのI2Cセットアップ
	i = LcdSetup(fd1);						//LCDの初期化
	if(i<0){                                //初期化に失敗した時
		printf("LCD setup error.\n");
		return EXIT_FAILURE;
	}
	
	while(1){                //AD変換処理
		adcCode = Mcp3208RW(SPI_SS0,MCP3208_CH7);
		if(adcCode != adcCodeOld){
			k = map(adcCode,0,0xfff,0,16);   //A/D変換した値を0~100へ			
			LcdClear(fd1);
			if (k > 0){
				for(i=0;i<k;i++){            //*横棒グラフの生成
					LcdWriteChar(fd1,'*');
				}
			}
			adcCodeOld = adcCode;
			#ifdef DEBUG			
				printf("AD7=%3XH %5.3fV k=%d\n", adcCode,adcCode*(3.3/4096),k);//デバック表示用
			#endif
		}
		delay(500);
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

