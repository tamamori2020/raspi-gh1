// List15-4_Join.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_FAILURE,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //sleep
#define NUM_THREADS     5   //スレッドの生成数
/* プロトタイプ宣言 */
void *Hello2(void *threadID);

int main(void){
    unsigned long i;
    void *retval;
    pthread_t thread[NUM_THREADS];

    for(i = 0; i < NUM_THREADS; i++){
        if(pthread_create(&thread[i], NULL, Hello2, (void *)i) != 0){
            fprintf(stderr,"Error pthread_create.\n");          //エラー処理
            exit(EXIT_FAILURE);
        }else{
            printf("main: pthread_create thread[%lu].\n", i);   //成功処理
        }
    }

    for(i = 0; i < NUM_THREADS; i++){
        if(pthread_join(thread[i], &retval) != 0){
            fprintf(stderr,"Error pthread_join.\n");
            exit(EXIT_FAILURE);
        }else{
            printf("retval[%lu] = %lu\n", i, (unsigned long)retval);
        }
    }
    return EXIT_SUCCESS;
}

//関数名  void *Hello2(void *threadID)
//引数    threadID　スレッドID
//戻り値  tid = threadID + 100
//概要    引数の秒数後にhello, worldを表示し、引数に100を加算した値を戻り値とする。
void *Hello2(void *threadID){
    unsigned long tid;              //thread id
    tid = (unsigned long)threadID;  //整数型に変換
    sleep(tid);                     //1秒単位の時間待ち
    printf("thread[%lu]: hello, world.\n", tid);
    tid += 100;
    pthread_exit((void *)tid);      //スレッドの終了
}

