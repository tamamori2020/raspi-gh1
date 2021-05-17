// List15-7_Alt01.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_FAILURE,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //usleep
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#define DELAYTIME   20      //バウンシングの収束時間ms
/* グローバル宣言　*/
static volatile int g_alt[8];   //SW0からSW7のオルタネート動作用変数
const int swGpio[8] = {4,5,6,26,17,27,20,21};   //SW0からSW7
const int ledGpio[4] = {23,22,25,24};           //LED0からLED3

/* プロトタイプ宣言 */
void *ThreadAlt(void *t);     //SW0-SW7のオルタネート動作の関数

int main (void){
    int i;
    pthread_t   thread0;        //スレッドの宣言
    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){     //LED0からLED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}
    for(i = 0; i < 8; i++){     //SW0-SW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i= 0; i < 8; i++){      //SW0-SW7をプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}
    for(i = 0; i < 8; i++){g_alt[i] = LOW;}  //g_altの初期化
    
    if(pthread_create(&thread0, NULL, ThreadAlt, (void *)DELAYTIME) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    while(g_alt[7] != HIGH){
        for(i = 0; i < 4; i++){     //LED0-LED4 に出力する
            digitalWrite(ledGpio[i], g_alt[i]);
        }
        usleep(1000);               //CPU使用率の抑制のため
    }

    if(pthread_join(thread0, NULL) != 0){ //スレッドの終了待ち
        fprintf(stderr,"Error pthread_join.\n");
        exit(EXIT_FAILURE);
    }
    
    for(i = 0; i < 4; i++){         //LEDの消灯
        digitalWrite(ledGpio[i], LOW);
    }
    printf("End of main.\n");
    return EXIT_SUCCESS;
}

//関数名  *ThreadAlt(void *t)
//引数    t バウンシングの収束時間ms
//戻り値  なし
//グローバル変数 g_alt[8],swGpio[8]
//概要    SW0からSW6についてバウンシング対策したオルタネート動作する。
//       SW7がHIGHのとき、スレッドを終了する。
void *ThreadAlt(void *t){
    int i;
    int sNow[8];
    int sOld[8];
    for(i = 0; i < 8; i++){sOld[i] = LOW;}      //sOldの初期化
    while(g_alt[7] != HIGH){
        for(i = 0; i < 8; i++){
            sNow[i] = digitalRead(swGpio[i]);
            if((sOld[i] == LOW)&&(sNow[i] == HIGH)){    //SWの立ち上がりを検出
                g_alt[i] = !g_alt[i];
            }
            sOld[i] = sNow[i];
        }

        for(i = 7; i >= 0; i--){        //デバッグ表示用
            printf("%d ",g_alt[i]);
            if(i == 4){printf(" ");}    //4bit区切り
            if(i == 0){printf("\n");}   //改行
        }
        delay((int)t);       //バウンシング対策の時間待ち
    }
    printf("End of ThreadAlt.\n");
    pthread_exit(NULL);
}
