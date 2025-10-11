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
}

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

	for (unsigned int i = 0; i < m_TriangleCount / 2; i++)
	{
		int r = rand() % 256;
		int g = rand() % 256;
		int b = rand() % 256;

		m_ColorBuff[i * 2 + 0].r = r;
		m_ColorBuff[i * 2 + 0].g = g;
		m_ColorBuff[i * 2 + 0].b = b;

		m_ColorBuff[i * 2 + 1].r = r;
		m_ColorBuff[i * 2 + 1].g = g;
		m_ColorBuff[i * 2 + 1].b = b;
	}

	m_VertBuff = NULL;
	m_VertBuff = new vector3[8];

	m_VertBuffTransformed = NULL;
	m_VertBuffTransformed = new vector3[8];

	m_IndexBuff = NULL;
	m_IndexBuff = new DWORD[36];

	memcpy(m_VertBuff, VertBuffTemp, 8 * sizeof(vector3));
	memcpy(m_IndexBuff, IndexBuffTemp, 36 * sizeof(DWORD));
}

void CMeshManager::Update_MeshManager()
{
	static float Angle = 0.0;

	matrix4x4 MatRotateY = {
		cosf(Angle),	0.0,	-sinf(Angle),	0.0,
		0.0,			1.0,	0.0,	0.0,
		sinf(Angle),	0.0,	cosf(Angle),	0.0,
		0.0,			0.0,	0.0,	1.0 };

	matrix4x4 MatRotateX = {
		1.0,			0.0,		0.0,		0.0,
		0.0,			cosf(Angle),	sinf(Angle),	0.0,
		0.0,			-sinf(Angle),	cosf(Angle),	0.0,
		0.0,			0.0,		0.0,		1.0 };

	Angle = Angle + PI / 50.0f;
	if (Angle > PI2)
		Angle = 0.0f;

	for ( UINT i = 0; i < m_VertCount; i++ )
	{
		vector3 Vec = Vec3_Mat4x4_Mul(m_VertBuff[i], MatRotateY);
		Vec = Vec3_Mat4x4_Mul(Vec, MatRotateX);

		Vec.z = Vec.z + 20.0f;
		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;

		Vec.x = Vec.x / ((float)m_ViewWidth / m_ViewHeight);

		Vec.x = Vec.x * m_ViewWidth / 2.0f + m_ViewWidth / 2.0f;
		Vec.y =-Vec.y * m_ViewHeight / 2.0f + m_ViewHeight / 2.0f;

		m_VertBuffTransformed[i] = Vec;
	}
}

void CMeshManager::Draw_MeshManager()
{
	Clear_BackBuffer();

	for ( UINT i = 0; i< m_TriangleCount; i++ )
	{
		vector3 Vec1 = m_VertBuffTransformed[ m_IndexBuff [ i * 3 + 0 ] ];
		vector3 Vec2 = m_VertBuffTransformed[ m_IndexBuff [ i * 3 + 1 ] ];
		vector3 Vec3 = m_VertBuffTransformed[ m_IndexBuff [ i * 3 + 2 ] ];

		float s = (Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);

		if(s <= 0)
			continue;

		m_CurrColor = m_ColorBuff[i];

		Draw_Colored_Triangle(Vec1.x, Vec1.y, Vec2.x, Vec2.y, Vec3.x, Vec3.y);
	}
	
	Present_BackBuffer();
}

void CMeshManager::Draw_Color_Poly(int y1, int y2)
{
	for ( int y = y1; y < y2; y++ )
	{
		for ( int x = (int)m_xl; x < (int)m_xr; x++ )
		{
			SetPixel(m_hBackBuffer, x, y, RGB(m_CurrColor.r, m_CurrColor.g, m_CurrColor.b));
		}

		m_xl += m_dxl;
		m_xr += m_dxr;
	}

}

void CMeshManager::Draw_Colored_Triangle(float x1,float y1,
					   float x2,float y2,
					   float x3,float y3)
{
	float t;
	int side;

	if( y2 < y1 )
	{
		SWAP(x2, x1, t);
		SWAP(y2, y1, t);
	}

	if( y3 < y1 )
	{
		SWAP(x3, x1, t);
		SWAP(y3, y1, t);
	}

	if( y3 < y2 )
	{
		SWAP(x3, x2, t);
		SWAP(y3, y2, t);
	}

	if ( y2 > y1 && y3 > y2 )
	{
		float dxdy1 = (x2 - x1) / (y2 - y1);
		float dxdy2 = (x3 - x1) / (y3 - y1);
		side = dxdy2 > dxdy1;
	}
	
	if (y1 == y2)
		side = x1 > x2;
	if (y3 == y2)
		side = x3 > x2;

	if( !side ) //если side = 0 левая сторона длиннее
	{
		m_xl = x1;
		m_dxl = (x3 - x1) / (y3 - y1);

		if ( y1 < y2 ) //треугольник с плоским низом x3 < x2
		{
			m_xr = x1;
			m_dxr = (x2 - x1) / (y2 - y1);

			Draw_Color_Poly((int)y1, (int)y2);
		}
		if ( y2 < y3 ) //треугольник с плоским верхом x1 < x2
		{
			m_xr = x2;
			m_dxr = (x3 - x2) / (y3 - y2);

			Draw_Color_Poly((int)y2, (int)y3);
		}
	}
	else
	{
		m_xr = x1;
		m_dxr = (x3 - x1) / (y3 - y1);

		if ( y1 < y2 ) //треугольинк с плоским низом x3 > x2
		{
			m_xl = x1;
			m_dxl = (x2 - x1) / (y2 - y1);

			Draw_Color_Poly((int)y1, (int)y2);
		}
		if ( y2 < y3 ) //треугольник с плоским верхом x1 > x2
		{
			m_xl = x2;
			m_dxl = (x3 - x2) / (y3 - y2);

			Draw_Color_Poly((int)y2, (int)y3);
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

