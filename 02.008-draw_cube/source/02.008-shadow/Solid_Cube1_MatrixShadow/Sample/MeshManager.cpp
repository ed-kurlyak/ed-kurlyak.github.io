//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#include "MeshManager.h"

CMeshManager::CMeshManager()
{
};

CMeshManager::~CMeshManager()
{
	Delete_BackBuffer();

	delete [] m_VertBuff;
	m_VertBuff = NULL;

	delete [] m_VertBuffTransformed;
	m_VertBuffTransformed = NULL;

	delete [] m_IndexBuff;
	m_IndexBuff = NULL;

	delete [] m_ColorBuff;
	m_ColorBuff = NULL;
}

vector3 CMeshManager::Vec3_Mat4x4_Mul(vector3 VecIn, matrix4x4 MatIn)
{
	    vector3 VecOut;

		VecOut.x =	VecIn.x * MatIn[0][0] +
					VecIn.y * MatIn[1][0] + 
					VecIn.z * MatIn[2][0] + 
							  MatIn[3][0];
        
		VecOut.y =	VecIn.x * MatIn[0][1] + 
					VecIn.y * MatIn[1][1] + 
					VecIn.z * MatIn[2][1] + 
							  MatIn[3][1];
        
		VecOut.z =	VecIn.x * MatIn[0][2] + 
					VecIn.y * MatIn[1][2] + 
					VecIn.z * MatIn[2][2] + 
							  MatIn[3][2];

		return VecOut;
}

vector4 CMeshManager::Vec4_Mat4x4_Mul(vector4 VecIn, matrix4x4 MatIn)
{
	    vector4 VecOut;

		VecOut.x =	VecIn.x * MatIn[0][0] +
					VecIn.y * MatIn[1][0] + 
					VecIn.z * MatIn[2][0] + 
					VecIn.w	* MatIn[3][0];
        
		VecOut.y =	VecIn.x * MatIn[0][1] + 
					VecIn.y * MatIn[1][1] + 
					VecIn.z * MatIn[2][1] + 
					VecIn.w	* MatIn[3][1];
        
		VecOut.z =	VecIn.x * MatIn[0][2] + 
					VecIn.y * MatIn[1][2] + 
					VecIn.z * MatIn[2][2] + 
					VecIn.w	* MatIn[3][2];

		VecOut.w =	VecIn.x * MatIn[0][3] + 
					VecIn.y * MatIn[1][3] + 
					VecIn.z * MatIn[2][3] + 
					VecIn.w	* MatIn[3][3];

		return VecOut;
}

float CMeshManager::Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2)
{
	return VecIn1.x*VecIn2.x + VecIn1.y*VecIn2.y + VecIn1.z* VecIn2.z;
}

float CMeshManager::Vec4_Dot(plane &VecIn1, vector4 &VecIn2)
{
	return VecIn1.x*VecIn2.x + VecIn1.y*VecIn2.y + VecIn1.z* VecIn2.z + VecIn1.d * VecIn2.w;
}

vector3 CMeshManager::Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut;

	VecOut.x = VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y;
	VecOut.y = VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z;
	VecOut.z = VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x;

	return VecOut;
}

vector3 CMeshManager::Vec3_Normalize(vector3 &VecIn)
{
	vector3 VecOut;

	float Len = sqrtf(VecIn.x * VecIn.x + VecIn.y * VecIn.y + VecIn.z * VecIn.z);

	VecOut.x = VecIn.x / Len;
	VecOut.y = VecIn.y / Len;
	VecOut.z = VecIn.z / Len;
	
	return VecOut;

}

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;

	Create_BackBuffer();

	m_VertCount = 8;
	m_TriangleCount = 12;
	
	vector3 VertBuffTemp[8] = {
	-5.0f,  -5.0f, -5.0f, // A
	 5.0f,  -5.0f, -5.0f, // B
	-5.0f,   5.0f, -5.0f, // C
	 5.0f,   5.0f, -5.0f, // D
				
	-5.0f,  -5.0f,  5.0f, // E
	 5.0f,  -5.0f,  5.0f, // F
	-5.0f,   5.0f,  5.0f, // G
	 5.0f,   5.0f,  5.0f};// H

	DWORD IndexBuffTemp[36] = {
	//front face	
	A, C, D,
	A, D, B,
	
	//left face
	E, G, C,
	E, C, A,

	//back face
	G, E, F,
	G, F, H,

	//right face
	B, D, H,
	B, H, F,

	//top face
	C, G, H,
	C, H, D,

	//bottom face
	E, A, B,
	E, B, F};

	color_rgb ColorBuffTemp[12] = {
	//front face
	0, 0, 255,
	0, 0, 255,

	//left face
	255, 0, 0,
	255, 0, 0,

	//back face
	0, 255, 0,
	0, 255, 0,

	//right face
	255, 255, 0,
	255, 255, 0,

	//top face
	0, 255, 255,
	0, 255, 255,

	//bottom face
	255, 0, 255,
	255, 0, 255 };

	m_VertBuff = NULL;
	m_VertBuff = new vector3[8];

	m_VertBuffTransformed = NULL;
	m_VertBuffTransformed = new vector3[8];

	m_IndexBuff = NULL;
	m_IndexBuff = new DWORD[36];

	m_ColorBuff = NULL;
	m_ColorBuff = new color_rgb[12];

	memcpy(m_VertBuff, VertBuffTemp, 8 * sizeof(vector3));
	memcpy(m_IndexBuff, IndexBuffTemp, 36 * sizeof(DWORD));
	memcpy(m_ColorBuff, ColorBuffTemp, 12 * sizeof(color_rgb));
}

void CMeshManager::Update_MeshManager(bool bDrawShadow, float Angle)
{
	matrix4x4 MatRotateY = {
		cosf(Angle),	0.0,	-sinf(Angle),	0.0,
		0.0,			1.0,	0.0,	0.0,
		sinf(Angle),	0.0,	cosf(Angle),	0.0,
		0.0,			0.0,	0.0,	1.0 };

	matrix4x4 MatRotateX = {
		1.0,			0.0,			0.0,			0.0,
		0.0,			cosf(Angle),	sinf(Angle),	0.0,
		0.0,			-sinf(Angle),	cosf(Angle),	0.0,
		0.0,			0.0,			0.0,			1.0 };

	//куб поднимаем на 10 единиц по Y вверх
	matrix4x4 MatWorld = {
		1.0,			0.0,			0.0,			0.0,
		0.0,			1.0,			0.0,			0.0,
		0.0,			0.0,			1.0,			0.0,
		0.0,			10.0,			0.0,			1.0 };

	//MATRIX VIEW CALCULATION
	vector3 VecRight = { 1.0f, 0.0f, 0.0 };
	vector3 VecUp = { 0.0f, 1.0f, 0.0f };
	//камеру поднимаем над кубом
	//+20 единиц по Y вверх от начала координат
	//и это +10 над кубом (так как куб уже на высоте +10 по Y)
	vector3 VecPos = { 0.0f, 20.0f, -35.0f };
	vector3 VecLook = { -1.0f * VecPos.x, -1.0f * VecPos.y, -1.0f * VecPos.z };

	VecLook = Vec3_Normalize(VecLook);

	VecUp = Vec3_Cross(VecLook, VecRight);
	VecUp = Vec3_Normalize(VecUp);
	VecRight = Vec3_Cross(VecUp, VecLook);
	VecRight = Vec3_Normalize(VecRight);

	float Px = -Vec3_Dot(VecPos, VecRight);
	float Py = -Vec3_Dot(VecPos, VecUp);
	float Pz = -Vec3_Dot(VecPos, VecLook);

	matrix4x4 MatView = {
		VecRight.x,		VecUp.x,	VecLook.x,		0.0f,
		VecRight.y,		VecUp.y,	VecLook.y,		0.0f,
		VecRight.z,		VecUp.z,	VecLook.z,		0.0f,
		Px,				Py,			Pz,				1.0f };

	//для точечного источника света
	vector4 lightDirection = {0.0f, -1.0f, 0.0f, 1.0f};
	//для паралельного источника света
	//vector4 lightDirection = {0.0f, -1.0f, 0.0f, 0.0f};
	
	//groundPlane должно быть нормализовано
	plane groundPlane={0.0f, 1.0f, 0.0f, 15};

	float k = -Vec4_Dot(groundPlane, lightDirection);
	
	matrix4x4 MatShadow = {
		groundPlane.x * lightDirection.x + k,	groundPlane.x * lightDirection.y,		groundPlane.x * lightDirection.z,		groundPlane.x * lightDirection.w,
		groundPlane.y * lightDirection.x,		groundPlane.y * lightDirection.y + k,	groundPlane.y * lightDirection.z,		groundPlane.y * lightDirection.w,
		groundPlane.z * lightDirection.x,		groundPlane.z * lightDirection.y,		groundPlane.z * lightDirection.z + k,	groundPlane.z * lightDirection.w,
		groundPlane.d * lightDirection.x,		groundPlane.d * lightDirection.y,		groundPlane.d * lightDirection.z,		groundPlane.d * lightDirection.w + k };

	for ( UINT i = 0; i < m_VertCount; i++ )
	{
		vector3 Vec = Vec3_Mat4x4_Mul(m_VertBuff[i], MatRotateY);
		//Vec = Vec3_Mat4x4_Mul(Vec, MatRotateX);
		
		Vec = Vec3_Mat4x4_Mul(Vec, MatWorld);
		
		vector4 Vec4 = {Vec.x, Vec.y, Vec.z, 1.0f};

		//если рисуем куб как тень то надо вершины
		//умножить на матрицу тени
		if(bDrawShadow)
			Vec4 = Vec4_Mat4x4_Mul(Vec4, MatShadow);
			
		Vec4 = Vec4_Mat4x4_Mul(Vec4, MatView);

		Vec.x = Vec4.x;
		Vec.y = Vec4.y;
		Vec.z = Vec4.z;

		//матрицу проекции не используем
		
		//тут добавлять к z не надо так как у нас
		//матирца вида позиция -35
		//Vec.z = Vec.z + 35.0f;
		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;
		Vec.x = Vec.x / ((float)m_ViewWidth / m_ViewHeight);

		Vec.x = Vec.x * m_ViewWidth / 2.0f + m_ViewWidth / 2.0f;
		Vec.y =-Vec.y * m_ViewHeight / 2.0f + m_ViewHeight / 2.0f;
		
		m_VertBuffTransformed[i] = Vec;
	}
}

void CMeshManager::Draw_MeshManager(bool bDrawShadow)
{
	for ( UINT i = 0; i< m_TriangleCount; i++ )
	{
		vector3 Vec1 = m_VertBuffTransformed[m_IndexBuff [i * 3 + 0]];
		vector3 Vec2 = m_VertBuffTransformed[m_IndexBuff [i * 3 + 1]];
		vector3 Vec3 = m_VertBuffTransformed[m_IndexBuff [i * 3 + 2]];

		//если рисуем куб (не тень куба)
		if(!bDrawShadow)
		{

			//отбрасывание задних поверхностей
			//косое (псевдоскалярное умножение векторов)
			float s = (Vec2.x - Vec1.x) * ( Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);

			if(s <= 0)
				continue;

			//рисуем цветной куб
			HBRUSH hBrush = CreateSolidBrush(RGB(m_ColorBuff[i].r, m_ColorBuff[i].g, m_ColorBuff[i].b));
			HBRUSH hOldBrush = (HBRUSH) SelectObject(m_hBackBuffer, hBrush);

			HPEN hPen = CreatePen(PS_SOLID, 1, RGB(m_ColorBuff[i].r, m_ColorBuff[i].g, m_ColorBuff[i].b));
			HPEN hOldPen = (HPEN) SelectObject(m_hBackBuffer, hPen);

			POINT Pt[3];

			Pt[0].x = (int)Vec1.x;
			Pt[0].y = (int)Vec1.y;

			Pt[1].x = (int)Vec2.x;
			Pt[1].y = (int)Vec2.y;

			Pt[2].x = (int)Vec3.x;
			Pt[2].y = (int)Vec3.y;

			Polygon(m_hBackBuffer, Pt, 3);

			SelectObject(m_hBackBuffer, hOldPen);
			DeleteObject(hPen);

			SelectObject(m_hBackBuffer, hOldBrush);
			DeleteObject(hBrush);
		}
		//если рисуем тень куба
		else
		{

			//тень рисуем черного цвета
			HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
			HBRUSH hOldBrush = (HBRUSH) SelectObject(m_hBackBuffer, hBrush);

			HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
			HPEN hOldPen = (HPEN) SelectObject(m_hBackBuffer, hPen);

			POINT Pt[3];
	
			Pt[0].x = (int)Vec1.x;
			Pt[0].y = (int)Vec1.y;

			Pt[1].x = (int)Vec2.x;
			Pt[1].y = (int)Vec2.y;

			Pt[2].x = (int)Vec3.x;
			Pt[2].y = (int)Vec3.y;

			Polygon(m_hBackBuffer, Pt, 3);

			SelectObject(m_hBackBuffer, hOldPen);
			DeleteObject(hPen);

			SelectObject(m_hBackBuffer, hOldBrush);
			DeleteObject(hBrush);
		}
	}
}

void CMeshManager::Create_BackBuffer()
{
	//размеры окна приложения (заднего буфера)
	GetClientRect(m_hWnd, &m_Rc);

	m_ViewWidth = m_Rc.right;
	m_ViewHeight = m_Rc.bottom;

	m_hDC = GetDC(m_hWnd);

	//кисть для очистки окна приложения (заднего буфера)
	//m_hBrush = CreateSolidBrush(RGB(0,0,0));
	m_hBrush = CreateSolidBrush(RGB(255,255,255));

	//наш задний буфер
	m_hBackBuffer = CreateCompatibleDC(m_hDC);
	m_hBitmap = CreateCompatibleBitmap(m_hDC, m_ViewWidth, m_ViewHeight);
	m_hOldBitmap = (HBITMAP) SelectObject(m_hBackBuffer, m_hBitmap);
}

void CMeshManager::Clear_BackBuffer()
{
	//очистка окна приложения
	FillRect(m_hBackBuffer, &m_Rc, m_hBrush);
}

void CMeshManager::Present_BackBuffer()
{
	//выводим задний буфер на экран приложения
	BitBlt(m_hDC, 0, 0, m_ViewWidth, m_ViewHeight, m_hBackBuffer, 0, 0, SRCCOPY);
}

void CMeshManager::Delete_BackBuffer()
{
	//уничтожение заднего буфера
	SelectObject(m_hBackBuffer, m_hOldBitmap);
	DeleteObject(m_hBitmap);

	DeleteObject(m_hBrush);

	ReleaseDC(m_hWnd, m_hDC);
	DeleteDC(m_hBackBuffer);
}

