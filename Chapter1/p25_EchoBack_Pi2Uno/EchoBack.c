// EchoBack.c
// gcc -Wall -o "%e" "%f" -lwiringPi -lpthread -g -O0 -lm libMyPi.a
// Geanyのオプションの"%e"は実行ファイル名, "%f"はソースファイル名のこと

#include <stdio.h>			//入出力
#include <stdlib.h>			//一般ユーティリティ
#include <wiringPi.h>		//wiringPi
#include <wiringSerial.h>	//シリアル通信用wiringPi
#include <termios.h>		//ttyパラメータの構造体

#define BPS 115200			//RS-232通信速度

int main (void){
	unsigned char txdData;
	int rxdData,error=0;
	int fd;
	struct termios options;		// 通信オプションの構造体の定義
	
	fd = serialOpen( "/dev/ttyS0", BPS);	//シリアルポートのオープン
	tcgetattr(fd, &options);		//通信オプションの取得
	options.c_cflag |= CS8;			//データ 8bit
	options.c_cflag &= ~PARENB;		//パリティなし
	options.c_cflag |= CSTOPB;		//ストップビット 2bit
	options.c_iflag &= ~IXON; 		//出力のXON/XOFFフロー制御なし
	options.c_iflag &= ~IXOFF;		//入力のXON/XOFFフロー制御なし
	tcsetattr(fd,TCSANOW,&options);	// 通信オプションの変更
	
	serialFlush(fd);				//受信バッファのクリア
	
	do{
		for(txdData='0';txdData<='z';txdData++){ 	//0からzまでのASCIIコードを送信する。
			serialPutchar (fd, txdData);			//1文字の送信
			rxdData = serialGetchar(fd);			//1文字の受信
			if(rxdData == txdData){
				printf("%c",rxdData); fflush(stdout);	// 文字の表示		
			}else{
				printf("\n expected %c:%d, received %c:%d \n",txdData,txdData,rxdData,rxdData);
				error++;
				break;
			}
		}
	}while(error == 0);
	serialClose(fd);
	return EXIT_SUCCESS;
}

