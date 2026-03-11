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

#define BITS_PER_PIXEL	32

#define SWAP(a,b,t) {t=a; a=b; b=t;}

#define MAX_POLYGONS 4000

struct color_rgb
{
	int r;
	int g;
	int b;

	color_rgb() {};
	color_rgb(float ir, float ig, float ib)
	{
		r = (int)ir;
		g = (int)ig;
		b = (int)ib;
	};
};

struct matrix4x4
{
	float Mat[4][4];
};

struct vector3
{
	float x,y,z;

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
	
	vector3 &operator = (const vector3 &VecIn)
	{
		x = VecIn.x;
		y = VecIn.y;
		z = VecIn.z;

		return *this;
	}

	vector3 operator * (const float ValIn)
	{
		vector3 VecOut;

		VecOut.x = x * ValIn;
		VecOut.y = y * ValIn;
		VecOut.z = z * ValIn;
		
		return VecOut;
	};

	vector3 operator + (const vector3 &VecIn)
	{
		vector3 VecOut;
		VecOut.x = x + VecIn.x;
		VecOut.y = y + VecIn.y;
		VecOut.z = z + VecIn.z;
		return VecOut;
	};

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
	
	void Create_Backbuffer();
	void Clear_BackBuffer();
	void Present_BackBuffer();
	void Delete_Backbuffer();

	void Update(vector3 *VecIn1, vector3 *VecIn2, float Scalar);
	
	matrix4x4 Mat4x4_Mat4x4_Mul(matrix4x4 &MatIn1, matrix4x4 &MatIn2);
	vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn);
	
	void Draw_Color_Triangle(float x1,float y1,
					   float x2,float y2,
					   float x3,float y3,
					   color_rgb Color );
	void Draw_Color_Poly(int y1, int y2, color_rgb Color);

	void Build_Side(vector3 *VertBuff, unsigned int *Indices);

	matrix4x4 MatrixRotationX(float Angle);
	matrix4x4 MatrixRotationZ(float Angle);

	void Do_QuickySorty(int Left, int Right, int Buffer[][2]);
	void SortPolyList(int Number,int Buffer[][2]);

	vector3 Calc_Reflect(vector3 Normal, vector3 VecIn);
	vector3 Vec3_Normalize(vector3 &VecIn);
	float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2);
	vector3 Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2);

	HWND m_hWnd;
	
	UINT m_VertCount;
	UINT m_TriangleCount;

	ULONG m_ViewWidth;
	ULONG m_ViewHeight;
	
	BITMAPINFOHEADER m_Bih;
	HDRAWDIB m_hDD;
	LPBYTE m_Data;
	
	//начальная точка
	float m_xl, m_xr;
	//смещение
	float m_dxl, m_dxr;

	vector3 *m_VertBuffTransformed;

	vector3 * m_CubeVertsArray;
	vector3 * m_SphereVertsArray;
	vector3 * m_DistorSphereVertsArray;
	vector3 * m_DistorCubeVertsArray;
	
	vector3 * m_ResultVertsArray;

	color_rgb * m_ColorArray;
	unsigned int * m_IndicesArray;

	UINT m_NumCellsPerRow;
	UINT m_NumCellsPerCol;
	float m_CellSpacing;

	UINT m_NumVertsPerRow;
	UINT m_NumVertsPerCol;

	int	*sort3dptr;
	int *info3dptr;

	int sort3d_buffer[MAX_POLYGONS][2];
	int info3d_buffer[MAX_POLYGONS*30];

	UINT SurfaceNum;

};

#endif