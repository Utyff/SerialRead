#include <afxres.h>
#include "serialread.h"

/*
#include <math.h>
#include <winbase.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <windows.h>
*/

DWORD   nSamplesPerSec=48800;
HANDLE  hPort;

int initSerial() {
    BOOL	bl;

    hPort = CreateFile(tzCOMport, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPort == INVALID_HANDLE_VALUE)
    {
        MsgError( GetLastError(), "Unable to open COM-port" );
        hPort = 0;
        return false;
    }

    bl=SetCommMask(hPort, EV_RXCHAR);
    if( !bl )
    {
        MsgError( GetLastError(), "SetCommMask" );
        CloseHandle(hPort);
        hPort = 0;
        return false;
    }

    bl=SetupComm(hPort, 64, 64); // set recomended IN/OUT queue size
    if( !bl )
    {
        MsgError( GetLastError() );
        CloseHandle(hPort);
        hPort = 0;
        return false;
    }

/*	COMMPROP *pr;
	pr = (COMMPROP*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(COMMPROP));
	bl = GetCommProperties(hPort, pr);
	DWORD dw = GetLastError();
	if (pr->wPacketLength != sizeof(COMMPROP))
	{
		pr=(COMMPROP*)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pr, pr->wPacketLength);
		pr->dwProvSpec1=COMMPROP_INITIALIZED;
		bl = GetCommProperties(hPort, pr);
		dw = GetLastError();
	}
	HeapFree(GetProcessHeap(),0,pr);  //*/

#define TIMEOUT 0

    COMMTIMEOUTS CommTimeOuts;
    CommTimeOuts.ReadIntervalTimeout = 0; //xFFFFFFFF;
    CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
    CommTimeOuts.ReadTotalTimeoutConstant = TIMEOUT;
    CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
    CommTimeOuts.WriteTotalTimeoutConstant = TIMEOUT;

    if(!SetCommTimeouts(hPort, &CommTimeOuts))
    {
        MsgError( GetLastError() );
        CloseHandle(hPort);
        hPort = 0;
        return false;
    }

    DCB ComDCM;

    memset(&ComDCM,0,sizeof(ComDCM));
    ComDCM.DCBlength = sizeof(DCB);
    if( !GetCommState(hPort, &ComDCM) )
    {
        MsgError( GetLastError() );
        CloseHandle(hPort);
        hPort = 0;
        return false;
    }

//dwMaxBaud			268435456
//dwSettableBaud	268856176
#define baudrate	  2000999

    ComDCM.BaudRate = DWORD(baudrate);
    ComDCM.ByteSize = 8;
    ComDCM.Parity = NOPARITY;
    ComDCM.StopBits = ONESTOPBIT;
    ComDCM.fAbortOnError = TRUE;
    ComDCM.fDtrControl = DTR_CONTROL_DISABLE;
    ComDCM.fRtsControl = RTS_CONTROL_DISABLE;
    ComDCM.fBinary = TRUE;
    ComDCM.fParity = FALSE;
    ComDCM.fInX = FALSE;
    ComDCM.fOutX = FALSE;
    ComDCM.XonChar = 0;
    ComDCM.XoffChar = (unsigned char)0xFF;
    ComDCM.fErrorChar = FALSE;
    ComDCM.fNull = FALSE;
    ComDCM.fOutxCtsFlow = FALSE;
    ComDCM.fOutxDsrFlow = FALSE;
    ComDCM.XonLim = 128;
    ComDCM.XoffLim = 128;

    if(!SetCommState(hPort, &ComDCM))
    {
        MsgError( GetLastError(), "SetCommState" );
        CloseHandle(hPort);
        hPort = 0;
        return false;
    }

    return true;
}
