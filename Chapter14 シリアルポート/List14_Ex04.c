// List14_Ex04.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_SUCCESS,etc
#include <termios.h>        //tcgetattr,etc
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#include <wiringSerial.h>   //serialOpen,etc


#define BPS 115200          //通信速度

/* グローバル変数 */
const int swGpio[8] = {4,5,6,26,17,27,20,21};   //SW GPIOを配列で定義

int main (void){
    int i,k;
    int fd;
    unsigned int swData;            //SW0からSW7までの状態を保存
    struct termios param;           //構造体変数

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

    wiringPiSetupGpio();            //BCMのGPIO番号を使用
    for(i = 0; i < 8; i++){         //SW0-SW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i = 0; i < 8; i++){         //SW0-SW7をプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}

    printf("送信テスト\n");
    serialFlush(fd);                //バッファをクリア

    while(1){
    swData = 0;
    for(i = 7; i >= 0; i--){        //MSBのSW7より8bitのデータを代入
        k=digitalRead(swGpio[i]);
        swData = swData<<1;
        swData = swData+k;
    }
        serialPrintf(fd,"SW data = %d\r\n",swData); //シリアルポートへ出力
        delay(100);
    }
    serialClose(fd);
    return EXIT_SUCCESS;
}
