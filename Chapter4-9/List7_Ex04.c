// List7_Ex04.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>			//入出力
#include <stdlib.h>			//一般ユーティリティ
#include <string.h>			//文字列操作
#include <limits.h>			//整数型の大きさ
#include <time.h>			//日付及び時間
#include <wiringPi.h>		//wiringPi
#include <wiringPiI2C.h>	//I2C用wiringPi
#include "MyPi.h"			//マイライブラリ

#define SW0	    4			//GPIO4をSW0と定義
#define SW1		5			//GPIO5をSW1と定義
#define LED0 	23			//GPIO23をLED0と定義	
#define LCD_ADR	0x3e		//LCD スレーブアドレス
#define ADT7410_ADR	0x48	//温度センサのスレーブアドレス
#define STANDBY		0		//測定の待機
#define START		1		//測定開始
#define STOP		2		//測定停止
#define DEBUG				//#ifdef DEBUGから#endifの有効

/* グローバル変数　*/
static volatile int g_state; 	//SW操作の変数

/* プロトタイプ宣言 */
void IntSw0(void);         	//SW0の割込み関数
void IntSw1(void);         	//SW1の割込み関数

int main(void){
	int i;
	FILE *fp;				//ファイルポインタ
	int fd1;				//LCD
	int fd2;				//ADT7410
	float temp;				//温度 実数値℃
	char s1[16] ;			//LCD16文字分のバッファの確保

	time_t now;
	char fileName[32];
	now = time(NULL);
	strftime(fileName, sizeof(fileName), "%Y%m%d%H%M%S.csv",localtime(&now));
	if((fp = fopen (fileName,"w"))==NULL){
		printf("file open error.\n");
		return EXIT_FAILURE;
	}
    wiringPiSetupGpio();			//GPIOの初期化命令
    pinMode(SW0, INPUT);		    //SW0を入力に設定
    pullUpDnControl(SW0,PUD_DOWN);	//プルダウン抵抗を有効にする
    pinMode(SW1, INPUT);		    //SW1を入力に設定
    pullUpDnControl(SW1,PUD_DOWN);	//プルダウン抵抗を有効にする

    pinMode(LED0, OUTPUT);			//LED0を出力に設定	
    digitalWrite(LED0,LOW);	
	fd1 = wiringPiI2CSetup(LCD_ADR);		//LCDのI2Cセットアップ
	i = LcdSetup(fd1);
    if(i<0){								//初期化に失敗した時
		printf("LCD setup error.\n");
		return EXIT_FAILURE;
	}
	LcdWriteString(fd1,"Push SW0");
	fd2 = wiringPiI2CSetup(ADT7410_ADR);	//温度センサのI2Cセットアップ
	
    wiringPiISR(SW0, INT_EDGE_RISING, (void*)IntSw0);
	wiringPiISR(SW1, INT_EDGE_RISING, (void*)IntSw1);
	g_state = STANDBY;	
	i = 1;
	while(1){
		if(g_state == START){
			digitalWrite(LED0,HIGH);
			temp=Adt7410Read(fd2);			//温度データの取得
			sprintf(s1,"%d,%4.1f",i,temp);	//摂氏度の値を文字列化
			LcdClear(fd1);					//画面クリア
			LcdWriteString(fd1, s1);		//LCDに温度を表示
			LcdWriteString(fd1, " C");		//単位の表示	    
			fprintf(fp,"%s\n",s1);			//ファイルに記録
			#ifdef DEBUG
				printf("%s\n",s1);			//ターミナルにデバック表示
			#endif
			i++;
			delay(500);						//0.5秒待ち時間
		}else if (g_state == STOP){
			digitalWrite(LED0,LOW);
			fclose(fp);						//ファイルのクローズ
			break;
		}
	}
	return EXIT_SUCCESS;
}

void IntSw0(void){
	g_state = START;
}

void IntSw1(void){
	g_state = STOP;
}
