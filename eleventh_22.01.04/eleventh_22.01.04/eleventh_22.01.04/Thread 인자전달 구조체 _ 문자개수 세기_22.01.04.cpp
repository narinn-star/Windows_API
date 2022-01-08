#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("RandGrp");
HWND MainWnd;

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
	MainWnd = hWnd;
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

struct ThreadParam {
	TCHAR ch;
	int y;	//위치 겹치지 않도록 
}Param[3] = { { 'a', 0 }, { 'b', 40 }, { 'c', 80 } };

TCHAR str[256];
//Worker Thread
DWORD WINAPI ThreadFunction(LPVOID temp)	//&ch에서 넘어옴
{
	HDC hdc;
	int count = 0;

	ThreadParam* p = (ThreadParam*)temp;
	TCHAR buf[128];
	for (int i = 0; i < lstrlen(str); i++)
	{
		if (str[i] == p->ch)
		{
			count++;
		}
	}
	hdc = GetDC(MainWnd);
	wsprintf(buf, TEXT("현재까지 입력한 %c의 개수는 %d개"), p->ch, count);
	TextOut(hdc, 100, 100 + p->y, buf, lstrlen(buf));
	ReleaseDC(MainWnd, hdc);
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	int len;
	static DWORD ThreadID[3];
	static HANDLE hThread;
	static TCHAR ch;
	switch (iMessage) {
	case WM_CHAR:
		//*/
		len = lstrlen(str);
		str[len] = (TCHAR)wParam;
		str[len + 1] = 0;
		//*/
		InvalidateRect(hWnd, NULL, FALSE);

		for (int i = 0; i < 3; i++) {
			//&ch 넘기고싶은 인자의 주소를 보냄	LPVOID로 넘어감.
			CloseHandle(CreateThread(NULL, 0, ThreadFunction, &Param[i], 0, &ThreadID[i]));
		}
		//CloseHandle(hThread);
	case WM_CREATE:

		return 0;


	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 100, 200, str, lstrlen(str));
		EndPaint(hWnd, &ps);
		return 0;
		//*/
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
