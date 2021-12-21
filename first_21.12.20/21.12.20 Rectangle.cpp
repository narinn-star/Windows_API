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

//전역 변수
//Q1. 왜 전역변수로만 선언해야하는가?
//A1. WndProc 실행될 때마다 (클릭할 때마다) 새로 선언되어 초기화됨.
//Q2. 전역변수로 선언했을 때 iCount=0으로 초기화 하지 않아도 되는 이유?
//A2. 전역변수는 알아서 0으로 초기화됨 (배열, 변수 모두)
/*
int px[1000];	//x좌표 저장할 배열
int py[1000];	//y좌표 저장할 배열
int iCount;	// 점의 개수 저장할 변수
*/

//위의 x, y 좌표 저장 -> 구조체로 작성
typedef struct tagmyPOINT{
	LONG x;
	LONG y;
} myPoint;

myPoint p[1000];
int iCount;	//점의 개수 저장할 변수

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
				//WndProc => Window Precedure
{	//이벤트 핸들러 부분

	HDC hdc;
	PAINTSTRUCT ps;
	int x, y;
	
	/*
	지역 변수로 선언하면 X
	int px[1000];	//x좌표 저장할 배열		//stack
	int py[1000];	//y좌표 저장할 배열
	int iCount = 0;	// 점의 개수 저장할	변수
	*/

	switch (iMessage) {
	case WM_CREATE:	//WM => Window Message
		hWndMain = hWnd;
		return 0;

		/*case WM_LBUTTONDOWN:	//사용자가 윈도우에서 왼쪽 버튼을 눌렀을 때
			//좌측 위에 사각형 그리기
			hdc = GetDC(hWnd);
			Rectangle(hdc, 10, 10, 100, 100);	//(10, 10)왼쪽 상단 점 (100, 100)오른쪽 하단 점
			//사각형 그리기 함수 호출
			return 0;*/

	case WM_LBUTTONDOWN:	//사용자가 윈도우에서 왼쪽 버튼을 눌렀을 때
		//좌측 위에 사각형 그리기
		hdc = GetDC(hWnd);	

		//lParam 32비트를 16으로 나누어 Y, X 순서로 좌표를 담음

		//x = LOWORD(lParam);		//lParam의 하위 16bit 빼내기 [ 매크로 ]

		x = (short)(lParam);  //short : 16bit _ casting - 형변환
		//=> 16bit만 보겠다.

		//x = (WORD)(lParam);	//Macro Expansion -- compiler가 컴파일 직전 다 바꾸고 컴파일 실행

		//y = HIWORD(lParam);		//lParam의 상위 16bit 빼내기

		y = (short)(lParam >> 16);	//shift Operator <- bitwise 연산자 중

		/*
		//그냥 변수를 사용했을 때
		px[iCount] = x;
		py[iCount] = y;
		iCount++;
		*/

		//구조체를 사용했을 때
		p[iCount].x = x;
		p[iCount].y = y;
		iCount++;


		Rectangle(hdc, x - 30, y - 30, x + 30, y + 30);			//사각형 그리기 함수 호출
		return 0;

	case WM_PAINT:	//화면(Window)을 그려야 할 시점이라고 OS가 알려주는 메시지
		hdc = BeginPaint(hWnd, &ps);

		//다시 화면을 올렸을 때 저장해 둔 배열의 좌표를 다 꺼내어 그려줌
		for (int i = 0; i < iCount; i++){
			//변수 사용할 때
			//Rectangle(hdc, px[i] - 30, py[i] - 30, px[i] + 30, py[i] + 30);
			//구조체 사용할 때
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
