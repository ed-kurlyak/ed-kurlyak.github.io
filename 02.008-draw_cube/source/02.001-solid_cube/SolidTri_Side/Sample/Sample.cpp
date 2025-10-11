//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define SWAP(a,b,t) { t=a; a=b; b=t; }

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

void Draw_Color_Triangle(float x1, float y1,
	float x2, float y2,
	float x3, float y3)
{
	float t;

	if (y2 < y1)
	{
		SWAP(x2, x1, t);
		SWAP(y2, y1, t);
	}
	if (y3 < y1)
	{
		SWAP(x3, x1, t);
		SWAP(y3, y1, t);

	}
	if (y3 < y2)
	{
		SWAP(x3, x2, t);
		SWAP(y3, y2, t);
	}

	float dxdy1 = 0.0;
	float dxdy2 = 0.0;

	if (y2 > y1)
		dxdy1 = (x2 - x1) / (y2 - y1);
	//if(y3 > y1)
	if (y3 > y2)
		dxdy2 = (x3 - x1) / (y3 - y1);

	int side = dxdy2 > dxdy1;

	if (y1 == y2)
		side = x1 > x2;
	if (y3 == y2)
		side = x3 > x2;

	if (!side)
	{
		//левая сторона треугольника длиннее
		//наш треугольник длинее левая сторона
		m_xl = x1;
		m_dxl = (x3 - x1) / (y3 - y1);

		if (y1 < y2)
		{
			m_xr = x1;
			m_dxr = (x2 - x1) / (y2 - y1);

			Draw_Color_Poly((int)y1, (int)y2);
		}
		if (y2 < y3)
		{
			m_xr = x2;
			m_dxr = (x3 - x2) / (y3 - y2);

			Draw_Color_Poly((int)y2, (int)y3);
		}
	}
	else
	{
		//правая сторона треугольника длиннее
	}

}


void Render_Scene()
{
	g_hDC = GetDC(g_hWnd);

	/*вершины треугольника

				v1(400,150)

						v2(600,250)
		
		v3(200,400)
	*/

	//левая сторона треугольника длиннее
	float x1 = 400.0f, y1 = 100.0f;
	float x2 = 600.0f, y2 = 250.0f;
	float x3 = 200.0f, y3 = 400.0f;

	Draw_Color_Triangle(x1, y1, x2, y2, x3, y3);

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