#include <iostream>
#include <tchar.h>
#include <Windows.h>
#include <stdio.h>

#include <string>
#include <fstream>


#include <time.h>
#include <sys/timeb.h>
#include <sstream>   

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

    timeb tb;   // <sys/timeb.h>                       
    tm tstruct; // 기본 사용가능   
    ostringstream oss;
    char buf[128];


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

        SYSTEMTIME cur_time;
        GetLocalTime(&cur_time);

        _tcout << _T("01_CreateNamePipe - ")  << cur_time.wSecond << _T("-") << cur_time.wMilliseconds << endl;



        if (hPipe == INVALID_HANDLE_VALUE)
        {
            _tcout << _T("CreatePipe Failed") << endl;
            return -1;
        }

        BOOL isOk = 0;
        isOk = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        GetLocalTime(&cur_time);
        _tcout << _T("02_ConnectNamePipe - ") << cur_time.wSecond << _T("-") << cur_time.wMilliseconds << endl;


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

    SYSTEMTIME cur_time;

    GetLocalTime(&cur_time);
    _tcout << _T("server_04_Read_Client_FileName Before - ") << cur_time.wSecond << _T("-") << cur_time.wMilliseconds << endl;

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

    
    GetLocalTime(&cur_time);
    _tcout << _T("server_04_Read_Client_FileName - ") << cur_time.wSecond << _T("-") << cur_time.wMilliseconds << endl;

    //FILE* filePtr = NULL;
    //_tfopen_s(&filePtr, fileName, _T("r"));


    // Simon Test
    ifstream f_readtxt(fileName, ios_base::in);

    TCHAR str[BUF_SIZE];

    string sRead;
    wstring wsTemp;
    wstring wsRead;
    DWORD txtReadByte = 0;
    if (f_readtxt.is_open())
    {
        while (f_readtxt.peek() != EOF)
        {
            getline(f_readtxt, sRead);
            wsTemp.assign(sRead.begin(), sRead.end());

            txtReadByte += sizeof(wsTemp);

            wsRead.append(wsTemp);
            //wsRead.append(_T("\n"));
        }
        f_readtxt.close();
    }

    

    //if (filePtr == NULL)
    //{
    //    _tcout << _T("File Open Fault!") << endl;
    //    return -1;
    //}

    DWORD bytesWritten = 0;
    DWORD bytesRead = 0;
    
    _tcout << _T("server_05_Wirte_ReadFile before - ") << cur_time.wSecond << _T("-") << cur_time.wMilliseconds << endl;

    WriteFile(
        hPipe,
        wsRead.c_str(),
        txtReadByte,
        &bytesWritten,
        NULL
    );

    GetLocalTime(&cur_time);
    _tcout << _T("server_05_Wirte_ReadFile - ") << cur_time.wSecond << _T("-") << cur_time.wMilliseconds << endl;


    //while (!feof(filePtr))
    //{
    //    bytesRead = fread(dataBuf, 1, BUF_SIZE, filePtr);
    //    
    //    _tcout << _T(".txt dataRead : ") << bytesRead << endl;
    //    _tcout << _T(".txt dataBuf : ") << dataBuf << endl;
    //    _tcout << _T(".txt dataBuf(sizeof) : ") << sizeof(dataBuf) << endl;

    //    WriteFile(
    //        hPipe,
    //        dataBuf,
    //        bytesRead,
    //        &bytesWritten,
    //        NULL
    //    );

    //    curTime = time(NULL);
    //    _tcout << _T(" >>> 04 Write File : ") << dataBuf << _T("   ") << curTime << endl;

    //    if (bytesRead != bytesWritten)
    //    {
    //        _tcout << _T("Pipe Wirte Messgae Eeror!") << endl;
    //        break;
    //    }
    //}

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    return 1;
}