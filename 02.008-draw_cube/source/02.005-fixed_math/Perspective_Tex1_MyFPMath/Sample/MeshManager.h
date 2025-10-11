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

#define PHD_360 65536
#define PHD_180 32768
#define PHD_90 16384
#define TABLE_SIZE 1024
#define ANGLE_STEP (PHD_360 / TABLE_SIZE) // 64
#define FIXED_SCALE (1 << 14) // 16384

#define One  (256.0f * 8.0f * 16384.0f)
//#define One 8589934592.0f

struct matrix4x4
{
	int Mat[4][4];
};

struct tex_coord2
{
	int tu, tv;
};

struct vector3
{
	int x,y,z;
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

	void Draw_Textured_Triangle(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3);
	void Draw_Textured_Poly(int y1, int y2);

	int Math_Cos(int Angle);
	int Math_Sin(int Angle);
	void Build_Cos_Table();
	int AngleToIndex(int angle);
	short int cos_table[TABLE_SIZE];

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

	//начальная позиция для левой стороны треугольника
	int m_xl;
	float m_ul, m_vl, m_zl;
	//приращение координат для левой стороны треугольника
	int  m_dxdyl;
	float m_dudyl, m_dvdyl, m_dzdyl; 

	//начальная позиция для правой стороны треугольника
	int m_xr;
	float m_ur, m_vr, m_zr;
	//приращение координат для правой стороны треугольника
	int m_dxdyr;
	float m_dudyr, m_dvdyr, m_dzdyr;

	UINT m_TextureWidth;
	UINT m_TextureHeight;

};

#endif