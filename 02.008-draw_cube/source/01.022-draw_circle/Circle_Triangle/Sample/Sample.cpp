//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define PI 3.14159265358979f

//кол-во треугольников
#define TriangleCount 12 

//кол-во вертексов
//равно 12 частей (треугольников) + 1 верт.завершающий
//и + 1 вершина в центре = 14
#define VertCount (TriangleCount + 1 + 1) 

//кол-во индексов
#define IndexCount (TriangleCount * 3) 

struct vector3
{
	float x,y,z;

	float tu,tv;
};

HWND g_hWnd;

vector3 VertBuff[VertCount];
vector3 VertBuffTransformed[VertCount];

unsigned int IndexBuff[IndexCount];

void Init_Scene()
{
	float Dl = PI * 2.0f / TriangleCount;

	for (int i=0; i < VertCount - 1; i++ )
	{
		float angle = i * Dl;

		VertBuff[i].x = 4.0f * sinf(angle);
		VertBuff[i].y = 4.0f * cosf(angle);
		VertBuff[i].z = 0.0f;

		//центр окружности, вершины расположены в центре
		//системы координат (0,0,0)
		//текстурные координаты (0,0) начинаются
		//в левом верхнем углу - делаем преобразование
		VertBuff[i].tu = 0.5f * (1.0f + sinf(angle));
		VertBuff[i].tv = 0.5f * (1.0f - cosf(angle));
	}

	VertBuff[VertCount - 1].x = 0.0f;
	VertBuff[VertCount - 1].y = 0.0f;
	VertBuff[VertCount - 1].z = 0.0f;

	for ( int i=0; i < TriangleCount; i++ )
	{
		IndexBuff[i *3 + 0] = VertCount - 1;
		IndexBuff[i *3 + 1] = i;
		IndexBuff[i *3 + 2] = i + 1;
	}
}

void Render_Scene()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	for ( int i = 0; i < VertCount; i++ )
	{
		vector3 Vec = VertBuff[i];

		Vec.z += 7.0f;

		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;

		Vec.x = Vec.x / (float(Rc.right) / float(Rc.bottom));

		Vec.x = Vec.x * float(Rc.right) / 2.0f + float(Rc.right) / 2.0f;
		Vec.y =-Vec.y * float(Rc.bottom) / 2.0f + float(Rc.bottom) / 2.0f;

		VertBuffTransformed[i] = Vec;
	}

	HDC hDC = GetDC(g_hWnd);
	HPEN hPen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN) SelectObject(hDC, hPen);

	for ( int i = 0; i < TriangleCount; i++ )
	{
		vector3 Vec1 = VertBuffTransformed[ IndexBuff[ i * 3 + 0] ];
		vector3 Vec2 = VertBuffTransformed[ IndexBuff[ i * 3 + 1] ];
		vector3 Vec3 = VertBuffTransformed[ IndexBuff[ i * 3 + 2] ];

		MoveToEx(hDC, (int)Vec1.x, (int)Vec1.y, NULL);
		LineTo(hDC, (int)Vec2.x, (int)Vec2.y);
		LineTo(hDC, (int)Vec3.x, (int)Vec3.y);
		LineTo(hDC, (int)Vec1.x, (int)Vec1.y);

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