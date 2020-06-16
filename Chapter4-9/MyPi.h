// MyPi.h

//A/DコンバータMCP3802用関数
extern unsigned short  Mcp3208RW(int ssNo, unsigned char adcChannel);

//D/AコンバータMCP4922用関数
extern int Mcp4922Write(int ssNo, unsigned char dacCh, unsigned short dacCode); 

//温度センサADT7410用関数
extern double Adt7410Read(int fd);

//LCD AQM1602用関数
extern int LcdSetup(int fd);
extern int LcdWriteChar(int fd, char c);
extern int LcdNewline(int fd);
extern int LcdClear(int fd);
extern int LcdWriteString(int fd, char *s);

