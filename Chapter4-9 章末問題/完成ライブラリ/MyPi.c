// MyPi.c
// gcc -Wall -c "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%f"はソースファイル名のこと

#include <stdio.h>			//入出力
#include <stdlib.h>			//一般ユーティリティ
#include <string.h>			//文字列操作
#include <wiringPi.h>		//wiringPi
#include <wiringPiI2C.h>	//I2C用wiringPi
#include <wiringPiSPI.h>	//SPI用wiringPi
#include "MyPi.h"			//マイライブラリ

//A/Dコンバータ MCP3802
//マクロ定義なし

//D/Aコンバータ MCP4922
#define MCP4922_DACA	0		//VOUTAの出力
#define MCP4922_DACB	1		//VOUTBの出力
#define MCP4922_GAx1	0x20	//ゲイン1倍
#define MCP4922_SHDN	0x10	//VOUTピン出力の有効

//温度センサ ADT7410
#define ADT7410_ADR	0x48	//温度センサのスレーブアドレス
#define ADT7410_REG	0		//温度レジスタの番号

//LCD　AQM1602A
#define LCD_ADR	0x3e		//LCD スレーブアドレス
#define LCD_IR	0x00		//インストラクション
#define LCD_DR 0x40			//データ

// LCD用グローバル変数
int g_position =0;			//文字の位置
int g_line = 0;				//行の位置
int g_charsPerLine = 16;	//最大表示文字数
int g_dispLines=2;			//最大表示行数

//関数名  unsigned short Mcp3208RW(int ssNo, unsigned char adcChannel)
//引数    ssNo スレーブ選択番号 0=SS0, 1=SS1
//引数    adcChannel    MPC3208のチャンネル番号　0から7
//戻り値  12bitAD変換データ 0000-0fff
//概要　　0Vから約3.3Vの範囲のアナログ電圧を12bit分解能でディジタル値に変換 
unsigned short Mcp3208RW(int ssNo, unsigned char adcChannel)
{
	unsigned short adcCode = 0;
	unsigned char buff[3];
	buff[0]=buff[1]=buff[2]=0;			
	buff[0] =0b00000110|(adcChannel>>2);  	// 0,0,0,0,0,ST,SGL,D2
	buff[1]=adcChannel<<6;					// D1,D0,0,0,0,0,0,0
	wiringPiSPIDataRW(ssNo, buff, sizeof(buff));
	//デバック表示用
	//printf("buff0=%3XH buff1=%x buff2=%3XH \n",buff[0],buff[1],buff[2]);  
	adcCode =(((buff[1]&0x0f) << 8) | buff[2]); // 0000+MSB----------LSB
	return adcCode;
}

//関数名  int Mcp4922Write(int ssNo,unsigned char dacCh, unsigned short dacCode)
//引数    ssNo スレーブ選択番号 0=SS0, 1=SS1
//引数    dacCh 0=VOUTA, 1=VOUTB
//引数    dacCode 12bit DACコード 0000-0fff
//戻り値  wiringPiSPIDataRWの戻り値
//概要　　0Vから約3.3Vの範囲で、12bit分解能でアナログ電圧を出力
int Mcp4922Write(int ssNo,unsigned char dacCh, unsigned short dacCode)
{
	int i;
	unsigned char buff[2];
	unsigned short dacData;
	buff[0]=0;
	if(dacCh == MCP4922_DACB){							//DACB
		buff[0] = buff[0]|0x80;	
	}
	// DACA、バッファなし、ゲイン1倍、VOUTA有効	
	buff[0]=buff[0]|MCP4922_GAx1|MCP4922_SHDN;
	dacData=((dacCode>>8)&0x0f);				
	buff[0]=buff[0]|dacData;					//MSB 4bit DAデータ上位4ビット
	dacData= dacCode&0x00ff;
	buff[1]=dacData;							//LSB 8bit DAデータ下位8ビット
	
	i=wiringPiSPIDataRW(ssNo, buff, sizeof(buff));  //SPIバスへ出力
	return i;
}

//関数名  double Adt7410Read(int fd)
//引数    fd  wiringPiI2CSetup関数で取得したファイルハンドル
//戻り値  double型、温度データ 単位℃
//概要    温度センサADT7410による温度データの取得
double Adt7410Read(int fd){
	unsigned short wdat;				//温度センサバイナリ値
	double temp;						//温度 実数値℃
	const float tempDelta = 0.0625F;	//温度の刻値　0.0625℃
	wdat = wiringPiI2CReadReg16(fd, ADT7410_REG);	//センサよりデータを取得
	wdat = (wdat&0xff00)>>8 | (wdat&0xff)<<8;		//上位と下位のバイトを交換する
	wdat = wdat >>3;								//13bitフォーマット形式
	if ((wdat&0x1000)==0){				//温度の正負を判断
		temp = wdat *tempDelta;			//プラス温度の計算
	}else{
		temp = ((~wdat&0x1fff)+1)*(-tempDelta);	// マイナス温度の計算、2の補数計算
	}
	return temp;
}

//関数名  int LcdSetup(int fd)
//引数    fd  wiringPiI2CSetup関数で取得したファイルハンドル
//戻り値  エラーのときは-1
//概要    LCDの初期化
int LcdSetup(int fd){
	int i,k;
	char lcdCmd1[6]={0x38,0x39,0x14,0x73,0x56,0x6c}; //データシートより
	char lcdCmd2[3]={0x38,0x01,0x0c};				 //データシートより
	
	for (i=0;i<6;i++){
		k = wiringPiI2CWriteReg8(fd,LCD_IR,lcdCmd1[i]);
		delay(20);		//20ms時間待ち
	}
	delay(200);			//200ms時間待ち
	
	for (i=0;i<3;i++){
		k = wiringPiI2CWriteReg8(fd,LCD_IR,lcdCmd2[i]);
		delay(20);		//20ms時間待ち	
	}
	return k;
}

//関数名  int LcdWriteChar(int fd, char c)
//引数    fd  wiringPiI2CSetup関数で取得したファイルハンドル
//       c   1文字
//戻り値  エラーのときは-1
//概要    1文字の表示
int LcdWriteChar(int fd, char c){
	int i;
    //行の最大文字数を超えたかの判定
	if( g_position==(g_charsPerLine*(g_line+1))){		
		LcdNewline(fd);		//改行処理
	}
	i = wiringPiI2CWriteReg8(fd,LCD_DR,c);
//	printf("%c",c);fflush;	//debug用
	g_position +=1;
    delay(1);               //1ms時間待ち
	return i;
}

//関数名  int LcdNewline(int fd)
//引数    fd  wiringPiI2CSetup関数で取得したファイルハンドル
//戻り値  エラーのときは-1
//概要    LCD改行処理
int LcdNewline(int fd){
	int i;
	if (g_line == (g_dispLines-1)){
		LcdClear(fd);
	}else{
		g_line +=1;
		i = wiringPiI2CWriteReg8(fd,LCD_IR,0xc0);
        delay(10);      //10ms時間待ち
	}
	return i;
}

//関数名  int LcdClear(int fd)
//引数    fd  wiringPiI2CSetup関数で取得したファイルハンドル
//戻り値  エラーのときは-1
//概要    LCDのディスプレイクリア
int LcdClear(int fd){
	int i;
	i = wiringPiI2CWriteReg8(fd,LCD_IR,0x01);
	g_position =0;
	g_line=0;
	delay(10);			//10ms時間待ち
	return i;
}

//関数名  int LcdWriteString(int fd, char *s)
//引数    fd  wiringPiI2CSetup関数で取得したファイルハンドル
//       *s   文字列
//戻り値  エラーのときは-1
//概要    LCDに文字列を表示
int LcdWriteString(int fd, char *s){
	int i,k;
	for(i=0;i<strlen(s);i++){
		k = LcdWriteChar(fd, s[i]);
//		putchar(s[i]);	//debug用
		delay(1);       //1ms時間待ち
	}
	return k;
}


