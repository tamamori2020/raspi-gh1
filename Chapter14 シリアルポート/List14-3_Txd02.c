// List14-3_Txd02.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_SUCCESS,etc
#include <termios.h>        //tcgetattr,etc
#include <wiringSerial.h>   //serialOpen,etc

#define BPS 115200          //通信速度

int main (void){
    char txdData ='A';      //送信データ
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

    printf("PL011 送信テスト\n");
    serialFlush(fd);                //バッファのクリア
    for(;;){
        serialPutchar (fd, txdData); //1文字の送信
    }
    serialClose(fd);
    return EXIT_SUCCESS;
}
