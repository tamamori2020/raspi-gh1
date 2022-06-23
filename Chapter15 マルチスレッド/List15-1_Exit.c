// List15-1_Exit.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_FAILURE,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //sleep

/* プロトタイプ宣言 */
void *Hello0();
void *Hello1();
void *Hello2();
void *Hello3();
void *Hello4();

int main(void){
    pthread_t thread0,thread1,thread2,thread3,thread4;
    
    pthread_create(&thread0,NULL,Hello0,NULL);
    printf("main: pthread_create thread0.\n");
    pthread_create(&thread1,NULL,Hello1,NULL);
    printf("main: pthread_create thread1.\n");
    pthread_create(&thread2,NULL,Hello2,NULL);
    printf("main: pthread_create thread2.\n");
    pthread_create(&thread3,NULL,Hello3,NULL);
    printf("main: pthread_create thread3.\n");
    pthread_create(&thread4,NULL,Hello4,NULL);
    printf("main: pthread_create thread4.\n");
    
    pthread_exit(NULL);
}

void *Hello0(){
    sleep(0);                        //0秒の時間待ち
    printf("thread0: hello, world.\n");
    pthread_exit(NULL);              //スレッドの終了
}

void *Hello1(){
    sleep(1);                        //1秒の時間待ち
    printf("thread1: hello, world.\n");
    pthread_exit(NULL);              //スレッドの終了
}

void *Hello2(){
    sleep(2);                        //2秒の時間待ち
    printf("thread2: hello, world.\n");
    pthread_exit(NULL);              //スレッドの終了
}

void *Hello3(){
    sleep(3);                        //3秒の時間待ち
    printf("thread3: hello, world.\n");
    pthread_exit(NULL);              //スレッドの終了
}

void *Hello4(){
    sleep(4);                        //4秒の時間待ち
    printf("thread4: hello, world.\n");
    pthread_exit(NULL);              //スレッドの終了
}


