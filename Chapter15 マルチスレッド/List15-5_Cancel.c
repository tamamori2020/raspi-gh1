// List15-5_Cancel.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
#include <stdio.h>          //printf,fgets,etc
#include <stdlib.h>         //EXIT_FAILURE,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //sleep
#define LOOP        10      //hello, worldの表示数
/* プロトタイプ宣言 */
void *Hello3(void *arg);

int main(void){
    unsigned long cnt = LOOP;
    char buffer[256];
    pthread_t thread0;

    printf("main: pthread_create thread0.\n");
    if(pthread_create(&thread0, NULL, Hello3, (void *)cnt) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    if (fgets(buffer, 256, stdin) == NULL) {
        fprintf(stderr,"Error fgets.\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_cancel(thread0) == 0){
        printf("main: pthread_cancel thread0.\n");  //成功の処理
    }else{
        fprintf(stderr,"Error pthread_cancel.\n");  //エラーの処理
    }

    return EXIT_SUCCESS;
}

//関数名  void *Hello3(void *arg)
//引数    arg　繰り返す値
//戻り値  なし
//概要    引数の値だけ、１秒毎にhello, world.を繰り返し表示する。
void *Hello3(void *arg){
    int i;
    unsigned long cnt;
    cnt = (unsigned long)arg;   //整数型に変換
    for(i = 0; i < cnt; i++){
        printf("thread0: #%d hello, world.\n",i);
        sleep(1);
    }
    pthread_exit(NULL);
}

