// List14_Ex08.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_SUCCESS,etc
#include <termios.h>        //tcgetattr,etc
#include <unistd.h>         //usleep
#include <wiringSerial.h>   //serialOpen,etc
#include <wiringPiI2C.h>    //wiringPiI2CSetup
#include "MyPi.h"           //マイライブラリ:LcdSetup,etc

#define BPS     115200      //通信速度
#define LCD_ADR 0x3e        //LCD スレーブアドレス

int main (void){
    int i;
    int c;                  //受信データ数
    char rxdData;
    int fdSri;              //シリアルポート用
    int fdLcd;              //LCD用
    struct termios param;   //構造体変数

    fdLcd = wiringPiI2CSetup(LCD_ADR);  //LCDのI2Cセットアップ
    i=LcdSetup(fdLcd);                  //LCDの初期化
    if(i<0){
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

    printf("LCD表示の受信テスト\n");
    serialFlush(fdSri);                //バッファをクリア

    while(1){
        c = serialDataAvail(fdSri); //受信データ数の取得
        if(c>0){
            for(i=0;i<c;i++){
                rxdData = serialGetchar(fdSri);
                serialPutchar(fdSri,rxdData);       //PC側へエコーバック
                if(0x20<=rxdData && rxdData<=0x7f){
                    LcdWriteChar(fdLcd,rxdData);    //LCDにの表示
                    printf("%c",rxdData);fflush(stdout); //ターミナルに表示
                }else if( rxdData == '\r'){         //改行(\r)なら、
                    LcdNewline(fdLcd);              //LCDの改行を実行
                }else{
                    serialPrintf(fdSri, "文字コードが範囲外です\r\n");
                }
            }
        }else{
            usleep(1000);       //CPU使用率の抑制のため
        }
    }
    serialClose(fdSri);
    return EXIT_SUCCESS;
}
