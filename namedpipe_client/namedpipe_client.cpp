// namedpipe_client.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <Windows.h>
#include <time.h>

#ifdef _UNICODE
#define _tcout wcout
#else
#define _tcout cout
#endif // _UNICODE
using namespace std;

#define BUF_SIZE 1024

int _tmain(int argc, TCHAR* argv[])
{
	HANDLE hPipe;
	TCHAR readDataBuf[BUF_SIZE + 1];
	TCHAR pipeName[] = _T("\\\\.\\pipe\\simple_pipe");


	clock_t start_t, end_t;

	start_t = clock();

	// Loop Connect Pipe Handle
	while (true)
	{
		hPipe = CreateFile(
			pipeName,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		time_t curTime = time(NULL);
		_tcout << _T("}} CLIEND 01 - CreateFile - Connect!!") << curTime << endl;

		// 연결 성공했다면 While 나간다.
		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tcout << _T("Cloud not Open Pipe") << endl;
			return 0;
		}

		if (!WaitNamedPipe(pipeName, 20000))
		{
			_tcout << _T("Could not Open Pipe") << endl;
			return 0;
		}
	}

	DWORD pipeMode = PIPE_READMODE_MESSAGE | PIPE_WAIT; //메시지 기반으로 모드 변경
	BOOL isSuccess = SetNamedPipeHandleState(
		hPipe // 서버 파이프와 연결된 핸들
		, &pipeMode // 변경할 모드 정보.
		, NULL
		, NULL
	);

	if (isSuccess == FALSE)
	{
		_tcout << _T("SetNamedPipeHandleState Faile!!") << endl;
		return 0;
	}


	TCHAR fileName[] = _T("..\\news.txt");
	DWORD bytesWritten = 0;
	
	isSuccess = WriteFile(
		hPipe // 서버 파이프와 연결된 핸들
		, fileName // 전송할 메시지
		, (_tcslen(fileName) + 1) * sizeof(TCHAR) // 메시지 길이
		, &bytesWritten // 전송된 바이트 수
		, NULL
	);
	time_t curTime = time(NULL);
	_tcout << _T("}} CLIEND 02 - Write File Name   ") << curTime << endl;

	if (isSuccess == FALSE)
	{
		_tcout << _T("WriteFile Faile!!") << endl;
		return 0; 
	}

	DWORD bytesRead = 0;

	while (true)
	{
		isSuccess = ReadFile(
			hPipe
			, readDataBuf
			, BUF_SIZE * sizeof(TCHAR)
			, &bytesRead
			, NULL
		);

		if (isSuccess == FALSE
			&& GetLastError() != ERROR_MORE_DATA)
		{
			break;
		}

		readDataBuf[bytesRead] = 0;
		curTime = time(NULL);
		_tcout << _T("}} CLIEND 03 Read Buf Data : ") << readDataBuf  << curTime << endl;
	}

	CloseHandle(hPipe);
	return 0;
}