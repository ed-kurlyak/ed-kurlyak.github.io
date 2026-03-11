//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

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

		VecOut.x =	VecIn.x * MatIn.Mat[0][0] +
					VecIn.y * MatIn.Mat[1][0] + 
					VecIn.z * MatIn.Mat[2][0] + 
							  MatIn.Mat[3][0];
        
		VecOut.y =	VecIn.x * MatIn.Mat[0][1] + 
					VecIn.y * MatIn.Mat[1][1] + 
					VecIn.z * MatIn.Mat[2][1] + 
							  MatIn.Mat[3][1];
        
		VecOut.z =	VecIn.x * MatIn.Mat[0][2] + 
					VecIn.y * MatIn.Mat[1][2] + 
					VecIn.z * MatIn.Mat[2][2] + 
							  MatIn.Mat[3][2];

		return VecOut;
}


void CMeshManager::Build_Side(vector3 * VertBuff, unsigned int * Indices)
{
	float ValY = (m_NumCellsPerRow * m_CellSpacing) / 2.0f;

	float Width = m_NumCellsPerRow * m_CellSpacing;
	float Depth = m_NumCellsPerCol * m_CellSpacing;

	float HalfWidth = 0.5f * Width;
	float HalfDepth = 0.5f * Depth;

	for (UINT i = 0; i < m_NumVertsPerCol; ++i)
	{
		float z = HalfDepth - i * m_CellSpacing;

		for (UINT j = 0; j < m_NumVertsPerRow; ++j)
		{
			float x = -HalfWidth + j * m_CellSpacing;

			float y = ValY;

			int Index = i * m_NumVertsPerRow + j;

			VertBuff[Index].x = x;
			VertBuff[Index].y = y;
			VertBuff[Index].z = z;
		}
	}

	int Index = 0;

	for (UINT i = 0; i < m_NumCellsPerCol; ++i)
	{
		for (UINT j = 0; j < m_NumCellsPerRow; ++j)
		{
			Indices[Index + 0] = i * m_NumVertsPerRow + j;
			Indices[Index + 1] = i * m_NumVertsPerRow + j + 1;
			Indices[Index + 2] = (i + 1) * m_NumVertsPerRow + j;

			Indices[Index + 3] = (i + 1) * m_NumVertsPerRow + j;
			Indices[Index + 4] = i * m_NumVertsPerRow + j + 1;
			Indices[Index + 5] = (i + 1) * m_NumVertsPerRow + j + 1;

			Index += 6;
		}
	}
}

matrix4x4 CMeshManager::MatrixRotationX(float Angle)
{
	matrix4x4 MatRotateX={
		1, 0, 0, 0,
        0, cosf(Angle), sinf(Angle), 0,
        0, -sinf(Angle), cosf(Angle), 0,
        0, 0, 0, 1};

	return MatRotateX;
}

matrix4x4 CMeshManager::MatrixRotationZ(float Angle)
{
	matrix4x4 MatRotateZ={
		cosf(Angle), sinf(Angle), 0, 0,
		-sinf(Angle), cosf(Angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

	return MatRotateZ;
}

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;

	GetClientRect(m_hWnd, &m_Rc);

	Create_BackBuffer();

	//************************************
	//MAKE CUBE
	//************************************

	m_NumCellsPerRow = 25;
	m_NumCellsPerCol = 25;
	m_CellSpacing = 2.0;

	m_NumVertsPerRow = m_NumCellsPerRow + 1;
	m_NumVertsPerCol = m_NumCellsPerCol + 1;

	//треугольников и вершин в одной стороне, сторон 6
	int NumTriangles = m_NumCellsPerRow * m_NumCellsPerCol * 2;
	int NumVert = m_NumVertsPerRow * m_NumVertsPerCol;

	//всего треугольников и вершин в кубе
	m_TriangleCount = m_NumCellsPerRow * m_NumCellsPerCol * 2 * 6;
	m_VertCount = m_NumVertsPerRow * m_NumVertsPerCol * 6;

	//общие массивы
	m_VertBuff = new vector3[m_VertCount];
	m_VertBuffTransformed = new vector3[m_VertCount];
	m_IndexBuff = new unsigned int[m_TriangleCount * 3];

	unsigned int * Indices = new unsigned int[NumTriangles * 3];

	vector3 * VertBuffTop = new vector3[NumVert];

	vector3 * VertBuffBottom = new vector3[NumVert];

	vector3 * VertBuffFront = new vector3[NumVert];

	vector3 * VertBuffBack = new vector3[NumVert];

	vector3 * VertBuffLeft = new vector3[NumVert];

	vector3 * VertBuffRight = new vector3[NumVert];

	//строим верхнюю сторону куба и переворачиваем
	//ее создавая другие стороны
	Build_Side(VertBuffTop, Indices);

	matrix4x4 MatRotate;

	//нижняя сторона
	MatRotate = MatrixRotationX(PI);
	for (int i = 0; i < NumVert; i++)
	{
		vector3 VecOut = Vec3_Mat4x4_Mul(VertBuffTop[i], MatRotate);
		VertBuffBottom[i] = vector3( VecOut.x, VecOut.y, VecOut.z );
	}

	//передняя сторона
	MatRotate = MatrixRotationX(-PI/2.0f);
	for (int i = 0; i < NumVert; i++)
	{
		vector3 VecOut = Vec3_Mat4x4_Mul(VertBuffTop[i], MatRotate);
		VertBuffFront[i] = vector3( VecOut.x, VecOut.y, VecOut.z );
	}

	//задняя сторона
	MatRotate = MatrixRotationX(PI/2.0f);
	for (int i = 0; i < NumVert; i++)
	{
		vector3 VecOut = Vec3_Mat4x4_Mul(VertBuffTop[i], MatRotate);
		VertBuffBack[i] = vector3( VecOut.x, VecOut.y, VecOut.z );
	}

	//левая сторона
	MatRotate = MatrixRotationZ(PI/2.0f);
	for (int i = 0; i < NumVert; i++)
	{
		vector3 VecOut = Vec3_Mat4x4_Mul(VertBuffTop[i], MatRotate);
		VertBuffLeft[i] = vector3( VecOut.x, VecOut.y, VecOut.z );
	}

	//правая сторона
	MatRotate = MatrixRotationZ(-PI/2.0f);
	for (int i = 0; i < NumVert; i++)
	{
		vector3 VecOut = Vec3_Mat4x4_Mul(VertBuffTop[i], MatRotate);
		VertBuffRight[i] = vector3( VecOut.x, VecOut.y, VecOut.z );
	}

	int Index = 0;

	for (int i = 0; i < NumVert; i++)
	{
		m_VertBuff[Index].x = VertBuffTop[i].x;
		m_VertBuff[Index].y = VertBuffTop[i].y;
		m_VertBuff[Index].z = VertBuffTop[i].z;

		Index++;
	}

	for (int i = 0; i < NumVert; i++)
	{
		m_VertBuff[Index].x = VertBuffBottom[i].x;
		m_VertBuff[Index].y = VertBuffBottom[i].y;
		m_VertBuff[Index].z = VertBuffBottom[i].z;

		Index++;
	}

	for (int i = 0; i < NumVert; i++)
	{
		m_VertBuff[Index].x = VertBuffFront[i].x;
		m_VertBuff[Index].y = VertBuffFront[i].y;
		m_VertBuff[Index].z = VertBuffFront[i].z;

		Index++;
	}

	for (int i = 0; i < NumVert; i++)
	{
		m_VertBuff[Index].x = VertBuffBack[i].x;
		m_VertBuff[Index].y = VertBuffBack[i].y;
		m_VertBuff[Index].z = VertBuffBack[i].z;

		Index++;
	}

	for (int i = 0; i < NumVert; i++)
	{
		m_VertBuff[Index].x = VertBuffLeft[i].x;
		m_VertBuff[Index].y = VertBuffLeft[i].y;
		m_VertBuff[Index].z = VertBuffLeft[i].z;

		Index++;
	}

	for (int i = 0; i < NumVert; i++)
	{
		m_VertBuff[Index].x = VertBuffRight[i].x;
		m_VertBuff[Index].y = VertBuffRight[i].y;
		m_VertBuff[Index].z = VertBuffRight[i].z;

		Index++;
	}

	delete[] VertBuffTop;

	delete[] VertBuffBottom;

	delete[] VertBuffFront;

	delete[] VertBuffBack;

	delete[] VertBuffLeft;

	delete[] VertBuffRight;

	Index = 0;

	for ( int j =0; j < 6; j++)
	{
		for (int i = 0; i < NumTriangles * 3; i++)
		{
			m_IndexBuff[Index] = Indices[i] + NumVert * j;
			Index++;
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
	if(Angle > PI2)
		Angle = 0.0f;

	for ( int i = 0; i < m_VertCount; i++ )
	{
		vector3 VecTemp = Vec3_Mat4x4_Mul(m_VertBuff[i], MatRotateX);
		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatRotateY);
		
		VecTemp.z = VecTemp.z + 85.0f;
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

		//используем псевдоскалярное (косое) произведение векторов
		//для отбрасывания задних поверхностей
		//векторы в экранных координатах
		float s = (Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);
		
		if( s  <= 0 )
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
