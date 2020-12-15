// List9_Ex02.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>      //入出力
#include <stdlib.h>     //一般ユーティリティ
#include <wiringPi.h>   //wiringPi
#define  PIR    16      //GPIO4をPIRと定義

void IntPIR(void);      //PIRの割込み関数
int main (void){
    wiringPiSetupGpio();    //BCMのGPIO番号を使用
    pinMode(PIR, INPUT);    //PIRを入力に設定
    wiringPiISR(PIR, INT_EDGE_RISING, (void*)IntPIR);
      
    while(1){
        printf("hello, world\n");
        delay(5000);
    }
   return EXIT_SUCCESS;	
}

void IntPIR(void){
    system("raspistill -t 100 -a 12 -a 1024 -o `date '+%Y%m%d-%H%M%S'`.jpg");
}
