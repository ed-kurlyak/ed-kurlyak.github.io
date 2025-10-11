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

//для D3DXMatrixShadow
#include <D3dx9math.h>
#pragma comment (lib,"D3dx9.lib")

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

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

struct vector4
{
	float x,y,z,w;
};

struct plane
{
	float x,y,z,d;
};

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();

	void Init_MeshManager(HWND hWnd);
	void Update_MeshManager(bool bDrawShadow, float Angle);
	void Draw_MeshManager (bool bDrawShadow) ;

	void Clear_BackBuffer();
	void Present_BackBuffer();

private:

	void Create_BackBuffer();
	void Delete_BackBuffer();
	
	vector3 Vec3_Mat4x4_Mul(vector3 VecIn, matrix4x4 MatIn);
	vector4 Vec4_Mat4x4_Mul(vector4 VecIn, matrix4x4 MatIn);
	float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2);
	float Vec4_Dot(plane &VecIn1, vector4 &VecIn2);
	vector3 Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2);
	vector3 Vec3_Normalize(vector3 &VecIn);

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
	color_rgb *m_ColorBuff;

	float m_dxl, m_dxr,
		m_xl, m_xr;
};

#endif