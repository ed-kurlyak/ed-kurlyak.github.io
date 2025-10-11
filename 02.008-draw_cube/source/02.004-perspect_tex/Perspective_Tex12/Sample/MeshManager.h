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

	float xs, ys;

	vector3 operator - (const vector3 &VecIn)
	{
		vector3 VecOut;
		
		VecOut.x = x - VecIn.x;
		VecOut.y = y - VecIn.y;
		VecOut.z = z - VecIn.z;
		
		return VecOut;
	}
};

struct color_rgb
{
	color_rgb() {};
	~color_rgb() {};

	color_rgb(int ir, int ig, int ib): r(ir), g(ig), b(ib) {};

	int r;
	int g;
	int b;

	color_rgb operator + (const color_rgb &ColorIn)
	{
		color_rgb ColorOut;
		
		ColorOut.r = r + ColorIn.r;
		ColorOut.g = g + ColorIn.g;
		ColorOut.b = b + ColorIn.b;
		
		return ColorOut;
	}

	color_rgb operator * (const float &ValIn)
	{
		color_rgb ColorOut;
		
		ColorOut.r = (int) (r * ValIn);
		ColorOut.g = (int) (g * ValIn);
		ColorOut.b = (int) (b * ValIn);
		
		return ColorOut;
	}
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
	
	void Read_BMP_File(char *szTexFileName);
	
	float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2);
	matrix4x4 Mat4x4_Mat4x4_Mul(matrix4x4 &MatIn1, matrix4x4 &MatIn2);
	vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 &MatIn);
	vector3 Vec3_Normalize(vector3 &VecIn);
	
	void Draw_Textured_Triangle(vector3 VecIn1, tex_coord2 TexIn1, color_rgb Color1,
						  vector3 VecIn2, tex_coord2 TexIn2, color_rgb Color2,
						  vector3 VecIn3, tex_coord2 TexIn3, color_rgb Color3);
	void Draw_Textured_Poly(int y1, int y2);

	HWND m_hWnd;

	UCHAR *m_Res;
	
	UINT m_VertCount;
	UINT m_TriangleCount;

	BITMAPINFOHEADER m_Bih;
	HDRAWDIB m_hDD;
	LPBYTE m_Data;
	LPBYTE m_DataTemp;

	ULONG m_ViewWidth;
	ULONG m_ViewHeight;

	vector3 *m_VertBuff;
	vector3 *m_VertBuffTransformed;
	tex_coord2 *m_TexCoordBuff;
	DWORD *m_IndexBuff;
	vector3 *m_NormalBuff;
	vector3 *m_NormalBuffTransformed;


	//начальная позиция для левой стороны треугольника
	float m_ul, m_vl, m_zl, m_xl;
	//приращение координат для левой стороны треугольника
	float  m_dudyl, m_dvdyl, m_dzdyl, m_dxdyl;

	//начальная позиция для правой стороны треугольника
	float m_ur, m_vr, m_zr, m_xr;
	//приращение координат для правой стороны треугольника
	float m_dudyr, m_dvdyr, m_dzdyr, m_dxdyr;

	//начальный цвет и приращение для левой и правой сторон треугольника
	float m_dredl, m_dgreenl, m_dbluel,
		m_dredr, m_dgreenr, m_dbluer,
		m_redl, m_greenl, m_bluel,
		m_redr, m_greenr, m_bluer;

	unsigned short RGB_Table[65536]; //256 * 256

	UINT m_TextureWidth;
	UINT m_TextureHeight;

};

#endif