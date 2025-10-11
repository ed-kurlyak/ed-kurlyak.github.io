//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

HWND g_hWnd;

HDC g_hDC;
float m_xl, m_xr, m_dxl, m_dxr;

void Draw_Color_Poly(int y1, int y2)
{
	for ( int y = y1; y < y2; y++ )
	{
		for ( int x = (int)m_xl; x < (int)m_xr; x++ )
		{
			SetPixel(g_hDC, x, y, RGB(255, 0, 0));
		}

		m_xl += m_dxl;
		m_xr += m_dxr;
	}
}


void Render_Scene()
{
	g_hDC = GetDC(g_hWnd);

	/*вершины треугольника

				v1(400,100)

						v2(550,250)
		
		v3(250,400)
	*/

	//левая сторона треугольника длиннее
	float x1 = 400, y1 = 100;
	float x2 = 600, y2 = 250;
	float x3 = 200, y3 = 400;

	m_xl = x1;
	m_dxl = (x3 - x1) / (y3 - y1);

	//рисуем верхний треугольник
	m_xr = x1;
	m_dxr = (x2 - x1) / (y2 - y1);
	Draw_Color_Poly((int)y1, (int)y2);

	//рисуем нижний треугольник
	m_xr = x2;
	m_dxr = (x3 - x2) / (y3 - y2);
	Draw_Color_Poly((int)y2, (int)y3);

	ReleaseDC(g_hWnd, g_hDC);

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