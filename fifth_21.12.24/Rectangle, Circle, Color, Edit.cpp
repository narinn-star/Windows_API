#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "resource.h"
#include <tchar.h>

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
	POINT x1;	//���� ��� ��
	POINT x2;	//���� �ϴ� ��
	COLORREF color;		//�޴� �÷� ���� �ڷᱸ�� �߰��ϱ�
	int iShape;			//0 : Rectangle(), 1 : Ellipse()
} myObject;

myObject obj[1000];
int iCount = 0;
COLORREF CurrentColor = RGB(255, 0, 0);		//����Ʈ �� ���� (RED)
int iCurrentShape = 0;

#define ID_EDIT 100
#define ID_BUTTON 200
HWND hEdit;

//*
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int sx, sy, oldx, oldy;
	static BOOL bNowDraw = FALSE;
	HDC hdc;
	PAINTSTRUCT ps;
	//HPEN hPen;
	HBRUSH hbrush;
	TCHAR str[128];
	//int iShape;

	switch (iMessage) {
	case WM_INITMENU:
		if (CurrentColor == RGB(255, 0, 0)){
			CheckMenuItem((HMENU)wParam, ID_RED, MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem((HMENU)wParam, ID_GREEN, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, ID_BLUE, MF_BYCOMMAND | MF_UNCHECKED);
		}
		else if (CurrentColor == RGB(0, 255, 0)){
			CheckMenuItem((HMENU)wParam, ID_RED, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, ID_GREEN, MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem((HMENU)wParam, ID_BLUE, MF_BYCOMMAND | MF_UNCHECKED);
		}
		else if (CurrentColor == RGB(0, 0, 255)){
			CheckMenuItem((HMENU)wParam, ID_RED, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, ID_GREEN, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, ID_BLUE, MF_BYCOMMAND | MF_CHECKED);
		}

		if (iCurrentShape == 0){
			CheckMenuItem((HMENU)wParam, ID_RECT, MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem((HMENU)wParam, ID_CIRC, MF_BYCOMMAND | MF_UNCHECKED);
		}
		else if (iCurrentShape == 1){
			CheckMenuItem((HMENU)wParam, ID_RECT, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, ID_CIRC, MF_BYCOMMAND | MF_CHECKED);
		}
		return 0;
	case WM_CREATE:
		hEdit = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
			ES_AUTOHSCROLL, 10, 10, 200, 25, hWnd, (HMENU)ID_EDIT, g_hInst, NULL);
		_itot(iCurrentShape, str, 10);
		SetWindowText(hEdit, str);

		CreateWindow(TEXT("button"), TEXT("����"), WS_CHILD | WS_VISIBLE |
			BS_PUSHBUTTON, 220, 10, 100, 25, hWnd, (HMENU)ID_BUTTON, g_hInst, NULL);				//220,10,100,25 => ��ġ ����
		return 0;

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
			//�׸�����  -  > Raster Operation
			//SetROP2(hdc,R2_XORPEN);	//XORPEN : ���󺹱�
			//hPen = CreatePen(PS_SOLID, 5, RGB(0, 255, 255));
			//hPen = CreatePen(PS_SOLID, 5, RGB(255,0,0));
			//hPen = CreatePen(PS_SOLID, 5, RGB(255, 255, 255));

			SetROP2(hdc, R2_NOTXORPEN);
			//hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

			//hbrush = CreateSolidBrush(RGB(255, 0, 0));
			//hPen = (HPEN)SelectObject(hdc, hPen);
			hbrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

			if (iCurrentShape == 0){
				Rectangle(hdc, sx, sy, oldx, oldy);
				//���� �� �����
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);

				Rectangle(hdc, sx, sy, oldx, oldy);
			}
			else {
				Ellipse(hdc, sx, sy, oldx, oldy);
				//���� �� �����
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);

				Ellipse(hdc, sx, sy, oldx, oldy);
			}
			//DeleteObject(SelectObject(hdc, hbrush));
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

		if (iCurrentShape == 0){
			Rectangle(hdc, sx, sy, oldx, oldy);
			//���� �� �����
			//oldx = LOWORD(lParam);
			//oldy = HIWORD(lParam);

			//Rectangle(hdc, sx, sy, oldx, oldy);
		}
		else {
			Ellipse(hdc, sx, sy, oldx, oldy);
			//���� �� �����
			//oldx = LOWORD(lParam);
			//oldy = HIWORD(lParam);

			//Ellipse(hdc, sx, sy, oldx, oldy);
		}

		obj[iCount].x1.x = sx;
		obj[iCount].x1.y = sy;
		obj[iCount].x2.x = oldx;
		obj[iCount].x2.y = oldy;
		obj[iCount].color = CurrentColor;
		obj[iCount].iShape = iCurrentShape;
		iCount++;

		DeleteObject(SelectObject(hdc, hbrush));
		//DeleteObject(SelectObject(hdc, hPen));
		ReleaseDC(hWnd, hdc);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		for (int i = 0; i < iCount; i++){
			hbrush = CreateSolidBrush(obj[i].color);
			hbrush = (HBRUSH)SelectObject(hdc, hbrush);		//�׸��� �����뿡 �귯�� ����
			if (obj[i].iShape == 0){
				Rectangle(hdc, obj[i].x1.x, obj[i].x1.y, obj[i].x2.x, obj[i].x2.y);
			}
			else{
				Ellipse(hdc, obj[i].x1.x, obj[i].x1.y, obj[i].x2.x, obj[i].x2.y);
			}

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
		case ID_RECT:
			iCurrentShape = 0;
			_itot(iCurrentShape, str, 10);
			SetWindowText(hEdit, str);
			break;
		case ID_CIRC:
			iCurrentShape = 1;
			_itot(iCurrentShape, str, 10);
			SetWindowText(hEdit, str);
			break;
		case ID_BUTTON:
			GetWindowText(hEdit, str, 128);			//EDIT Control�� ���� �޾ƿ�
			iCurrentShape = _ttoi(str);				//str�� int�� atoi�� �߸��ڵ� #include <tchar.h> �ʿ���
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

/* ROP ��带 ������� �ʰ� ������� ���� ��
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