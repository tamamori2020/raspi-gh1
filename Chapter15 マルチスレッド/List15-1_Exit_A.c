// List15-1_Exit_A.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_FAILURE,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //sleep
#define NUM_THREADS     5   //スレッドの生成数
/* プロトタイプ宣言 */
void *Hello(void *threadID);

int main(void){
    unsigned long i;
    pthread_t thread[NUM_THREADS];
    
    for(i = 0; i < NUM_THREADS; i++){
        if(pthread_create(&thread[i], NULL, Hello, (void *)i) != 0){
            fprintf(stderr,"Error pthread_create.\n");          //エラー処理
            exit(EXIT_FAILURE);
        }else{
            printf("main: pthread_create thread[%lu].\n", i);   //成功処理
        }
    }
    pthread_exit(NULL);
}

//関数名  void *Hello(void *threadID)
//引数    threadID　スレッドID
//戻り値  なし
//概要    引数の秒数後に、hello, worldを表示する。
void *Hello(void *threadID){
    unsigned long tid;               //thread id
    tid = (unsigned long)threadID;   //整数型に変換
    sleep(tid);                      //1秒単位の時間待ち
    printf("thread[%lu]: hello, world.\n", tid);
    pthread_exit(NULL);              //スレッドの終了
}

