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
typedef struct tagmyObject
{
	POINT x1;
	POINT x2;
	COLORREF color;
	int iShape; //0 = Rectangle , 1 = ellipse();
} myObject;

myObject obj[1000];
int iCount = 0;
COLORREF CurrentColor = RGB(255, 0, 0);  //default  ????
int iCurrentShape = 0;
//*

HWND hList, hScrollRed, hScrollGreen, hScrollBlue;
HWND hStaticRed, hStaticGreen, hStaticBlue;
TCHAR *Items[] = { TEXT("Rectangle"), TEXT("Ellipse") };
TCHAR *Colors[] = { TEXT("????"), TEXT("?ʷ?"), TEXT("?Ķ?") };

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int iRed, iGreen, iBlue;
	int TempPos;
	switch (iMessage) {

	case WM_INITDIALOG:
		//p.238
		hList = GetDlgItem(hDlg, IDC_LIST1);

		for (int i = 0; i < 2; i++) {
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)Items[i]);
		}
		SendMessage(hList, LB_SETCURSEL, iCurrentShape, 0);

		iRed = GetRValue(CurrentColor);
		iGreen = GetGValue(CurrentColor);
		iBlue = GetBValue(CurrentColor);

		hScrollRed = GetDlgItem(hDlg, IDC_SCROLLBAR1);
		hScrollGreen = GetDlgItem(hDlg, IDC_SCROLLBAR2);
		hScrollBlue = GetDlgItem(hDlg, IDC_SCROLLBAR3);

		SetScrollRange(hScrollRed, SB_CTL, 0, 255, TRUE);	//0 ???? 255????	(RANGE)
		SetScrollPos(hScrollRed, SB_CTL, iRed, TRUE);		//???? THUMB ??ġ	(THUMB)
		SetScrollRange(hScrollGreen, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hScrollGreen, SB_CTL, iGreen, TRUE);
		SetScrollRange(hScrollBlue, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hScrollBlue, SB_CTL, iBlue, TRUE);

		SetDlgItemInt(hDlg, IDC_STATIC_RED, iRed, TRUE);
		SetDlgItemInt(hDlg, IDC_STATIC_GREEN, iGreen, TRUE);
		SetDlgItemInt(hDlg, IDC_STATIC_BLUE, iBlue, TRUE);

		return TRUE;

	case WM_HSCROLL:		//??ũ???? WM ?޽????? ?߻???Ŵ
		//if ((HWND)lParam == hScrollRed) TempPos = iRed;
		//if ((HWND)lParam == hScrollGreen) TempPos = iGreen;
		//if ((HWND)lParam == hScrollBlue) TempPos = iBlue;
		TempPos = GetScrollPos((HWND)lParam, SB_CTL);

		switch (LOWORD(wParam)) {
		case SB_LINELEFT:				//???? (?ּ?) 0???ٴ? Ŀ????
			TempPos = max(0, TempPos - 1);
			break;
		case SB_LINERIGHT:				//?????? (?ִ?)	255???ٴ? ?۾ƾ???
			TempPos = min(255, TempPos + 1);
			break;
		case SB_PAGELEFT:				
			TempPos = max(0, TempPos - 10);
			break;
		case SB_PAGERIGHT:				
			TempPos = min(255, TempPos + 10);
			break;
		case SB_THUMBTRACK:
			TempPos = HIWORD(wParam);
			break;
		}
		if ((HWND)lParam == hScrollRed) iRed = TempPos;
		if ((HWND)lParam == hScrollGreen) iGreen = TempPos;
		if ((HWND)lParam == hScrollBlue) iBlue = TempPos;
		SetScrollPos((HWND)lParam, SB_CTL, TempPos, TRUE);
		
		SetDlgItemInt(hDlg, IDC_STATIC_RED, iRed, TRUE);
		SetDlgItemInt(hDlg, IDC_STATIC_GREEN, iGreen, TRUE);
		SetDlgItemInt(hDlg, IDC_STATIC_BLUE, iBlue, TRUE);

		//wsprintf(str, "%d", Blue);				//SetInt?? ????
		//SetWindowText(hSBlue, str);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			iCurrentShape = SendMessage(hList, LB_GETCURSEL, 0, 0);
			CurrentColor = RGB(iRed, iGreen, iBlue);
			
			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int sx, sy, oldx, oldy;
	static BOOL bNowDraw = FALSE;
	HDC hdc;
	HBRUSH hBrush;
	PAINTSTRUCT ps;
	TCHAR str[128];
	int iShape, i;
	switch (iMessage) {
	case WM_CREATE:
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDM_DIALOG:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1),
				hWnd, DlgProc);
			break;
		}
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
			//?׸???????  -  > Raster Operation
			SetROP2(hdc, R2_NOTXORPEN);//1110 - 1111 -> 0001 -1111 -> 1110 ?????? ???·? ???ư???. ?ݺ??ҽ?
			//?????? ?ȼ??? ???? xor?????? ?̿??Ͽ? ???? ex) 0000?ȼ??? 1111?ȼ??? ?׸??? (1111) ?? 1111?ȼ??? ?׸??? ?ٽ?0000?? ?ȴ?.
			//hPen = CreatePen(PS_SOLID, 5, RGB(0, 255, 255));
			hBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
			if (iCurrentShape == 0){
				Rectangle(hdc, sx, sy, oldx, oldy);
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);
				Rectangle(hdc, sx, sy, oldx, oldy);
			}
			else{
				Ellipse(hdc, sx, sy, oldx, oldy);
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);
				Ellipse(hdc, sx, sy, oldx, oldy);
			}
			//???? ?? ??????
			ReleaseDC(hWnd, hdc);
		}
		return 0;
	case WM_LBUTTONUP:
		bNowDraw = FALSE;
		hdc = GetDC(hWnd);
		hBrush = CreateSolidBrush(CurrentColor); // ???? ?? ????
		hBrush = (HBRUSH)SelectObject(hdc, hBrush); // ?׸??? ?????뿡 ?귯?? ????
		if (iCurrentShape == 0){
			Rectangle(hdc, sx, sy, oldx, oldy);
		}
		else{
			Ellipse(hdc, sx, sy, oldx, oldy);
		}
		obj[iCount].x1.x = sx;
		obj[iCount].x1.y = sy;
		obj[iCount].x2.x = oldx;
		obj[iCount].x2.y = oldy;
		obj[iCount].color = CurrentColor;
		obj[iCount].iShape = iCurrentShape;
		iCount++;
		DeleteObject(SelectObject(hdc, hBrush));
		ReleaseDC(hWnd, hdc);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		for (int i = 0; i < iCount; i++){
			hBrush = CreateSolidBrush(obj[i].color);
			hBrush = (HBRUSH)SelectObject(hdc, hBrush); // ?׸??? ?????뿡 ?귯?? ????
			if (obj[i].iShape == 0){
				Rectangle(hdc, obj[i].x1.x, obj[i].x1.y, obj[i].x2.x, obj[i].x2.y);
			}
			else{
				Ellipse(hdc, obj[i].x1.x, obj[i].x1.y, obj[i].x2.x, obj[i].x2.y);
			}
			DeleteObject(SelectObject(hdc, hBrush));
		}
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}