//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct tex_coord2
{
	int tu, tv;
};

struct vector3
{
	float x,y,z;
};

HWND g_hWnd;
UCHAR *m_pRes;

void Read_BMP_File(char *szTexFileName)
{
	FILE *fp;

	fopen_s(&fp, szTexFileName,"rb");
	if(fp==NULL) printf("Error Open File");

	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(bfh), 1, fp);

	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(bih), 1, fp);
	
	m_pRes = new unsigned char [bih.biWidth*bih.biHeight*4];
	fread(m_pRes,bih.biWidth*bih.biHeight*4,1,fp);

	fclose(fp);
}

void Draw_Textured_Triangle(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3)
{
	//деление на Z
	float iz1 = 1.0f / VecIn1.z;
	float iu1 = TexIn1.tu / VecIn1.z;
	float iv1 = TexIn1.tv / VecIn1.z;
	
	float iz2 = 1.0f / VecIn2.z;
	float iu2 = TexIn2.tu / VecIn2.z;
	float iv2 = TexIn2.tv / VecIn2.z;
	
	float iz3 = 1.0f / VecIn3.z;
	float iu3 = TexIn3.tu / VecIn3.z;
	float iv3 = TexIn3.tv / VecIn3.z;

	//начальная координата для левой стороны треугольника
	float zl = iz1;
	float ul = iu1;
	float vl = iv1;
	float xl = VecIn1.x;

	//приращение для левой стороны треугольника
	float dzdyl = (iz3 - iz1) / (VecIn3.y - VecIn1.y);
	float dudyl = (iu3 - iu1) / (VecIn3.y - VecIn1.y);
	float dvdyl = (iv3 - iv1) / (VecIn3.y - VecIn1.y);
	float dxdyl = (VecIn3.x - VecIn1.x) / (VecIn3.y - VecIn1.y);

	//начальная координата для правой стороны треугольника
	float zr = iz1;
	float ur = iu1;
	float vr = iv1;
	float xr = VecIn1.x;

	//приращение для правой стороны треугольника
	float dzdyr = (iz2 - iz1) / (VecIn2.y - VecIn1.y);
	float dudyr = (iu2 - iu1) / (VecIn2.y - VecIn1.y);
	float dvdyr = (iv2 - iv1) / (VecIn2.y - VecIn1.y);
	float dxdyr = (VecIn2.x - VecIn1.x) / (VecIn2.y - VecIn1.y);
	
	HDC hDC = GetDC(g_hWnd);

	int x1, x2;
	int dx;
	float ui, vi, zi;
	float du, dv, dz;

	for ( int yi = (int) VecIn1.y; yi < (int)VecIn2.y; yi++ )
	{
		x1 = (int) xl;
		x2 = (int) xr;

		//интерполяция по X координате
		dx = (x2 - x1);
		if(dx > 0) // делить на 0 нельзя
		{
			du = (ur - ul)/dx;
			dv = (vr - vl)/dx;
			dz = (zr - zl)/dx;
		}
		else
		{
			du = ur - ul;
			dv = vr - vl;
			dz = zr - zl;
		}

		int xln = (int)xl;

		float dxt = 1 - (xl - xln);

		zi = zl + dxt * dz;
		ui = ul + dxt * du;
		vi = vl + dxt * dv;

		for (int xi=x1; xi<x2; xi++)
		{
			float z = 1.0f/zi;
			float u = ui * z;
			float v = vi * z;

			UINT t = (int)u  + (((int)v) << 8);

			t = t*3;
			
			SetPixel(hDC, xi, yi, RGB(m_pRes[t + 2],m_pRes[t + 1],m_pRes[t + 0]));
				
			ui+=du;
			vi+=dv;
			zi+=dz;
		}

		xl+=dxdyl;
		ul+=dudyl;
		vl+=dvdyl;
		zl+=dzdyl;

		xr+=dxdyr;
		ur+=dudyr;
		vr+=dvdyr;
		zr+=dzdyr;
	}

	ReleaseDC(g_hWnd, hDC);
}

void Render_Scene()
{
	vector3 Vec1; tex_coord2 Tex1;
    vector3 Vec2; tex_coord2 Tex2;
    vector3 Vec3; tex_coord2 Tex3;

	/*вершины треугольника

				v1(400,100) t(255,0)

		v3(200,300) t(0,255)	v2(600,300) t(255,255)
	*/

	//экранные координаты
	Vec1.x = 400.0; Vec1.y = 100.0; Vec1.z = 12.0; 
	Tex1.tu = 255; Tex1.tv = 0;

	//экранные координаты
	Vec2.x = 600.0; Vec2.y = 300.0; Vec2.z = 12.0;
	Tex2.tu = 255; Tex2.tv = 255;

	//экранные координаты
	Vec3.x = 200.0; Vec3.y = 300.0; Vec3.z = 12.0;
	Tex3.tu = 0; Tex3.tv =  255;

	Draw_Textured_Triangle(Vec1, Tex1, Vec2, Tex2, Vec3, Tex3 );

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

	Read_BMP_File(".//texture.bmp");
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