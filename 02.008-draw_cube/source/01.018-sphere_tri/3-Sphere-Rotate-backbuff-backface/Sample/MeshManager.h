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

#define FCMP(a,b) ( (fabs(a-b) < 0.005f) ? 1 : 0 )

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();
	
	void Init_MeshManager(HWND hWnd);
	void Update_MeshManager();
	void Draw_MeshManager () ;

private:

	void Create_BackBuffer();
	void Delete_BackBuffer();
	void Clear_BackBuffer();
	void Present_BackBuffer();

	vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn);

	HWND m_hWnd;

	int m_VertCount;
	int m_TriangleCount;

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
	unsigned int *m_IndexBuff;



};

#endif