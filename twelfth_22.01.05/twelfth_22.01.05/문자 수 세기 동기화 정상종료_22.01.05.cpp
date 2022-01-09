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
	int y;
}Param[3] = { { 'a' ,0}, { 'b',40 }, { 'c' ,80} };

bool run = true;
TCHAR str[256];
HANDLE hEvent;

DWORD WINAPI ThreadFunction(LPVOID temp)	//&ch���� �Ѿ��
{
	HDC hdc;
	int count;

	ThreadParam* p = (ThreadParam*)temp;
	TCHAR buf[128];
	while (run) //false
	{
		WaitForSingleObject(hEvent, INFINITE);

		count = 0;
		for (int i = 0; i < lstrlen(str); i++)
		{
			if (str[i] == p->ch)
			{
				count++;
			}
		}
		hdc = GetDC(MainWnd);
		wsprintf(buf, TEXT("������� �Է��� %c�� ������ %d��"), p->ch, count);
		TextOut(hdc, 100, 100 + p->y, buf, lstrlen(buf));
		ReleaseDC(MainWnd, hdc);
		ResetEvent(hEvent);
	}
	wsprintf(buf, TEXT("%c count Thread ����"), p->ch);
	MessageBox(NULL, buf, TEXT("�˸�"), MB_OK);

	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	int len;
	static DWORD ThreadID[3];
	static HANDLE hThread[3];
	static TCHAR ch;
	switch (iMessage) {

	case WM_CHAR:

		len = lstrlen(str);
		str[len] = (TCHAR)wParam;
		str[len + 1] = 0;
		InvalidateRect(hWnd, NULL, FALSE);
		SetEvent(hEvent);
		return 0;

	case WM_CREATE:
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		for (int i = 0; i < 3; i++) {
			hThread[i] = CreateThread(NULL, 0, ThreadFunction, &Param[i], 0, &ThreadID[i]);	//&ch �ѱ����� ������ �ּҸ� ����	LPVOID�� �Ѿ.
			//CloseHandle();  //CloseHandle ��� X WM_DESTROY�� WaitForMultipleObjects���� ��ٸ����ֱ� ����
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 105, 200, str, lstrlen(str));
		EndPaint(hWnd, &ps);
		return 0;
		//*/
	case WM_DESTROY:
		run = false;
		SetEvent(hEvent); //�������� �ѹ��� ������ϱ� �Ѵ�

		WaitForMultipleObjects(3, hThread, TRUE, INFINITE);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
