#include <afxres.h>
#include <tchar.h>
#include <iostream>
#include <cstdio>
#include "serialread.h"


TCHAR   tzCOMport[20] =_T("\\\\.\\COM5");
DWORD   HPT_freq = 0;				// HP timer frequency in counts per second
int     stop=0;

void CheckSysTimer();
BOOL WINAPI consoleHandler(DWORD signal);

int main()
{
    std::cout << " -- Serial read v1.0\n\n";
    GetTimerParam();
    CheckSysTimer();

    if( !initSerial() ) return 1;

    if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
    {
        std::cout << "\nERROR: Could not set control handler\n";
        return 1;
    }

    readSerial();
/*    while (0)
    {
        if( stop==1 )  break;
        Sleep(100);
    } //*/

    std::cout << "\nExit";
    return 0;
}

BOOL WINAPI consoleHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT) {
        std::cout << "\n\nCtrl-C handled.\n";
        stop=1;
        return TRUE;
    }
    return FALSE;
}

DWORD MsgError(DWORD err, const char *szInfo)
{
    TCHAR *ErrMsg;

    if( !FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                       err, MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPTSTR)&ErrMsg, 0, NULL) )
        ErrMsg = _T("Can`t get error message");

    std::cout << "\n\n";
    if( szInfo ) {
        std::cout << " == " << szInfo << std::endl;
    }
    std::cout << " == Error code - "   << err    << std::endl;
    std::cout << " == Error message: " << ErrMsg << std::endl;

    LocalFree(ErrMsg);

    return err;
}

void CheckSysTimer()
{
    LARGE_INTEGER HPT;

    timeBeginPeriod(1);

    if( QueryPerformanceFrequency(&HPT) )	HPT_freq = HPT.LowPart;
}

DWORD GetSysTicks()  // return system miliseconds from machine start  or  from 1-st day of month
{
    LARGE_INTEGER	li;
    SYSTEMTIME		SysTime;

    if( HPT_freq )
    {
        QueryPerformanceCounter(&li);
        return	(DWORD)(li.QuadPart * 1000 / HPT_freq);
    }
    else
    {
        GetSystemTime(&SysTime); // GetTickCount ???
        return	(DWORD)SysTime.wMilliseconds + 1000 * (SysTime.wSecond + 60 *
                      (SysTime.wMinute + 60 * (SysTime.wHour + 24*SysTime.wDay)));   // miliseconds from 1-st day of month
    }
}

void GetTimerParam()	// Get timer specs
{
    long		dw1=0, dw2, CurrentTickSize;
    double		dbl=0;
    TIMECAPS	tc={0};
    SYSTEMTIME  SysTime;
    LARGE_INTEGER HPT, li;

    timeGetDevCaps(&tc, sizeof(tc)); // Min/Max timer

    if( !QueryPerformanceFrequency(&HPT) )
    {
        return;  // not supported, it impossible
    }

    Sleep(1); // for sleep measure start from TICK begin

    QueryPerformanceCounter(&li);  // get TICK start
    dw1 = li.LowPart;
    GetSystemTime(&SysTime);
    dw2=SysTime.wMilliseconds;

    Sleep(1);

    QueryPerformanceCounter(&li);
    dbl = (li.LowPart - dw1) / ((double)HPT.LowPart / 1000.0);  // TICK size by HPT
    GetSystemTime(&SysTime);
    CurrentTickSize = SysTime.wMilliseconds-dw2;       // TICK size by system clock
    if( CurrentTickSize<0 )  CurrentTickSize += 1000;  // correction if seconds tick

    std::cout << "High performance timer frequency: " << HPT.LowPart << std::endl;
    std::cout << "Timer supported resolution, in milliseconds, min: " << tc.wPeriodMin << ", max: " << tc.wPeriodMax << std::endl;
    std::cout << "Tick size by HPT: " << dbl << std::endl;
    std::cout << "Tick size by system clock: " << CurrentTickSize << std::endl;

//	timeBeginPeriod(1); // set minimal time
}
