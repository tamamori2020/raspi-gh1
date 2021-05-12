// List9_Ex01A.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと
// 修正：CPU使用率の抑制のためusleep(1000)を挿入

#include <stdio.h>          //printf
#include <stdlib.h>         //EXIT_SUCCESS
#include <unistd.h>         //usleep
#include <wiringPi.h>       //wiringPiSetupGpio,etc

const int swGpio[8] = {4,5,6,26,17,27,20,21};   //SW GPIOを配列で定義
/* プロトタイプ宣言 */
int SwChk(void);

int main (void){
    int i;
    wiringPiSetupGpio();        //BCMのGPIO番号を使用
    for(i=0;i<8;i++){           //SW0-SW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i=0;i<8;i++){           //SW0-SW7をプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}
      
    while(1){
        i = SwChk();            //SWのチェック
        switch(i){
        case 1:
            printf("none\n");
            system("raspistill -f -ifx none");      //設定無し
            break;
        case 2:
            printf("negative\n");
            system("raspistill -f -ifx negative");  //反転
            break;
        case 3:
            printf("solarise\n");
            system("raspistill -f -ifx solarise");  //ソラライズ
            break;
        case 4:
            printf("posterise\n");
            system("raspistill -f -ifx posterise"); //ポスタライズ
            break;
        case 5:
            printf("sketch\n");
            system("raspistill -f -ifx sketch");    //スケッチ
            break;
        case 6:
            printf("denoise\n");
            system("raspistill -f -ifx denoise");   //ノイズ除去
            break;
        case 7:
            printf("emboss\n");
            system("raspistill -f -ifx emboss");    //エンボス
            break;
        case 8:
            printf("oilpaint\n");
            system("raspistill -f -ifx oilpaint");  //油絵
            break;
        }
    usleep(1000);       //CPU使用率の抑制のため
    }
   return EXIT_SUCCESS;	
}

/***************************
 関数名    int SwChk(void)
 引数     なし
 戻り値
 0 何も押されていない
 1 SW0が押された
 2 SW1が押された
 3 SW2が押された
 4 SW3が押された
 5 SW4が押された
 6 SW5が押された
 7 SW6が押された
 8 SW7が押された
 SW0からSW7の状態を取得する関数。
 同時に2つ以上のスイッチが押された場合、
 LSB側のSWを優先する。
 ***************************/
int SwChk(void){
    int i,swData=0;
    for(i=0;i<8;i++){    //SW0からSW7まで検査
        if (digitalRead(swGpio[i]) == HIGH){
            swData=i+1;
            break;
        }else{
            swData =0;
        }
    }
    return swData;
}
