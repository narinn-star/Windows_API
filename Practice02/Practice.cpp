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
COLORREF CurrentColor = RGB(255, 0, 0);  //default  ����
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
	GetClientRect(hwnd, &rect);// ������� ��, �����ϴ� ��
	hBrush = CreateSolidBrush(iColor); //resource.h�� ���� define���� �����Ǿ�����
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
	InvalidateRect(hCtrl, NULL, TRUE);				// Dialog box�� ������ ���ν����� AboutDlgProc�� �ƴ϶� 
	//NULL : hwnd ��ü, TRUE : BackGround Brush�� ��� �о������
	UpdateWindow(hCtrl);								// static control�� �Ⱥ��̴� WndProc���� WM_PAINT ����
	//hCtrl�� WM_PAINT�� ó���ض� (�����츦 �����ض�)
	PaintWindow(hCtrl, iColor, iFigure, Width);				//WndProc - WM_PAINT - PaintWindowȣ�⿡�� �������ڵ� ���� �޶���
	//WndProc : hwnd, PaintTheBlock : hCtrl
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int iRed, iGreen, iBlue;
	static int iShape, iWidth;		//��ȭ���� ���κ���
	static HWND hCtrlBlock;			//�̸����� ������ �ڵ�
	int TempPos;
	switch (iMessage) {

	case WM_INITDIALOG:
		//p.238

		//�̸����� â
		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);
		iShape = iCurrentShape;
		iWidth = iCurrentWidth;
		iRed = GetRValue(CurrentColor);
		iGreen = GetGValue(CurrentColor);
		iBlue = GetBValue(CurrentColor);

		//����Ʈ �ڽ�
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

		//��ũ�� ��

		hScrollRed = GetDlgItem(hDlg, IDC_SCROLLBAR1);
		hScrollGreen = GetDlgItem(hDlg, IDC_SCROLLBAR2);
		hScrollBlue = GetDlgItem(hDlg, IDC_SCROLLBAR3);

		SetScrollRange(hScrollRed, SB_CTL, 0, 255, TRUE);	//0 ���� 255����	(RANGE)
		SetScrollPos(hScrollRed, SB_CTL, iRed, TRUE);		//���� THUMB ��ġ	(THUMB)
		SetScrollRange(hScrollGreen, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hScrollGreen, SB_CTL, iGreen, TRUE);
		SetScrollRange(hScrollBlue, SB_CTL, 0, 255, TRUE);
		SetScrollPos(hScrollBlue, SB_CTL, iBlue, TRUE);

		//Static Text (RGB��)
		SetDlgItemInt(hDlg, IDC_STATIC_RED, iRed, TRUE);
		SetDlgItemInt(hDlg, IDC_STATIC_GREEN, iGreen, TRUE);
		SetDlgItemInt(hDlg, IDC_STATICBLUE, iBlue, TRUE);


		return TRUE;

	case WM_HSCROLL:		//��ũ���� WM �޽����� �߻���Ŵ
		//if ((HWND)lParam == hScrollRed) TempPos = iRed;
		//if ((HWND)lParam == hScrollGreen) TempPos = iGreen;
		//if ((HWND)lParam == hScrollBlue) TempPos = iBlue;
		TempPos = GetScrollPos((HWND)lParam, SB_CTL);

		switch (LOWORD(wParam)) {
		case SB_LINELEFT:				//���� (�ּ�) 0���ٴ� Ŀ����
			TempPos = max(0, TempPos - 1);
			break;
		case SB_LINERIGHT:				//������ (�ִ�)	255���ٴ� �۾ƾ���
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

		PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iShape, iWidth);			//�̸����� â �׸��� (�̸����� â(������), ����ID, ��� ID)

		//wsprintf(str, "%d", Blue);				//SetDlgItemInt�� ���
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
		//��ȭ���ڰ� ���� �׷��� ��, ��ȭ���ڸ� �ٸ� ���ø����̼��� ������ ��  
		// return TRUE �� ���� �Ϳ� ����. WM_PAINT�� �ᱹ �Ʒ��� return FALSE�� ����.
		PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iCurrentShape, iCurrentWidth);		//������ ���� �̸����⿡ ��Ÿ���� ����

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
				InvalidateRect(hWnd, NULL, TRUE);		//OK��ư ������ �ٽ� �׸���.
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
			//�׸�����  -  > Raster Operation
			SetROP2(hdc, R2_NOTXORPEN);//1110 - 1111 -> 0001 -1111 -> 1110 ������ ���·� ���ư���. �ݺ��ҽ�
			//������ �ȼ��� ���� xor������ �̿��Ͽ� ���� ex) 0000�ȼ��� 1111�ȼ��� �׸��� (1111) �� 1111�ȼ��� �׸��� �ٽ�0000�� �ȴ�.
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
			//���� �� �����
			ReleaseDC(hWnd, hdc);
		}
		return 0;
	case WM_LBUTTONUP:
		bNowDraw = FALSE;
		hdc = GetDC(hWnd);
		
		hBrush = CreateSolidBrush(CurrentColor); // ���� �� ����
		hBrush = (HBRUSH)SelectObject(hdc, hBrush); // �׸��� �����뿡 �귯�� ����

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
			hBrush = (HBRUSH)SelectObject(hdc, hBrush); // �׸��� �����뿡 �귯�� ����
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