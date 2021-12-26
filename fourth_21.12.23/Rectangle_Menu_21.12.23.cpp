#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("RopMode");

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
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
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

typedef struct tagmyObject{
	POINT x1;	//좌측 상단 점
	POINT x2;	//우측 하단 점
	COLORREF color;		//메뉴 컬러 지정 자료구조 추가하기
} myObject;

myObject obj[1000];
int iCount = 0;
COLORREF CurrentColor = RGB(255, 0, 0);		//디폴트 값 지정 (RED)

//*
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int sx, sy, oldx, oldy;
	static BOOL bNowDraw = FALSE;
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN hPen;
	HBRUSH hbrush;

	switch (iMessage) {
	case WM_LBUTTONDOWN:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		oldx = sx;
		oldy = sy;

		bNowDraw = TRUE;
		return 0;
	case WM_MOUSEMOVE:
		if (bNowDraw) {
			hdc = GetDC(hWnd);
			//그리기모드  -  > Raster Operation
			//SetROP2(hdc,R2_XORPEN);	//XORPEN : 원상복구
			//hPen = CreatePen(PS_SOLID, 5, RGB(0, 255, 255));
			//hPen = CreatePen(PS_SOLID, 5, RGB(255,0,0));
			//hPen = CreatePen(PS_SOLID, 5, RGB(255, 255, 255));

			SetROP2(hdc, R2_NOTXORPEN);
			//hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

			hbrush = CreateSolidBrush(RGB(255, 0, 0));
			//hPen = (HPEN)SelectObject(hdc, hPen);
			SelectObject(hdc, GetStockObject(NULL_BRUSH));

			obj[iCount].x1.x = sx;
			obj[iCount].x1.y = sy;
			obj[iCount].x2.x = oldx;
			obj[iCount].x2.y = oldy;

			Rectangle(hdc, sx, sy, oldx, oldy);
			//이전 선 지우기
			oldx = LOWORD(lParam);
			oldy = HIWORD(lParam);

			Rectangle(hdc, sx, sy, oldx, oldy);
			DeleteObject(SelectObject(hdc, hbrush));
			//DeleteObject(SelectObject(hdc, hPen));

			ReleaseDC(hWnd, hdc);
		}
		return 0;
	case WM_LBUTTONUP:
		bNowDraw = FALSE;
		hdc = GetDC(hWnd);
		//hPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));

		hbrush = CreateSolidBrush(CurrentColor);
		hbrush = (HBRUSH)SelectObject(hdc, hbrush);
		//hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		//hPen = (HPEN)SelectObject(hdc, hPen);

		Rectangle(hdc, sx, sy, oldx, oldy);

		obj[iCount].x1.x = sx;
		obj[iCount].x1.y = sy;
		obj[iCount].x2.x = oldx;
		obj[iCount].x2.y = oldy;
		iCount++;
		obj[iCount].color = CurrentColor;

		DeleteObject(SelectObject(hdc, hbrush));
		//DeleteObject(SelectObject(hdc, hPen));
		ReleaseDC(hWnd, hdc);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		for (int i = 0; i < iCount; i++){
			hbrush = CreateSolidBrush(obj[i].color);
			hbrush = (HBRUSH)SelectObject(hdc, hbrush);

			Rectangle(hdc, obj[i].x1.x, obj[i].x1.y, obj[i].x2.x, obj[i].x2.y);
			DeleteObject(SelectObject(hdc, hbrush));
		}
		EndPaint(hWnd, &ps);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case ID_RED:
			CurrentColor = RGB(255, 0, 0);
			break;
		case ID_GREEN:
			CurrentColor = RGB(0, 255, 0);
			break;
		case ID_BLUE:
			CurrentColor = RGB(0, 0, 255);
			break;
		}
			return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
//*/

/* ROP 모드를 사용하지 않고 흰색으로 지울 때
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
static int sx,sy,oldx,oldy;
static BOOL bNowDraw = FALSE;
HDC hdc;
switch (iMessage) {
case WM_LBUTTONDOWN:
sx = LOWORD(lParam);
sy = HIWORD(lParam);
oldx = sx;
oldy = sy;
bNowDraw = TRUE;
return 0;
case WM_MOUSEMOVE:
if (bNowDraw) {
hdc=GetDC(hWnd);
SelectObject(hdc,GetStockObject(WHITE_PEN));
MoveToEx(hdc,sx,sy,NULL);
LineTo(hdc,oldx,oldy);
oldx=LOWORD(lParam);
oldy=HIWORD(lParam);
SelectObject(hdc,GetStockObject(BLACK_PEN));
MoveToEx(hdc,sx,sy,NULL);
LineTo(hdc,oldx,oldy);
ReleaseDC(hWnd,hdc);
}
return 0;
case WM_LBUTTONUP:
bNowDraw = FALSE;
hdc=GetDC(hWnd);
MoveToEx(hdc,sx,sy,NULL);
LineTo(hdc,oldx,oldy);
ReleaseDC(hWnd, hdc);
return 0;
case WM_DESTROY:
PostQuitMessage(0);
return 0;
}
return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}
//*/
