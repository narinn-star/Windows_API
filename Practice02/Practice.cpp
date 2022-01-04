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
	POINT L;
	COLORREF color;
	int iShape = 0;
	int iWidth;
	int iCnt;
	POINT LP[1000];
} myObject;

myObject obj[1000];
COLORREF CurrentColor = RGB(255, 0, 0);  //default  빨강
int iCount = 0;
int iCurrentShape = 0;
int iCurrentWidth = 0;
//*

HWND hList, hCombo, hScrollRed, hScrollGreen, hScrollBlue;
HWND hStaticRed, hStaticGreen, hStaticBlue;
TCHAR *Items[] = { TEXT("Rectangle"), TEXT("Ellipse"), TEXT("Line") };
TCHAR *Width[] = { TEXT("2"), TEXT("5"), TEXT("8") };

void PaintWindow(HWND hwnd, int iColor, int iFigure, int iWidth)
{
	HPEN			hPen;
	HBRUSH          hBrush;
	HDC             hdc;
	RECT            rect;

	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);// 좌측상단 점, 우측하단 점
	hBrush = CreateSolidBrush(iColor); //resource.h에 색상 define으로 지정되어있음
	// BLACK : 1000, BLUE : 1001 ...
	hBrush = (HBRUSH)SelectObject(hdc, hBrush);

	if (iFigure == 0)
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	else  if (iFigure == 1)
		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
	else if (iFigure == 2){
		hPen = CreatePen(PS_SOLID, iWidth, iColor);
		hPen = (HPEN)SelectObject(hdc, hPen);

		MoveToEx(hdc, 20, 20, NULL);
		LineTo(hdc, 80, 80);
	}

	DeleteObject(SelectObject(hdc, hBrush));
	ReleaseDC(hwnd, hdc);
}

void PaintTheBlock(HWND hCtrl, int iColor, int iFigure, int Width)
{
	InvalidateRect(hCtrl, NULL, TRUE);				// Dialog box의 윈도우 프로스져인 AboutDlgProc이 아니라 
	//NULL : hwnd 전체, TRUE : BackGround Brush로 모두 밀어버리기
	UpdateWindow(hCtrl);								// static control의 안보이는 WndProc으로 WM_PAINT 전달
	//hCtrl의 WM_PAINT를 처리해라 (윈도우를 수정해라)
	PaintWindow(hCtrl, iColor, iFigure, Width);				//WndProc - WM_PAINT - PaintWindow호출에서 윈도우핸들 값만 달라짐
	//WndProc : hwnd, PaintTheBlock : hCtrl
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int iRed, iGreen, iBlue;
	static int iShape, iWidth;		//대화상자 내부변수
	static HWND hCtrlBlock;			//미리보기 윈도우 핸들
	int TempPos;
	switch (iMessage) {

	case WM_INITDIALOG:
		//p.238

		//미리보기 창
		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);
		iShape = iCurrentShape;
		iWidth = iCurrentWidth;
		iRed = GetRValue(CurrentColor);
		iGreen = GetGValue(CurrentColor);
		iBlue = GetBValue(CurrentColor);

		//리스트 박스
		hList = GetDlgItem(hDlg, IDC_LIST1);
		for (int i = 0; i < 3; i++) {
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)Items[i]);
		}
		SendMessage(hList, LB_SETCURSEL, iShape, 0);

		hCombo = GetDlgItem(hDlg, IDC_COMBO1);
		for (int i = 0; i < 3; i++){
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)Width[i]);
		}
		if (iCurrentWidth == 2)
			iWidth = 0;
		else if (iCurrentWidth == 5)
			iWidth = 1;
		else if (iCurrentWidth == 8)
			iWidth = 2;
		SendMessage(hCombo, CB_SETCURSEL, iWidth, 0);

		//스크롤 바

		hScrollRed = GetDlgItem(hDlg, IDC_SCROLLBAR1);
		hScrollGreen = GetDlgItem(hDlg, IDC_SCROLLBAR2);
		hScrollBlue = GetDlgItem(hDlg, IDC_SCROLLBAR3);

		SetScrollRange(hScrollRed, SB_CTL, 0, 255, TRUE);	//0 부터 255까지	(RANGE)
		SetScrollPos(hScrollRed, SB_CTL, iRed, TRUE);		//최초 THUMB 위치	(THUMB)
		SetScrollRange(hScrollGreen, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hScrollGreen, SB_CTL, iGreen, TRUE);
		SetScrollRange(hScrollBlue, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hScrollBlue, SB_CTL, iBlue, TRUE);

		//Static Text (RGB값)
		SetDlgItemInt(hDlg, IDC_STATIC_RED, iRed, TRUE);
		SetDlgItemInt(hDlg, IDC_STATIC_GREEN, iGreen, TRUE);
		SetDlgItemInt(hDlg, IDC_STATICBLUE, iBlue, TRUE);


		return TRUE;

	case WM_HSCROLL:		//스크롤은 WM 메시지를 발생시킴
		//if ((HWND)lParam == hScrollRed) TempPos = iRed;
		//if ((HWND)lParam == hScrollGreen) TempPos = iGreen;
		//if ((HWND)lParam == hScrollBlue) TempPos = iBlue;
		TempPos = GetScrollPos((HWND)lParam, SB_CTL);

		switch (LOWORD(wParam)) {
		case SB_LINELEFT:				//왼쪽 (최소) 0보다는 커야함
			TempPos = max(0, TempPos - 1);
			break;
		case SB_LINERIGHT:				//오른쪽 (최대)	255보다는 작아야함
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
		SetDlgItemInt(hDlg, IDC_STATICBLUE, iBlue, TRUE);

		PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iShape, iWidth);			//미리보기 창 그리기 (미리보기 창(윈도우), 색깔ID, 모양 ID)

		//wsprintf(str, "%d", Blue);				//SetDlgItemInt로 사용
		//SetWindowText(hSBlue, str);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			iCurrentShape = iShape;
			CurrentColor = RGB(iRed, iGreen, iBlue);
			iWidth = SendMessage(hCombo, CB_GETCURSEL, 0, 0);

			if (iWidth == 0)
				iCurrentWidth = 2;
			else if (iWidth == 1)
				iCurrentWidth = 5;
			else if (iWidth == 2)
				iCurrentWidth = 8;

			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case IDC_LIST1:
			iShape = SendMessage(hList, LB_GETCURSEL, 0, 0);
			PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iShape, iWidth);
			break;
		}
		break;
	case WM_PAINT:
		//대화상자가 최초 그려질 때, 대화상자를 다른 애플리케이션이 가렸을 때  
		// return TRUE 가 없는 것에 유의. WM_PAINT는 결국 아래의 return FALSE로 간다.
		PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iCurrentShape, iCurrentWidth);		//없으면 최초 미리보기에 나타나지 않음

		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int sx, sy, oldx, oldy;
	static BOOL bNowDraw = FALSE;
	HDC hdc;
	HPEN hPen;
	HBRUSH hBrush;
	PAINTSTRUCT ps;
	TCHAR str[128];
	switch (iMessage) {
	case WM_CREATE:
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDG_DIALOG:
			if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc))
				InvalidateRect(hWnd, NULL, TRUE);		//OK버튼 누르면 다시 그린다.
			break;
		}
		return 0;

	case WM_LBUTTONDOWN:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		oldx = sx;
		oldy = sy;

		obj[iCount].L.x = sx;
		obj[iCount].L.y = sy;

		bNowDraw = TRUE;
		return 0;
	case WM_MOUSEMOVE:
		if (bNowDraw) {
			hdc = GetDC(hWnd);
			//그리기모드  -  > Raster Operation
			SetROP2(hdc, R2_NOTXORPEN);//1110 - 1111 -> 0001 -1111 -> 1110 원래의 상태로 돌아간다. 반복할시
			//바탕과 픽셀의 색을 xor연산을 이용하여 결정 ex) 0000픽셀에 1111픽셀을 그리고 (1111) 또 1111픽셀을 그리면 다시0000이 된다.
			//hPen = CreatePen(PS_SOLID, 5, RGB(0, 255, 255));
			hBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
			//hPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
			if (iCurrentShape == 0){
				Rectangle(hdc, sx, sy, oldx, oldy);
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);
				Rectangle(hdc, sx, sy, oldx, oldy);
			}
			else if(iCurrentShape == 1){
				Ellipse(hdc, sx, sy, oldx, oldy);
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);
				Ellipse(hdc, sx, sy, oldx, oldy);
			}
			else if (iCurrentShape == 2){
				hPen = CreatePen(PS_SOLID, iCurrentWidth, CurrentColor);
				hPen = (HPEN)SelectObject(hdc, hPen);
				MoveToEx(hdc, sx, sy, NULL);
				LineTo(hdc, oldx, oldy);
				oldx = LOWORD(lParam);
				oldy = HIWORD(lParam);
				MoveToEx(hdc, sx, sy, NULL);
				LineTo(hdc, oldx, oldy);
			}
			//이전 선 지우기
			ReleaseDC(hWnd, hdc);
		}
		return 0;
	case WM_LBUTTONUP:
		bNowDraw = FALSE;
		hdc = GetDC(hWnd);
		
		hBrush = CreateSolidBrush(CurrentColor); // 빨간 붓 선언
		hBrush = (HBRUSH)SelectObject(hdc, hBrush); // 그리기 도구통에 브러쉬 넣음

		if (iCurrentShape == 0){
			Rectangle(hdc, sx, sy, oldx, oldy);
		}
		else if (iCurrentShape == 1){
			Ellipse(hdc, sx, sy, oldx, oldy);
		}
		else if (iCurrentShape == 2){
			hPen = CreatePen(PS_SOLID, iCurrentWidth, CurrentColor);
			hPen = (HPEN)SelectObject(hdc, hPen);
			MoveToEx(hdc, sx, sy, NULL);
			
			LineTo(hdc, oldx, oldy);
			obj[iCount].L.x = sx;
			obj[iCount].L.y = sy;
		}
		obj[iCount].x1.x = sx;
		obj[iCount].x1.y = sy;
		obj[iCount].x2.x = oldx;
		obj[iCount].x2.y = oldy;
		obj[iCount].color = CurrentColor;
		obj[iCount].iShape = iCurrentShape;
		obj[iCount].iWidth = iCurrentWidth;
		iCount++;
		
		DeleteObject(SelectObject(hdc, hBrush));
		ReleaseDC(hWnd, hdc);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		for (int i = 0; i < iCount; i++){
			hBrush = CreateSolidBrush(obj[i].color);
			hBrush = (HBRUSH)SelectObject(hdc, hBrush); // 그리기 도구통에 브러쉬 넣음
			if (obj[i].iShape == 0){
				Rectangle(hdc, obj[i].x1.x, obj[i].x1.y, obj[i].x2.x, obj[i].x2.y);
			}
			else if (obj[i].iShape == 1){
				Ellipse(hdc, obj[i].x1.x, obj[i].x1.y, obj[i].x2.x, obj[i].x2.y);
			}
			else if (obj[i].iShape == 2){
				hPen = CreatePen(PS_SOLID, obj[i].iWidth, obj[i].color);
				hPen = (HPEN)SelectObject(hdc, hPen);
				MoveToEx(hdc, obj[i].LP[0].x, obj[i].LP[0].y, NULL);
				for (int j = 0; j < obj[i].iCnt; j++){
					LineTo(hdc, obj[i].LP[j].x, obj[i].LP[j].y);
				}
				DeleteObject(SelectObject(hdc, hPen));
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