#ifndef SERIALREAD_SERIALREAD_H
#define SERIALREAD_SERIALREAD_H

extern TCHAR	tzCOMport[20];
extern int      stop;

DWORD MsgError(DWORD err, const char *szInfo=nullptr);
void  GetTimerParam();
int   initSerial();
DWORD WINAPI readSerial();
DWORD GetSysTicks();

#endif //SERIALREAD_SERIALREAD_H
