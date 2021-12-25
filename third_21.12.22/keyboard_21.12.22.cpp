//#define UNICODE
#define __UNICODE		//c 함수용

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("KeyDown");

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

//참고 : 상수를 변수로 선언  <--- p.20, line : -12

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static int x = 100;		//static : 정적 변수	(전역변수처럼 생각) - 선언한 클래스, 함수 내에서의 전역변수
	static int y = 100;

	//static TCHAR *c = TEXT("A");
	static TCHAR c[] = TEXT("가ab");		//Neutral Codes
	//static char c[] = "가ab";				//멀티바이트 MBCS
	//static wchar_t c[] = L"가ab";			//유니코드 UNICODE
	static BOOL key = TRUE;

	switch (iMessage) {
	case WM_KEYDOWN:
		switch (wParam) { 
		case VK_LEFT:	//0x25 (0x => 16진수)	VK : 가상 키 코드 (p.85)
			x -= 8;
			break;
		case VK_RIGHT:
			x += 8;
			break;
		case VK_UP:
			y -= 8;
			break;
		case VK_DOWN:
			y += 8;
			break;
		case VK_SPACE:
			key = !key;
			//static TCHAR c[] = ("A");
			//key ? _tcscpy(c, TEXT("다ab")) : _tcscpy(c, TEXT("갸ab"));
			//key ? wcscpy(c, TEXT("다ab")) : _tcscpy(c, TEXT("갸ab"));
			key ? strcpy(c, TEXT("다ab")) : _tcscpy(c, TEXT("갸ab"));
			
			//static TCHAR *c = TEXT("A");
			//if (key)
			//	c = TEXT("A");
			//else if (!key)
			//	c = TEXT("#");
			break;
		}
		//화면을 다시 그려야 할 경우가 발생했을 때
		//OS에게 이 윈도우(hWnd)로 WM_PAINT 발생시키도록 요구
		//== OS에게 이 윈도우(hWnd)에 다시 그려야 할 영역이 생겼다고 전달
		//InvalidateRect(hWnd,NULL,TRUE);
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, x, y, c, strlen(c));	//x, y 위치에 "A"문자를 1개 출력
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
