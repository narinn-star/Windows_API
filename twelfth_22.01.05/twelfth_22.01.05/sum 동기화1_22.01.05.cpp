#include <windows.h>
#include<stdlib.h>
#include <time.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Key");
static TCHAR str[256];
HWND hWndMain;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

struct ThreadParam2 {
	int istart, iend;
} Param[4] = {
	{ 1, 25000, },
	{ 25001, 50000, },
	{ 50001, 75000, },
	{ 75001, 100000, }
};

//25001, n3 = 50001, n4 = 75001;

//int sum;
LONGLONG sum;  //64bit integer data type 
CRITICAL_SECTION crit;

DWORD WINAPI ThreadFunc3(LPVOID temp)
{
	HDC hdc;
	int count = 0;

	ThreadParam2 par;
	par = *(ThreadParam2*)temp;
	hdc = GetDC(hWndMain);

	for (int i = par.istart; i <= par.iend; i++) {
		EnterCriticalSection(&crit);
		sum = sum + i;
		LeaveCriticalSection(&crit);
	}

	ReleaseDC(hWndMain, hdc);
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	//static TCHAR str[256];
	int len;
	DWORD ThreadID;
	HANDLE hThread;

	TCHAR buf[32];
	switch (iMessage) {
	case WM_CREATE:
		InitializeCriticalSection(&crit);
		hWndMain = hWnd;
		return 0;
	case WM_LBUTTONDOWN:
		sum = 0;
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		hWndMain = hWnd; //이게 없으면 프로그램내에서 뜨질X
		hThread = CreateThread(NULL, 0, ThreadFunc3, &Param[0], 0, &ThreadID);
		CloseHandle(hThread);
		hThread = CreateThread(NULL, 0, ThreadFunc3, &Param[1], 0, &ThreadID);
		CloseHandle(hThread);
		hThread = CreateThread(NULL, 0, ThreadFunc3, &Param[2], 0, &ThreadID);
		CloseHandle(hThread);
		hThread = CreateThread(NULL, 0, ThreadFunc3, &Param[3], 0, &ThreadID);
		CloseHandle(hThread);
		/*
		hThread = CreateThread(NULL, 0, ThreadFunc2, &ch1, 0, &ThreadID);
		CloseHandle(hThread);
		hThread = CreateThread(NULL, 0, ThreadFunc2, &ch2, 0, &ThreadID);
		CloseHandle(hThread);
		*/
		Sleep(3000);
		//WaitForMultipleObjects(4, hThread, TRUE, INFINITE);
		//hThread : HANDLE, TRUE : 모두 다 끝날 때까지, INFINITE : 영원히 기다림
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		wsprintf(buf, TEXT("%I64u"), sum);
		TextOut(hdc, 20, 30, buf, lstrlen(buf));
		EndPaint(hWnd, &ps);
		return 0;


	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
