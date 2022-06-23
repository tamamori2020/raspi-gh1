// List15-6_LED_A.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
// 点滅時間を3分に変更
#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_FAILURE,etc
#include <pthread.h>        //pthread_create,etc
#include <wiringPi.h>       //wiringPiSetupGpio,etc

#define LED0        23      //LEDのGPIO番号の定義
#define LED1        22
#define BLINK_TIME  180000   //点滅時間（期間）ms

/* プロトタイプ宣言 */
void *Thread1Hz(void *arg); //1Hz信号
void *Thread2Hz(void *arg); //2Hz信号

int main(void){
    int t;
    pthread_t thread0,thread1;  //スレッドの宣言
    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    pinMode(LED0, OUTPUT);      //出力に設定
    pinMode(LED1, OUTPUT);
    
    t = BLINK_TIME;
    if(pthread_create(&thread0, NULL, Thread1Hz, (void *)t) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&thread1, NULL, Thread2Hz, (void *)t) != 0){ 
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    pthread_exit(NULL);
}

//関数名  void *Thread1Hz(void *arg)
//引数    arg　時間(ms)
//戻り値  なし
//概要    引数の時間(ms)だけ、LED0を１Hzで点滅する。
void *Thread1Hz(void *arg){
    int i,t;
    t = (int) arg;
    for(i = 0; i < t/1000; i++){    //点滅時間を1Hzの周期(1000ms)で割る
        printf("LED0: #%d\n",i);
        digitalWrite(LED0,HIGH);    //点灯
        delay(500);                 //500ms待つ
        digitalWrite(LED0,LOW);     //消灯
        delay(500);                 //500ms待つ
    }
    printf("End of Thread1Hz.\n");
    pthread_exit(NULL);
}

//関数名  void *Thread2Hz(void *arg)
//引数    arg　時間(ms)
//戻り値  なし
//概要    引数の時間(ms)だけ、LED1を2Hzで点滅させる
void *Thread2Hz(void *arg){
    int i,t;
    t = (int) arg;
    for(i = 0; i < t/500; i++){     //点滅時間を2Hzの周期(500ms)で割る
        printf("LED1: #%d\n",i);
        digitalWrite(LED1,HIGH);    //点灯
        delay(250);                 //250ms待つ
        digitalWrite(LED1,LOW);     //消灯
        delay(250);                 //250ms待つ
    }
    printf("End of Thread2Hz.\n");
    pthread_exit(NULL);
}
