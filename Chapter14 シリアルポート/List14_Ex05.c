// List14_Ex05.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_SUCCESS,etc
#include <termios.h>        //tcgetattr,etc
#include <wiringPi.h>       //delay,etc
#include <wiringSerial.h>   //serialOpen,etc
#include <wiringPiI2C.h>    //wiringPiI2CSetup,etc

#define BPS     115200      //通信速度
#define ADT7410_ADR 0x48    //温度センサのスレーブアドレス
#define ADT7410_REG 0       //温度レジスタの番号

double Adt7410Read(int fd); //温度測定の関数

int main (void){
    int fdSri;              //シリアルポート用
    int fdTmp;              //温度センサ用
    float temp;             //温度 実数値℃
    struct termios param;   //構造体変数

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
    
    fdTmp = wiringPiI2CSetup(ADT7410_ADR);  //温度センサのI2Cオープン	
    printf("温度データの送信テスト\n");
    serialFlush(fdSri);                 //バッファをクリア

    while(1){
        temp = Adt7410Read(fdTmp);        //温度の測定
        serialPrintf(fdSri,"%4.1f C\r\n",temp); //シリアルポートへ出力
        delay(500);
    }
    serialClose(fdSri);
    return EXIT_SUCCESS;
}

//関数名  double Adt7410Read(int fd)
//引数    fd  wiringPiI2CSetup関数で取得したファイルハンドル
//戻り値  double型、温度データ 単位℃
//概要    温度センサADT7410による温度データの取得
double Adt7410Read(int fd){
    unsigned short wdat;                //温度センサバイナリ値
    double temp;                        //温度 実数値℃
    const float tempDelta = 0.0625F;    //温度の刻値　0.0625℃
    wdat = wiringPiI2CReadReg16(fd, ADT7410_REG);   //センサよりデータを取得
    wdat = (wdat & 0xff00) >> 8 | (wdat & 0xff) << 8; //上位と下位のバイトを交換する
    wdat = wdat >> 3;                   //13bitフォーマット形式
    if ((wdat & 0x1000) == 0){          //温度の正負を判断
        temp = wdat * tempDelta;        //プラス温度の計算
    }else{
        temp = ((~wdat & 0x1fff) + 1)*(-tempDelta); // マイナス温度の計算、2の補数計算
    }
    return temp;
}
