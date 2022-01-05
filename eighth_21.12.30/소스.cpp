#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("AnimWin");

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

int pan[3][3];
COLORREF iColor[] = { RGB(255, 255, 255), RGB(255, 0, 0), RGB(0, 255, 0) };

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	HBRUSH hBrush, oldBrush;
	PAINTSTRUCT ps;
	TCHAR *Mes = TEXT("마우스 왼쪽 버튼을 누르시면 애니메이션을 볼 수 있습니다");
	static int xBlock, yBlock, turn = 1;
	int x, y;

	switch (iMessage) {
	case WM_SIZE:
		xBlock = LOWORD(lParam) / 3;
		yBlock = HIWORD(lParam) / 3;
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;

	case WM_LBUTTONDOWN:
		hdc = GetDC(hWnd);
		x = LOWORD(lParam) / xBlock;		//HitTest => 어느 칸을 눌렀는지 알아내기
		y = HIWORD(lParam) / yBlock;

		if (pan[x][y] == 0){
			pan[x][y] = turn;
			turn = (turn == 1 ? 2 : 1);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hBrush = CreateSolidBrush(iColor[0]);
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		for (int i = 0; i < 3; i++){
			for (int j = 0; j < 3; j++){
				Rectangle(hdc, i * xBlock, j * yBlock, (i + 1) * xBlock, (j + 1) * yBlock);

				if (pan[i][j] == 1){
					hBrush = (HBRUSH)CreateSolidBrush(iColor[1]);
					oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
					Ellipse(hdc, i * xBlock, j * yBlock, (i + 1) * xBlock, (j + 1) * yBlock);
					SelectObject(hdc, oldBrush);
					DeleteObject(hBrush);
				}
				else if (pan[i][j] == 2){
					hBrush = (HBRUSH)CreateSolidBrush(iColor[2]);
					oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
					Ellipse(hdc, i * xBlock, j * yBlock, (i + 1) * xBlock, (j + 1) * yBlock);
					SelectObject(hdc, oldBrush);
					DeleteObject(hBrush);
				}

			}
		}
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}