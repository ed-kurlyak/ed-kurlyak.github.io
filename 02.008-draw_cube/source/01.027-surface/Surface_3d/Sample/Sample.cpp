//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

HWND g_hWnd;

#define PI 3.1415926535897932384626433f

struct Vec2
{
	float x;
	float y;
};

float Fun(float x, float y)
{
	return cosf(sqrtf(x * x + y * y));
}

Vec2 Transform(float InX, float InY, float InZ)
{
	Vec2 VecOut;

	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	float Theta = 0.5f;
	float Phi = 0.6f;

	float Height = Fun((float)InX, (float)InY);

	float x = InX * sinf(Theta) + InY * cosf(Theta);
	float y = InX * cosf(Theta) * cosf(Phi) - InY * sinf(Theta) * cosf(Phi) + Height * sinf(Phi);
	float z = InX * cosf(Theta) * sinf(Phi) - InY * sinf(Theta) * sinf(Phi) - Height * cosf(Phi);

	z = z + 60.0f;

	float D = 10.0f;

	x = D * x / z;
	y = D * y / z;

	VecOut.x = x * Rc.bottom / 2.0f + Rc.right / 2.0f;
	VecOut.y = -y * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

	return VecOut;

}

void Render_Scene()
{
	HDC hDC = GetDC(g_hWnd);

	float xMin = -5.0f;
	float xMax = 5.0f;
	float yMin = -5.0f;
	float yMax = 5.0f;

	int xCount = 20;
	int yCount = 20;

	float xStep = (xMax - xMin) / xCount; // 0.5
	float yStep = (yMax - yMin) / yCount;

	//вертикальные линии (X фиксирован, Y меняется)
	for (int i = 0; i <= xCount; i++)
	{
		float x = xMin + i * xStep;

		for (int j = 0; j < yCount; j++)
		{
			float y1 = yMin + j * yStep;
			float y2 = yMin + (j + 1) * yStep;

			Vec2 Vec1 = Transform(x, y1, 0);
			Vec2 Vec2 = Transform(x, y2, 0);

			MoveToEx(hDC, (int)Vec1.x, (int)Vec1.y, NULL);
			LineTo(hDC, (int)Vec2.x, (int)Vec2.y);
		}
	}

	//горизонтальные линии (Y фиксирован, X меняется)
	for (int j = 0; j <= yCount; j++)
	{
		float y = yMin + j * yStep;

		for (int i = 0; i < xCount; i++)
		{
			float x1 = xMin + i * xStep;
			float x2 = xMin + (i + 1) * xStep;

			Vec2 Vec1 = Transform(x1, y, 0);
			Vec2 Vec2 = Transform(x2, y, 0);

			MoveToEx(hDC, (int)Vec1.x, (int)Vec1.y, NULL);
			LineTo(hDC, (int)Vec2.x, (int)Vec2.y);
		}
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