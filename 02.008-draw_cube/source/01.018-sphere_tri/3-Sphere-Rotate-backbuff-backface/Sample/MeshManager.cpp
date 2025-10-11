//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include "MeshManager.h"

CMeshManager::CMeshManager()
{
}

CMeshManager::~CMeshManager()
{ 
	Delete_BackBuffer();

	delete [] m_VertBuff;
	m_VertBuff = NULL;

	delete [] m_VertBuffTransformed;
	m_VertBuffTransformed = NULL;

	delete [] m_IndexBuff;
	m_IndexBuff = NULL;
};

vector3 CMeshManager::Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn)
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

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;

	GetClientRect(m_hWnd, &m_Rc);

	Create_BackBuffer();

	int Slices = 20;
	int Stacks = 10;

	float ThetaFac = PI2 / Slices;
	float PhiFac = PI / Stacks;

	float Radius = 8.0f;

	m_VertCount = (Slices + 1) * (Stacks + 1);
	m_TriangleCount = Slices * Stacks * 2;

	m_VertBuff = new vector3[m_VertCount];
	m_VertBuffTransformed = new vector3[m_VertCount];

	int Index = 0;

	for ( int l = 0; l <= Slices; l++ )
	{
		for ( int b = 0; b <= Stacks; b++ )
		{
			m_VertBuff[Index].x = Radius * sinf(PhiFac * b) * sinf(ThetaFac * l);
			m_VertBuff[Index].y = Radius * cosf(PhiFac * b);
			m_VertBuff[Index].z = Radius * sinf(PhiFac * b) * cosf(ThetaFac * l);

			Index++;
		}
	}

	m_IndexBuff = new unsigned int[m_TriangleCount * 3];

	Index = 0;

	for ( int l = 0; l < Slices; l++ )
	{
		for ( int b = 0; b < Stacks; b++ )
		{
			int next = l * (Stacks + 1) + b;
			int nextSection = (l + 1) * (Stacks + 1) + b;

			m_IndexBuff[Index + 0] = next;
			m_IndexBuff[Index + 1] = next + 1;
			m_IndexBuff[Index + 2] = nextSection + 1;

			m_IndexBuff[Index + 3] = next;
			m_IndexBuff[Index + 4] = nextSection + 1;
			m_IndexBuff[Index + 5] = nextSection;

			Index += 6;
		}
	}
		
}

void CMeshManager::Update_MeshManager()
{
	static float Angle = 0.0f;

	matrix4x4 MatRotateY = {
	cosf(Angle),	0.0,	-sinf(Angle),	0.0,
	0.0,			1.0,	0.0,			0.0,
	sinf(Angle),	0.0,	cosf(Angle),	0.0,
	0.0,			0.0,	0.0,			1.0f };

	matrix4x4 MatRotateX = {
	1.0,			0.0,			0.0,			0.0,
	0.0,			cosf(Angle),	sinf(Angle),	0.0,
	0.0,			-sinf(Angle),	cosf(Angle),	0.0,
	0.0,			0.0,			0.0,			1.0f };

	Angle = Angle + PI / 100.0f;
	if(Angle > PI2 )
		Angle = 0.0f;

	for ( int i = 0; i < m_VertCount; i++ )
	{
		vector3 VecTemp = Vec3_Mat4x4_Mul(m_VertBuff[i], MatRotateX);
		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatRotateY);
		
		VecTemp.z = VecTemp.z + 15.0f;
		VecTemp.x = VecTemp.x / VecTemp.z;
		VecTemp.y = VecTemp.y / VecTemp.z;

		VecTemp.x = VecTemp.x / ((float)m_ViewWidth / m_ViewHeight);

		VecTemp.x = VecTemp.x * m_ViewWidth / 2.0f + m_ViewWidth / 2.0f;
		VecTemp.y =-VecTemp.y * m_ViewHeight / 2.0f + m_ViewHeight / 2.0f;

		m_VertBuffTransformed[i] = VecTemp;
	}

}

void CMeshManager::Draw_MeshManager()
{
	Clear_BackBuffer();
	
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0) );
	HPEN hOldPen = (HPEN) SelectObject(m_hBackBuffer, hPen);

	for ( int i = 0; i < m_TriangleCount; i++ )
	{
		vector3 Vec1 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 0]];
		vector3 Vec2 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 1]];
		vector3 Vec3 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 2]];

		
		//отбрасываем "вырожденные" треугольники сферы
		//на полюсах
		/*
		if(FCMP(Vec1.x,Vec2.x) && FCMP(Vec1.y, Vec2.y))
			continue;
		if(FCMP(Vec1.x,Vec3.x) && FCMP(Vec1.y, Vec3.y))
			continue;
		if(FCMP(Vec2.x,Vec3.x) && FCMP(Vec2.y, Vec3.y))
			continue;
		*/
		
		//используем псевдоскалярное (косое) произведение векторов
		//для отбрасывания задних поверхностей
		//координаты в экранных координатах
		
		float s = (Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);
		
		//здесь значение 0.1f что бы отбрасывать вырожденные треугольники
		//у которых площадь равна нулю
		if( s  < 0.1f )
			continue;

		MoveToEx(m_hBackBuffer, (int) Vec1.x, (int) Vec1.y, NULL);
		LineTo(m_hBackBuffer, (int) Vec2.x, (int) Vec2.y);
		LineTo(m_hBackBuffer, (int) Vec3.x, (int) Vec3.y);
		LineTo(m_hBackBuffer, (int) Vec1.x, (int) Vec1.y);
	}

	SelectObject(m_hBackBuffer, hOldPen);
	DeleteObject(hPen);

	Present_BackBuffer();

	Sleep(50);
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

