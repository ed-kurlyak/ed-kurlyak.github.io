//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <math.h>
#include <stdio.h>

#include <vfw.h> //для DrawDibDraw
#pragma comment(lib, "Vfw32.lib") //для DrawDibDraw

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

#define SWAP(a,b,t) { t = a; a = b; b = t; }

#define BITS_PER_PIXEL	32

typedef float matrix4x4[16];

//matrix offset
enum {	M00, M01, M02, M03,
		M10, M11, M12, M13,
		M20, M21, M22, M23,
		M30, M31, M32, M33 };

struct tex_coord2
{
	int tu, tv;
};

struct vector3
{
	union
	{
		float Vec[3];
		struct
		{
			float x,y,z;
		};
	};
};

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();

	void Init_MeshManager(HWND hwnd);
	void Update_MeshManager();
	void Draw_MeshManager();

private:
	
	void Create_BackBuffer();
	void Clear_BackBuffer();
	void Present_BackBuffer();
	void Delete_BackBuffer();

	vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn);

	void Read_BMP_File(char *szTexFileName);

	void Draw_Textured_Triangle(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3);
	void Draw_Textured_Poly(int y1, int y2);

	HWND m_hWnd;

	UCHAR *m_Res;
	UINT m_TextureWidth;
	UINT m_TextureHeight;

	UINT m_VertCount;
	UINT m_TriangleCount;

	BITMAPINFOHEADER m_Bih;
	HDRAWDIB m_hDD;
	LPBYTE m_Data;
	LPBYTE m_DataTemp;

	ULONG m_ViewWidth;
	ULONG m_ViewHeight;
	
	vector3 *m_VertBuffTransformed;
	vector3 *m_VertBuff;
	tex_coord2 *m_TexCoordBuff;
	DWORD *m_IndexBuff;

	//начальная позиция для левой стороны треугольника
	float m_ul, m_vl, m_zl, m_xl;
	//приращение координат для левой стороны треугольника
	float  m_dudyl, m_dvdyl, m_dzdyl, m_dxdyl;

	//начальная позиция для правой стороны треугольника
	float m_ur, m_vr, m_zr, m_xr;
	//приращение координат для правой стороны треугольника
	float m_dudyr, m_dvdyr, m_dzdyr, m_dxdyr;
};

#endif