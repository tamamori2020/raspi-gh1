// List13_Ex03.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ
#include <math.h>           //数学
#include <wiringPi.h>       //wiringPi
#include <time.h>           //日付及び時間

#define DP          19      //小数点
#define INTERVAL     1      //各桁の点灯時間ms

//com[0]1桁目,com[1]2桁目,com[2]3桁目,com[3]4桁目
const int com[4] = {9,10,11,18};

//７セグメントLED表示器 a, b, c, d, e, f, g
const int a2g[7] = {17,27,20,21,12,13,16};   //GPIOを配列で定義 
/*              a
             f     b
               g
            e     c
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
                            {0,0,0,0,0,0,0}   //ブランク
                            };
void Com4bit(int comData);

int main(void)
{
    int i, k;
    int keta,n;                     //keta 表示する桁、n　ループ処理用
    int segData;                    //時間表示用
    int num[4];                     //各桁の表示用の値
    
    time_t now;             //time_tは時刻を保存するデータ型
    struct tm *local;       //tmは時刻の構造体で時(tm_hour)、分(tm_min)、秒(tm_sec)等がint型メンバ
    int hour, minute, second, secondOld = 0; //secondOld printfデバッグで使用
    wiringPiSetupGpio();            //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){         //4個のトランジスタを出力に設定
        pinMode(com[i], OUTPUT);
    }
    for(i = 0; i < 4; i++){         //４桁のトランジスタをOFF
        digitalWrite(com[i], LOW);
    }
    for(i = 0; i < 7; i++){         //7セグメントLEDのAからGのGPIOを出力に設定
        pinMode(a2g[i], OUTPUT);
    }      
    pinMode(DP, OUTPUT);            //dp(小数点）のGPIOを出力に設定
            
    while(1){    
        now = time(NULL);           //現在の時刻を取得
        local = localtime(&now);    //ローカル時間(日本時間)に変換する
        hour = local -> tm_hour;    //時(tm_hour)の取得
        minute = local -> tm_min;   //分(tm_min)の取得
        second = local -> tm_sec;   //秒(tm_sec)の取得

        segData = hour*100 + minute; //時と分を組み合わせて４桁にする。

        keta = 1;
        for(n = 0; n < 4; n++){     //4桁の表示
            Com4bit(keta);          //表示する桁をGPIOへ出力する
            keta = keta << 1;       //桁の1bit左シフト
            k = segData / pow(10, n);
            num[n] = k % 10;
            for(i = 0; i < 7; i++){ //セグメントaからgを出力
                digitalWrite(a2g[i], disp0_9[num[n]][i]);
            }                        
            delay(INTERVAL);        //点灯時間 
        }

        //ターミナルにデバック表示
        if(abs(second - secondOld) >= 1){
            printf("%d時",hour);
            printf("%d分",minute);
            printf("%d秒\n",second);
            secondOld = second;
        }
    }
    return EXIT_SUCCESS;
}

void Com4bit(int comData){
    int i;
    for (i = 0; i < 4; i++){            //１桁目から順番に出力
        digitalWrite(com[i], comData & 1);
        comData = comData >> 1;         //1bit 右へシフト
    }
}
