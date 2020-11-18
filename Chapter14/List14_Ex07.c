// List14_Ex07.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <string.h>         //文字列操作
#include <ctype.h>          //文字操作
#include <wiringPi.h>       //wiringPi
#include <wiringSerial.h>   //シリアルポート
#include <termios.h>        //ターミナルインタフェース
#include <unistd.h>         //POSIX
#include "MyPi.h"           //マイライブラリ

#define BPS 115200          //通信速度

/* グローバル変数 */
const int ledGpio[4] ={23,22,25,24};    //LEDのGPIOを配列で定義
/* プロトタイプ宣言 */
void Led4bit(int ledData);              //関数のプロトタイプ宣言
int isDigitString(char *str);

int main (void){
    int i;
    int k;              //str[10]配列の番号として使用
    int c;              //受信バイト数
    int rxvData;        //受信データ
    char str[10];       //受信データの文字列用配列
    int fd;
    struct termios param; //構造体変数

    int ledData;
    wiringPiSetupGpio();    //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){ //LED0からLED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);
    }

    fd = serialOpen( "/dev/serial0", BPS); //シリアルポートのオープン
    if( fd == -1){
        fprintf(stderr,"Error serialOpen.\n");
        exit(EXIT_FAILURE);
    }
    tcgetattr(fd, &param);          //パラメータの取得
        param.c_cflag &= ~CSIZE;    //データビットのクリア
        param.c_cflag |= CS8;       //データビットを8bitにセット
        param.c_cflag &= ~PARENB;   //パリティビットを無効にセット
        param.c_cflag &= ~CSTOPB;   //ストップビットを1bitにセット
    tcsetattr(fd,TCSANOW,&param);   //パラメータの有効化

    printf("LED表示データの受信テスト\n");
    serialFlush(fd);                //バッファをクリア
    k = 0;
    serialPrintf(fd,"\r\n10進数で0から15までの値を入力してください >>>");
    while(1){
        c = serialDataAvail(fd);    //受信データ数の取得
        if(c > 0){
            for(i = 0; i < c; i++){
                rxvData = serialGetchar(fd);
                serialPutchar(fd,rxvData);  //PC側へエコーバック
                if( rxvData == '\r'){       //改行(\r)なら、
                    str[k++] = '\0';        //nullを加えて文字列にする
                    k=0;
                    printf("%s\n",str);     //ターミナルに表示
                    ledData = atoi(str);    //文字列を整数に変換

                    if(isDigitString(str) == EXIT_FAILURE){ //10進数の数字のチェック
                        Led4bit(0);         //LEDの消灯
                        serialPrintf(fd,"\r\n10進数の値ではありません\r\n");
                    }else if(ledData < 0 || 15 < ledData){
                        Led4bit(0);         //LEDの消灯
                        serialPrintf(fd,"\r\n入力の値が範囲外です\r\n");
                    }else{
                        Led4bit(ledData);   //LEDを点灯
                    }
                    serialPrintf(fd,"\r\n10進数で0から15までの値を入力してください >>>");
                }else{
                    str[k++] = rxvData;     //配列に受信データを保存
                }
            }
        }
    }
    serialClose(fd);
    return EXIT_SUCCESS;
}

//関数名  void Led4bit(int ledData)
//引数    ledData  4bitデータ　0から15
//戻り値  なし
//概要    ledDataをLED0からLED３へ出力
void Led4bit(int ledData)
{
    int i;
    for (i = 0; i < 4; i++){            //LSBのLED0から順番に出力する
        digitalWrite(ledGpio[i], ledData&1); //LSBだけの値にして出力
        ledData = ledData >> 1;         //1bit 右シフト
    }
}

//関数名  int isDigitString(char *str)
//引数    *str 文字列
//戻り値  文字列が10進数の数字のときEXIT_SUCCESS、以外のときEXIT_FAILURE
//概要    文字列が10進数の数字かどうかチェック
int isDigitString(char *str){
    int i;
    for (i = 0; i < strlen(str); i++){
        if(isdigit(str[i]) == 0){   //文字が10進数の数字かどうかチェック
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
