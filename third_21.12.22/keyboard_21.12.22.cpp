//#define UNICODE
#define __UNICODE		//c �Լ���

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

//���� : ����� ������ ����  <--- p.20, line : -12

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static int x = 100;		//static : ���� ����	(��������ó�� ����) - ������ Ŭ����, �Լ� �������� ��������
	static int y = 100;

	//static TCHAR *c = TEXT("A");
	static TCHAR c[] = TEXT("��ab");		//Neutral Codes
	//static char c[] = "��ab";				//��Ƽ����Ʈ MBCS
	//static wchar_t c[] = L"��ab";			//�����ڵ� UNICODE
	static BOOL key = TRUE;

	switch (iMessage) {
	case WM_KEYDOWN:
		switch (wParam) { 
		case VK_LEFT:	//0x25 (0x => 16����)	VK : ���� Ű �ڵ� (p.85)
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
			//key ? _tcscpy(c, TEXT("��ab")) : _tcscpy(c, TEXT("��ab"));
			//key ? wcscpy(c, TEXT("��ab")) : _tcscpy(c, TEXT("��ab"));
			key ? strcpy(c, TEXT("��ab")) : _tcscpy(c, TEXT("��ab"));
			
			//static TCHAR *c = TEXT("A");
			//if (key)
			//	c = TEXT("A");
			//else if (!key)
			//	c = TEXT("#");
			break;
		}
		//ȭ���� �ٽ� �׷��� �� ��찡 �߻����� ��
		//OS���� �� ������(hWnd)�� WM_PAINT �߻���Ű���� �䱸
		//== OS���� �� ������(hWnd)�� �ٽ� �׷��� �� ������ ����ٰ� ����
		//InvalidateRect(hWnd,NULL,TRUE);
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, x, y, c, strlen(c));	//x, y ��ġ�� "A"���ڸ� 1�� ���
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
