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

	//메시지 루프
	while (GetMessage(&Message, NULL, 0, 0)) {		// WM_QUIT이면 FALSE를 리턴하여 while문 탈출 (WinMain 종료)
		TranslateMessage(&Message);
		DispatchMessage(&Message);					//윈도우 프로시저 호출
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
		SendMessage(hWnd, WM_TIMER, 1, 0);		//윈도우 생성되면 바로 발생시킴
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
		//wsprintf(sTime, TEXT("지금 시간은 %d:%d:%d입니다"),
		wsprintf(sTime,TEXT("지금 시간은 %d:%d:%02d 입니다."),
			st.wHour, st.wMinute, st.wSecond);

		//문자열 크기(폭과 높이) 가져오기
		GetTextExtentPoint(hdc, sTime, lstrlen(sTime), &size);
		rt.bottom = 100 + size.cy;  // 여기서 size는 바로 앞 문자열의 크기이다. 
		rt.right = 100 + size.cx;
		//InvalidateRect(hWnd, NULL, TRUE);		//다시 그릴 영역 (NULL : 윈도우 client 영역 전체, TRUE : 전부 지우고 다시 그리기)
		InvalidateRect(hWnd,&rt,TRUE);			//무효영역을 rt만큼 지정
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 100, 100, sTime, lstrlen(sTime));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("종료할까요?"), TEXT("확인"), MB_YESNO) == IDYES) {
			break;					//DefWindowProc(hWnd, iMessage, wParam, lParam))으로 감 -> 종료
		}
		else {
			return 0;
		}
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);				//WM_QUIT ( 0: 리턴값 )
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
