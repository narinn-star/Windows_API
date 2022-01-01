#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("�簢�� �ٷ��");

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
void PaintTheBlock(HWND hCtrl, int iColor, int iFigure);//�̸�����
COLORREF iCurrentColor = RGB(255, 0, 0);
TCHAR *Items[] = { TEXT("10"), TEXT("30"), TEXT("50") };
HWND hList;
HWND hList2;

int mSelect = 10;

int save = 1003;
int save2 = 1008;
int Graph = 0; //���

BOOL CALLBACK InfoDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int iRed, iGreen, iBlue;
	static HWND hCtrlBlock; //�̸�����
	switch (iMessage) {
	case WM_INITDIALOG:

		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT); //�̸�����

		iRed = GetRValue(iCurrentColor);
		iGreen = GetGValue(iCurrentColor);
		iBlue = GetBValue(iCurrentColor);

		//�̸�����
		hList = GetDlgItem(hDlg, IDC_LIST1);
		for (int i = 0; i < 3; i++) {
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)Items[i]);
		}
		SendMessage(hList, LB_SETCURSEL, (mSelect - 10) / 10, 0);
		CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO4, IDC_RADIO1);
		CheckRadioButton(hDlg, IDC_RADIO5, IDC_RADIO7, IDC_RADIO5);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_RADIO1:
			PaintTheBlock(hCtrlBlock, RGB(255, 0, 0), mSelect);
			return TRUE;
		case IDC_RADIO2:
			PaintTheBlock(hCtrlBlock, RGB(0, 255, 0), mSelect);
			return TRUE;
		case IDC_RADIO3:
			PaintTheBlock(hCtrlBlock, RGB(0, 0, 255), mSelect);
			return TRUE;
		case IDC_RADIO4:
			PaintTheBlock(hCtrlBlock, RGB(255, 255, 0), mSelect);
			return TRUE;
		case IDC_LIST1:
			mSelect = 10 + 10 * SendMessage(hList, LB_GETCURSEL, 0, 0);
			PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), mSelect);
			return TRUE;

		case IDC_RADIO5:
			Graph = 0;
			break;
		case IDC_RADIO6:
			Graph = 1;
			break;
		case IDC_RADIO7:
			Graph = 2;
			break;
			//���

		case IDOK:
			mSelect = 10 + 10 * SendMessage(hList, LB_GETCURSEL, 0, 0);
			iCurrentColor = RGB(iRed, iGreen, iBlue);

			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
			break;
		}

	case WM_PAINT:
		PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), mSelect);//�̸�����
		break;
	}

	return FALSE;
}

typedef struct _line {
	POINT LP[1000];
	POINT P;
	int iWidth;
	COLORREF iColor;
	int iSize;
	int iGraph;
	int iCount;
}line;

line lines[500];
int iCurrentWidth;
int iCurrentSize;
int iLines;
int iTempCount;

HBRUSH hBrush, oldBrush;
HPEN hPen, oldPen;
HWND hwnd;
HDC hdc;

void PaintWindow(HWND hwnd, int iColora, int iFigure)
{
	HDC hdc;
	PAINTSTRUCT ps;

	hdc = GetDC(hwnd);

	iFigure = iFigure * 2;
	hBrush = CreateSolidBrush(iColora);
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	switch (Graph) {
	case 0:
		Rectangle(hdc, 5, 5, iFigure, iFigure);
		break;
	case 1:
		Ellipse(hdc, 5, 5, iFigure, iFigure);
		break;
	case 2:
		hPen = CreatePen(PS_SOLID, iFigure / 10, iColora);
		hPen = (HPEN)SelectObject(hdc, hPen);

		MoveToEx(hdc, 20, 20, NULL);
		LineTo(hdc, 80, 80);
		break;
	}
	SelectObject(hdc, oldBrush);
	DeleteObject(hBrush);

	EndPaint(hwnd, &ps);
}
//�̸�����

void PaintTheBlock(HWND hCtrl, int iColorf, int iFigure)
{
	InvalidateRect(hCtrl, NULL, TRUE); // Dialog box�� ������ ���ν����� AboutDlgProc�� �ƴ϶� 
	UpdateWindow(hCtrl);               // static control�� ������ WndProc���� WM_PAINT ����
	PaintWindow(hCtrl, iColorf, iFigure);
}
//�̸�����



LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{				//Window Procedure
	HDC hdc;
	PAINTSTRUCT ps;
	static int x;
	static int y;

	switch (iMessage) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_40008:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1),
				hWnd, InfoDlgProc);
			break;
		}
		return 0;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_BACK: //������ �簢�� �����
			iLines--;
			InvalidateRect(hWnd, NULL, TRUE);
			//OS�� hWnd���� ��ȿ����(�ٽñ׷��� �� ����)�� �߻�����...
			//�˾Ҵ�. �ش� ������� WM_PAINT Message�� �����ش�...
			break;
		}
		//InvalidateRect(hWnd, NULL, FALSE);

		return 0;
	case WM_CREATE:
		hWndMain = hWnd;
		return 0;

	case WM_LBUTTONDOWN:
		hdc = GetDC(hWnd);
		//��� �׸��� �Լ��� �׸��� �������� �䱸�Ѵ�.
		x = LOWORD(lParam);
		y = HIWORD(lParam);

		lines[iLines].iWidth = mSelect / 10;
		lines[iLines].P.x = x;
		lines[iLines].P.y = y;
		lines[iLines].iSize = mSelect;
		lines[iLines].iGraph = Graph;
		lines[iLines].iColor = iCurrentColor;
		if (Graph != 2)
			iLines++;
		switch (Graph) {
		case 0:
			hBrush = CreateSolidBrush(iCurrentColor);
			hBrush = (HBRUSH)SelectObject(hdc, hBrush);
			Rectangle(hdc, x - mSelect, y - mSelect, x + mSelect, y + mSelect);
			break;
		case 1:
			hBrush = CreateSolidBrush(iCurrentColor);
			hBrush = (HBRUSH)SelectObject(hdc, hBrush);
			Ellipse(hdc, x - mSelect, y - mSelect, x + mSelect, y + mSelect);
			break;
		case 2:
			lines[iLines].LP[iTempCount].x = x;
			lines[iLines].LP[iTempCount].y = y;
			iTempCount++;
			break;


		}
		return 0;
		//�ٽ� �׸��� å��(ȭ���籸��) �� ���α׷���
		//os�� �ٽ� �׸� ������ �˷��ش�

	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON) {
			if (Graph == 2)
			{
				hdc = GetDC(hWnd);
				hPen = CreatePen(PS_SOLID, mSelect / 10, iCurrentColor);
				oldPen = (HPEN)SelectObject(hdc, hPen);
				SelectObject(hdc, hPen);
				MoveToEx(hdc, x, y, NULL);
				x = LOWORD(lParam);
				y = HIWORD(lParam);
				LineTo(hdc, x, y);
				lines[iLines].LP[iTempCount].x = x;
				lines[iLines].LP[iTempCount].y = y;
				iTempCount++;

				ReleaseDC(hWnd, hdc);
			hPen = (HPEN)SelectObject(hdc, oldPen);
			DeleteObject(hPen);
			}
		}
		return 0;
	case WM_LBUTTONUP:
		if (Graph == 2)
		{
			lines[iLines].iCount = iTempCount;
			lines[iLines].iColor = iCurrentColor;
			iLines++;
			iTempCount = 0;
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		for (int i = 0; i < iLines; i++)
		{
			hBrush = CreateSolidBrush(lines[i].iColor);
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

			switch (lines[i].iGraph) {
			case 0:
				Rectangle(hdc, lines[i].P.x - lines[i].iSize, lines[i].P.y - lines[i].iSize, lines[i].P.x + lines[i].iSize, lines[i].P.y + lines[i].iSize);
				break;
			case 1:
				Ellipse(hdc, lines[i].P.x - lines[i].iSize, lines[i].P.y - lines[i].iSize, lines[i].P.x + lines[i].iSize, lines[i].P.y + lines[i].iSize);
				break;
			case 2:
				hPen = CreatePen(PS_SOLID, lines[i].iWidth, lines[i].iColor);
				oldPen = (HPEN)SelectObject(hdc, hPen);
				MoveToEx(hdc, lines[i].LP[0].x, lines[i].LP[0].y, NULL);
				for (int j = 0; j < lines[i].iCount; j++)
				{

					//MoveToEx(hdc, lines[i].p[j - 1].x, 
					//	lines[i].p[j - 1].y, NULL);
					LineTo(hdc, lines[i].LP[j].x,
						lines[i].LP[j].y);

				}
				SelectObject(hdc, oldPen);
				DeleteObject(hPen);
				break;
			}
			SelectObject(hdc, oldBrush);
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


//