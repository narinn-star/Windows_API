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
COLORREF CurrentColor = RGB(255, 0, 0);  //default  빨강
int iCount;	//점의 개수 저장할 변수
int iCurrentSize = 20;

void PaintWindow(HWND hwnd, int iColor, int iSize)
{
	HBRUSH          hBrush;
	HDC             hdc;
	RECT            rect;

	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);// 좌측상단 점, 우측하단 점
	hBrush = CreateSolidBrush(iColor); //resource.h에 색상 define으로 지정되어있음
	// BLACK : 1000, BLUE : 1001 ...
	hBrush = (HBRUSH)SelectObject(hdc, hBrush);

	Rectangle(hdc, rect.left, rect.top , rect.right + (iCurrentSize), rect.bottom + (iCurrentSize));

	DeleteObject(SelectObject(hdc, hBrush));
	ReleaseDC(hwnd, hdc);
}

void PaintTheBlock(HWND hCtrl, int iColor, int iSize)
{
	InvalidateRect(hCtrl, NULL, TRUE);				// Dialog box의 윈도우 프로스져인 AboutDlgProc이 아니라 
	//NULL : hwnd 전체, TRUE : BackGround Brush로 모두 밀어버리기
	UpdateWindow(hCtrl);								// static control의 안보이는 WndProc으로 WM_PAINT 전달
	//hCtrl의 WM_PAINT를 처리해라 (윈도우를 수정해라)
	PaintWindow(hCtrl, iColor, iSize);				//WndProc - WM_PAINT - PaintWindow호출에서 윈도우핸들 값만 달라짐
	//WndProc : hwnd, PaintTheBlock : hCtrl
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static int iRed, iGreen, iBlue;
	static HWND hCtrlBlock;			//미리보기 윈도우 핸들
	int TempPos, size = 0;
	switch (iMessage) {
	case WM_INITDIALOG:
		//p.238

		//미리보기 창
		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);
		iRed = GetRValue(CurrentColor);
		iGreen = GetGValue(CurrentColor);
		iBlue = GetBValue(CurrentColor);

		//콤보 박스
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
		SetDlgItemInt(hDlg, IDC_STATIC_BLUE, iBlue, TRUE);


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
		SetDlgItemInt(hDlg, IDC_STATIC_BLUE, iBlue, TRUE);

		PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iCurrentSize);			//미리보기 창 그리기 (미리보기 창(윈도우), 색깔ID, 모양 ID)

		//wsprintf(str, "%d", Blue);				//SetDlgItemInt로 사용
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
		//대화상자가 최초 그려질 때, 대화상자를 다른 애플리케이션이 가렸을 때  
		// return TRUE 가 없는 것에 유의. WM_PAINT는 결국 아래의 return FALSE로 간다.
		PaintTheBlock(hCtrlBlock, RGB(iRed, iGreen, iBlue), iCurrentSize);		//없으면 최초 미리보기에 나타나지 않음

		break;
	}
	return FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
//WndProc => Window Precedure
{	//이벤트 핸들러 부분
	HDC hdc;
	PAINTSTRUCT ps;
	int x, y;

	HBRUSH hBrush, OldBrush;	//브러쉬	(OldBrush 대신 hBrush로 모두 작성해도 상관은 없음. 버려지는 것을 삭제 OR 재활용 하는 개념 이해하기

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

	case WM_LBUTTONDOWN:	//사용자가 윈도우에서 왼쪽 버튼을 눌렀을 때
		//좌측 위에 사각형 그리기
		hdc = GetDC(hWnd);

		x = (short)(lParam);  //short : 16bit _ casting - 형변환
		//=> 16bit만 보겠다.
		y = (short)(lParam >> 16);	//shift Operator <- bitwise 연산자 중

		//구조체를 사용했을 때
		p[iCount].x = x;
		p[iCount].y = y;
		p[iCount].color = CurrentColor;
		p[iCount].iSize = iCurrentSize;
		iCount++;

		//모든 그리기 함수는 그리기 도구통을 요구함
		hBrush = CreateSolidBrush(CurrentColor);		//브러쉬 만들고
		SelectObject(hdc, hBrush);		//그리기 도구통에 넣기
		Rectangle(hdc, x - iCurrentSize, y - iCurrentSize, x + iCurrentSize, y + iCurrentSize);			//사각형 그리기 함수 호출
		//Rectangle()은 펜을 꺼내 테두리, 그리고 붓을 꺼내어 내부를 채운다.	default는 검정 테두리, 흰색 내부
		return 0;

	case WM_PAINT:	//화면(Window)을 그려야 할 시점이라고 OS가 알려주는 메시지
		hdc = BeginPaint(hWnd, &ps);

		//다시 화면을 올렸을 때 저장해 둔 배열의 좌표를 다 꺼내어 그려줌
		for (int i = 0; i < iCount; i++){
			//구조체 사용할 때
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
