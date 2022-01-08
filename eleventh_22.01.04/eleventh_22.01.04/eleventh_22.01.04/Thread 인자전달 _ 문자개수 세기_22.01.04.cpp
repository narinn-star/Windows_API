#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND MainWnd;
LPCTSTR lpszClass = TEXT("RandGrp");
TCHAR str[256];

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
	MainWnd = hWnd;
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

DWORD WINAPI ThreadFunc(LPVOID temp)
{
	HDC hdc;
	int len;
	TCHAR* ch = (TCHAR*)temp;
	TCHAR buf[128];
	int count = 0;
	for (int i = 0; i < lstrlen(str); i++)
	{
		if (str[i] == *ch)
			count++;
	}

	hdc = GetDC(MainWnd);
	wsprintf(buf, TEXT("현재까지 입력한 %c의 개수는 %d 입니다"), *ch, count);
	TextOut(hdc, 10, 10, buf, lstrlen(buf));
	ReleaseDC(MainWnd, hdc);
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	DWORD ThreadID;
	HANDLE hThread;
	int i, len;
	static TCHAR ch;

	static TCHAR c1;

	switch (iMessage) {
	case WM_CHAR:
		//*
		len = lstrlen(str);
		str[len] = (TCHAR)wParam;
		str[len + 1] = 0;
		InvalidateRect(hWnd, NULL, FALSE);

		c1 = TEXT('a');
		hThread = CreateThread(NULL, 0, ThreadFunc, &c1, 0, &ThreadID);
		//CloseHandle(hThread);
		return 0;
	case WM_CREATE:

		return 0;



	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 100, 100, str, lstrlen(str));
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
