//List6_Ex04.c 
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>          //入出力
#include <stdlib.h>         //一般ユーティリティ		
#include <wiringPi.h>       //wiringPi

#define BUZZER    18        //GPIO18をBUZZERと定義

//Pi3B/3B+とPi4Bのどちらかを選択。
//Pi4B(54MHz)のドレミの分周値  
#define DO1		2064    //低いド	SW7
#define RE		1839    //レ		SW6
#define MI		1638    //ミ		SW5
#define FA		1546    //ファ	SW4
#define SO		1378    //ソ		SW3
#define RA		1227    //ラ		SW2
#define SI		1093    //シ		SW1
#define DO2		1032    //高いド	SW0

//Pi3B/3B+(19.2MHz)のドレミの分周値 
//#define DO1		734     //低いド	SW7
//#define RE		654     //レ		SW6
//#define MI		582     //ミ		SW5
//#define FA		550     //ファ	SW4
//#define SO		490     //ソ		SW3
//#define RA		436     //ラ		SW2
//#define SI		389     //シ		SW1
//#define DO2		367     //高いド	SW0

const int swGpio[8] = {4,5,6,26,17,27,20,21};   //SW GPIOを配列で定義
/* プロトタイプの定義 */
int SwChk(void);

int main (void){
    int i;
    int divisor;                //分周値
    int divisorOld;             //一つ前のdivisorの値
    unsigned int range = 100;   //PWMのレンジ値100分割
    int value;                  //pwmWrite関数の引数
	wiringPiSetupGpio();	    //BCMのGPIO番号を使用
    for(i=0;i<8;i++){			//SW0-SW7を入力に設定
        pinMode(swGpio[i], INPUT);}
    for(i=0;i<8;i++){			//SW0-SW7をプルダウン抵抗をつける
        pullUpDnControl(swGpio[i],PUD_DOWN);}	
	pinMode(BUZZER, PWM_OUTPUT);	//BUZZERをPWM出力に設定
	pwmSetClock(DO1); 				//内部クロックの分周の設定。DO1を設定。
	pwmSetRange(range);			    //階調の設定	
	pwmSetMode(PWM_MODE_MS);		//信号波形モードの設定
	
	while(1){
		i = SwChk();				//SWのチェック
		value = range/2;			//デューティ比50%
			switch(i){
            case 8:
				divisor = DO1;		//ド
				break;
			case 7:
				divisor = RE;		//レ
				break;
			case 6:
				divisor = MI;		//ミ
				break;
			case 5:
				divisor = FA;		//ファ
				break;
			case 4:
				divisor = SO;		//ソ
				break;
			case 3:
				divisor = RA;		//ラ
				break;
			case 2:
				divisor = SI;		//シ
				break;
			case 1:
				divisor = DO2;		//ド
				break;
			default:
				value = 0;	//PWM信号出力の停止、音を鳴らさない
			}
			if(divisor != divisorOld){	//同じ値の重ね書きの防止
				pwmSetClock(divisor);                
				divisorOld = divisor;
			}
            pwmWrite(BUZZER, value);
	}
	return EXIT_SUCCESS;
}

/***************************
 関数名	int SwChk(void)
 引数	なし
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
	    for(i=0;i<8;i++){	//SW0からSW7まで検査
		    if (digitalRead(swGpio[i]) == HIGH){
			    swData=i+1;
			    break;
		    }else{
			    swData =0;
		    }
	    }
    return swData;
}
