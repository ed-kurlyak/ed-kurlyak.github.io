//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>

#include <d3dx9.h>
#include <math.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

HWND g_hWnd;

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

LPDIRECT3DTEXTURE9 g_pTexture;

LPDIRECT3DDEVICE9 g_d3d_Device;

LPDIRECT3D9 g_d3d;

struct MY_VERTEX
{
    D3DXVECTOR4 p;
	D3DXVECTOR2 t;

    static const DWORD FVF;
};

const DWORD MY_VERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

MY_VERTEX g_VertBuffTransformed[24];

MY_VERTEX g_VertBuff[24] = {
D3DXVECTOR4(-5.000000f,-5.000000f,-5.000000f,	1.0f),	D3DXVECTOR2(1.0f,1.0f),
D3DXVECTOR4(-5.000000f,-5.000000f,-5.000000f,	1.0f),	D3DXVECTOR2(1.0f,0.0f),
D3DXVECTOR4(5.000000f,-5.000000f,5.000000f,		1.0f),	D3DXVECTOR2(0.0f,0.0f),
D3DXVECTOR4(5.000000f,-5.000000f,-5.000000f,	1.0f),	D3DXVECTOR2(0.0f,1.0f),
D3DXVECTOR4(-5.000000f,5.000000f,-5.000000f,	1.0f),	D3DXVECTOR2(0.0f,1.0f),
D3DXVECTOR4(5.000000f,5.000000f,-5.000000f,		1.0f),	D3DXVECTOR2(1.0f,1.0f),
D3DXVECTOR4(5.000000f,5.000000f,5.000000f,		1.0f),	D3DXVECTOR2(1.0f,0.0f),
D3DXVECTOR4(-5.000000f,5.000000f,5.000000f,		1.0f),	D3DXVECTOR2(0.0f,0.0f),
D3DXVECTOR4(-5.000000f,-5.000000f,-5.000000f,	1.0f),	D3DXVECTOR2(0.0f,1.0f),
D3DXVECTOR4(5.000000f,-5.000000f,-5.000000f,	1.0f),	D3DXVECTOR2(1.0f,1.0f),
D3DXVECTOR4(5.000000f,5.000000f,-5.000000f,		1.0f),	D3DXVECTOR2(1.0f,0.0f),
D3DXVECTOR4(-5.000000f,5.000000f,-5.000000f,	1.0f),	D3DXVECTOR2(0.0f,0.0f),
D3DXVECTOR4(5.000000f,-5.000000f,-5.000000f,	1.0f),	D3DXVECTOR2(0.0f,1.0f),
D3DXVECTOR4(5.000000f,-5.000000f,5.000000f,		1.0f),	D3DXVECTOR2(1.0f,1.0f),
D3DXVECTOR4(5.000000f,5.000000f,5.000000f,		1.0f),	D3DXVECTOR2(1.0f,0.0f),
D3DXVECTOR4(5.000000f,5.000000f,-5.000000f,		1.0f),	D3DXVECTOR2(0.0f,0.0f),
D3DXVECTOR4(5.000000f,-5.000000f,5.000000f,		1.0f),	D3DXVECTOR2(0.0f,1.0f),
D3DXVECTOR4(-5.000000f,-5.000000f,5.000000f,	1.0f),	D3DXVECTOR2(1.0f,1.0f),
D3DXVECTOR4(-5.000000f,5.000000f,5.000000f,		1.0f),	D3DXVECTOR2(1.0f,0.0f),
D3DXVECTOR4(5.000000f,5.000000f,5.000000f,		1.0f),	D3DXVECTOR2(0.0f,0.0f),
D3DXVECTOR4(-5.000000f,-5.000000f,5.000000f,	1.0f),	D3DXVECTOR2(0.0f,1.0f),
D3DXVECTOR4(-5.000000f,-5.000000f,-5.000000f,	1.0f),	D3DXVECTOR2(1.0f,1.0f),
D3DXVECTOR4(-5.000000f,5.000000f,-5.000000f,	1.0f),	D3DXVECTOR2(1.0f,0.0f),
D3DXVECTOR4(-5.000000f,5.000000f,5.000000f,		1.0f),	D3DXVECTOR2(0.0f,0.0f ) };

WORD g_IndexBuff[36] = {
		0,2,1, 		// 1 triangle
		2,0,3,		// 2 triangle
		4,6,5,		// 3 triangle
		6,4,7,		// 4 triangle
		8,10,9,		// 5 triangle
		10,8,11,	// 6 triangle	
		12,14,13,	// 7 triangle
		14,12,15,	// 8 triangle
		16,18,17,	// 9 triangle
		18,16,19,	// 10 triangle
		20,22,21,	// 11 triangle
		22,20,23};	// 12 triangle


void Render_Scene()
{

	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	static float Angle = 0.0;

	D3DXMATRIX  MatRotateY = D3DXMATRIX (
		cosf(Angle),	0.0,	-sinf(Angle),	0.0,
		0.0,			1.0,	0.0,			0.0,
		sinf(Angle),	0.0,	cosf(Angle),	0.0,
		0.0,			0.0,	0.0,			1.0 );

	Angle = Angle + PI / 100.0f;
	if (Angle > PI2)
		Angle = 0.0f;

	//MATRIX WORLD
	D3DXMATRIX MatWorld = D3DXMATRIX(
		1.0f, 0.0, 0.0, 0.0,
		0.0, 1.0f, 0.0, 0.0,
		0.0, 0.0, 1.0f, 0.0,
		0.0f, 0.0f, 20.0f, 1.0f ); 

	//MATRIX VIEW
	D3DXMATRIX MatView = D3DXMATRIX(
		1.0f, 0.0, 0.0, 0.0,
		0.0, 1.0f, 0.0, 0.0,
		0.0, 0.0, 1.0f, 0.0,
		0.0f, 0.0f, 0.0f, 1.0f ); 

	//MATRIX PROJECTION CALCULATION
	float Fov = PI / 2.0f; // FOV 90 degree
	float Aspect = (float)Rc.right / Rc.bottom;
	float ZFar = 100.0f;
	float ZNear = 1.0f;

	float h, w, Q;

	w = (1.0f / tanf(Fov * 0.5f)) / Aspect;
	h = 1.0f / tanf(Fov * 0.5f);
	Q = ZFar / (ZFar - ZNear);

	D3DXMATRIX MatProj = D3DXMATRIX(
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, Q, 1,
		0, 0, -Q * ZNear, 0 );
	
	//cube model has 24 vertices with tex coords
	for ( int i = 0; i < 24; i++ )
	{
		MY_VERTEX Vec1;
		MY_VERTEX Vec2;

		D3DXVec4Transform(&Vec1.p, &g_VertBuff[i].p, &MatRotateY);
		D3DXVec4Transform(&Vec2.p, &Vec1.p, &MatWorld);
		D3DXVec4Transform(&Vec1.p, &Vec2.p, &MatView);
		D3DXVec4Transform(&Vec2.p, &Vec1.p, &MatProj);
			
		Vec2.p.x = Vec2.p.x / Vec2.p.w; //перспективное деление
		Vec2.p.y = Vec2.p.y / Vec2.p.w; //перспективное деление
		Vec2.p.z = Vec2.p.z / Vec2.p.w; //готовим Z для Z буфера значения от 0 до 1
		Vec2.p.w = 1.0f / Vec2.p.w; //готовим W для текстурирования с учетом
								//перспективы, если неправильно рассчитать
								//W то неправильно наложится текстура
								//например написать Vec2.w = Vec2.w;
								//не поделить на 1

		Vec2.p.x = Vec2.p.x * Rc.right / 2.0f + Rc.right / 2.0f;
		Vec2.p.y = -Vec2.p.y * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

		Vec2.t.x = g_VertBuff[i].t.x;
		Vec2.t.y = g_VertBuff[i].t.y;

		g_VertBuffTransformed[i] = Vec2;
	}

	HRESULT hr;

	hr = g_d3d_Device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 60, 100, 150), 1.0f, 0 );
	if(FAILED(hr)) return;

	hr = g_d3d_Device->BeginScene();
	if(FAILED(hr)) return;

	g_d3d_Device->SetTexture(0, g_pTexture);

	g_d3d_Device->SetFVF( MY_VERTEX::FVF );

	g_d3d_Device->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST,0,24,12, g_IndexBuff,D3DFMT_INDEX16, g_VertBuffTransformed, sizeof(MY_VERTEX));

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

	HRESULT hr;

	hr = g_d3d_Device->SetRenderState( D3DRS_LIGHTING, FALSE );
	if(FAILED(hr)) return false;
	hr = g_d3d_Device->SetRenderState( D3DRS_ZENABLE, FALSE );
	if(FAILED(hr)) return false;
	hr = g_d3d_Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	if(FAILED(hr)) return false;
	hr = g_d3d_Device->SetRenderState( D3DRS_DITHERENABLE, TRUE );
	if(FAILED(hr)) return false;

	hr = g_d3d_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	if(FAILED(hr)) return false;
	hr = g_d3d_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	if(FAILED(hr)) return false;

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	D3DXCreateTextureFromFile( g_d3d_Device, "texture256.bmp" ,&g_pTexture );

	MSG Msg;

	while(true)
	{
		if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			if(Msg.message ==	WM_QUIT)
				break;
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}

		if(GetKeyState(VK_ESCAPE) & 0xFF00)
			break;

		Render_Scene();
	}

	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return (int)Msg.wParam;
}