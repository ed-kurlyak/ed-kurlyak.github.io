//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

HWND g_hWnd;

void Render_Scene()
{
	HDC hDC = GetDC(g_hWnd);

	/*вершины треугольника

		v1(200,100)			v2(600,100)

				v3(400,300)
	*/
	
	//координаты треугольника в экранных координатах
	float	x1 = 200.0f, y1 = 100.0f;
	float	x2 = 600.0f, y2 = 100.0f;
	float	x3 = 400.0f, y3 = 300.0f;

	//указываем начало линии и конец линии
	float xl = x1;
	float xr = x2;

	//вычисляем наклон (приращение координат)
	float dxl = (x3 - x1) / (y3 - y1);
	float dxr = (x3-  x2) / (y3 - y2);

	for (int y = (int)y1; y < (int)y3; y++)
	{
		//проводим линию красного цвета от xl до xr на высоте y
		for ( int x = (int)xl; x < (int)xr; x++ )
		{
			SetPixel(hDC, x, y, RGB(255, 0, 0));
		}

		//добавляем приращение для левой и правой стороны треугольника
		xl += dxl;
		xr += dxr;
	}

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