#include <afxres.h>
#include <iostream>
#include "serialread.h"



DWORD   nSamplesPerSec=48800;
HANDLE  hPort;
DWORD   dwSPSBeginTime, dwSPSReceivedBytes, dwSPS;  // for sample rate calculation


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

    ComDCM.BaudRate = baudrate;
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


DWORD WINAPI readSerial()
{
#define CAP_BUFFER_SIZE 64*1024
    BYTE		btBuf[CAP_BUFFER_SIZE];
    DWORD		dwBytes, dwTime;
    struct _COMSTAT status;
    unsigned long etat;

// -------- Main capting circle
    while( !stop )  // Capture circle until exit signal.
    {
        if( !ClearCommError(hPort, &etat, &status) )   // Get the number of bytes in the read queue
        {
            MsgError(etat, "COM port error");
            return 13;
        }

        if( status.cbInQue )  // is there bytes ready for read?
        {
            if( !ReadFile( hPort, btBuf, status.cbInQue, &dwBytes, NULL ) )  // Need to set limit bytes for read
            {
                MsgError(GetLastError(), "Can`t read from COM-port");
                return 13;
            }
        }
        else dwBytes=0;

        if( dwBytes ) // calculate sample per second
        {
            dwTime = GetSysTicks();
            dwSPSReceivedBytes += dwBytes;
            if( (dwTime-dwSPSBeginTime) >= 1000 )
            {
                if( dwSPSBeginTime )
                {
                    dwSPS = (DWORD) ((double) dwSPSReceivedBytes / (dwTime - dwSPSBeginTime) * 1000.0);
                    std::cout << dwSPS << " " << dwTime-dwSPSBeginTime << std::endl;
                }
                dwSPSReceivedBytes = 0;
                dwSPSBeginTime = dwTime;
            }
        }

        Sleep(1);
    } // ----------- Main caturing circle

    return 25; // Exit from capture thread
}
