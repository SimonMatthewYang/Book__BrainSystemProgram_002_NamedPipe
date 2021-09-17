﻿#include <iostream>
#include <tchar.h>
#include <Windows.h>
#include <stdio.h>
#include <time.h>

#ifdef _UNICODE
#define _tcout wcout
#else
#define _tcout cout
#endif // _UNICODE
using namespace std;

#define BUF_SIZE 1024
int CommToClient(HANDLE, clock_t&, clock_t&);

int _tmain(int argc, TCHAR* argv[])
{
    TCHAR pipeName[] = _T("\\\\.\\pipe\\simple_pipe");
    HANDLE hPipe;

    clock_t start_t, end_t;

    start_t = clock();

    while (true)
    {
        hPipe = CreateNamedPipe(
            pipeName,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            BUF_SIZE,
            BUF_SIZE,
            20000,
            NULL
        );
        time_t curTime = time(NULL);
        _tcout << _T(">>> 01 Create Pipe   ") << curTime <<  endl;

        if (hPipe == INVALID_HANDLE_VALUE)
        {
            _tcout << _T("CreatePipe Failed") << endl;
            return -1;
        }

        BOOL isOk = 0;
        isOk = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        curTime = time(NULL);
        _tcout << _T(">>> 02 ConnectNamedPipe   ") << curTime << endl;

        if (isOk)
            CommToClient(hPipe, start_t, end_t);
        else
            CloseHandle(hPipe);
    }
    return 1;
}

int CommToClient(HANDLE hPipe, clock_t& start_t, clock_t& end_t)
{
    TCHAR fileName[MAX_PATH];
    TCHAR dataBuf[BUF_SIZE];

    BOOL isSuccess;
    DWORD fileNameSize;
    isSuccess = ReadFile(
        hPipe,
        fileName,
        MAX_PATH * sizeof(TCHAR),
        &fileNameSize,
        NULL
    );
    
    // Error!
    if (!isSuccess || fileNameSize == 0)
    {
        _tcout << _T("Pipe Read Message Error !!! ") << endl;
        return -1;
    }
    time_t curTime = time(NULL);
    _tcout << _T(" >>> 03 Read FileName : ") << fileName << _T("   ") << curTime << endl;

    FILE* filePtr = NULL;
    _tfopen_s(&filePtr, fileName, _T("r"));


    if (filePtr == NULL)
    {
        _tcout << _T("File Open Fault!") << endl;
        return -1;
    }

    DWORD bytesWritten = 0;
    DWORD bytesRead = 0;
    
    while (!feof(filePtr))
    {
        bytesRead = fread(dataBuf, 1, BUF_SIZE, filePtr);

        WriteFile(
            hPipe,
            dataBuf,
            bytesRead,
            &bytesWritten,
            NULL
        );

        curTime = time(NULL);
        _tcout << _T(" >>> 04 Write File : ") << dataBuf << _T("   ") << curTime << endl;

        if (bytesRead != bytesWritten)
        {
            _tcout << _T("Pipe Wirte Messgae Eeror!") << endl;
            break;
        }
    }

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    return 1;
}