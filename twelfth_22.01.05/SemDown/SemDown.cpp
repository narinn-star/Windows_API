#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("SemDown");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
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

HANDLE hSem;
int Y = 0;
DWORD WINAPI ThreadDownLoad(LPVOID Param)
{
	HDC hdc;
	int i, y, j, s;
	TCHAR str[256];

	srand(GetTickCount());
	s = rand() % 5 + 1;
	hdc = GetDC(hWndMain);

	Y += 20;
	y = Y;
	TextOut(hdc, 10, y, "�����", 6);
	GdiFlush();
	WaitForSingleObject(hSem, INFINITE);
	for (i = 0; i < 100; i++) {
		wsprintf(str, "�ٿ�ε��� : %d%%�Ϸ�", i);
		for (j = 0; j < i; j++)
			strcat(str, "I");
		TextOut(hdc, 10, y, str, lstrlen(str));
		Sleep(20 * s);
	}
	lstrcpy(str, "�ٿ�ε带 �Ϸ��߽��ϴ�");
	TextOut(hdc, 10, y, str, lstrlen(str));
	ReleaseSemaphore(hSem, 1, NULL);
	ReleaseDC(hWndMain, hdc);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HANDLE hThread;
	DWORD ThreadID;
	TCHAR* Mes = "���콺 ���� ��ư�� ������ �ٿ�ε带 �����մϴ�";

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		hSem = CreateSemaphore(NULL, 5, 5, NULL);		//���ÿ� 5���� �����Ű�ڴ�.
		return 0;
	case WM_LBUTTONDOWN:
		hThread = CreateThread(NULL, 0, ThreadDownLoad, NULL, 0, &ThreadID);
		CloseHandle(hThread);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 10, 0, Mes, lstrlen(Mes));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		CloseHandle(hSem);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
