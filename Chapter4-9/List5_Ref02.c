// List5_Ref02.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ		
#include <wiringPi.h>       //wiringPi

const int ledGpio[4] ={23,22,25,24};            //LED GPIOを配列で定義
const int swGpio[8] = {4,5,6,26,17,27,20,21};   //SW GPIOを配列で定義

/* LEDのビットフィールド */
union {
    unsigned char BYTE;
    struct {
        unsigned char B0:1;	//LED0
	    unsigned char B1:1;	//LED1
	    unsigned char B2:1;	//LED2
	    unsigned char B3:1;	//LED3
	    unsigned char	:4;	//空の4bit
    }BIT;
}LED;

/* SWのビットフィールド */
union {
    unsigned char BYTE;
    struct{
        unsigned char B0:1;	//SW0
	    unsigned char B1:1;	//SW1
	    unsigned char B2:1;	//SW2
	    unsigned char B3:1;	//SW3
	    unsigned char B4:1;	//SW4
	    unsigned char B5:1;	//SW5
	    unsigned char B6:1;	//SW6
	    unsigned char B7:1;	//SW7
    }BIT;
}SW;

/* プロトタイプ宣言 */
void Led4bit(int ledData);
void Sw8bit(void);

int main (void){
    int i;   
    wiringPiSetupGpio();	//BCMのGPIO番号を使用
    for(i=0;i<4;i++){		//LED0-LED3を出力に設定
        pinMode(ledGpio[i], OUTPUT);}
    for(i=0;i<8;i++){		//SW0-SW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i=0;i<8;i++){		//SW0-SW7をプルダウン抵抗をつける
	    pullUpDnControl(swGpio[i],PUD_DOWN);}

    while(1){
        Sw8bit();           //SW入力
        LED.BIT.B0 = SW.BIT.B0 | SW.BIT.B4;
        LED.BIT.B1 = SW.BIT.B1 | SW.BIT.B5;
        LED.BIT.B2 = SW.BIT.B2 | SW.BIT.B6;
        LED.BIT.B3 = SW.BIT.B3 | SW.BIT.B7;
        Led4bit(LED.BYTE);  //LED出力
    }
    return EXIT_SUCCESS;
}

//関数名  void Led4bit(int ledData)
//引数    ledData  4bitデータ　0から15
//戻り値  なし
//概要    ledDataをLED0からLED３へ出力
void Led4bit(int ledData)
{
    int i;
    for (i=0; i<4; i++){               //LSBのLED0から順番に出力する
        digitalWrite(ledGpio[i], ledData&1);	//LSBだけの値にして出力
        ledData = ledData >> 1;        //1bit 右シフト
    }
}

//関数名  void Sw8bit(void)
//引数　　なし
//戻り値　なし
//概要　　SW0からSW７の値をSW共用体に入力
void Sw8bit(void){
    int i,k;
    SW.BYTE=0;
    for(i=7; i>-1; i--){
       k=digitalRead(swGpio[i]);
       SW.BYTE = SW.BYTE << 1;
       SW.BYTE = SW.BYTE + k;
    }
//    printf("%d\n",SW.BYTE);	//デバッグ用
}
