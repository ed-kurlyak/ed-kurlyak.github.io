//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

typedef float  matrix4x4[4][4];

struct vector3
{
	vector3() {};
	vector3(float ix, float iy, float iz) : x(ix), y(iy), z(iz) {};

	union
	{
		float Vec[3];
		struct
		{
			float x,y,z;
		};
	};

	float xs, ys;

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

		xs = VecIn.xs;
		ys = VecIn.ys;

		return *this;
	}
};

enum { A, B, C, D, E, F, G, H };

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();

	void Init_MeshManager(HWND hWnd);
	void Update_MeshManager();
	void Draw_MeshManager() ;

private:

	vector3 Vect3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn);
	float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2);
	vector3 Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2);
	vector3 Vec3_Normalize(vector3 &VecIn);
	vector3 Vec3_Scale(vector3 &VecIn, float Val);

	HWND m_hWnd;

	RECT Rc;

	//camera position
	vector3 VecCamPos;
	
	unsigned int m_VertCount;
	unsigned int m_TriangleCount;

	vector3 *m_VertBuff;
	vector3 *m_VertBuffTransformed;
	unsigned int *m_IndexBuff;
};

#endif