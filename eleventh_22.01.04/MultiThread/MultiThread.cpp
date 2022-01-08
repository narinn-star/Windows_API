#include <windows.h>

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass=TEXT("MultiThread");

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
	  ,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;

	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);

	while (GetMessage(&Message,NULL,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

DWORD WINAPI ThreadFunc(void * prc)
{
	HDC hdc;
	BYTE Blue=0;
	HBRUSH hBrush, hOldBrush;
	//RECT * rc;
	RECT rc;
	//rc = (RECT *) prc;  //casting
	rc = *(RECT*)prc;  //casting
	hdc=GetDC(hWndMain);
	for (;;) {
		Blue+=5;
		Sleep(20);
		hBrush = CreateSolidBrush(RGB(0,0,Blue));
		hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		//Rectangle(hdc,rc->left, rc->top, rc->right, rc->bottom);
		Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
		SelectObject(hdc, hOldBrush);
		DeleteObject(hBrush);
	}
	ReleaseDC(hWndMain, hdc);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	DWORD ThreadID;
	static RECT arRect[]={
		{100,100,200,200},{300,100,400,200},
		{100,300,200,400},{300,300,400,400},
	};
	int i;

	switch (iMessage) {
	case WM_CREATE:
		hWndMain=hWnd;
		for (i=0;i<4;i++) {
			CloseHandle(CreateThread(NULL,0,ThreadFunc,&arRect[i],0,&ThreadID));
		}
		return TRUE;
	case WM_LBUTTONDOWN:
		hdc=GetDC(hWnd);
		Ellipse(hdc,LOWORD(lParam)-10, HIWORD(lParam)-10,
			LOWORD(lParam)+10, HIWORD(lParam)+10);
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}
