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

// Subdivision span-size
#define SUBDIVSHIFT	4
#define SUBDIVSIZE	(1 << SUBDIVSHIFT)

#define SWAP(a,b,t) { t = a; a = b; b = t; }

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

	void Draw_Textured_Triangle_Perspective(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3);
	void Draw_Textured_Poly_Perspective(int y1, int y2);

	void Draw_Textured_Triangle_Affine(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3);
	void Draw_Textured_Poly_Affine(int y1, int y2);

	void Draw_Textured_Triangle_Perspective_DivSub(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3);
	void Draw_Textured_Poly_Perspective_DivSub(int y1, int y2);


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

	//for triangle rasterization (persp, subdiv, affine)
	float m_dxdyl, m_dxdyr;
	float m_xl, m_xr;

	//for perspective
	float m_dizdx, m_duizdx, m_dvizdx, m_dizdy, m_duizdy, m_dvizdy;
	float m_izl, m_uizl, m_vizl;
	float m_dizdyl, m_duizdyl, m_dvizdyl;
	//for perspective sub div
	float dizdxn, duizdxn, dvizdxn;

	//for affine
	float dudx, dvdx, dudy, dvdy;
	float m_ul, m_vl;
	float m_dudyl, m_dvdyl;

	UINT m_TextureWidth;
	UINT m_TextureHeight;

};

#endif