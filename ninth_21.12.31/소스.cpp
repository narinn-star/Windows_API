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
int turn = 1;
int icount;

int winpoint(int a){		//a = 1 �Ǵ� 2 
	//���� 3��, ���� 3��, �밢�� 2��
	if (pan[0][0] == a && pan[1][0] == a && pan[2][0] == a)
		return 1;
	else if (pan[0][1] == a && pan[1][1] == a && pan[2][1] == a)
		return 1;
	else if (pan[0][2] == a && pan[1][2] == a && pan[2][2] == a)
		return 1;
	else if (pan[0][0] == a && pan[0][1] == a && pan[0][2] == a)
		return 1;
	else if (pan[1][0] == a && pan[1][1] == a && pan[1][2] == a)
		return 1;
	else if (pan[2][0] == a && pan[2][1] == a && pan[2][2] == a)
		return 1;
	else if (pan[0][0] == a && pan[1][1] == a && pan[2][2] == a)
		return 1;
	else if (pan[2][0] == a && pan[1][1] == a && pan[0][2] == a)
		return 1;
	//�ƹ��� �̱� ����� ����
	else
		return 0;
} 

void init() {									//���� ���� �׸���.
	int i, j;
	turn = 1;									//������ ���� �ɶ��� ����ؼ�, turn == 1�̸� ����� turn (������ ����ڰ� �����ϵ��� ��)
	icount = 0;									//���߿� icount == 9�� ��, ��� ĭ�� ��ĥ�� �Ǿ����Ƿ� ����� ǥ���ϱ� ���� cnt
	for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++){
		pan[i][j] = 0;							//�� �ʱ�ȭ (��� ��ĥ���� ���� ������ �ʱ�ȭ��Ŵ)
	}

	InvalidateRect(hWndMain, NULL, TRUE);		//�ٽ� �׸���
}

COLORREF iColor[] = { RGB(255, 255, 255), RGB(255, 0, 0), RGB(0, 255, 0) };

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC					hdc;
	HBRUSH				hBrush, oldBrush;
	PAINTSTRUCT			ps;
	TCHAR				*Mes = TEXT("���콺 ���� ��ư�� �����ø� �ִϸ��̼��� �� �� �ֽ��ϴ�");
	TCHAR				buf[128];

	static int xBlock, yBlock;
	int x, y;

	switch (iMessage) {
	case WM_SIZE:
		xBlock = LOWORD(lParam) / 3;
		yBlock = HIWORD(lParam) / 3;
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;

	case WM_LBUTTONDOWN:
		hdc = GetDC(hWnd);
		x = LOWORD(lParam) / xBlock;		//HitTest => ��� ĭ�� �������� �˾Ƴ���
		y = HIWORD(lParam) / yBlock;

		if (pan[x][y] == 0){
			//����ִ� ���� �θ� �ϴ� �׸���. 
			pan[x][y] = turn;
			icount++;							//�׸��� ���� ĭ �� �ϳ� ������Ű��
			InvalidateRect(hWnd, NULL, TRUE);	//�ٽ� �׸���
			UpdateWindow(hWnd);
		}
		else		//������� ���� ���� ���� ��ư�� Ŭ������ ��
			return 0;

		//����¸� üũ, �� �̻� �Ѱ��� ������ Ȯ��	
		if (winpoint(1)){
			wsprintf(buf, TEXT("RED Win.\n   New Game ?"));
			if (MessageBox(hWnd, buf, TEXT("TicTacToe"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				init();
			else
				SendMessage(hWnd, WM_CLOSE, 0L, 0L);
			return 0;
		}

		//����� ���
		else if (icount == 9){
			if (MessageBox(hWnd, TEXT("   Game Over.\n   New Game ?"), TEXT("TicTacToe"),
				MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				init();
			else
				SendMessage(hWnd, WM_CLOSE, 0L, 0L);
			return 0;
		}
		
		while (1) {
			int tx = rand() % 3; // ���� ����
			int ty = rand() % 3; // ���� ����
			if (pan[tx][ty] == 0) {
				pan[tx][ty] = 2;
				icount++;
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				break;
			}
		}

		//computer �¸� check
		if (winpoint(2)){
			wsprintf(buf, TEXT("Green Win.\n   New Game ?"));
			if (MessageBox(hWnd, buf, TEXT("TicTacToe"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				init();
			else
				SendMessage(hWnd, WM_CLOSE, 0L, 0L);
			return 0;
		}

		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hBrush = CreateSolidBrush(iColor[0]);
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		for (int i = 0; i < 3; i++){
			for (int j = 0; j < 3; j++){
				Rectangle(hdc, i * xBlock, j * yBlock, (i + 1) * xBlock, (j + 1) * yBlock);
			}
		}

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (pan[i][j] == 1)
				{
					hBrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
					oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
					Ellipse(hdc, i * xBlock, j * yBlock, (i + 1) * xBlock, (j + 1) * yBlock);
					SelectObject(hdc, oldBrush);
					DeleteObject(hBrush);
				}
				else if (pan[i][j] == 2){
					hBrush = (HBRUSH)CreateSolidBrush(RGB(0, 255, 0));
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