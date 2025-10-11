//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <cmath>

#include ".\scitech\INCLUDE\MGRAPH.H"

#pragma comment (lib, ".\\scitech\\LIB\\WIN32\\VC\\MGLLT.LIB")
#pragma comment(lib, "winmm.lib")

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

#define SWAP(a,b,t) { t = a; a = b; b = t; }

// структура вершины
struct Vertex
{
    float x, y, z;
    float s, t;
};

// структура для интерполяции по строке
struct Edge
{
    float x, s_over_z, t_over_z, one_over_z;
};


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

	void Init_MeshManager(HWND hWnd, HINSTANCE hInstance);
	void Update_MeshManager();
	void Draw_MeshManager();

private:
	
	void Create_BackBuffer();
	void Create_Palette();
	void Clear_BackBuffer();
	void Present_BackBuffer();
	void Delete_BackBuffer();
	
	void Read_BMP_File(char *szTexFileName);
	
	vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 &MatIn);

	void Draw_Textured_Triangle(vector3 VecIn1, tex_coord2 Tex1,
										  vector3 VecIn2, tex_coord2 Tex2,
										  vector3 VecIn3, tex_coord2 Tex3);
	Edge InterpolateEdge(const Vertex& a, const Vertex& b, float y);
	void DrawSpan(int y, Edge left, Edge right);
	
	HWND m_hWnd;
	HINSTANCE m_hInst;

	UCHAR *m_Res;
	
	UINT m_VertCount;
	UINT m_TriangleCount;

	MGLDC* windc;
	MGLDC* dibdc;

	unsigned char * m_BackBuffer;

	int m_ViewWidth;
	int m_ViewHeight;

	vector3 *m_VertBuff;
	vector3 *m_VertBuffTransformed;
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

	UINT m_TextureWidth;
	UINT m_TextureHeight;
	RGBQUAD RgbPal[256];

};

#endif