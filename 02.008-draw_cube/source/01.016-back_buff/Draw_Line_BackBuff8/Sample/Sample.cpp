//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>
#include <math.h>

#include <ddraw.h>
#include <d3d.h>
#include <d3dtypes.h>
#include <d3dcaps.h>

#pragma comment (lib, "ddraw.lib")
#pragma comment (lib, "dxguid.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

RECT g_WindowRect = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};

LPDIRECTDRAW         g_pDD1           = NULL;
LPDIRECTDRAW4        g_pDD4           = NULL;
LPDIRECTDRAWSURFACE4 g_pDdsPrimary    = NULL;
LPDIRECTDRAWSURFACE4 g_pDdsBackBuffer = NULL;
RECT                 g_RcScreenRect;
RECT                 g_RcViewportRect;
LPDIRECTDRAWPALETTE g_pPalette = NULL;
unsigned char * g_Data = NULL;

HWND g_hWnd;

HRESULT Init_Scene()
{
	HRESULT hr;

	hr = DirectDrawCreate( NULL, &g_pDD1, NULL );
	if( FAILED( hr ) )
		return hr;

	hr = g_pDD1->QueryInterface( IID_IDirectDraw4, (VOID**)&g_pDD4 );
	if( FAILED( hr ) )
		return hr;

	
	hr = g_pDD4->SetCooperativeLevel( g_hWnd, DDSCL_NORMAL );
	if( FAILED( hr ) )
		return hr;

	DDSURFACEDESC2 ddsd;
	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
	ddsd.dwSize         = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags        = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	// Create the primary surface.
	hr = g_pDD4->CreateSurface( &ddsd, &g_pDdsPrimary, NULL );
	if( FAILED( hr ) )
		return hr;


	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
	ddsd.dwSize         = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
	ddsd.dwWidth  = WINDOW_WIDTH;
	ddsd.dwHeight = WINDOW_HEIGHT;

	GetClientRect( g_hWnd, &g_RcScreenRect );
	GetClientRect( g_hWnd, &g_RcViewportRect );
	ClientToScreen( g_hWnd, (POINT*)&g_RcScreenRect.left );
	ClientToScreen( g_hWnd, (POINT*)&g_RcScreenRect.right );
	//ddsd.dwWidth  = g_RcScreenRect.right - g_RcScreenRect.left;
	//ddsd.dwHeight = g_RcScreenRect.bottom - g_RcScreenRect.top;

	// 8-битный формат
	ZeroMemory(&ddsd.ddpfPixelFormat, sizeof(DDPIXELFORMAT));
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_RGB;
	ddsd.ddpfPixelFormat.dwFourCC = 0;
	ddsd.ddpfPixelFormat.dwRGBBitCount = 8;

	ddsd.ddpfPixelFormat.dwRBitMask = 0;
	ddsd.ddpfPixelFormat.dwGBitMask = 0;
	ddsd.ddpfPixelFormat.dwBBitMask = 0;
	ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;

	hr = g_pDD4->CreateSurface( &ddsd, &g_pDdsBackBuffer, NULL );
	if( FAILED( hr ) )
		return hr;

	PALETTEENTRY entries[256];

	for(int i=0;i<256;i++)
	{
		entries[i].peRed   = i;
		entries[i].peGreen = i;
		entries[i].peBlue  = i;
		entries[i].peFlags = 0;
	}

	//синий цвет
	entries[0].peRed   = 0;
	entries[0].peGreen = 0;
	entries[0].peBlue  = 255;
	entries[0].peFlags = 0;

	//желтый цвет
	entries[1].peRed   = 255;
	entries[1].peGreen = 255;
	entries[1].peBlue  = 127;
	entries[1].peFlags = 0;


	hr = g_pDD4->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, entries, &g_pPalette, NULL);
	if(FAILED(hr))
		return hr;

	hr = g_pDdsBackBuffer->SetPalette(g_pPalette);
	if(FAILED(hr))
		return hr;

	return S_OK;

}

VOID On_Move(int x, int y)
{
	DWORD dwWidth  = g_RcScreenRect.right - g_RcScreenRect.left;
	DWORD dwHeight = g_RcScreenRect.bottom - g_RcScreenRect.top;
    SetRect( &g_RcScreenRect, x, y, x + dwWidth, y + dwHeight );
}

void Draw_Scene()
{

	DDSURFACEDESC2 Desc;
	ZeroMemory(&Desc, sizeof(DDSURFACEDESC2));
	Desc.dwSize = sizeof(DDSURFACEDESC2);

	HRESULT hr = g_pDdsBackBuffer->Lock((RECT*)&g_RcViewportRect, &Desc, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR , NULL);

	g_Data = (unsigned char *)Desc.lpSurface;

	for ( unsigned int y = 0; y < WINDOW_HEIGHT; y++)
	{
		for ( unsigned int x = 0; x < WINDOW_WIDTH; x++ )
		{
			int Index =  y * WINDOW_WIDTH + x;

			g_Data[Index] = 0;
		}
	}

	hr = g_pDdsBackBuffer->Unlock(NULL);

	//приложение отслеживет сообщение On_Move()
	//что бы правильно рассчитать позицию окна на экране
	
	hr = g_pDdsPrimary->Blt( &g_RcScreenRect, g_pDdsBackBuffer, 
                               &g_RcViewportRect, DDBLT_WAIT, NULL );
	
}

HRESULT Destroy()
{
	if (g_pPalette)
	{
		g_pPalette->Release();
		g_pPalette = NULL;
	}

	if (g_pDdsBackBuffer)
	{
		g_pDdsBackBuffer->Release();
		g_pDdsBackBuffer = NULL;
	}

	if (g_pDdsPrimary)
	{
		g_pDdsPrimary->Release();
		g_pDdsPrimary = NULL;
	}

	if (g_pDD4)
	{
		g_pDD4->Release();
		g_pDD4 = NULL;
	}

	if (g_pDD1)
	{
		g_pDD1->Release();
		g_pDD1 = NULL;
	}

	return S_OK;
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

		case WM_MOVE:
			// Move messages need to be tracked to update the screen rects
			// used for blitting the backbuffer to the primary.
			On_Move( (SHORT)LOWORD(lParam), (SHORT)HIWORD(lParam) );
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
					640, 480,
					NULL,
					NULL,
					hInstance,
					NULL);
	if(!g_hWnd)
		return 0;

	AdjustWindowRectEx(&g_WindowRect,
		GetWindowStyle(g_hWnd),
		GetMenu(g_hWnd) != NULL,
		GetWindowExStyle(g_hWnd));

	UINT WidthScreen = GetSystemMetrics(SM_CXSCREEN);
	UINT HeightScreen = GetSystemMetrics(SM_CYSCREEN);

	UINT WidthX = g_WindowRect.right - g_WindowRect.left;
	UINT WidthY = g_WindowRect.bottom - g_WindowRect.top;

	UINT PosX = (WidthScreen - WidthX) / 2;
	UINT PosY = (HeightScreen - WidthY) / 2;

	MoveWindow(g_hWnd,
		PosX,
		PosY,
		WidthX,
		WidthY,
		FALSE);


	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	RECT rc;
	GetClientRect(g_hWnd, &rc);

	Init_Scene();

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

		Draw_Scene();
	}

	Destroy();

	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return (int)msg.wParam;
}