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

vector3 CMeshManager::MidPoint(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut = (VecIn1 + VecIn2) * 0.5f;

	return VecOut;
}

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;

	GetClientRect(m_hWnd, &m_Rc);

	Create_BackBuffer();

	//************************************
	//MAKE CUBE
	//************************************

	vector3 VertBuff[8] = {
	-25.0f,  -25.0f, -25.0f, // A
	 25.0f,  -25.0f, -25.0f, // B
	-25.0f,   25.0f, -25.0f, // C
	 25.0f,   25.0f, -25.0f, // D
				
	-25.0f,  -25.0f,  25.0f, // E
	 25.0f,  -25.0f,  25.0f, // F
	-25.0f,   25.0f,  25.0f, // G
	 25.0f,   25.0f,  25.0f};// H

	unsigned int IndexBuff[36] = {
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

	unsigned int NumSubdivisions = 3;

	m_VertBuff.Resize(8);
	m_IndexBuff.Resize(36);

	memcpy(&m_VertBuff[0], VertBuff, sizeof(vector3) * 8);
	memcpy(&m_IndexBuff[0], IndexBuff, sizeof(unsigned int) * 36);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	for(unsigned int j = 0; j < NumSubdivisions; ++j)
	{
		//тут вызвается конструктор копирования list
		list <vector3> m_VertBuffTemp = m_VertBuff;
		list <unsigned int> m_IndexBuffTemp = m_IndexBuff;
		
		/*
		//тут вызывается оператор присваивания копированием list
		list <vector3> m_VertBuffTemp;
		list <unsigned int> m_IndexBuffTemp;
		 
		m_VertBuffTemp = m_VertBuff;
        m_IndexBuffTemp = m_IndexBuff;
		*/

		m_VertBuff.Resize(0);
		m_IndexBuff.Resize(0);

		unsigned int NumTris = m_IndexBuffTemp.Get_Size() / 3;

		for(unsigned int i = 0; i < NumTris; i++)
		{
			vector3 v0 = m_VertBuffTemp[ m_IndexBuffTemp[i*3+0] ];
			vector3 v1 = m_VertBuffTemp[ m_IndexBuffTemp[i*3+1] ];
			vector3 v2 = m_VertBuffTemp[ m_IndexBuffTemp[i*3+2] ];

			vector3 m0 = MidPoint(v0, v1);
	        vector3 m1 = MidPoint(v1, v2);
		    vector3 m2 = MidPoint(v0, v2);

			m_VertBuff.Add_To_List(v0); // 0
			m_VertBuff.Add_To_List(v1); // 1
			m_VertBuff.Add_To_List(v2); // 2
			m_VertBuff.Add_To_List(m0); // 3
			m_VertBuff.Add_To_List(m1); // 4
			m_VertBuff.Add_To_List(m2); // 5

			m_IndexBuff.Add_To_List(i*6+0);
			m_IndexBuff.Add_To_List(i*6+3);
			m_IndexBuff.Add_To_List(i*6+5);

			m_IndexBuff.Add_To_List(i*6+3);
			m_IndexBuff.Add_To_List(i*6+4);
			m_IndexBuff.Add_To_List(i*6+5);

			m_IndexBuff.Add_To_List(i*6+5);
			m_IndexBuff.Add_To_List(i*6+4);
			m_IndexBuff.Add_To_List(i*6+2);

			m_IndexBuff.Add_To_List(i*6+3);
			m_IndexBuff.Add_To_List(i*6+1);
			m_IndexBuff.Add_To_List(i*6+4);
		}
	}

	m_VertBuffTransformed.Resize(m_VertBuff.Get_Size());

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

	for ( unsigned int i = 0; i < m_VertBuff.Get_Size(); i++ )
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

	unsigned int NumTris = m_IndexBuff.Get_Size() / 3;

	for ( unsigned int i = 0; i < NumTris; i++ )
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
