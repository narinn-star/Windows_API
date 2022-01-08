#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("UIThread");
DWORD WINAPI ThreadFunc(LPVOID temp);
LRESULT CALLBACK DeCompProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	//메인 윈도우
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = TEXT("UIThread");
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	//위에 뜨는 윈도우
	//WndClass.cbClsExtra=0; 써도되고 안써도 되고
	//WndClass.cbWndExtra=0; 써도되고 안써도 되고
	WndClass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_CROSS);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = g_hInst;
	WndClass.lpfnWndProc = DeCompProc;
	WndClass.lpszClassName = "DecompWnd";
	//WndClass.lpszMenuName = NULL;					//이미 들어가있기 때문에 필요없음
	//WndClass.style = CS_HREDRAW | CS_VREDRAW;		//이미 들어가있기 때문에 필요없음
	RegisterClass(&WndClass);

	hWnd = CreateWindow(TEXT("UIThread"), lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	hWndMain = hWnd;

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR* Mes = "왼쪽 버튼을 누르면 압축 해제 스레드를 생성합니다.";
	HANDLE hThread;
	DWORD ThreadID;

	switch (iMessage) {
	case WM_LBUTTONDOWN:
		hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &ThreadID);
		CloseHandle(hThread);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 10, 10, Mes, lstrlen(Mes));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

DWORD WINAPI ThreadFunc(LPVOID temp)
{
	HWND hWnd;
	MSG Message;

	hWnd = CreateWindow("DecompWnd", "압축해제중", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 150,
		hWndMain, (HMENU)NULL, g_hInst, NULL);
	ShowWindow(hWnd, SW_SHOW);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK DeCompProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	TCHAR Cap[256];
	int Value;

	switch (iMessage) {
	case WM_CREATE:
		CreateWindow("button", "시작", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			50, 80, 90, 25, hWnd, (HMENU)0, g_hInst, NULL);
		CreateWindow("button", "닫기", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			250, 80, 90, 25, hWnd, (HMENU)1, g_hInst, NULL);
		Value = 0;
		SetProp(hWnd, "VALUE", (HANDLE)Value);
		SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(0, BN_CLICKED), (LPARAM)0); // MAKEWPARAM -> GetDlgItemText(hWnd, 0, Cap, 256);
		//윈도우 생성 시 바로 시작
		return 0;
	case WM_TIMER:
		Value = (int)GetProp(hWnd, "VALUE");
		// 여기에 압축 해제 코드가 있어야 함
		Value++;
		wsprintf(Cap, "압축 해제 중 : %d", Value);
		SetWindowText(hWnd, Cap);
		SetProp(hWnd, "VALUE", (HANDLE)Value);
		if (Value == 100) {
			SetWindowText(hWnd, "압축 해제 완료");
			KillTimer(hWnd, 1);
			EnableWindow(GetDlgItem(hWnd, 0), FALSE);
		}
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0:		//시작, 중지 버튼
			GetDlgItemText(hWnd, 0, Cap, 256);
			if (lstrcmp(Cap, "시작") == 0) {		//strcmp -> 같으면 0 ( 시작일 때 )
				SetDlgItemText(hWnd, 0, "중지");
				SetTimer(hWnd, 1, 200, NULL);
			}
			else {
				SetDlgItemText(hWnd, 0, "시작");
				KillTimer(hWnd, 1);
			}
			break;
		case 1:		//닫기 버튼
			DestroyWindow(hWnd);
			break;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
