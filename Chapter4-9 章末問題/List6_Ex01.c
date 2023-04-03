// List6_Ex01.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
// 2021-05-12 CPU使用率の抑制のためusleep(1000)を挿入
// 2023-04-01 コメント修正 100Hzの周期は10ms

#include <stdio.h>          //printf
#include <stdlib.h>         //EXIT_SUCCESS
#include <unistd.h>         //usleep
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#include <softPwm.h>        //softPwmCreate,softPwmWrite

#define LED0    23                            //GPIO23をLED0と定義
const int swGpio[8] = {4,5,6,26,17,27,20,21}; //SW GPIOを配列で定義
/* プロトタイプの定義 */
int SwChk(void);

int main (void){
    int i,swData;
    int pwmRange = 100;        //100Hzの周期は10ms、pwmRange*100us=10ms
    int initialValue = 0;      //softPwmCreate関数実行時には出力信号0V
    int value;                 //softPwmWrite関数の引数
    wiringPiSetupGpio();       //BCMのGPIO番号を使用
    pinMode(LED0, OUTPUT);     //LED0を出力に設定
    for(i = 0; i < 8; i++){         //SW0-SW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i = 0; i < 8; i++){         //SW0-SW7をプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}
    softPwmCreate(LED0,initialValue,pwmRange); //ソフトPWMの設定

    while(1){
        swData = SwChk();
        value = (double)swData * 12.5 + 0.5;  //四捨五入
//      printf("%d \n",value);
        softPwmWrite(LED0,value);   //デューティ比の設定と出力
        usleep(1000);               //CPU使用率の抑制のため
    }
    return EXIT_SUCCESS;
}

/***************************
 関数名  int SwChk(void)
 引数    なし
 戻り値
 0 何も押されていない
 1 SW0が押された
 2 SW1が押された
 3 SW2が押された
 4 SW3が押された
 5 SW4が押された
 6 SW5が押された
 7 SW6が押された
 8 SW7が押された
 SW0からSW7の状態を取得する関数。
 同時に2つ以上のスイッチが押された場合、
 LSB側のSWを優先する。
 ***************************/
int SwChk(void){
    int i,swData=0;
    for(i = 0; i < 8; i++){     //SW0からSW7まで検査
        if (digitalRead(swGpio[i]) == HIGH){
            swData=i+1;
            break;
        }else{
            swData =0;
        }
    }
    return swData;
}

