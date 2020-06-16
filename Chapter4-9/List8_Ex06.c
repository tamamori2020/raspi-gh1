// List8_Ex06.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>			//入出力
#include <stdlib.h>			//一般ユーティリティ
#include <string.h>			//文字列操作
#include <wiringPi.h>		//wiringPi
#include <wiringPiSPI.h>	//SPI用wiringPi
#include <softPwm.h>		//ソフトウェア方式PWM用wiringPi
#include "MyPi.h"			//マイライブラリ

//LED
#define LED0 23				//GPIO23をLED0と定義

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
	unsigned short adcCode = 0;
    int pwmRange = 100;		//100Hzの周期は1ms、pwmRange*100us=10ms
    int initialValue = 0;   //softPwmCreate関数実行時には出力信号0V
	int value;              //softPwmWrite関数の引数	
	wiringPiSetupGpio();
	wiringPiSPISetup(SPI_SS0, SPI_SPEED);      //ADコンバータ
	pinMode(LED0, OUTPUT);                     //LED0を出力に設定
	softPwmCreate(LED0,initialValue,pwmRange); //ソフトPWMの設定
	
	while(1){
		adcCode = Mcp3208RW(SPI_SS0,MCP3208_CH7);   //VR1の電圧を0-FFFHへA/D変換
//		value = (float)adcCode/0xfff*100;           //A/D変換した値を0~100へ　map関数使用しない場合
		value = map(adcCode,0,0xfff,0,100);         //A/D変換した値を0~100へ
		#ifdef DEBUG
			printf("%3XH　%d\n",adcCode, value);	    //デバック用
		#endif
		softPwmWrite(LED0,value);                   //デューティ比の設定
		delay(100);                                 //待ち時間100ms

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
