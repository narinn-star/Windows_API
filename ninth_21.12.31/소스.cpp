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

int winpoint(int a){		//a = 1 또는 2 
	//가로 3줄, 세로 3줄, 대각선 2줄
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
	//아무도 이긴 사람이 없다
	else
		return 0;
} 

void init() {									//판을 새로 그린다.
	int i, j;
	turn = 1;									//게임이 연속 될때를 대비해서, turn == 1이면 사용자 turn (무조건 사용자가 시작하도록 함)
	icount = 0;									//나중에 icount == 9일 때, 모든 칸에 색칠이 되었으므로 비김을 표시하기 위한 cnt
	for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++){
		pan[i][j] = 0;							//판 초기화 (모두 색칠되지 않은 판으로 초기화시킴)
	}

	InvalidateRect(hWndMain, NULL, TRUE);		//다시 그리기
}

COLORREF iColor[] = { RGB(255, 255, 255), RGB(255, 0, 0), RGB(0, 255, 0) };

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC					hdc;
	HBRUSH				hBrush, oldBrush;
	PAINTSTRUCT			ps;
	TCHAR				*Mes = TEXT("마우스 왼쪽 버튼을 누르시면 애니메이션을 볼 수 있습니다");
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
		x = LOWORD(lParam) / xBlock;		//HitTest => 어느 칸을 눌렀는지 알아내기
		y = HIWORD(lParam) / yBlock;

		if (pan[x][y] == 0){
			//비어있는 곳에 두면 일단 그린다. 
			pan[x][y] = turn;
			icount++;							//그리고 나서 칸 수 하나 증가시키기
			InvalidateRect(hWnd, NULL, TRUE);	//다시 그리기
			UpdateWindow(hWnd);
		}
		else		//비어있지 않은 곳에 왼쪽 버튼을 클릭했을 때
			return 0;

		//사람승리 체크, 더 이상 둘곳이 없는지 확인	
		if (winpoint(1)){
			wsprintf(buf, TEXT("RED Win.\n   New Game ?"));
			if (MessageBox(hWnd, buf, TEXT("TicTacToe"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				init();
			else
				SendMessage(hWnd, WM_CLOSE, 0L, 0L);
			return 0;
		}

		//비겼을 경우
		else if (icount == 9){
			if (MessageBox(hWnd, TEXT("   Game Over.\n   New Game ?"), TEXT("TicTacToe"),
				MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				init();
			else
				SendMessage(hWnd, WM_CLOSE, 0L, 0L);
			return 0;
		}
		
		while (1) {
			int tx = rand() % 3; // 난수 생성
			int ty = rand() % 3; // 난수 생성
			if (pan[tx][ty] == 0) {
				pan[tx][ty] = 2;
				icount++;
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				break;
			}
		}

		//computer 승리 check
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