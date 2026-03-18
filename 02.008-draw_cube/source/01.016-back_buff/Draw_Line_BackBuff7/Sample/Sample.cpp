//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>

#pragma comment (lib, "ddraw.lib")
#pragma comment (lib, "dxguid.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_BPP 32

bool g_bFocus = true;

#define INITGUID
#define INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

HWND g_hWnd;

DDSURFACEDESC g_Ddsd;
LPDIRECTDRAW g_lpDD;
LPDIRECTDRAW2 g_lpDD2;
LPDIRECTDRAWSURFACE g_lpDDSPrimary;
LPDIRECTDRAWSURFACE g_lpDDSBack;

RECT g_WindowRect = { 0,0,WINDOW_WIDTH, WINDOW_HEIGHT };

UCHAR *g_BackBuffer; // secondary back buffer
int g_BackLPitch;    // memory line pitch for back buffer

int Fill_Surface(LPDIRECTDRAWSURFACE lpDDS, USHORT Color, RECT *client = 0)
{

	DDBLTFX DDBltFx; // this contains the DDBLTFX structure

	// clear out the structure and set the size field 
	INIT_STRUCT(DDBltFx);
	
	// set the dwfillcolor field to the desired Color
	DDBltFx.dwFillColor = Color; 

	lpDDS->Blt(NULL,     // ptr to dest rectangle
           NULL,       // ptr to source surface, NA            
           NULL,       // ptr to source rectangle, NA
           DDBLT_COLORFILL | DDBLT_WAIT,   // fill and wait                   
           &DDBltFx);  // ptr to DDBLTFX structure

	// return success
	return 1;
}
void Init()
{
	DirectDrawCreate(NULL, &g_lpDD, NULL);
	g_lpDD->QueryInterface(IID_IDirectDraw2, (LPVOID*)&g_lpDD2);

	g_lpDD2->SetCooperativeLevel(g_hWnd, DDSCL_NORMAL);

	// Create the primary surface
	memset(&g_Ddsd,0,sizeof(g_Ddsd));
	g_Ddsd.dwSize = sizeof(g_Ddsd);
	// fullscreen mode
	g_Ddsd.dwFlags = DDSD_CAPS;
	g_Ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	// set the backbuffer count to 0 for windowed mode
	// 1 for fullscreen mode, 2 for triple buffering
	g_Ddsd.dwBackBufferCount = 0;
	// create the primary surface
	g_lpDD2->CreateSurface(&g_Ddsd,&g_lpDDSPrimary,NULL);

	// create the backbuffer
	memset(&g_Ddsd, 0, sizeof(g_Ddsd));
	g_Ddsd.dwSize = sizeof(g_Ddsd);
	g_Ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	g_Ddsd.dwWidth = WINDOW_WIDTH;
	g_Ddsd.dwHeight = WINDOW_HEIGHT;
	g_Ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	//create backbuffer
	g_lpDD2->CreateSurface(&g_Ddsd, &g_lpDDSBack, NULL);

	DDCOLORKEY ColorKey; // used to set color key
	ColorKey.dwColorSpaceLowValue = 0;
	ColorKey.dwColorSpaceHighValue = 0;

	// now set the color key for source blitting
	g_lpDDSBack->SetColorKey(DDCKEY_SRCBLT, &ColorKey);

	Fill_Surface(g_lpDDSBack,0);
}

UCHAR* Lock_Back_Surface(void)
{
	// this function locks the secondary back surface and returns a pointer to it
	// and updates the global variables secondary buffer, and g_BackLPitch

	// is this surface already locked
	if (g_BackBuffer)
	{
		// return to current lock
		return(g_BackBuffer);
	} // end if

	// lock the primary surface
	INIT_STRUCT(g_Ddsd);
	g_lpDDSBack->Lock(NULL,&g_Ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL); 

	// set globals
	g_BackBuffer = (UCHAR *)g_Ddsd.lpSurface;
	g_BackLPitch = g_Ddsd.lPitch;

	// return pointer to surface
	return(g_BackBuffer);

} // end Lock_Back_Surface

int Unlock_Back_Surface(void)
{
	// this unlocks the secondary

	// is this surface valid
	if (!g_BackBuffer)
		return(0);

	// unlock the secondary surface
	g_lpDDSBack->Unlock(NULL);

	// reset the secondary surface
	g_BackBuffer = NULL;
	g_BackLPitch = 0;

	// return success
	return(1);
} // end Unlock_Back_Surface

inline void Mem_Set_DWORD(void *dest, DWORD data, int count)
{
	// this function fills or sets unsigned 16-bit aligned memory
	// count is number of words
	_asm 
	{ 
		mov edi, dest   ; edi points to destination memory
			mov ecx, count  ; number of 32-bit words to move
			mov eax,  data   ; 32-bit data
			rep stosd       ; move data
	} // end asm

} // end Mem_Set_WORD

int Flip(void)
{
	RECT    DestRect;    // used to compute destination rectangle

	// get the window's rectangle in screen coordinates
	GetWindowRect(g_hWnd, &DestRect);

	DestRect.left += -g_WindowRect.left;
	DestRect.top += -g_WindowRect.top;

	DestRect.right = DestRect.left + WINDOW_WIDTH;
	DestRect.bottom = DestRect.top + WINDOW_HEIGHT;

	while( 1 )
    {
		HRESULT DDRetVal;
		DDRetVal = g_lpDDSPrimary->Blt(&DestRect, g_lpDDSBack, NULL, DDBLT_WAIT, NULL);
        if( DDRetVal == DD_OK )
        {
			break;
        }
        
		if( DDRetVal == DDERR_SURFACELOST )
        {
			DDRetVal = g_lpDDSPrimary->Restore();
            if( DDRetVal != DD_OK )
            {
				break;
            }
        }
        if( DDRetVal != DDERR_WASSTILLDRAWING )
        {
			break;
        }
	}

	return 1;
}
void Render_Scene()
{
	if(g_bFocus)
	{
		Fill_Surface(g_lpDDSBack, 0);
		Lock_Back_Surface();

		//рисуем куб на экране тут, цвет белый
		if(g_BackBuffer)
		{
			for ( int y = 200; y < 220; y++ )
			{
				for ( int x = 100; x < 400; x++ )
				{
					int Index = x + y * (g_BackLPitch / 4);
					Mem_Set_DWORD((DWORD *)g_BackBuffer+(unsigned int)Index, 0xFFFFFFFF,(unsigned int)1);
				}
			}
		}

		Unlock_Back_Surface();
		Flip();
	}
}

int Shutdown(void)
{
// this function release all the resources directdraw
// allocated, mainly to com objects
g_lpDD2->RestoreDisplayMode();

// release the secondary surface
if (g_lpDDSBack)
    g_lpDDSBack->Release();

// release the primary surface
if (g_lpDDSPrimary)
   g_lpDDSPrimary->Release();

// finally, the main dd object
if (g_lpDD2)
    g_lpDD2->Release();

// return success
return 1;
} // end Shutdown

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

		case WM_ACTIVATE:
			switch(LOWORD(wParam))
			{
				case WA_ACTIVE:
				case WA_CLICKACTIVE:
				g_bFocus = true;
				break;
			
				case WA_INACTIVE:
				g_bFocus = false;
				break;
			}
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