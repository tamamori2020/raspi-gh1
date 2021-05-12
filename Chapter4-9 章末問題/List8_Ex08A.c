// List8_Ex8A.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
// 修正：CPU使用率の抑制のためusleep(1000)を挿入

#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_SUCCESS
#include <unistd.h>         //usleep
#include <time.h>           //strftime,localtime
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#include <wiringPiSPI.h>    //wiringPiSPISetup
#include <wiringPiI2C.h>    //wiringPiI2CSetup
#include "MyPi.h"           //マイライブラリ

#define SW0        4        //GPIO4をSW0と定義
#define SW1        5        //GPIO5をSW1と定義
#define LED0       23       //GPIO23をLED0と定義	
#define LCD_ADR    0x3e     //LCD スレーブアドレス
#define ADT7410_ADR 0x48    //温度センサのスレーブアドレス
#define STANDBY    0        //測定の待機
#define START      1        //測定開始
#define STOP       2        //測定停止
#define DEBUG               //#ifdef DEBUGから#endifの有効

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

/* グローバル変数　*/
static volatile int g_state; //SW操作の変数
FILE *fp;                   //ファイルポインタ
/* プロトタイプ宣言 */
void IntSw0(void);          //SW0の割込み関数
void IntSw1(void);          //SW1の割込み関数

int main(void){
    int i;
    FILE *fp;               //ファイルポインタ
    int fd1;                //LCD
    char s1[16] ;           //LCD16文字分のバッファの確保
    unsigned short adcCode = 0;  //ADCコード
    time_t now;
    char fileName[32];
    char timeStamp[16];

    now = time(NULL);       //日時の取得
    strftime(fileName, sizeof(fileName), "%Y%m%d%H%M%S.csv",localtime(&now));
    if((fp = fopen (fileName,"w"))==NULL){  //ファイルのオープン
        printf("file open error.\n");       //オープンに失敗した時
        return EXIT_FAILURE;
    }

    wiringPiSetupGpio();            //BCMのGPIO番号を使用
    pinMode(SW0, INPUT);            //SW0を入力に設定
    pullUpDnControl(SW0,PUD_DOWN);  //プルダウン抵抗を有効にする
    pinMode(SW1, INPUT);            //SW1を入力に設定
    pullUpDnControl(SW1,PUD_DOWN);  //プルダウン抵抗を有効にする

    pinMode(LED0, OUTPUT);          //LED0を出力に設定
    digitalWrite(LED0,LOW);
    fd1 = wiringPiI2CSetup(LCD_ADR); //LCDのI2Cセットアップ
    i = LcdSetup(fd1);               //LCDの初期化
    if(i<0){                         //初期化に失敗した時
        printf("LCD setup error.\n");
        return EXIT_FAILURE;
    }
    wiringPiSPISetup(SPI_SS0, SPI_SPEED);  //SPIのセットアップ
    wiringPiISR(SW0, INT_EDGE_RISING, (void*)IntSw0);
    wiringPiISR(SW1, INT_EDGE_RISING, (void*)IntSw1);
    g_state = STANDBY;
    LcdWriteString(fd1,"Push SW0");
    
    while(1){
        if(g_state == START){
            digitalWrite(LED0,HIGH);
            now = time(NULL);
            strftime(timeStamp,sizeof(timeStamp),"%T",localtime(&now));  //"%T"="%H:%M:%S"
            adcCode = Mcp3208RW(SPI_SS0,MCP3208_CH7);
            sprintf(s1,"%5.3f",((3.3/4096) * adcCode));  //実数を文字列に変換
            LcdClear(fd1);                      //画面クリア
            LcdWriteString(fd1,timeStamp);      //LCDの１行目に時刻を表示
            LcdNewline(fd1);                    //LCDの改行
            LcdWriteString(fd1,s1);             //LCDの２行目に電圧を表示
            LcdWriteString(fd1," V");           //単位の表示
            fprintf(fp,"%s,%s\n",timeStamp,s1); //ファイルに記録
            #ifdef DEBUG
                 printf("%s, %s V\n",timeStamp,s1);  //ターミナルにデバック表示
            #endif
            delay(1000);                //待ち時間1秒
        }else if (g_state == STOP){
            digitalWrite(LED0,LOW);
            fclose(fp);                 //ファイルのクローズ
            break;
        }
        usleep(1000);       //CPU使用率の抑制のため
    }
    return EXIT_SUCCESS;
}

void IntSw0(void){
    g_state = START;
}

void IntSw1(void){
    g_state = STOP;
}

