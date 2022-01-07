#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("MyTimer");

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

	//�޽��� ����
	while (GetMessage(&Message, NULL, 0, 0)) {		// WM_QUIT�̸� FALSE�� �����Ͽ� while�� Ż�� (WinMain ����)
		TranslateMessage(&Message);
		DispatchMessage(&Message);					//������ ���ν��� ȣ��
	}
	return (int)Message.wParam;
}

RECT rt = { 100, 100 };
SIZE size;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	SYSTEMTIME st;
	static TCHAR sTime[128];
	//static RECT rt = { 100,100,400,120 };

	switch (iMessage) {
	case WM_CREATE:
		SetTimer(hWnd, 1, 1000, NULL);
		SendMessage(hWnd, WM_TIMER, 1, 0);		//������ �����Ǹ� �ٷ� �߻���Ŵ
		return 0;
	case WM_RBUTTONDOWN:
		hdc = GetDC(hWnd);
		//Ellipse(hdc, 30, 200, 60, 300);
		Ellipse(hdc, 60, 60, 200, 300);
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_TIMER:
		GetLocalTime(&st);
		hdc = GetDC(hWnd);
		//wsprintf(sTime, TEXT("���� �ð��� %d:%d:%d�Դϴ�"),
		wsprintf(sTime,TEXT("���� �ð��� %d:%d:%02d �Դϴ�."),
			st.wHour, st.wMinute, st.wSecond);

		//���ڿ� ũ��(���� ����) ��������
		GetTextExtentPoint(hdc, sTime, lstrlen(sTime), &size);
		rt.bottom = 100 + size.cy;  // ���⼭ size�� �ٷ� �� ���ڿ��� ũ���̴�. 
		rt.right = 100 + size.cx;
		//InvalidateRect(hWnd, NULL, TRUE);		//�ٽ� �׸� ���� (NULL : ������ client ���� ��ü, TRUE : ���� ����� �ٽ� �׸���)
		InvalidateRect(hWnd,&rt,TRUE);			//��ȿ������ rt��ŭ ����
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 100, 100, sTime, lstrlen(sTime));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("�����ұ��?"), TEXT("Ȯ��"), MB_YESNO) == IDYES) {
			break;					//DefWindowProc(hWnd, iMessage, wParam, lParam))���� �� -> ����
		}
		else {
			return 0;
		}
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);				//WM_QUIT ( 0: ���ϰ� )
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
