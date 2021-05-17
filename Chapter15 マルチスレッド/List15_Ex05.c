// List15_Ex05.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 libMyPi.a -lm
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //printf,etc
#include <stdlib.h>         //EXIT_SUCCESS,etc
#include <string.h>         //strcmp,strcpy,etc
#include <termios.h>        //tcgetattr,etc
#include <pthread.h>        //pthread_create,etc
#include <unistd.h>         //usleep
#include <wiringPi.h>       //digitalWrite,etc
#include <wiringSerial.h>   //serialPrintf,etc
#define BPS     115200      //通信速度
#define BUFFER  256         //文字列の最大値
#define LED0    23          //LEDのGPIO番号の定義
#define STOP    0           //消灯
#define START   1           //点滅
#define END     2           //スレッドの終了

/* グローバル宣言 */
char g_cmd[BUFFER] = "";        //受信コマンド
static volatile int g_cmd_LED0; //LED0コマンド
static volatile int fdSri;      //シリアルポート用
/* プロトタイプ宣言 */
void *serialReceive1(void *arg); //受信のスレッド
void *Thread1Hz1(void *arg);    //LED0 1Hzの点滅

int main (void){
    char cmdOld[BUFFER] = "";   //前回の受信コマンド
    struct termios param;       //構造体変数
    pthread_t threadSerial, threadLED0;   //スレッドの宣言

    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    pinMode(LED0, OUTPUT);      //出力に設定

    fdSri = serialOpen( "/dev/serial0", BPS); //シリアルポートのオープン
    if( fdSri == -1){
        fprintf(stderr,"Error serialOpen.\n");
        exit(EXIT_FAILURE);
    }
    tcgetattr(fdSri, &param);           //パラメータの取得
        param.c_cflag &= ~CSIZE;        //データビットのクリア
        param.c_cflag |= CS8;           //データビットを8bitにセット
        param.c_cflag &= ~PARENB;       //パリティビットを無効にセット
        param.c_cflag &= ~CSTOPB;       //ストップビットを1bitにセット
    tcsetattr(fdSri,TCSANOW,&param);    //パラメータの有効化

    if(pthread_create(&threadSerial, NULL, serialReceive1, NULL) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&threadLED0, NULL, Thread1Hz1, NULL) != 0){
        fprintf(stderr,"Error pthread_create.\n");
        exit(EXIT_FAILURE);
    }

    serialFlush(fdSri);                //バッファをクリア
    serialPrintf(fdSri,"\r\nLED0 blinking test.\r\n");
    serialPrintf(fdSri,"Enter command (start, stop, end) >>>");
    printf("\nLED0 blinking test.\n");
    printf("Enter command (start, stop, end) >>>");fflush(stdout);
    while(1){
        if(strcmp(g_cmd,cmdOld) != 0){
            if(strcmp(g_cmd,"start") == 0){         //点滅: start
                g_cmd_LED0 = START;
                serialPrintf(fdSri,"\r\n%s is received.\r\n",g_cmd);
                printf("\n%s is received.\n",g_cmd);   
            }else if(strcmp(g_cmd,"stop") == 0){    //停止: stop
                g_cmd_LED0 = STOP;
                serialPrintf(fdSri,"\r\n%s is received.\r\n",g_cmd);
                printf("\n%s is received.\n",g_cmd);   
            }else if(strcmp(g_cmd,"end") == 0){     //終了: end
                g_cmd_LED0 = END;
                serialPrintf(fdSri,"\r\n%s is received.\r\n",g_cmd);
                printf("\n%s is received.\n",g_cmd);   
                break;
            }else{                                  //エラー処理
                serialPrintf(fdSri,"\r\nError: %s is unknown.\r\n",g_cmd);
                printf("\nError: %s is unknown.\n",g_cmd);
                int i = 0;
                while(g_cmd[i] != '\0'){
                    serialPrintf(fdSri,"%c(0x%02x) ",g_cmd[i],g_cmd[i]);
                    printf("%c(0x%02x) ",g_cmd[i],g_cmd[i]); fflush(stdout); //文字の表示
                    i++;
                }
                serialPrintf(fdSri,"\r\n");printf("\n");
            }
            strcpy(cmdOld,g_cmd);   //cmdOldの更新
            serialPrintf(fdSri,"\r\nEnter command (start, stop, end) >>>");
            printf("\nEnter command (start, stop, end) >>>");fflush(stdout);
        }
        usleep(1000);        //CPU使用率の抑制のため
    }

    if(pthread_join(threadSerial, NULL) != 0){  //threadSerialスレッドの終了待ち
        fprintf(stderr,"Error pthread_join.\n");
        exit(EXIT_FAILURE);
    }
    
    if(pthread_join(threadLED0, NULL) != 0){    //threadLED0スレッドの終了待ち
        fprintf(stderr,"Error pthread_join.\n");
        exit(EXIT_FAILURE);
    }

    serialPrintf(fdSri,"End of main.\r\n");
    serialClose(fdSri);
    printf("End of main.\n");
    return EXIT_SUCCESS;
}

//関数名  void *serialReceive1(void *arg)
//引数    なし
//戻り値  なし
//グローバル変数 g_cmd_LED0,g_cmd,fdSri
//概要    受信した改行コードを受けて文字列をg_cmdに格納する
void *serialReceive1(void *arg){
    int i, k=0;
    int c;              //受信データ数
    int rxdData;        //受信データ
    char str[BUFFER];   //受信データの文字列

    while(g_cmd_LED0 != END){
        c = serialDataAvail(fdSri);    //受信データ数の取得
        if(c > 0){
            for(i = 0; i < c; i++){
                rxdData = serialGetchar(fdSri);
                printf("%c",rxdData);fflush(stdout);    //ターミナルに1文字表示
                serialPutchar(fdSri,rxdData);       //PC側へエコーバック
                
                switch( rxdData ){
                    case '\b':  k--;                //Back Space(BS:\b)なら文字位置を1文字戻す
                                if(k < 0) k = 0;    //過剰にBSが入力された場合の対策
                                break;
                    case '\r':  str[k++] = '\0';    //Enter(CR:\r)なら、nullを加えて文字列にする
                                strcpy(g_cmd, str);
                                k=0;
                                break;
                    default:    str[k++] = rxdData; //文字列に受信データを代入
                }
            }
        }
        usleep(1000);           //CPU使用率の抑制のため
    }
    serialPrintf(fdSri,"End of serialReceive1.\r\n");
    printf("End of serialReceive1.\n");
    pthread_exit(NULL);
}

//関数名  void *Thread1Hz1(void *arg)
//引数    なし
//戻り値  なし
//グローバル変数 g_cmd_LED0
//概要    LED0を1Hzで点滅させる
//       g_cmd_LED0=(START:点滅, STOP:消灯,END:スレッド終了)
void *Thread1Hz1(void *arg){
    while(1){
        if(g_cmd_LED0 == START){        //点滅
            digitalWrite(LED0,HIGH);    //点灯
            delay(500);                 //500ms待つ
            digitalWrite(LED0,LOW);     //消灯
            delay(500);                 //500ms待つ
        }else if(g_cmd_LED0 == END){    //スレッド終了
            break;
        }else{                          //startとend以外は消灯
            usleep(1000);               //CPU使用率の抑制のため
        }
    }
    serialPrintf(fdSri,"End of Thread1Hz.\r\n");
    printf("End of Thread1Hz.\n");
    pthread_exit(NULL);
}
