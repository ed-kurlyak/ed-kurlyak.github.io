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

struct vector3
{
	float x,y,z;
};

typedef float matrix4x4[4][4];

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();
	
	void Init_MeshManager(HWND hWnd);
	void Update_MeshManager();
	void Draw_MeshManager () ;

private:

	vector3 Vec3_Mat4x4_Mul(vector3 VecIn, matrix4x4 MatIn);

	HWND m_hWnd;

	RECT m_Rc;

	int m_VertCount;
	int m_TriangleCount;

	vector3 *m_VertBuff;
	vector3 *m_VertBuffTransformed;
	unsigned int *m_IndexBuff;
};

#endif