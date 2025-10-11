//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>

#include <d3dx9.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

HWND g_hWnd;
LPDIRECT3DDEVICE9 g_d3d_Device;
LPDIRECT3D9 g_d3d;

void Draw_Line(UCHAR *BackBuffer, int BackPitch)
{
	//проводим линию шириной 400 пикслей
	//координатами от x = 100 до x = 500
	//по высоте от y = 150 до y = 160

	UCHAR *ScreenPtr  = NULL;
	ScreenPtr = BackBuffer + (150 * BackPitch);

	for ( int y = 150; y < 160; y++ )
	{
		int Indx =  100 * 4;

		for ( int x = 100; x < 500; x++ )
		{
			ScreenPtr[Indx + 0] = 127; //blue
			ScreenPtr[Indx + 1] = 255; //green
			ScreenPtr[Indx + 2] = 255; //red
			ScreenPtr[Indx + 3] = 0;

			Indx += 4;
		}

		ScreenPtr+= BackPitch;
	}

}

void Render_Scene()
{

	HRESULT hr;

	hr = g_d3d_Device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 60, 100, 150), 1.0f, 0 );
	if(FAILED(hr)) return;

	hr = g_d3d_Device->BeginScene();
	if(FAILED(hr)) return;
	
	IDirect3DSurface9* pBackBuffer;
	g_d3d_Device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);

	D3DSURFACE_DESC pDesc;
	pBackBuffer->GetDesc(&pDesc);

	//int NumWidth  = pDesc.Width;
	//int NumHeight = pDesc.Height;

	D3DLOCKED_RECT LockedRect;
	::ZeroMemory(&LockedRect, sizeof(LockedRect));
	
	pBackBuffer->LockRect(&LockedRect,NULL,D3DLOCK_NOSYSLOCK);
	
	UCHAR *pSurfBits = static_cast<UCHAR*>(LockedRect.pBits);

	UCHAR *ScreenPtr  = NULL;
	ScreenPtr = pSurfBits + (150 * LockedRect.Pitch);

	Draw_Line(pSurfBits, LockedRect.Pitch) ;

	pBackBuffer->UnlockRect();

	hr = g_d3d_Device->EndScene();
	if(FAILED(hr)) return;
    
    hr = g_d3d_Device->Present( NULL, NULL, NULL, NULL );
	if(FAILED(hr)) return;


}

LRESULT CALLBACK WndProc(HWND g_hWnd,
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
			return DefWindowProc(g_hWnd, uMsg, wParam, lParam);
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

	if(!RegisterClass(&wcl))
		return 0;

	g_hWnd = CreateWindow("Sample", "Software Rendering DirectX9 Backbuffer Demo",
					WS_OVERLAPPEDWINDOW,
					0, 0,
					640, 480,
					NULL,
					NULL,
					hInstance,
					NULL);
	if(!g_hWnd)
		return 0;

	g_d3d = Direct3DCreate9 (D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	if( FAILED( g_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_d3d_Device ) ) )
    {
		MessageBox(NULL,"Error create device!", "Info",MB_OK);
		return false;
    }


	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

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

	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return (int)msg.wParam;
}