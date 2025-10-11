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

enum { A, B, C, D, E, F, G, H };

struct matrix4x4
{
	float Mat[4][4];
};

struct vector3
{
	float x,y,z;

	float xs, ys;

	vector3() {};
	vector3(float ix, float iy, float iz)
	{
		x = ix;
		y = iy;
		z = iz;
	};

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

	void Draw_Color_Triangle(vector3 VecIn1,
							vector3 VecIn2,
							vector3 VecIn3,
					   vector3 Normal1,
					   vector3 Normal2,
					   vector3 Normal3 );
	void Draw_Color_Poly(int y1, int y2);

	HWND m_hWnd;

	UINT m_VertCount;
	UINT m_TriangleCount;
	
	ULONG m_ViewWidth;
	ULONG m_ViewHeight;

	vector3 *m_VertBuffTransformed;
	vector3 *m_VertBuff;
	WORD *m_IndexBuff;
	vector3 *m_NormalBuff;
	vector3 *m_NormalBuffTransformed;

	BITMAPINFOHEADER m_Bih;
	HDRAWDIB m_hDD;
	LPBYTE m_Data;

	float m_dnxl, m_dnyl, m_dnzl,
		m_dnxr, m_dnyr, m_dnzr,
		m_nxl, m_nyl, m_nzl,
		m_nxr, m_nyr, m_nzr,

		m_dvWxl, m_dvWyl, m_dvWzl,
		m_dvWxr, m_dvWyr, m_dvWzr,
		m_vWxl, m_vWyl, m_vWzl,
		m_vWxr, m_vWyr, m_vWzr,

		m_dxl, m_dxr, m_xl, m_xr;
};

#endif