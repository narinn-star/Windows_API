#include <windows.h>

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

//���� ����
//Q1. �� ���������θ� �����ؾ��ϴ°�?
//A1. WndProc ����� ������ (Ŭ���� ������) ���� ����Ǿ� �ʱ�ȭ��.
//Q2. ���������� �������� �� iCount=0���� �ʱ�ȭ ���� �ʾƵ� �Ǵ� ����?
//A2. ���������� �˾Ƽ� 0���� �ʱ�ȭ�� (�迭, ���� ���)
/*
int px[1000];	//x��ǥ ������ �迭
int py[1000];	//y��ǥ ������ �迭
int iCount;	// ���� ���� ������ ����
*/

//���� x, y ��ǥ ���� -> ����ü�� �ۼ�
typedef struct tagmyPOINT{
	LONG x;
	LONG y;
} myPoint;

myPoint p[1000];
int iCount;	//���� ���� ������ ����

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
				//WndProc => Window Precedure
{	//�̺�Ʈ �ڵ鷯 �κ�

	HDC hdc;
	PAINTSTRUCT ps;
	int x, y;
	
	/*
	���� ������ �����ϸ� X
	int px[1000];	//x��ǥ ������ �迭		//stack
	int py[1000];	//y��ǥ ������ �迭
	int iCount = 0;	// ���� ���� ������	����
	*/

	switch (iMessage) {
	case WM_CREATE:	//WM => Window Message
		hWndMain = hWnd;
		return 0;

		/*case WM_LBUTTONDOWN:	//����ڰ� �����쿡�� ���� ��ư�� ������ ��
			//���� ���� �簢�� �׸���
			hdc = GetDC(hWnd);
			Rectangle(hdc, 10, 10, 100, 100);	//(10, 10)���� ��� �� (100, 100)������ �ϴ� ��
			//�簢�� �׸��� �Լ� ȣ��
			return 0;*/

	case WM_LBUTTONDOWN:	//����ڰ� �����쿡�� ���� ��ư�� ������ ��
		//���� ���� �簢�� �׸���
		hdc = GetDC(hWnd);	

		//lParam 32��Ʈ�� 16���� ������ Y, X ������ ��ǥ�� ����

		//x = LOWORD(lParam);		//lParam�� ���� 16bit ������ [ ��ũ�� ]

		x = (short)(lParam);  //short : 16bit _ casting - ����ȯ
		//=> 16bit�� ���ڴ�.

		//x = (WORD)(lParam);	//Macro Expansion -- compiler�� ������ ���� �� �ٲٰ� ������ ����

		//y = HIWORD(lParam);		//lParam�� ���� 16bit ������

		y = (short)(lParam >> 16);	//shift Operator <- bitwise ������ ��

		/*
		//�׳� ������ ������� ��
		px[iCount] = x;
		py[iCount] = y;
		iCount++;
		*/

		//����ü�� ������� ��
		p[iCount].x = x;
		p[iCount].y = y;
		iCount++;


		Rectangle(hdc, x - 30, y - 30, x + 30, y + 30);			//�簢�� �׸��� �Լ� ȣ��
		return 0;

	case WM_PAINT:	//ȭ��(Window)�� �׷��� �� �����̶�� OS�� �˷��ִ� �޽���
		hdc = BeginPaint(hWnd, &ps);

		//�ٽ� ȭ���� �÷��� �� ������ �� �迭�� ��ǥ�� �� ������ �׷���
		for (int i = 0; i < iCount; i++){
			//���� ����� ��
			//Rectangle(hdc, px[i] - 30, py[i] - 30, px[i] + 30, py[i] + 30);
			//����ü ����� ��
			Rectangle(hdc, p[i].x - 30, p[i].y - 30, p[i].x + 30, p[i].y + 30);
		}

		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
