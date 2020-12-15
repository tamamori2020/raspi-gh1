// List14-4_Rxd02.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <wiringPi.h>       //wiringPi
#include <wiringSerial.h>   //シリアルポート
#include <termios.h>        //ターミナルインタフェース
#include <unistd.h>         //POSIX

#define BPS 115200          //通信速度

int main (void){
    int i;
    int rxdData;            //受信データを格納する変数
    int c;                  //受信データ数
    int fd;
    struct termios param;   //構造体変数

    fd = serialOpen( "/dev/serial0", BPS); //シリアルポートのオープン
    if( fd == -1){
        fprintf(stderr,"Error serialOpen.\n");
        exit(EXIT_FAILURE);
    }
    tcgetattr(fd, &param);          //パラメータの取得
        param.c_cflag &= ~CSIZE;    //データビットのクリア
        param.c_cflag |= CS7;       //データビットを7bitにセット
        param.c_cflag |= PARENB;    //パリティビットを有効にセット
        param.c_cflag |= PARODD;    //奇数パリティにセット
        param.c_cflag |= CSTOPB;    //ストップビットを2bitにセット
    tcsetattr(fd,TCSANOW,&param);   //パラメータの有効化

    printf("PL011 受信テスト\n");
    serialFlush(fd);                //バッファをクリア
    while(1){
        c = serialDataAvail(fd);    //受信データ数の取得
        if(c>0){
            for (i=0;i<c;i++){
                rxdData = serialGetchar(fd);    // 文字の受信
                //serialPutchar(fd,rxdData);      //エコーバック
                printf("%c",rxdData); fflush(stdout);    //文字の表示
            }
        }
    }
    serialClose(fd);
    return EXIT_SUCCESS;
}
