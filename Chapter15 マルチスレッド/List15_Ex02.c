// List15_Ex02.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_FAILURE,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //usleep
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#define LED0            23  //GPIO23をLED0と定義
#define LED1            22  //GPIO22をLED1と定義
#define NUM_THREADS     3   //スレッドの生成数
/* グローバル宣言 */
static volatile int g_sw7;  //SW7用
const int swGpio[3] = {4,5,21}; //swGpio[0](SW0用),swGpio[1](SW1用),swGpio[2](SW7用)
/* プロトタイプ宣言 */
void *ThreadSW0(void *arg);     //SW0用
void *ThreadSW1(void *arg);     //SW1用
void *ThreadSW7(void *arg);     //SW7用

int main(void){
    int i;
    pthread_t   thread0,thread1,thread7; //スレッドの宣言
    g_sw7 = LOW;
    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    pinMode(LED0, OUTPUT);      //LED0を出力に設定
    pinMode(LED1, OUTPUT);      //LED1を出力に設定

    for(i = 0; i < 3; i++){     //SW0,SW1,SW7を入力に設定
        pinMode(swGpio[i], INPUT);
        pullUpDnControl(swGpio[i],PUD_DOWN); //プルダウン抵抗をつける
    }

    if(pthread_create(&thread0, NULL, ThreadSW0, NULL) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&thread1, NULL, ThreadSW1, NULL) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&thread7, NULL, ThreadSW7, NULL) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    while(g_sw7 != HIGH){
        digitalWrite(LED0, HIGH);   //LED0を0.5秒間隔で点滅
        delay(500);
        digitalWrite(LED0, LOW);
        delay(500);
    }
    printf("End of main\n");
    pthread_exit(NULL);
}

//関数名  void *ThreadSW0(void *arg)
//引数    NULL
//戻り値  NULL
//概要    SW0がHIGHのとき、LED1を点灯する。
void *ThreadSW0(void *arg){
    while(g_sw7 != HIGH){
        if(digitalRead(swGpio[0]) == HIGH){
            digitalWrite(LED1,HIGH);
        }
        usleep(1000);    //CPU使用率の抑制のため
    }
    printf("End of ThreadSW0\n");
    pthread_exit(NULL);
}

//関数名  void *ThreadSW1(void *arg)
//引数    NULL
//戻り値  NULL
//概要    SW1がHIGHのとき、LED1を消灯する。
void *ThreadSW1(void *arg){
    while(g_sw7 != HIGH){
        if(digitalRead(swGpio[1]) == HIGH){
            digitalWrite(LED1,LOW);
        }
        usleep(1000);       //CPU使用率の抑制のため
    }
    printf("End of ThreadSW1\n");
    pthread_exit(NULL);
}

//関数名  void *ThreadAlt07(void *arg)
//引数    NULL
//戻り値  NULL
//概要    SW7がHIGHのとき、プログラムを終了する。
void *ThreadSW7(void *arg){
    while(1){
        if(digitalRead(swGpio[2]) == HIGH){
            g_sw7 = HIGH;
            break;
        }
        usleep(1000);           //CPU使用率の抑制のため
    }
    digitalWrite(LED1, LOW);    //LED0の消灯
    printf("End of ThreadSW7\n");
    return NULL;
}
