#ifndef SERIALREAD_SERIALREAD_H
#define SERIALREAD_SERIALREAD_H

extern TCHAR	tzCOMport[20];

DWORD MsgError(DWORD err, const char *szInfo=nullptr);
void  GetTimerParam();
int initSerial();
DWORD GetSysTicks();

#endif //SERIALREAD_SERIALREAD_H
