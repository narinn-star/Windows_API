// 두개의 스레드를 만드는 예제. 스레드 인수를 설명한다.
//여러개의 스레드가 공유자원 하나를 접근할 때 스레드의 순서를 제어
#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("SHThread2");
HWND hWndMain;
LONGLONG g_x;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	if (!hPrevInstance) {
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hInstance = hInstance;
		WndClass.lpfnWndProc = (WNDPROC)WndProc;
		WndClass.lpszClassName = lpszClass;
		WndClass.lpszMenuName = MAKEINTRESOURCE(101);
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&WndClass);
	}

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	HACCEL hAccel;
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(102));

	while (GetMessage(&Message, NULL, 0, 0)) {
		if (!TranslateAccelerator(hWnd, hAccel, &Message)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
	return Message.wParam;
}

typedef struct tagmyPOINT{
	POINT pt;
	COLORREF iColor;
} mypoint;
mypoint p[1000];

struct ThreadParam {
	int x, y;			//출력 위치
	TCHAR name[10];		//색 이름
	COLORREF MyColor;	//각 스레드 색 저장 및 비교
	//TCHAR color[40];
}Param[3] = {
	{300, 350, TEXT("Red"), RGB(255, 0, 0)},
	{300, 370, TEXT("Green"), RGB(0, 255, 0)},
	{300, 390, TEXT("Blue"), RGB(0, 0, 255)}
};

HANDLE hEvent;
COLORREF iCurrentColor = RGB(255, 0, 0);
int iCount;
int red, green, blue;
bool run = true;

// 인수로 전달받은 RECT 영역에 사각형을 그린다.
DWORD WINAPI ThreadFunc(LPVOID prc)
{
	HDC hdc;
	ThreadParam* P = (ThreadParam*)prc;
	TCHAR buf[256];
	TCHAR str[256];
	int count = 0;
	while (run) {
		count = 0;
		for (int i = 0; i < iCount; i++) {
			if (P->MyColor == p[i].iColor) {
				count++;
			}
		}
		hdc = GetDC(hWndMain);
		wsprintf(buf, TEXT("현재까지 입력한 %s의 개수는 %d개"), P->name, count);
		TextOut(hdc, P->x, 100 + P->y, buf, lstrlen(buf));
		ReleaseDC(hWndMain, hdc);
		WaitForSingleObject(hEvent, INFINITE);
		ResetEvent(hEvent);
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush;
	DWORD ThreadID1;
	static HANDLE hThread[3];
	TCHAR str[100];
	int i, x, y;

	switch (iMessage) {
	case WM_CREATE:
		//hMutex = CreateMutex(NULL, FALSE, NULL);			//FALSE : signaled (신호가 왔다), TRUE : nonsignaled
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 40001:
			iCurrentColor = RGB(255, 0, 0);
			break;
		case 40002:
			iCurrentColor = RGB(0, 255, 0);
			break;
		case 40003:
			iCurrentColor = RGB(0, 0, 255);
			break;
		}
		return 0;

	case WM_RBUTTONDOWN:
		
		return 0;
	case WM_LBUTTONDOWN:
		hdc = GetDC(hWnd);
		SetFocus(hWnd);

		x = LOWORD(lParam);
		y = HIWORD(lParam);

		p[iCount].pt.x = x;
		p[iCount].pt.y = y;
		p[iCount].iColor = iCurrentColor;

		hBrush = CreateSolidBrush(iCurrentColor);
		SelectObject(hdc, hBrush);
		iCount++;
		//SetEvent(hEvent);

		Rectangle(hdc, x - 20, y - 20, x + 20, y + 20);
		ReleaseDC(hWnd, hdc);

		//WaitForMultipleObjects(3, hThread, TRUE, INFINITE);
		//InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_LBUTTONUP:
		for (int i = 0; i < 3; i++) {
			hThread[i] = CreateThread(NULL, 0, ThreadFunc, &Param[i], 0, &ThreadID1);
		}
		SetEvent(hEvent);
		return 0;

	case WM_PAINT:
		hWndMain = hWnd;
		SetEvent(hEvent);
		hdc = BeginPaint(hWnd, &ps);
		for (int i = 0; i < iCount; i++) {
			hBrush = CreateSolidBrush(p[i].iColor);
			hBrush = (HBRUSH)SelectObject(hdc, hBrush);
			Rectangle(hdc, p[i].pt.x - 20, p[i].pt.y - 20, p[i].pt.x + 20, p[i].pt.y + 20);

			DeleteObject(SelectObject(hdc, hBrush));
		}
		//ReleaseDC(hWnd, hdc);
		//wsprintf(str, TEXT("1~1000000까지의 합 : %I64u"), g_x);
		//TextOut(hdc, 200, 150, str, lstrlen(str));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_CLOSE:
		run = false;
		SetEvent(hEvent);
		WaitForMultipleObjects(3, hThread, TRUE, INFINITE);
		break;//*return 0 가 아닌 break; 로 썼을때 thread가 다 파괴됨 

	case WM_DESTROY:
		CloseHandle(hEvent);
		PostQuitMessage(0);
		return 0;
	}
	
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
