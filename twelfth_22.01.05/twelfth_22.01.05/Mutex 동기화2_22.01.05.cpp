// �ΰ��� �����带 ����� ����. ������ �μ��� �����Ѵ�.
//�������� �����尡 �����ڿ� �ϳ��� ������ �� �������� ������ ����
#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("SHThread2");
HWND hWndMain;
LONGLONG g_x;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	if (!hPrevInstance) {
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hInstance = hInstance;
		WndClass.lpfnWndProc = (WNDPROC)WndProc;
		WndClass.lpszClassName = lpszClass;
		WndClass.lpszMenuName = NULL;
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&WndClass);
	}
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

HANDLE hMutex;

// �μ��� ���޹��� RECT ������ �簢���� �׸���.
DWORD WINAPI ThreadFunc(LPVOID prc)
{

	POINT p = *(LPPOINT)prc;
	for (int i = p.x; i <= p.y; i++) {
		WaitForSingleObject(hMutex, INFINITE);
		//hMutex�� sinaled�� �Ǹ� hMutex�� nonsignaled�� �ٲٰ� �����ܰ�� �Ѿ
		//hMutex�� nonsignaled�� signaled�� �� ������ ��ٸ��� ����
		g_x += i;		//g_x : sum
		ReleaseMutex(hMutex);
		//�����鼭 �ٽ� ��ȣ���¸� ����� ���� (nonsignaled -> signaled)
	}
	return 0;
}

long FAR PASCAL WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	DWORD ThreadID1, ThreadID2, ThreadID3, ThreadID4;
	static HANDLE hThread[4];
	static LPPOINT prc1, prc2, prc3, prc4;
	TCHAR str[100];
	int i;

	switch (iMessage) {
	case WM_CREATE:
		hMutex = CreateMutex(NULL, FALSE, NULL);			//FALSE : signaled (��ȣ�� �Դ�), TRUE : nonsignaled
		return 0;
	case WM_LBUTTONDOWN:
		g_x = 0;
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);

		prc1 = (LPPOINT)malloc(sizeof(POINT));
		prc1->x = 1;
		prc1->y = 250000;
		hThread[0] = CreateThread(NULL, 0, ThreadFunc, prc1, 0, &ThreadID1);

		prc2 = (LPPOINT)malloc(sizeof(POINT));
		prc2->x = 250001;
		prc2->y = 500000;
		hThread[1] = CreateThread(NULL, 0, ThreadFunc, prc2, 0, &ThreadID2);

		prc3 = (LPPOINT)malloc(sizeof(POINT));
		prc3->x = 500001;
		prc3->y = 750000;
		hThread[2] = CreateThread(NULL, 0, ThreadFunc, prc3, 0, &ThreadID3);

		prc4 = (LPPOINT)malloc(sizeof(POINT));
		prc4->x = 750001;
		prc4->y = 1000000;
		hThread[3] = CreateThread(NULL, 0, ThreadFunc, prc4, 0, &ThreadID4);

		//Sleep(5000);
		WaitForMultipleObjects(4, hThread, TRUE, INFINITE);
		//4 : 4���� �����带, hThread : HANDLE, TRUE : ��� �� ���� ������, INFINITE : ������ ��ٸ�
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		wsprintf(str, TEXT("1~1000000������ �� : %I64u"), g_x);
		TextOut(hdc, 200, 150, str, lstrlen(str));
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		for (i = 0; i < 4; i++)
			CloseHandle(hThread[i]);
		free(prc1);
		free(prc2);
		free(prc3);
		free(prc4);

		PostQuitMessage(0);
		return 0;

	default:
		return(DefWindowProc(hWnd, iMessage, wParam, lParam));
	}
}
