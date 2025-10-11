//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

#define SWAP(a,b,t) { t = a; a = b; b = t; }

#define MAX_SCANLINES 1200 //maximum height of screen

struct matrix4x4
{
	float Mat[4][4];
};

struct tex_coord2
{
	int tu, tv;
};

struct vector3
{
	float x,y,z;
};

struct XBUF_PERSP_FP
{
			int	XLeft;
			float UOZleft;
			float VOZleft;
			float OOZleft;
			int	XRight;
			float UOZright;
			float VOZright;
			float OOZright;
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
	
	void Read_BMP_File(char *szTexFileName);
	
	vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 &MatIn);

	int Generate_Slopes(int Side);
	void Draw_Textured_Triangle(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3);
	void Draw_Textured_Poly(int y1, int y2);

	HWND m_hWnd;

	UCHAR *m_Res;
	
	UINT m_VertCount;
	UINT m_TriangleCount;

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

	vector3 *m_VertBuff;
	vector3 *m_VertBuffTransformed;
	tex_coord2 *m_TexCoordBuff;
	DWORD *m_IndexBuff;

	XBUF_PERSP_FP 	XBuffer[MAX_SCANLINES];	// Edge buffer for Polygon
	
	float x1, x2, x3;
	float y1, y2, y3;
	float iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;

	UINT m_TextureWidth;
	UINT m_TextureHeight;

};

#endif