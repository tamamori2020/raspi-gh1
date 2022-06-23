// List15-9_Mutex.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_SUCCESS,etc
#include <wiringPi.h>       //digitalWrite,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //usleep
#define LOOP        10      //LEDの点滅回数
#define NUM_THREADS 4       //スレッドの生成数

/* グローバル宣言 */
pthread_mutex_t     mutex;              //mutexの宣言
const int ledGpio[4] = {23,22,25,24};   //LED0からLED3
const int swGpio[4] = {4,5,6,26};       //SW0からSW3

/* プロトタイプの宣言 */
void *Mutex_SW_LED(void *arg);

int main (void){
    int i;
    pthread_t   thread[NUM_THREADS];
    wiringPiSetupGpio();                        //BCMのGPIO番号を使用
    for(i = 0; i < 4; i++){
        pinMode(ledGpio[i], OUTPUT);            //LED0-LED3を出力
        pinMode(swGpio[i], INPUT);              //SW0-SW3を入力
    }
    
    if(pthread_mutex_init(&mutex, NULL) != 0){  //mutex初期化
        fprintf(stderr,"Error pthread_mutex_init.\n");
        return EXIT_FAILURE;
    }
    
    for(i = 0; i < NUM_THREADS; i++){           //スレッドの生成と起動
        if(pthread_create(&thread[i], NULL, Mutex_SW_LED, (void *)i) != 0){
            fprintf(stderr,"Error pthread_create.\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("Push SW0,SW1,SW2,SW3.\n\n");
    
    for(i = 0; i < NUM_THREADS; i++){           //スレッドの終了待ち
        if(pthread_join(thread[i], NULL) != 0){
            fprintf(stderr,"Error pthread_join.\n");
            exit(EXIT_FAILURE);
        }
    }

    if(pthread_mutex_destroy(&mutex) !=0){      //mutexの削除
        fprintf(stderr,"Error pthread_mutex_destroy.\n");
        return EXIT_FAILURE;
    }

    printf("End of main.\n");
    return EXIT_SUCCESS;                        //正常終了
}

//関数名  void *Mutex_SW_LED(void *arg)
//引数    SWとLEDに共通したビット番号(index)
//戻り値  なし
//グローバル変数 mutex,swGpio[4],ledGpio[4]
//概要    mutexを使用して、SWを押したらLEDを点滅する。
void *Mutex_SW_LED(void *arg){
    int i,index;
    index = (int)arg;
    while(digitalRead(swGpio[index]) != HIGH){
        usleep(1000);                       //CPU使用率の抑制のため
    }
    
    if(pthread_mutex_lock(&mutex) != 0){    //mutexロック
        fprintf(stderr,"Error pthread_mutex_lock.\n");
        exit(EXIT_FAILURE);
    }
    
    for(i = 0; i < LOOP; i++){
        digitalWrite(ledGpio[index], HIGH); //点灯
        delay(500);                         //500ms待つ
        digitalWrite(ledGpio[index], LOW);  //消灯
        delay(500);                         //500ms待つ
        printf("LED%d: #%d\n", index, i);
    }
    
    if(pthread_mutex_unlock(&mutex) != 0){  //mutexアンロック
        fprintf(stderr,"Error pthread_mutex_unlock.\n");
        exit(EXIT_FAILURE);
    }
    
    printf("End of MutexLed%d.\n",index);
    pthread_exit(NULL);
}
