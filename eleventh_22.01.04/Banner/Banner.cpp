#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("Banner");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
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

// ������� �Ѱ��� �μ� ����ü
struct ThreadParam {
	int x, y, w, h;
	TCHAR* Mes[3];
	int interval;
} Param[3] = {
	{10,10,180,50,"���� �� ����","��ʰ� ���ÿ�","����ǰ� �ֽ��ϴ�",100},
	{210,10,180,50,"���� ��ġ��","���ڿ� �ֱⰡ","�ٸ��ϴ�",500},
	{410,10,180,50,"���α�������","���⿡ ���� �ͼ�","��������",1000}
};

// ��ư���� ID
#define ID_R1 101
#define ID_R2 102
#define ID_R3 103
#define ID_PAUSE 104
#define ID_RUN 105

// ������ ���� �Լ�-��ʸ� �ֱ������� ��ü�Ѵ�.
DWORD WINAPI ThreadFunc(LPVOID Param)
{
	HDC hdc;
	ThreadParam* P = (ThreadParam*)Param;
	int Idx = 0;
	hdc = GetDC(hWndMain);
	for (;;) {
		Rectangle(hdc, P->x, P->y, P->x + P->w, P->y + P->h);
		TextOut(hdc, P->x + 5, P->y + 5, P->Mes[Idx % 3], lstrlen(P->Mes[Idx % 3]));
		GdiFlush();
		Sleep(P->interval);
		Idx++;
	}
	ReleaseDC(hWndMain, hdc);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static HANDLE hThread[3];
	static DWORD ThreadID[3];
	static int NowThread = 0;
	int i;

	switch (iMessage) {
	case WM_CREATE:
		// ������ ��� ���� ��ư���� �����Ѵ�.
		hWndMain = hWnd;
		CreateWindow("button", "����", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			200, 100, 80, 25, hWnd, (HMENU)ID_PAUSE, g_hInst, NULL);
		CreateWindow("button", "����", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			200, 130, 80, 25, hWnd, (HMENU)ID_RUN, g_hInst, NULL);
		CreateWindow("button", "Thread0", WS_CHILD | WS_VISIBLE |
			BS_AUTORADIOBUTTON | WS_GROUP,
			100, 100, 100, 30, hWnd, (HMENU)ID_R1, g_hInst, NULL);
		CreateWindow("button", "Thread1", WS_CHILD | WS_VISIBLE |
			BS_AUTORADIOBUTTON,
			100, 120, 100, 30, hWnd, (HMENU)ID_R2, g_hInst, NULL);
		CreateWindow("button", "Thread2", WS_CHILD | WS_VISIBLE |
			BS_AUTORADIOBUTTON,
			100, 140, 100, 30, hWnd, (HMENU)ID_R3, g_hInst, NULL);
		CheckRadioButton(hWnd, ID_R1, ID_R3, ID_R1);

		// �� ���� �����带 �����.
		for (i = 0; i < 3; i++) {
			hThread[i] = CreateThread(NULL, 0, ThreadFunc, &Param[i], 0, &ThreadID[i]);
		}
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_R1:
			NowThread = 0;
			break;
		case ID_R2:
			NowThread = 1;
			break;
		case ID_R3:
			NowThread = 2;
			break;
		case ID_PAUSE:
			//���� ������
			SuspendThread(hThread[NowThread]);	//block ���� (����)
			break;
		case ID_RUN:
			//������ �簳
			ResumeThread(hThread[NowThread]);	//ready ����
			break;
		}
		return 0;
	case WM_DESTROY:
		for (i = 0; i < 3; i++) {
			CloseHandle(hThread[i]);
		}
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

