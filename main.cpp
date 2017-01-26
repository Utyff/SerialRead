#include <afxres.h>
#include <tchar.h>
#include <iostream>
#include "serialread.h"


TCHAR	tzCOMport[20] =_T("\\\\.\\COM");

int main() {
    std::cout << " -- Serial read v1.0" << std::endl << std::endl;
    GetTimerParam();
    return 0;
}


DWORD MsgError(DWORD err, const char *szInfo)
{
    TCHAR *ErrMsg;

    if( !FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                       err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&ErrMsg, 0, NULL) )
        return err;

    if( szInfo ) {
        std::cout << "\n\n ==" << szInfo << std::endl;
    }
    std::cout << "\n\n == Error code - " << err << std::endl;
    std::cout << "\n\n == Error message: " << ErrMsg << std::endl;

    LocalFree(ErrMsg);

    return err;
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
