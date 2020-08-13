// List13-2_7Seg02.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <wiringPi.h>       //wiringPi

#define DP          19      //小数点
#define INTERVAL     1      //各桁の点灯時間ms

//com[0]1桁目,com[1]2桁目,com[2]3桁目,com[3]4桁目
const int com[4] = {9,10,11,18};

//７セグメントLED表示器 a, b, d, d, e, f, g
const int a2g[7] = {17,27,20,21,12,13,16};//GPIOを配列で定義 
/*              a
             f     b
               g
            e     d
              d     dp  */
const int disp0_9[11][7] = {{1,1,1,1,1,1,0},  //0
                            {0,1,1,0,0,0,0},  //1
                            {1,1,0,1,1,0,1},  //2
                            {1,1,1,1,0,0,1},  //3
                            {0,1,1,0,0,1,1},  //4
                            {1,0,1,1,0,1,1},  //5
                            {1,0,1,1,1,1,1},  //6
                            {1,1,1,0,0,0,0},  //7
                            {1,1,1,1,1,1,1},  //8
                            {1,1,1,1,0,1,1},  //9
                            {0,0,0,0,0,0,0}   //消灯
                            };           

int main (void){
    int i, k;
    int segData;                    //0000から9999のカウント用
    int num1, num2, num3, num4;     //各桁の表示用の値
    wiringPiSetupGpio();            //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){         //4個のトランジスタを出力に設定
        pinMode(com[i], OUTPUT);
    }
    for(i = 0; i < 4; i++){         //4個のトランジスタをOFF
        digitalWrite(com[i],LOW);
    }
    for(i = 0; i < 7; i++){         //セグメントaからgのGPIOを出力に設定
        pinMode(a2g[i],OUTPUT);
    }      
    pinMode(DP, OUTPUT);            //dp(小数点）のGPIOを出力に設定
            
    while(1){
        for(segData=0; segData <= 9999; segData++){
            //7Seg 1桁目
            digitalWrite(com[3], LOW);  //4桁目OFF
            digitalWrite(com[0], HIGH); //1桁目ON
            num1 = segData % 10;        //10で割った余りから1桁目を取得   
            for(i = 0; i < 7; i++){     //セグメントaからgを出力
                digitalWrite(a2g[i], disp0_9[num1][i]);
            }                        
            delay(INTERVAL);            //点灯時間
            
            //7Seg 2桁目
            digitalWrite(com[0], LOW);  //1桁目OFF
            digitalWrite(com[1], HIGH); //2桁目ON
            k = segData / 10;           //2桁目を1桁目へ
            num2 = k % 10;              //10で割った余りから2桁目を取得            
            for(i = 0; i < 7; i++){     //セグメントaからgを出力
                digitalWrite(a2g[i], disp0_9[num2][i]);
            }                        
            delay(INTERVAL);            //点灯時間

            //7Seg 3桁目
            digitalWrite(com[1], LOW);  //2桁目OFF
            digitalWrite(com[2], HIGH); //3桁目ON
            k = segData / 100;          //3桁目を1桁目へ
            num3 = k % 10;              //10で割った余りから3桁目を取得            
            for(i = 0; i < 7; i++){     //セグメントaからgを出力
                digitalWrite(a2g[i], disp0_9[num3][i]);
            }                        
            delay(INTERVAL);            //点灯時間
            
            //7Seg 4桁目
            digitalWrite(com[2], LOW);  //3桁目OFF
            digitalWrite(com[3], HIGH); //4桁目ON
            k = segData / 1000;         //4桁目を1桁目へ
            num4 = k % 10;              //10で割った余りから4桁目を取得
            for(i = 0; i < 7; i++){     //セグメントaからgを出力
                digitalWrite(a2g[i], disp0_9[num4][i]);
            }                        
            delay(INTERVAL);            //点灯時間
            
            //ターミナルにデバック表示
            printf("segData = %d %d %d %d %d\n",segData,num4,num3,num2,num1);
        }
    }
    return EXIT_SUCCESS;
}
