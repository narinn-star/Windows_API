//스레드의 순서를 제어하는 것은 동일, 시간 종속 관계를 제어 (스레드 간의 순서 관계 제어)
#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("ManualEvent");

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

HANDLE hEvent;
DWORD WINAPI ThreadSend(LPVOID temp)
{
	WaitForSingleObject(hEvent, INFINITE);
	HDC hdc = GetDC(hWndMain);
	TextOut(hdc, 210, 100, TEXT("전송완료"), 8);
	ReleaseDC(hWndMain, hdc);
	return 0;
}

DWORD WINAPI ThreadSave(LPVOID temp)
{
	WaitForSingleObject(hEvent, INFINITE);
	HDC hdc = GetDC(hWndMain);
	TextOut(hdc, 110, 100, TEXT("저장완료"), 8);
	ReleaseDC(hWndMain, hdc);
	return 0;
}

DWORD WINAPI ThreadPrint(LPVOID temp)
{
	WaitForSingleObject(hEvent, INFINITE);
	HDC hdc = GetDC(hWndMain);
	TextOut(hdc, 10, 100, TEXT("인쇄완료"), 8);
	ReleaseDC(hWndMain, hdc);
	return 0;
}

DWORD WINAPI ThreadCalc(LPVOID temp)
{
	HDC hdc = GetDC(hWndMain);
	for (int i = 0; i < 10; i++) {
		TextOut(hdc, 10, 50, TEXT("계산중"), 6);
		GdiFlush();
		Sleep(300);
		TextOut(hdc, 10, 50, TEXT("기다려"), 6);
		GdiFlush();
		Sleep(300);
	}
	TextOut(hdc, 10, 50, TEXT("계산완료"), 8);
	ReleaseDC(hWndMain, hdc);
	PulseEvent(hEvent);		//자동으로 모든 스레드를 다 깨우고 nonsignaled로 전환 (Set, Reset이 필요가 없음)
	//SetEvent(hEvent);  //signaled로 바꾸어줌
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	DWORD ThreadID;
	TCHAR Mes[100];

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); 
		// 두번째 인자 TRUE : 수동 _ signaled < - > nonsignaled 전환이 수동으로 이루어짐
		// 세번째 인자 FALSE : nonsignaled  //nonsignaled event
		// 최초의 nonsignaled 때문에 전송 저장 인쇄 스레드는 signaled가 되기를 기다리고 있음

		// 자동 이벤트
		//hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
		// 자동 이벤트를 사용할 시 전송 저장 인쇄 스레드 중 하나만 깨어남
		// signaled < - > nonsignaled 전환이 자동으로 이루어짐
		return 0;
	case WM_LBUTTONDOWN:
		InvalidateRect(hWnd, NULL, TRUE);
		//ResetEvent(hEvent); //nonsignaled로 바꾸어줌 (수동으로 사용하고있기 때문)
		CloseHandle(CreateThread(NULL, 0, ThreadCalc, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, ThreadPrint, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, ThreadSave, NULL, 0, &ThreadID));
		CloseHandle(CreateThread(NULL, 0, ThreadSend, NULL, 0, &ThreadID));
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		wsprintf(Mes, TEXT("마우스 왼쪽 버튼을 누르면 계산을 시작합니다"));
		TextOut(hdc, 10, 10, Mes, lstrlen(Mes));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		CloseHandle(hEvent);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

