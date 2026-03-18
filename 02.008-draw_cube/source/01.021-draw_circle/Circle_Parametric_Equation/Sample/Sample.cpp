//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

HWND g_hWnd;

#define PI 3.14159265358979f

void Render_Scene()
{
	HDC hDC = GetDC(g_hWnd);

	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(hDC, &Rc, hBrush);

	Sleep(500);

	//наша окружность будет 2PI то есть 360 градусов
	float PI2 = PI * 2.0f;

	//количество линий (степень апрокимации)
	int Parts = 12;

	//вычисляем приращение для цикла
	float Dl = PI2 / (float)Parts;

	//координаты начальной точки
	int PrevX = 0;
	int PrevY = 0;

	//создаем перо- будем проводить линии между точками
	HPEN hPen = CreatePen(PS_SOLID, 4, RGB( 0, 0, 0) );
	HPEN hOldPen = (HPEN) SelectObject(hDC, hPen);

	for ( int i = 0 ; i <= Parts; i++ )
	{
		float Angle = i * Dl;
		//вычисляем координаты окружности x,y
		//радиус окружности 100 пикселей
		float x = 100.0f * cosf(Angle);
		float y = 100.0f * sinf(Angle);
		
		//преобразовываем в экранные координаты
		int StartX = (int) ( (float) Rc.right / 2.0f + x );
		int StartY = (int) ( (float) Rc.bottom / 2.0f - y );

		//если первая итерация цикла
		if(i == 0)
		{
			PrevX = StartX;
			PrevY = StartY;
			continue;
		}

		//проводим линию
		MoveToEx(hDC, PrevX, PrevY, NULL);
		LineTo(hDC, StartX, StartY);

		Sleep(500);
		
		//присваиваем значения для следующего цикла
		PrevX = StartX;
		PrevY = StartY;

	}

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);

	ReleaseDC(g_hWnd, hDC);
}


LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT uMsg,
						 WPARAM wParam,
						 LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

int PASCAL WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WNDCLASS wcl;
	wcl.style = CS_HREDRAW | CS_VREDRAW;
	wcl.lpfnWndProc = WndProc;
	wcl.cbClsExtra = 0L;
	wcl.cbWndExtra = 0L;
	wcl.hInstance = hInstance;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcl.lpszMenuName = NULL;
	wcl.lpszClassName = "Sample";

	/*
	WNDCLASS wcl = { CS_HREDRAW|CS_VREDRAW, WndProc,
			0, 0, hInstance, NULL, LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_WINDOW+1),
			NULL, "Sample"};

	*/

	if(!RegisterClass(&wcl))
		return 0;

	g_hWnd = CreateWindow("Sample", "Sample Application",
					WS_OVERLAPPEDWINDOW,
					0, 0,
					WINDOW_WIDTH, WINDOW_HEIGHT,
					NULL,
					NULL,
					hInstance,
					NULL);
	if(!g_hWnd)
		return 0;

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	Render_Scene();

	MSG msg;

	while(true)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message ==	WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(GetKeyState(VK_ESCAPE) & 0xFF00)
			break;
	}

	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return (int)msg.wParam;
}