//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>

#include ".\scitech\INCLUDE\MGRAPH.H"

#pragma comment (lib, ".\\scitech\\LIB\\WIN32\\VC\\MGLLT.LIB")
#pragma comment(lib, "winmm.lib")

//ignore LIBC.lib Linker options

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

HWND g_hWnd;
HINSTANCE g_hInstance;

MGLDC* windc;
MGLDC* dibdc;

void Init()
{
	pixel_format_t	pf;

	MGL_setAppInstance(g_hInstance);

	MGL_registerDriver(MGL_PACKED24NAME, PACKED24_driver);

	//для оконного режима
	MGL_initWindowed("");

	if (!MGL_changeDisplayMode(grWINDOWED))
		MGL_fatalError("Unable to use window mode!");

	if ((windc = MGL_createWindowedDC(g_hWnd)) == NULL)
		MGL_fatalError("Unable to create Windowed DC!");

	MGL_getPixelFormat(windc, &pf);

	if ((dibdc = MGL_createMemoryDC(WINDOW_WIDTH, WINDOW_HEIGHT, 24, &pf)) == NULL)
		MGL_fatalError("Unable to create Memory DC!");
}

void Render_Scene()
{
	MGL_beginDirectAccess();

	unsigned char* BackBuffer = (unsigned char*)dibdc->surface;

	//очищаем m_BackBuffer (экран)
	for (int x = 0; x < WINDOW_WIDTH; x++)
	{
		for (int y = 0; y < WINDOW_HEIGHT; y++)
		{
			int Index = (y * WINDOW_WIDTH + x) * 3;

			BackBuffer[Index + 0] = 77; //blue
			BackBuffer[Index + 1] = 32; //green
			BackBuffer[Index + 2] = 0; //red
		}
	}

	//рисуем линию
	for (int y = 100; y < 120; y++)
	{
		for (int x = 100; x < 400; x++)
		{
			int Index = (y * WINDOW_WIDTH + x) * 3;

			BackBuffer[Index + 0] = 127; //b
			BackBuffer[Index + 1] = 255; //g
			BackBuffer[Index + 2] = 255; //r

		}
	}

	MGL_endDirectAccess();

	//MGL present back buffer
	HDC hdcScreen = GetDC(g_hWnd);
	MGL_setWinDC(windc, hdcScreen);

	MGL_bitBltCoord(windc, dibdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, MGL_REPLACE_MODE);

	ReleaseDC(g_hWnd, hdcScreen);

}

int Shutdown(void)
{
	if (windc)
		MGL_destroyDC(windc);
	if (dibdc)
		MGL_destroyDC(dibdc);

	windc = dibdc = NULL;
	return 1;
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

	g_hInstance = hInstance;

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

	Init();

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

		Render_Scene();
	}

	Shutdown();

	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return (int)msg.wParam;
}