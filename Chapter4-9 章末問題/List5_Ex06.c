// List5_Ex06.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>
#include <stdlib.h>         //EXIT_SUCCESS
#include <unistd.h>         //usleep
#include <wiringPi.h>       //wiringPiSetupGpio,pinMode,etc

/* LEDとSWのGPIOを配列で定義 */
const int ledGpio[4] = {23,22,25,24};           //グローバル定数
const int swGpio[8] = {4,5,6,26,17,27,20,21};

int main (void){
    int i,f;
    int s[3];
    wiringPiSetupGpio();                //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){             //LED0からLED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}
    for(i = 0; i < 8; i++){             //SW0からSW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i = 0; i < 8; i++){             //SW0からSW7にプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}
 
    while(1){
        for (i = 0; i < 3; i++){        //SW0とSW3の状態から順番に調べてループ処理する
            s[i] = digitalRead(swGpio[i]);
        }
        
        f = (s[0]&&s[1]) || (s[0]&&s[2]) || (s[1]&&s[2]); //その１　論理代数で処理
        digitalWrite(ledGpio[0], f);
        
        //f = s[0]+s[1]+s[2];  //その２　if文で条件判断処理
        //if(1 < f){
            //digitalWrite(ledGpio[0], HIGH); //点灯
        //}else{
            //digitalWrite(ledGpio[0], LOW);  //消灯
        //}
        
        usleep(1000);       //CPU使用率の抑制のため
    }
    return EXIT_SUCCESS;
}
