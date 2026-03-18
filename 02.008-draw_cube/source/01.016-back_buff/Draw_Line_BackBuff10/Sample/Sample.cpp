//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>

#include ".\scitech\INCLUDE\MGRAPH.H"

#pragma comment (lib, ".\\scitech\\LIB\\WIN32\\VC\\MGLLT.LIB")
#pragma comment(lib, "winmm.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

HWND g_hWnd;
HINSTANCE g_hInst;

MGLDC* windc;
MGLDC* dibdc;

unsigned char * BackBuffer = NULL;

int Screen_Width;
int Screen_Height;

void Create_Palette()
{
	
	palette_t Pal[256];

	for (int i = 0; i < 256; i++)
	{
		if (i == 0)
		{
			Pal[i].red = 0;
			Pal[i].green = 0;
			Pal[i].blue = 0;
		}
		else
		{
			Pal[i].red = 255;
			Pal[i].green = 255;
			Pal[i].blue = 127;
		}
	}

	MGL_setPalette(dibdc, Pal, 256, 0);
	MGL_realizePalette(dibdc, 256, 0, false);
}

void Create_BackBuffer()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	Screen_Width = Rc.right;
	Screen_Height = Rc.bottom;

	pixel_format_t pf;

	MGL_setAppInstance(g_hInst);

	MGL_registerDriver(MGL_PACKED8NAME, PACKED8_driver);

	MGL_initWindowed("");

	if (!MGL_changeDisplayMode(grWINDOWED))
		MGL_fatalError("Unable to use window mode!");

	if ((windc = MGL_createWindowedDC(g_hWnd)) == NULL)
		MGL_fatalError("Unable to create Windowed DC!");

	MGL_getPixelFormat(windc, &pf);

	if ((dibdc = MGL_createMemoryDC(Screen_Width, Screen_Height, 8, &pf)) ==
		NULL)
		MGL_fatalError("Unable to create Memory DC!");
}

void Clear_BackBuffer()
{
	MGL_beginDirectAccess();

	BackBuffer = (unsigned char *)dibdc->surface;
	
	for (int x = 0; x < Screen_Width; x++)
	{
		for (int y = 0; y < Screen_Height; y++)
		{
			int Index = y * Screen_Width + x;

			BackBuffer[Index + 0] = 0;
		}
	}
}

void Present_BackBuffer()
{
	MGL_endDirectAccess();

	// MGL present back buffer
	HDC hdcScreen = GetDC(g_hWnd);
	MGL_setWinDC(windc, hdcScreen);

	MGL_bitBltCoord(windc, dibdc, 0, 0, Screen_Width, Screen_Height, 0, 0,
					MGL_REPLACE_MODE);

	ReleaseDC(g_hWnd, hdcScreen);
}

void Delete_BackBuffer()
{
	if (windc)
		MGL_destroyDC(windc);
	if (dibdc)
		MGL_destroyDC(dibdc);

	windc = dibdc = NULL;
}

void Render_Scene()
{

	Clear_BackBuffer();

	//рисуем линию
	for (int y = 100; y < 120; y++)
	{
		for (int x = 100; x < 400; x++)
		{
			int Index = y * Screen_Width + x;

			BackBuffer[Index + 0] = 22;

		}
	}

	Present_BackBuffer();

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

	g_hInst = hInstance;

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

	Create_BackBuffer();
	Create_Palette();

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

	Delete_BackBuffer();

	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return (int)msg.wParam;
}