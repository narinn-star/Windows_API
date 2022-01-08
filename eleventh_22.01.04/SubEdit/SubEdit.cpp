#include <windows.h>

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass=TEXT("SubEdit");

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

#define ID_EDIT1 100
#define ID_EDIT2 101
HWND hEdit1, hEdit2;
WNDPROC OldEditProc;
LRESULT CALLBACK EditSubProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch (iMessage) {
	case WM_KEYDOWN:
		if (wParam==VK_RETURN) {
			MessageBox(hWnd,TEXT("Enter is Pressed"),TEXT("Edit"),MB_OK);
			SetFocus(hWnd);
		}
		//����Ű�� �ƴ� �ٸ� Ű�� �ԷµǾ��� ���� �ٷ� break; -> CallWindowProc() -> OldEditProc ȣ�� 

		/*
		if (wParam==VK_TAB) {
			SetFocus(hEdit2);
		}
		if (wParam==VK_LEFT) wParam=VK_RIGHT;
		else if (wParam==VK_RIGHT) wParam=VK_LEFT;
		*/
		break;
	/*
	case WM_LBUTTONDOWN:
		SendMessage(hWnd,EM_SETSEL,0,-1);
		return 0;
	*/
	}
	return CallWindowProc(OldEditProc,hWnd,iMessage,wParam,lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR *Mes=TEXT("����Ʈ�� EnterŰ �Է��� �����մϴ�");
	switch (iMessage) {
	case WM_CREATE:
		hEdit1=CreateWindow(TEXT("edit"),NULL,WS_CHILD | WS_VISIBLE | WS_BORDER |
			ES_AUTOHSCROLL,
			10,10,200,25,hWnd,(HMENU)ID_EDIT1,g_hInst,NULL);
		hEdit2=CreateWindow(TEXT("edit"),NULL,WS_CHILD | WS_VISIBLE | WS_BORDER |
			ES_AUTOHSCROLL,
			10,50,200,25,hWnd,(HMENU)ID_EDIT2,g_hInst,NULL);
		SetFocus(hEdit1);

		OldEditProc=(WNDPROC)SetWindowLongPtr(hEdit1,GWLP_WNDPROC,(LONG_PTR)EditSubProc);
		//hEdit1�� �ִ� ������ ���ν����� EditSubProc�̶�� ���ν����� �����ϰڴ�.
		//���� hEdit1�� �ִ� ������ ���ν����� OldEditProc�� ����
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			MessageBox(hWnd,TEXT("����Ű�� �������ϴ�."),TEXT("�˸�"),MB_OK);
		}
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		TextOut(hdc,10,100,Mes,lstrlen(Mes));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		SetWindowLongPtr(hEdit1,GWLP_WNDPROC,(LONG_PTR)OldEditProc);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}
