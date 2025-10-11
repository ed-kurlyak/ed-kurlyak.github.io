//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define NUM_PARTS 10

struct vector3
{
	float x,y,z;

	vector3 & operator + (const vector3 & v)
	{
		x = x + v.x;
		y = y + v.y;
		z = z + v.z;

		return *this;
	}

	vector3 & operator * (const float &v)
	{
		x = x * v;
		y = y * v;
		z = z * v;

		return *this;
	}
};

HWND g_hWnd;

vector3 VertBuffBezier[NUM_PARTS + 1];
vector3 VertBuffBezierTransformed[NUM_PARTS + 1];

void Cubic_Bezier_Curve(vector3 Vec1,
						vector3 Vec2,
						vector3 Vec3,
						vector3 Vec4,
						float Scalar,
						vector3 *VecOut)
{
	*VecOut = Vec1 * (1.0f - Scalar) * (1.0f - Scalar) * (1.0f - Scalar) + \
		Vec2 * 3 * Scalar * (1.0f - Scalar) * (1.0f - Scalar) + \
		Vec3 * 3 * Scalar * Scalar * (1.0f - Scalar) + \
		Vec4 * Scalar * Scalar * Scalar;
}

void Init_Scene()
{
	float s = 1.0f / float(NUM_PARTS);

	float Scalar = 0.0f;

	//опорные точки
	vector3 Vec1 = {-15.0f,	 0.0f,	0.0f};
	vector3 Vec2 = { -5.0f,	10.0f,	0.0f};
	vector3 Vec3 = {  5.0f,	10.0f,	0.0f};
	vector3 Vec4 = { 15.0f,	 0.0f,	0.0f};

	//наша кривая буде тсотоять из 10 отрезков (линий)
	//это 11 вершин
	for ( int i = 0; i < (NUM_PARTS + 1); i++ )
	{

		vector3 vOut;
		Cubic_Bezier_Curve(Vec1, Vec2, Vec3, Vec4,
							Scalar,
							&vOut);

		VertBuffBezier[i] = vOut;
		
		Scalar += s;

	}
}

void Render_Scene()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	for ( int i = 0; i < (NUM_PARTS + 1); i++ )
	{
		vector3 Vec = VertBuffBezier[i];

		Vec.z += 15.0f;

		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;

		Vec.x = Vec.x / ( float(Rc.right) / float(Rc.bottom) );

		Vec.x = Vec.x * float(Rc.right) / 2.0f + float(Rc.right) / 2.0f;
		Vec.y =-Vec.y * float(Rc.bottom) / 2.0f + float(Rc.bottom) / 2.0f;

		VertBuffBezierTransformed[i] = Vec;
	}

	HDC hDC = GetDC(g_hWnd);
	HPEN hPen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN) SelectObject(hDC, hPen);

	vector3 VecPrev;

	for ( int i = 0; i < (NUM_PARTS + 1); i++ )
	{
		vector3 Vec = VertBuffBezierTransformed[i];

		if( i == 0 )
		{
			VecPrev = Vec;
			continue;
		}

		MoveToEx(hDC, (int)VecPrev.x, (int)VecPrev.y, NULL);
		LineTo(hDC, (int)Vec.x, (int)Vec.y);

		VecPrev = Vec;
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

	Init_Scene();
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