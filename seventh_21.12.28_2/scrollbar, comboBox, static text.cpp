#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("Class");

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
//*

HWND hCombo, hScrollRed, hScrollGreen, hScrollBlue;
HWND hStaticRed, hStaticGreen, hStaticBlue;
TCHAR *Items[] = { TEXT("20"), TEXT("40"), TEXT("60")};

typedef struct tagmyPOINT{
	LONG x;
	LONG y;
	int iSize = 0; 
	COLORREF color;
} myPoint;

myPoint p[1000];
COLORREF CurrentColor = RGB(255, 0, 0);  //default  ����
int iCount;	//���� ���� ������ ����
int iCurrentSize = 20;

void PaintWindow(HWND hwnd, int iColor, int iSize)
{
	HBRUSH          hBrush;
	HDC             hdc;
	RECT            rect;

	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);// ������� ��, �����ϴ� ��
	hBrush = CreateSolidBrush(iColor); //resource.h�� ���� define���� �����Ǿ�����
	// BLACK : 1000, BLUE : 1001 ...
	hBrush = (HBRUSH)SelectObject(hdc, hBrush);

	Rectangle(hdc, rect.left, rect.top , rect.right + (iCurrentSize), rect.bottom + (iCurrentSize));

	DeleteObject(SelectObject(hdc, hBrush));
	ReleaseDC(hwnd, hdc);
}

void PaintTheBlock(HWND hCtrl, int iColor, int iSize)
{
	InvalidateRect(hCtrl, NULL, TRUE);				// Dialog box�� ������ ���ν����� AboutDlgProc�� �ƴ϶� 
	//NULL : hwnd ��ü, TRUE : BackGround Brush�� ��� �о������
	UpdateWindow(hCtrl);								// static control�� �Ⱥ��̴� WndProc���� WM_PAINT ����
	//hCtrl�� WM_PAINT�� ó���ض� (�����츦 �����ض�)
	PaintWindow(hCtrl, iColor, iSize);				//WndProc - WM_PAINT - PaintWindowȣ�⿡�� �������ڵ� ���� �޶���
	//WndProc : hwnd, PaintTheBlock : hCtrl
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int iRed, iGreen, iBlue;
	static HWND hCtrlBlock;			//�̸����� ������ �ڵ�
	int TempPos, size = 0;
	switch (iMessage) {
	case WM_INITDIALOG:
		//p.238

		//�̸����� â
		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);
		iRed = GetRValue(CurrentColor);
		iGreen = GetGValue(CurrentColor);
		iBlue = GetBValue(CurrentColor);

		//�޺� �ڽ�
		hCombo = GetDlgItem(hDlg, IDC_COMBO1);
		for (int i = 0; i<3; i++) {
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)Items[i]);
		}

		if (iCurrentSize == 20)
			size = 0;
		else if (iCurrentSize == 40)
			size = 1;
		else if (iCurrentSize == 60)
			size = 2;
		

		SendMessage(hCombo, CB_SETCURSEL, size, 0);

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
		SetDlgItemInt(hDlg, IDC_STATIC_BLUE, iBlue, TRUE);


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
		SetDlgItemInt(hDlg, IDC_STATIC_BLUE, iBlue, TRUE);

		PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iCurrentSize);			//�̸����� â �׸��� (�̸����� â(������), ����ID, ��� ID)

		//wsprintf(str, "%d", Blue);				//SetDlgItemInt�� ���
		//SetWindowText(hSBlue, str);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			CurrentColor = RGB(iRed, iGreen, iBlue);
			size = SendMessage(hCombo, CB_GETCURSEL, 0, 0);

			if (size == 0)
				iCurrentSize = 20;
			else if (size == 1)
				iCurrentSize = 40;
			else if (size == 2)
				iCurrentSize = 60;

			EndDialog(hDlg, IDOK);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		break;

	case WM_PAINT:
		//��ȭ���ڰ� ���� �׷��� ��, ��ȭ���ڸ� �ٸ� ���ø����̼��� ������ ��  
		// return TRUE �� ���� �Ϳ� ����. WM_PAINT�� �ᱹ �Ʒ��� return FALSE�� ����.
		PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iCurrentSize);		//������ ���� �̸����⿡ ��Ÿ���� ����

		break;
	}
	return FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
//WndProc => Window Precedure
{	//�̺�Ʈ �ڵ鷯 �κ�
	HDC hdc;
	PAINTSTRUCT ps;
	int x, y;

	HBRUSH hBrush, OldBrush;	//�귯��	(OldBrush ��� hBrush�� ��� �ۼ��ص� ����� ����. �������� ���� ���� OR ��Ȱ�� �ϴ� ���� �����ϱ�

	switch (iMessage) {
	case WM_CREATE:	//WM => Window Message
		hWndMain = hWnd;
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDG_DIALOG:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1),
				hWnd, DlgProc);
			break;
		}
		return 0;

	case WM_LBUTTONDOWN:	//����ڰ� �����쿡�� ���� ��ư�� ������ ��
		//���� ���� �簢�� �׸���
		hdc = GetDC(hWnd);

		x = (short)(lParam);  //short : 16bit _ casting - ����ȯ
		//=> 16bit�� ���ڴ�.
		y = (short)(lParam >> 16);	//shift Operator <- bitwise ������ ��

		//����ü�� ������� ��
		p[iCount].x = x;
		p[iCount].y = y;
		p[iCount].color = CurrentColor;
		p[iCount].iSize = iCurrentSize;
		iCount++;

		//��� �׸��� �Լ��� �׸��� �������� �䱸��
		hBrush = CreateSolidBrush(CurrentColor);		//�귯�� �����
		SelectObject(hdc, hBrush);		//�׸��� �����뿡 �ֱ�
		Rectangle(hdc, x - iCurrentSize, y - iCurrentSize, x + iCurrentSize, y + iCurrentSize);			//�簢�� �׸��� �Լ� ȣ��
		//Rectangle()�� ���� ���� �׵θ�, �׸��� ���� ������ ���θ� ä���.	default�� ���� �׵θ�, ��� ����
		return 0;

	case WM_PAINT:	//ȭ��(Window)�� �׷��� �� �����̶�� OS�� �˷��ִ� �޽���
		hdc = BeginPaint(hWnd, &ps);

		//�ٽ� ȭ���� �÷��� �� ������ �� �迭�� ��ǥ�� �� ������ �׷���
		for (int i = 0; i < iCount; i++){
			//����ü ����� ��
			hBrush = CreateSolidBrush(p[i].color);
			OldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			Rectangle(hdc, p[i].x - iCurrentSize, p[i].y - iCurrentSize, p[i].x + iCurrentSize, p[i].y + iCurrentSize);
			SelectObject(hdc, OldBrush);
			DeleteObject(hBrush);
		}

		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
