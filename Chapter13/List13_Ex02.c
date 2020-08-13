// List13_Ex02.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <math.h>           //数学
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
const int disp0_F[17][7] = {{1,1,1,1,1,1,0},  //0
                            {0,1,1,0,0,0,0},  //1
                            {1,1,0,1,1,0,1},  //2
                            {1,1,1,1,0,0,1},  //3
                            {0,1,1,0,0,1,1},  //4
                            {1,0,1,1,0,1,1},  //5
                            {1,0,1,1,1,1,1},  //6
                            {1,1,1,0,0,0,0},  //7
                            {1,1,1,1,1,1,1},  //8
                            {1,1,1,1,0,1,1},  //9
                            {1,1,1,0,1,1,1},  //A
                            {0,0,1,1,1,1,1},  //b
                            {1,0,0,1,1,1,0},  //C
                            {0,1,1,1,1,0,1},  //d
                            {1,0,0,1,1,1,1},  //E
                            {1,0,0,0,1,1,1},  //F
                            {0,0,0,0,0,0,0},  //消灯                                                                                                                                
                            };
void Com4bit(int comData);

int main (void){
    int i,k;
    int keta,n;                     //keta 表示する桁、n　ループ処理用
    int segData;                    //0000からFFFFのカウント用
    int num[4];                     //各桁の表示用の値
    wiringPiSetupGpio();            //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){         //4個のトランジスタを出力に設定
        pinMode(com[i], OUTPUT);
    }
    for(i = 0; i < 4; i++){         //4個のトランジスタをOFF
        digitalWrite(com[i], LOW);
    }
    for(i = 0; i < 7; i++){         //セグメントaからgのGPIOを出力に設定
        pinMode(a2g[i], OUTPUT);
    }      
    pinMode(DP,OUTPUT);             //dp(小数点）のGPIOを出力に設定
            
    while(1){
        for(segData = 0; segData <= 0xffff; segData++){
            keta = 1;
            for(n = 0; n < 4; n++){   //4桁の表示
                Com4bit(keta);        //表示する桁をGPIOへ出力する
                keta = keta << 1;     //桁の1bit左シフト
                k = segData / pow(16, n);   //16進数で計算
                num[n] = k % 16;            //16進数で計算
                for(i = 0; i < 7; i++){    //セグメントaからgを出力
                    digitalWrite(a2g[i], disp0_F[num[n]][i]);
                }                        
                delay(INTERVAL);    //点灯時間 
            }
            //ターミナルにデバック表示
            printf("segData = %d %X %X %X %X\n",segData,num[3],num[2],num[1],num[0]);
        }
    }
    return EXIT_SUCCESS;
}

void Com4bit(int comData){
    int i;
//    printf("comData = %d\n",comData);     //デバッグ用
    for (i = 0; i < 4; i++){                //１桁目から順番に出力
        digitalWrite(com[i], comData & 1);
        comData = comData >> 1;         //1bit 右へシフト
    }
}
