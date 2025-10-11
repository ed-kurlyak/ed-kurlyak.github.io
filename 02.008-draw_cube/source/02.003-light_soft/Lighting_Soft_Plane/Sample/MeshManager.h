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

#define BITS_PER_PIXEL	32

#define SWAP(a,b,t) {t=a; a=b; b=t;}

struct matrix4x4
{
	float Mat[4][4];
};

struct vector3
{
	float x,y,z;

	float xs, ys;

	float xw, yw;

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

	void Init_MeshManager(HWND hWnd);
	void Update_MeshManager();
	void Draw_MeshManager() ;

private:
	
	void Create_BackBuffer();
	void Clear_BackBuffer();
	void Present_BackBuffer();
	void Delete_BackBuffer();
	
	matrix4x4 Mat4x4_Mat4x4_Mul(matrix4x4 &MatIn1, matrix4x4 &MatIn2);
	vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn);
	float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2);
	vector3 Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2);
	vector3 Vec3_Normalize(vector3 &VecIn);
	float Vec3_Len(vector3 &VecIn);
	float Calc_Attenuation(float d, float FalloffStart, float FalloffEnd);
	float Max(float a, float b);
	void Build_Side(vector3 * VertBuff, unsigned int * Indices);

	void Draw_Color_Triangle(float x1,float y1,
					   float x2,float y2,
					   float x3,float y3,
					   color_rgb Color1,
					   color_rgb Color2,
					   color_rgb Color3);
	void Draw_Color_Poly(int y1, int y2);

	HWND m_hWnd;

	UINT m_VertCount;
	UINT m_TriangleCount;

	ULONG m_ViewWidth;
	ULONG m_ViewHeight;

	vector3 *m_VertBuffTransformed;
	vector3 *m_VertBuff;
	unsigned int *m_IndexBuff;

	BITMAPINFOHEADER m_Bih;
	HDRAWDIB m_hDD;
	LPBYTE m_Data;

	float m_dredl, m_dgreenl, m_dbluel,
		m_dredr, m_dgreenr, m_dbluer,
		m_redl, m_greenl, m_bluel,
		m_redr, m_greenr, m_bluer,
		m_dxl, m_dxr, m_xl, m_xr;

	UINT m_NumCellsPerRow;
	UINT m_NumCellsPerCol;
	float m_CellSpacing;

	UINT m_NumVertsPerRow;
	UINT m_NumVertsPerCol;
};

#endif