// List13-1_7Seg01.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <wiringPi.h>       //wiringPi

#define DP 19               //小数点
//７セグメントLED表示器 a, b, d, c, e, f, g
const int a_g[7] = {17,27,20,21,12,13,16};//セグメントとGPIOの定義
/*              a
             f     b
               g
            e     c
              d     dp */
const int disp0_9[11][7] = {{1,1,1,1,1,1,0},  //0　表示パターンの定義
                            {0,1,1,0,0,0,0},  //1
                            {1,1,0,1,1,0,1},  //2
                            {1,1,1,1,0,0,1},  //3
                            {0,1,1,0,0,1,1},  //4
                            {1,0,1,1,0,1,1},  //5
                            {1,0,1,1,1,1,1},  //6
                            {1,1,1,0,0,0,0},  //7
                            {1,1,1,1,1,1,1},  //8
                            {1,1,1,1,0,1,1},  //9
                            {0,0,0,0,0,0,0}   //ブランク
                            };

int main (void){
    int i, k;
    wiringPiSetupGpio();            //BCMのGPIO番号を使用
    for(i = 0; i < 7; i++){         //セグメントaからgのGPIOを出力に設定
        pinMode(a_g[i], OUTPUT);
    }      
    pinMode(DP,OUTPUT);             //dp(小数点）のGPIOを出力に設定

    while(1){
        for(i = 0; i < 11; i++){    //表示パターンをセット
            printf("%d\n",i);       //表示パターンの行番号をターミナルに表示
            for(k = 0; k < 7; k++){ //表示パターンのセグメントaからgを出力
                digitalWrite(a_g[k],disp0_9[i][k]);
            }
            delay(1000);            //１秒待つ
        }
     }
    return EXIT_SUCCESS;
}

