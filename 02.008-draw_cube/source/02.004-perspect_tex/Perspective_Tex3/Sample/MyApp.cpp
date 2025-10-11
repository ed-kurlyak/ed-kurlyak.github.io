//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#include "MyApp.h"

int WINAPI WinMain(HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	CMyApp App;

	return App.Program_Begin(hInstance, nCmdShow);
}

LRESULT CALLBACK CMyApp::Static_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	if ( uMsg == WM_CREATE )
		SetWindowLong( hWnd, GWL_USERDATA, (LONG)(LONG_PTR)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	CMyApp *MainWndProc = (CMyApp*)(LONG_PTR)GetWindowLong( hWnd, GWL_USERDATA );
	if (MainWndProc) return MainWndProc->WndProc( hWnd, uMsg, wParam, lParam );
	
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

LRESULT CMyApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//проверяем сообщения
	switch (uMsg)
	{
		//мы получили сообщение закрыть приложение
		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

int CMyApp::Program_Begin(HINSTANCE	hInstance, int nCmdShow)
{
	WNDCLASS wcl;

	wcl.style			= CS_HREDRAW | CS_VREDRAW;;
	wcl.lpfnWndProc		= (WNDPROC) Static_WndProc;
	wcl.cbClsExtra		= 0L;
	wcl.cbWndExtra		= 0L;
	wcl.hInstance		= GetModuleHandle(NULL);
	wcl.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wcl.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcl.lpszMenuName	= NULL;
	wcl.lpszClassName	= CLASSNAME;
	
	if(!RegisterClass (&wcl)) return 0;

	m_hWnd = CreateWindow(CLASSNAME, APPNAME,
			  WS_OVERLAPPEDWINDOW,
              0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
              NULL, NULL, hInstance, this);

	if (!m_hWnd) return 0;
	
	RECT WindowRect = {0,0,WINDOW_WIDTH,WINDOW_HEIGHT};

	AdjustWindowRectEx(&WindowRect,
		GetWindowStyle(m_hWnd),
		GetMenu(m_hWnd) != NULL,
		GetWindowExStyle(m_hWnd));

	UINT WidthScreen = GetSystemMetrics(SM_CXSCREEN);
	UINT HeightScreen = GetSystemMetrics(SM_CYSCREEN);

	UINT WidthX = WindowRect.right - WindowRect.left;
	UINT WidthY = WindowRect.bottom - WindowRect.top;

	UINT PosX =  (WidthScreen - WidthX)/2;
	UINT PosY =  (HeightScreen - WidthY)/2;
	
	MoveWindow(m_hWnd,
		PosX,
        PosY,
        WidthX,
        WidthY,
        FALSE);

	p_d3d = Direct3DCreate9 (D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    //d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	if( FAILED( p_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, &p_d3d_Device ) ) )
    {
		MessageBox(NULL,"Error create device!", "Info",MB_OK);
		return false;
    }

	ShowWindow (m_hWnd, nCmdShow);
	UpdateWindow (m_hWnd);
	SetForegroundWindow(m_hWnd);

	m_MeshManager.Init_MeshManager(m_hWnd);

	MSG msg;

    while (TRUE)
    {
	    if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
		    if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
        
		if(GetKeyState(VK_ESCAPE) & 0xFF00)
			break;

		RenderScene();
	}
	
	DestroyWindow(m_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return (int) msg.wParam;
}

void CMyApp::RenderScene()
{

	HRESULT hr;

	hr = p_d3d_Device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 60, 100, 150), 1.0f, 0 );
	if(FAILED(hr)) return;

	hr = p_d3d_Device->BeginScene();
	if(FAILED(hr)) return;
	
	IDirect3DSurface9* pBackBuffer;
	p_d3d_Device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);

	D3DSURFACE_DESC pDesc;
	pBackBuffer->GetDesc(&pDesc);

	int iNumWidth  = pDesc.Width;
	int iNumHeight = pDesc.Height;

	D3DLOCKED_RECT lockedrect;
	::ZeroMemory(&lockedrect, sizeof(lockedrect));
	
	pBackBuffer->LockRect(&lockedrect,NULL,D3DLOCK_NOSYSLOCK);
	
	UCHAR *pSurfBits = static_cast<UCHAR*>(lockedrect.pBits);

	m_MeshManager.Update_MeshManager();
	m_MeshManager.Draw_MeshManager(pSurfBits, lockedrect.Pitch) ;

	pBackBuffer->UnlockRect();

	hr = p_d3d_Device->EndScene();
	if(FAILED(hr)) return;
    
    hr = p_d3d_Device->Present( NULL, NULL, NULL, NULL );
	if(FAILED(hr)) return;

}


