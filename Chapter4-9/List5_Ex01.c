// List5_Ex01.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ		
#include <wiringPi.h>       //wiringPi

const int ledGpio[4] ={23,22,25,24};    //LEDのGPIOを配列で定義
const int swGpio[4] = {4,5,6,26};   	//SW GPIOを配列で定義

int main (void){
    int i;
    wiringPiSetupGpio();			//BCMのGPIO番号を使用
    for(i=0;i<4;i++){				//LED0からLED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}
    for(i=0;i<4;i++){				//SW0からSW3を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i=0;i<4;i++){				//SW0からSW3にプルダウン抵抗をつける
	    pullUpDnControl(swGpio[i],PUD_DOWN);}

    while(1){
        for (i= 0;i<4;i++){
            if(digitalRead(swGpio[i])==HIGH){    //SW0の状態から順番に調べてループ処理する
			    digitalWrite(ledGpio[i], HIGH);  //HIGHを出力する
            }else{ 
                digitalWrite(ledGpio[i], LOW);   //LOWを出力する
		    }
        }
	/*for(i=0;i<4;i++){			//別解
	      digitalWrite(ledGpio[i],digitalRead(swGpio[i]));
	  } */
    }
    return EXIT_SUCCESS;
}
