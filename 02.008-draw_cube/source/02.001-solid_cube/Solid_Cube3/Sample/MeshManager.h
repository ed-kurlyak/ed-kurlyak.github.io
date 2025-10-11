//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <math.h>
#include <stdio.h>

#include <vfw.h>
#pragma comment(lib, "Vfw32.lib")

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

#define SWAP(a,b,t) {t=a; a=b; b=t;}

enum { A, B, C, D, E, F, G, H };

struct color_rgb
{
	int r;
	int g;
	int b;
};

typedef float matrix4x4[4][4];

struct vector3
{
	float x,y,z;
};

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();

	void Init_MeshManager(HWND hWnd);
	void Update_MeshManager();
	void Draw_MeshManager();

private:
	
	void Create_BackBuffer();
	void Clear_BackBuffer();
	void Present_BackBuffer();
	void Delete_BackBuffer();
	
	vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn);
	
	void Draw_Colored_Triangle(float x1,float y1,
					   float x2,float y2,
					   float x3,float y3);
	void Draw_Color_Poly(int y1, int y2);

	HWND m_hWnd;

	//контекст окна приложения
	HDC m_hDC;
	//размеры окна приложения
	UINT m_ViewWidth;
	UINT m_ViewHeight;
	//наш задний буфер
	HDC m_hBackBuffer;
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	//кисть для очистки заднего буфера (окна приложения)
	HBRUSH m_hBrush;
	//область для очистки заднего буфера (окна приложения)
	RECT m_Rc;

	UINT m_VertCount;
	UINT m_TriangleCount;
	
	vector3 *m_VertBuff;
	vector3 *m_VertBuffTransformed;
	DWORD *m_IndexBuff;
	color_rgb m_ColorBuff[12];
	color_rgb m_CurrColor;

	float m_dxl, m_dxr,
		m_xl, m_xr;
};

#endif