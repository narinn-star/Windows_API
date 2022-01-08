#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("SendUserMes");

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

#define WM_SORI WM_USER+1
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HWND hWndSori;
	TCHAR* Mes = TEXT("UserMes ���α׷����� �޽����� �����ϴ�");
	switch (iMessage) {
	case WM_LBUTTONDOWN:
		hWndSori = FindWindow(NULL, TEXT("UserMes"));
		//FindWindow() => ���� ������ִ� ������� �� UserMes��� ĸ���� ���� ������ ã�� 
					// -> �ش� ������ �ڵ� ����, ã�� ���ϸ� NULL
		if (hWndSori == NULL) {
			MessageBox(hWnd, TEXT("UserMes ���α׷��� ����Ǿ� ���� �ʽ��ϴ�"),
				TEXT("�̷�����"), MB_OK);
		}
		else {
			SendMessage(hWndSori, WM_SORI, 0, 0);
			//PostMessage(hWndSori, WM_SORI, 0, 0);
			//SendMessage : ������ ���ν��� ȣ��
			//PostMessage : ���� ť�� �־���. �ٷ� ����
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 50, 50, Mes, lstrlen(Mes));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

