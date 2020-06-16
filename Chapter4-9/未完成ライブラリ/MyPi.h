// MyPi.h

//LCD AQM1602用関数
extern int LcdSetup(int fd);
extern int LcdWriteChar(int fd, char c);
extern int LcdNewline(int fd);
extern int LcdClear(int fd);
extern int LcdWriteString(int fd, char *s);

