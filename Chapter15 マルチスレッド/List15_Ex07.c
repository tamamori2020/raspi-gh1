// List15_Ex07.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_FAILURE,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //sleep,usleep
#include <wiringPi.h>       //wiringPiSetupGpio,etc
#define SW0         4       //GPIO4をSW0と定義
#define SW1         5       //GPIO21をSW7と定義
#define NUM_THREADS 2       //スレッドの生成数
#define LOOP        5       //表示数
/* グローバル宣言 */
static volatile int g_updn; //共有リソース
/* プロトタイプ宣言 */
void* ThreadInc(void* arg);
void* ThreadDec(void* arg);

int main(void){
    g_updn = 0;             //初期化
    pthread_t thread[NUM_THREADS];
    wiringPiSetupGpio();    //BCMのGPIO番号を使用
    pinMode(SW0, INPUT);    //SWを入力に設定
    pinMode(SW1, INPUT);
    pullUpDnControl(SW0,PUD_DOWN); //SWにプルダウン抵抗をつける
    pullUpDnControl(SW1,PUD_DOWN);
    
    if(pthread_create(&thread[0], NULL, ThreadInc, NULL) != 0){
        fprintf(stderr,"Error pthread_create.\n");  //エラー処理
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&thread[1], NULL, ThreadDec, NULL) != 0){
        fprintf(stderr,"Error pthread_create.\n");  //エラー処理
        exit(EXIT_FAILURE);
    }
    
    printf("Push SW0 and SW1.\n\n");
    
    for(int i = 0; i < NUM_THREADS; i++){
        if(pthread_join(thread[i], NULL) != 0){
            fprintf(stderr,"Error pthread_join.\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("End of main.\n");
    return EXIT_SUCCESS;        //正常終了
}

//関数名  void *ThreadInc(void *arg)
//引数    なし
//戻り値  なし
//グローバル変数 g_updn
//概要    1秒毎にg_updnをインクリメントする
void *ThreadInc(void *arg)
{
    int i;
    do{                     //SW0の入力待ち
        i = digitalRead(SW0);
        usleep(1000);       //CPU使用率の抑制のため
    }while(i != HIGH);
    printf("Start ThreadInc.\n");
    for (i = 0; i < LOOP; i++){
        g_updn++;
        printf("Increment: #%d\n", g_updn);
        sleep(1);
    }
    printf("End of ThreadInc.\n");
    pthread_exit(NULL);     //スレッドの終了
}

//関数名  void *ThreadDec(void *arg)
//引数    なし
//戻り値  なし
//グローバル変数 g_updn
//概要    g_updnをデクリメントする
void *ThreadDec(void *arg)
{
    int i;
    do{                     //SW1の入力待ち
        i = digitalRead(SW1);
        usleep(1000);       //CPU使用率の抑制のため
    }while(i != HIGH);
    printf("Start ThreadDec.\n");
    for (i = 0; i < LOOP; i++){
        g_updn--;
        printf("Decrement: #%d\n", g_updn);
    }
    printf("End of ThreadDec.\n");
    pthread_exit(NULL);     //スレッドの終了
}
