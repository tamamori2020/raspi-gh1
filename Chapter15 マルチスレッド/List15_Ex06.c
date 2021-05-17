// List15_Ex06.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_SUCCESS,etc
#include <string.h>         //strcmp,strcpy,etc
#include <termios.h>        //tcgetattr,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //usleep
#include <wiringPi.h>       //digitalWrite,etc
#include <wiringSerial.h>   //serialPrintf,etc
#include <wiringPiI2C.h>    //wiringPiI2CSetup
#include "MyPi.h"           //マイライブラリ,LcdSetup,etc
#define SW7     21          //GPIO21をSW7と定義
#define BPS     115200      //通信速度
#define LCD_ADR 0x3e        //LCD スレーブアドレス
/* グローバル宣言　*/
static volatile int fdSri;  //シリアルポート用
static volatile int fdLcd;  //LCD用
/* プロトタイプ宣言 */
void *serialRxdLCD(void *arg); //受信のスレッド

int main (void){
    struct termios param;       //構造体変数
    pthread_t threadSerial;     //スレッドの宣言
    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    pinMode(SW7,INPUT);         //SW7を入力に設定
    pullUpDnControl(SW7,PUD_DOWN);      //SW7にプルダウン抵抗をつける
    fdLcd = wiringPiI2CSetup(LCD_ADR);  //LCDのI2Cセットアップ
    if(LcdSetup(fdLcd) < 0){            //LCDの初期化
        printf("LCD setup error.\n");
        return EXIT_FAILURE;
    }

    fdSri = serialOpen( "/dev/serial0", BPS); //シリアルポートのオープン
    if( fdSri == -1){
        fprintf(stderr,"Error serialOpen.\n");
        exit(EXIT_FAILURE);
    }
    
    tcgetattr(fdSri, &param);           //パラメータの取得
        param.c_cflag &= ~CSIZE;        //データビットのクリア
        param.c_cflag |= CS8;           //データビットを8bitにセット
        param.c_cflag &= ~PARENB;       //パリティビットを無効にセット
        param.c_cflag &= ~CSTOPB;       //ストップビットを1bitにセット
    tcsetattr(fdSri,TCSANOW,&param);    //パラメータの有効化

    if(pthread_create(&threadSerial, NULL, serialRxdLCD, NULL) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    printf("LCD表示の受信テスト\n");
    serialFlush(fdSri);                //バッファをクリア

    if(pthread_join(threadSerial, NULL) != 0){  //threadSerialスレッドの終了待ち
        fprintf(stderr,"Error pthread_join.\n");
        exit(EXIT_FAILURE);
    }

    serialPrintf(fdSri,"End of main.\r\n");
    serialClose(fdSri);
    printf("End of main.\n");
    return EXIT_SUCCESS;
}

//関数名  void *serialRxdLCD(void *arg)
//引数    なし
//戻り値  なし
//グローバル変数 fdSri,fdLcd
//概要    受信した文字をLCDに表示する
void *serialRxdLCD(void *arg){
    int i;
    int c;                  //受信データ数
    char rxvData;
    while(digitalRead(SW7) != HIGH){
        c = serialDataAvail(fdSri); //受信データ数の取得
        if(c > 0){
            for(i = 0; i < c; i++){
                rxvData = serialGetchar(fdSri);
                serialPutchar(fdSri,rxvData);       //PC側へエコーバック
                if(0x20 <= rxvData && rxvData <= 0x7f){
                    LcdWriteChar(fdLcd,rxvData);    //LCDにの表示
                    printf("%c",rxvData);fflush(stdout); //ターミナルに表示
                }else if( rxvData == '\r'){         //改行(\r)なら、
                    LcdNewline(fdLcd);              //LCDの改行を実行
                }else{
                    serialPrintf(fdSri, "文字コードが範囲外です\r\n");
                }
            }
        }
        usleep(1000);           //CPU使用率の抑制のため
    }
    serialPrintf(fdSri,"\r\nEnd of serialRxdLCD.\r\n");
    printf("\nEnd of serialRxdLCD.\n");
    pthread_exit(NULL);
}
