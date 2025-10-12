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

IDirect3DVertexDeclaration9 *g_pVertDecl;
ID3DXEffect* pEffect;

D3DXHANDLE              g_hTech;

LPDIRECT3DVERTEXBUFFER9 g_VB;
LPDIRECT3DINDEXBUFFER9 g_IB;

LPDIRECT3DTEXTURE9 g_pTexture;
LPDIRECT3DDEVICE9 g_d3d_Device;
LPDIRECT3D9 g_d3d;

struct MY_VERTEX
{
	float x,y,z,w;
	float tu,tv;

    static const DWORD FVF;
};

const DWORD MY_VERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

//matrix offset
enum {	M00, M01, M02, M03,
		M10, M11, M12, M13,
		M20, M21, M22, M23,
		M30, M31, M32, M33	};

MY_VERTEX VertBuff[24] = {
	-5.000000f,-5.000000f,-5.000000f,	1.0f,	1.0f,1.0f,
	-5.000000f,-5.000000f,-5.000000f,	1.0f,	1.0f,0.0f,
	5.000000f,-5.000000f,5.000000f,		1.0f,	0.0f,0.0f,
	5.000000f,-5.000000f,-5.000000f,	1.0f,	0.0f,1.0f,
	-5.000000f,5.000000f,-5.000000f,	1.0f,	0.0f,1.0f,
	5.000000f,5.000000f,-5.000000f,		1.0f,	1.0f,1.0f,
	5.000000f,5.000000f,5.000000f,		1.0f,	1.0f,0.0f,
	-5.000000f,5.000000f,5.000000f,		1.0f,	0.0f,0.0f,
	-5.000000f,-5.000000f,-5.000000f,	1.0f,	0.0f,1.0f,
	5.000000f,-5.000000f,-5.000000f,	1.0f,	1.0f,1.0f,
	5.000000f,5.000000f,-5.000000f,		1.0f,	1.0f,0.0f,
	-5.000000f,5.000000f,-5.000000f,	1.0f,	0.0f,0.0f,
	5.000000f,-5.000000f,-5.000000f,	1.0f,	0.0f,1.0f,
	5.000000f,-5.000000f,5.000000f,		1.0f,	1.0f,1.0f,
	5.000000f,5.000000f,5.000000f,		1.0f,	1.0f,0.0f,
	5.000000f,5.000000f,-5.000000f,		1.0f,	0.0f,0.0f,
	5.000000f,-5.000000f,5.000000f,		1.0f,	0.0f,1.0f,
	-5.000000f,-5.000000f,5.000000f,	1.0f,	1.0f,1.0f,
	-5.000000f,5.000000f,5.000000f,		1.0f,	1.0f,0.0f,
	5.000000f,5.000000f,5.000000f,		1.0f,	0.0f,0.0f,
	-5.000000f,-5.000000f,5.000000f,	1.0f,	0.0f,1.0f,
	-5.000000f,-5.000000f,-5.000000f,	1.0f,	1.0f,1.0f,
	-5.000000f,5.000000f,-5.000000f,	1.0f,	1.0f,0.0f,
	-5.000000f,5.000000f,5.000000f,		1.0f,	0.0f,0.0f };

MY_VERTEX VertBuffTransformed[24];

WORD IndexBuff[36] = {
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


typedef float matrix4x4[16];
		
void Vec4_Mat4x4_Mul(MY_VERTEX &VecOut, MY_VERTEX &VecIn, matrix4x4 Mat)
{
	VecOut.x =	VecIn.x * Mat[M00] +
				VecIn.y * Mat[M10] +
				VecIn.z * Mat[M20] +
				VecIn.w * Mat[M30];

	VecOut.y =	VecIn.x * Mat[M01] +
				VecIn.y * Mat[M11] +
				VecIn.z * Mat[M21] +
				VecIn.w * Mat[M31];

	VecOut.z =	VecIn.x * Mat[M02] +
				VecIn.y * Mat[M12] +
				VecIn.z * Mat[M22] +
				VecIn.w * Mat[M32];

	VecOut.w =	VecIn.x * Mat[M03] +
				VecIn.y * Mat[M13] +
				VecIn.z * Mat[M23] +
				VecIn.w * Mat[M33];

	VecOut.tu = VecIn.tu;
	VecOut.tv = VecIn.tv;
}

void Render_Scene()
{

	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	static float Angle = 0.0;

	matrix4x4 MatRotateY = {
		cosf(Angle),	0.0,	-sinf(Angle),	0.0,
		0.0,			1.0,	0.0,			0.0,
		sinf(Angle),	0.0,	cosf(Angle),	0.0,
		0.0,			0.0,	0.0,			1.0 };


	Angle = Angle + PI / 100.0f;
	if (Angle > PI2)
		Angle = 0.0f;

	//MATRIX WORLD
	matrix4x4 MatWorld = {
		1.0f, 0.0, 0.0, 0.0,
		0.0, 1.0f, 0.0, 0.0,
		0.0, 0.0, 1.0f, 0.0,
		0.0f, 0.0f, 20.0f, 1.0f }; 

	//MATRIX VIEW
	matrix4x4 MatView = {
		1.0f, 0.0, 0.0, 0.0,
		0.0, 1.0f, 0.0, 0.0,
		0.0, 0.0, 1.0f, 0.0,
		0.0f, 0.0f, 0.0f, 1.0f }; 


	//MATRIX PROJECTION CALCULATION
	float Fov = PI / 2.0f; // FOV 90 degree
	float Aspect = (float)Rc.right / Rc.bottom;
	float ZFar = 100.0f;
	float ZNear = 1.0f;

	float h, w, Q;

	w = (1.0f / tanf(Fov * 0.5f)) / Aspect;
	h = 1.0f / tanf(Fov * 0.5f);
	Q = ZFar / (ZFar - ZNear);

	//полный расчет матрицы проекции
	matrix4x4 MatProj = {
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, Q, 1,
		0, 0, -Q * ZNear, 0 };

	//куб имеет 24 вершины
	for ( int i = 0; i < 24; i++ )
	{
		MY_VERTEX Vec1;
		MY_VERTEX Vec2;

		Vec4_Mat4x4_Mul(Vec1, VertBuff[i], MatRotateY);
		Vec4_Mat4x4_Mul(Vec2, Vec1, MatWorld);
		Vec4_Mat4x4_Mul(Vec1, Vec2, MatView);
		Vec4_Mat4x4_Mul(Vec2, Vec1, MatProj);

			
		Vec2.x = Vec2.x / Vec2.w; //перспективное деление
		Vec2.y = Vec2.y / Vec2.w; //перспективное деление
		Vec2.z = Vec2.z / Vec2.w; //готовим Z для Z буфера значения от 0 до 1
		Vec2.w = 1.0f / Vec2.w; //готовим W для текстурирования с учетом
								//перспективы, если неправильно рассчитать
								//W то неправильно наложится текстура
								//например написать Vec2.w = Vec2.w;
								//не поделить на 1

		Vec2.x = Vec2.x * Rc.right / 2.0f + Rc.right / 2.0f;
		Vec2.y = -Vec2.y * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

		VertBuffTransformed[i] = Vec2;
	}

	MY_VERTEX* v = 0;
	g_VB->Lock(0, 0, (void**)&v, 0);

	memcpy(&v[0], &VertBuffTransformed[0], sizeof(MY_VERTEX) * 24);

	g_VB->Unlock();


	HRESULT hr;

	hr = g_d3d_Device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 60, 100, 150), 1.0f, 0 );
	if(FAILED(hr)) return;

	hr = g_d3d_Device->BeginScene();
	if(FAILED(hr)) return;

	pEffect->SetTechnique(g_hTech);

	UINT uPass = 0;
	hr = pEffect->Begin(&uPass, 0);
	if(FAILED(hr)) return;
	
	hr = pEffect->BeginPass(0);
	if(FAILED(hr)) return;

	g_d3d_Device->SetTexture( 0, g_pTexture );

	//g_d3d_Device->SetFVF( D3DVERTEX::FVF );
    g_d3d_Device->SetStreamSource( 0, g_VB, 0,  sizeof(MY_VERTEX) );
	g_d3d_Device->SetIndices(g_IB);
	g_d3d_Device->SetVertexDeclaration(g_pVertDecl);

	hr =g_d3d_Device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			24,
			0,
			12);

	hr = pEffect->EndPass();
	if(FAILED(hr)) return;

	hr = pEffect->End();
	if(FAILED(hr)) return;


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
	hr = g_d3d_Device->SetRenderState( D3DRS_ZENABLE, TRUE );
	if(FAILED(hr)) return false;
	hr = g_d3d_Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	if(FAILED(hr)) return false;

	
	hr = g_d3d_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	if(FAILED(hr)) return false;

	hr = g_d3d_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	if(FAILED(hr)) return false;
	
	hr = g_d3d_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	if(FAILED(hr)) return false;
	
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	D3DXCreateTextureFromFile( g_d3d_Device, "texture256.bmp" ,&g_pTexture );

	g_d3d_Device->CreateVertexBuffer(24 * sizeof(MY_VERTEX), D3DUSAGE_WRITEONLY,
		MY_VERTEX::FVF, D3DPOOL_MANAGED, &g_VB, 0);


	g_d3d_Device->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_IB, 0);


	WORD* k = 0;

	g_IB->Lock(0, 0, (void**)&k, 0);

	memcpy(&k[0], &IndexBuff[0], sizeof(WORD) * 36);

	/*
	//Bottom face.
	k[0] = 0; k[1] = 2; k[2] = 1;
	k[3] = 2; k[4] = 0; k[5] = 3;
	

	//Top face.
	k[6] = 4; k[7]  = 6; k[8]  = 5;
	k[9] = 6; k[10] = 4; k[11] = 7;
	


	//Front face.
	k[12] = 8; k[13] = 10; k[14] = 9;
	k[15] = 10; k[16] = 8; k[17] = 11;

	//Right face.
	k[18] = 12; k[19] = 14; k[20] = 13;
	k[21] = 14; k[22] = 12; k[23] = 15;


	//Back face.
	k[24] = 16; k[25] = 18; k[26] = 17;
	k[27] = 18; k[28] = 16; k[29] = 19;
	

	//Left face.
	k[30] = 20; k[31] = 22; k[32] = 21;
	k[33] = 22; k[34] = 20; k[35] = 23;
	
*/
	g_IB->Unlock();


//		HRESULT hr;
	ID3DXBuffer* errorBuffer = 0;

	hr = D3DXCreateEffectFromFile(g_d3d_Device, ".\\Shader\\textured.fx", 0, 0, 0, 0, &pEffect, &errorBuffer);
	if( errorBuffer )
	{
     ::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
	 errorBuffer->Release();
	}
	if(FAILED(hr)) return 0;

	g_hTech = pEffect->GetTechniqueByName("TexturedTech");
    //mhWVP  = pEffect->GetParameterByName(0, "gWVP");


	D3DVERTEXELEMENT9 decl[]=
   {
	   {0,	0, D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
	   {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},  
	   D3DDECL_END()
   };

   hr = g_d3d_Device->CreateVertexDeclaration(decl, &g_pVertDecl);
   if(FAILED(hr)) return 0;



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