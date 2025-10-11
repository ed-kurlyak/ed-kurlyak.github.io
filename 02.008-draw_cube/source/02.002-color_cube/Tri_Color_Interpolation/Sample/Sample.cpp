//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

HWND g_hWnd;

HDC g_hDC;

void DrawLine(int XFrom, int XTo, int YVal, 
	float rl, float gl, float bl, float rr, float gr, float br)
{
	float tr = rl;
	float tg = gl;
	float tb = bl;

	float tdr = (rr - rl) / ((float)XTo - (float)XFrom);
	float tdg = (gr - gl) / ((float)XTo - (float)XFrom);
	float tdb = (br - bl) / ((float)XTo - (float)XFrom);

	for ( int i = XFrom; i < XTo; i++)
	{	
		SetPixel(g_hDC, i, YVal, RGB((int)tr, (int)tg, (int)tb));

		tr += tdr;
		tg += tdg;
		tb += tdb;
	}
}

void RenderScene()
{
	g_hDC = GetDC(g_hWnd);

	/*вершины треугольника

				v1(400,100)

		v3(200,300			v2(600,300)
	*/
	
	//координаты треугольника в экранных координатах
	float x1 = 400.0f, y1 = 100.0f;
	float x2 = 600.0f, y2 = 300.0f;
	float x3 = 200.0f, y3 = 300.0f;
	
	int r1 = 255; int g1 = 0;   int b1 = 0;
	int r2 = 0;   int g2 = 255; int b2 = 0;
	int r3 = 0;   int g3 = 0;   int b3 = 255;

	float dlr = (float)(r3 - r1) / (float)(y3 - y1);
	float dlg = (float)(g3 - g1) / (float)(y3 - y1);
	float dlb = (float)(b3 - b1) / (float)(y3 - y1);

	float drr = (float)(r2 - r1) / (float)(y2 - y1);
	float drg = (float)(g2 - g1) / (float)(y2 - y1);
	float drb = (float)(b2 - b1) / (float)(y2 - y1);

	float rl = (float)r1;
	float gl = (float)g1;
	float bl = (float)b1;

	float rr = (float)r1;
	float gr = (float)g1;
	float br = (float)b1;
	
	float dl = (x3 - x1) / (y3 - y1);
	float dr = (x2-  x1) / (y2 - y1);

	float xl = x1;
	float xr = x1;

	for (int y = (int)y1; y < y2; y++)
	{
		DrawLine((int)xl, (int)xr, y, 
			rl, gl, bl, rr, gr, br);
		
		xl += dl;
		xr += dr;

		rl += dlr;
		gl += dlg;
		bl += dlb;

		rr += drr;
		gr += drg;
		br += drb;
	}

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

	RenderScene();

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