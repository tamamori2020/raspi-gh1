// List15_Ex03.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_FAILURE,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //usleep
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#define LED0        23      //LEDのGPIO番号の定義
#define DELAYTIME   20      //バウンシングの収束時間ms
/* グローバル宣言　*/
static volatile int g_alt[2];   //g_alt[0](SW0用),g_alt[1](SW7用)
static volatile unsigned int g_cnt0;  //SW0のカウント
const int swGpio[2] = {4,21};   //swGpio[0](SW0用),swGpio[1](SW7用)
/* プロトタイプ宣言 */
void *ThreadAlt07(void *t);     //SW0とSW7の入力

int main (void){
    int i;
    unsigned int mdl0;          //g_cnt0を３で除算した余り
    g_cnt0 = 0;                 //初期化
    pthread_t   thread0;        //スレッドの宣言
    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    pinMode(LED0, OUTPUT);      //出力に設定

    for(i = 0; i < 2; i++){         //SW0,SW7を入力に設定
        pinMode(swGpio[i], INPUT);
        pullUpDnControl(swGpio[i],PUD_DOWN); //プルダウン抵抗をつける
        g_alt[i] = LOW;             //g_altの初期化
    }
    if(pthread_create(&thread0, NULL, ThreadAlt07, (void *)DELAYTIME) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    while(g_alt[1] != HIGH){        //g_alt[1] = SW7
        mdl0 = g_cnt0 % 3;
        switch(mdl0){
            case 0: digitalWrite(LED0, LOW);    //消灯
                    break;
            case 1: digitalWrite(LED0, HIGH);   //点灯
                    break;
            case 2: digitalWrite(LED0, LOW);    //点滅
                    delay(250);
                    digitalWrite(LED0, HIGH);
                    delay(250);
                    break;
        }
        usleep(1000);                       //CPU使用率の抑制のため
    }

    if(pthread_join(thread0, NULL) != 0){   //スレッドの終了待ち
        fprintf(stderr,"Error pthread_join.\n");
        exit(EXIT_FAILURE);
    }
    
    digitalWrite(LED0, LOW);     //LEDの消灯
    printf("End of main.\n");
    return EXIT_SUCCESS;
}

//関数名  void *ThreadAlt07(void *t)
//引数    t バウンシングの収束時間ms
//戻り値  なし
//グローバル変数 g_alt[2],g_cnt0,swGpio[2]
//概要    SW0についてバウンシング対策したオルタネート動作する。
//       SW7がHIGHのとき、スレッドを終了する。
void *ThreadAlt07(void *t){
    int i;
    int sNow[2];
    int sOld[2];
    for(i = 0; i < 2; i++){sOld[i] = LOW;}      //sOldの初期化
    while(g_alt[1] != HIGH){        //SW7(g_alt[1])がHIGHのときループから抜けてスレッドの終了
        for(i = 0; i < 2; i++){
            sNow[i] = digitalRead(swGpio[i]);
            if((sOld[i] == LOW)&&(sNow[i] == HIGH)){    //SWの立ち上がりを検出
                g_alt[i] = !g_alt[i];
                g_cnt0++;           //カウント
            }
            sOld[i] = sNow[i];
        }

        for(i = 1; i >= 0; i--){        //デバッグ表示用
            printf("%d ",g_alt[i]);
            if(i == 0){printf("\n");}   //改行
        }
        delay((int)t);       //バウンシング対策の時間待ち
    }
    printf("End of ThreadAlt.\n");
    pthread_exit(NULL);
}
