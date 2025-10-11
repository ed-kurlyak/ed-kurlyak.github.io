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

void CMeshManager::Init_MeshManager(HWND hwnd)
{
	m_hWnd = hwnd;

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

	m_IndexBuff = new DWORD[m_TriangleCount * 3];
	m_ColorBuff = new color_rgb[m_TriangleCount];

	Index = 0;
	int ColorIndex = 0;

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

			int tr = rand() % 256;
			int tg = rand() % 256;
			int tb = rand() % 256;
			
			m_ColorBuff[ColorIndex].r = tr;
			m_ColorBuff[ColorIndex].g = tg;
			m_ColorBuff[ColorIndex].b = tb;

			ColorIndex++;

			m_ColorBuff[ColorIndex].r = tr;
			m_ColorBuff[ColorIndex].g = tg;
			m_ColorBuff[ColorIndex].b = tb;

			ColorIndex++;
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

	for ( UINT i = 0; i < m_VertCount; i++ )
	{
		vector3 Vec = Vec3_Mat4x4_Mul(m_VertBuff[i], MatRotateX);
		Vec = Vec3_Mat4x4_Mul(Vec, MatRotateY);
		
		Vec.z = Vec.z + 15.0f;
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
	
	for ( UINT i = 0; i < m_TriangleCount; i++ )
	{
		vector3 Vec1 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 0]];
		vector3 Vec2 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 1]];
		vector3 Vec3 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 2]];

		/*
		//убираем вырожденные треугольники на полюсах
		if(FCMP(Vec1.x,Vec2.x) && FCMP(Vec1.y, Vec2.y))
			continue;
		if(FCMP(Vec1.x,Vec3.x) && FCMP(Vec1.y, Vec3.y))
			continue;
		if(FCMP(Vec2.x,Vec3.x) && FCMP(Vec2.y, Vec3.y))
			continue;
		*/

		float s = (Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);

		//здесь значение 0.1f используется что бы отбрасывать вырожденные
		//треугольники у которых площадь равна нулю (или прилизительно ноль)
		if(s <= 0.1f)
			continue;

		m_CurrColor = m_ColorBuff[i];

		Draw_Color_Triangle(Vec1.x, Vec1.y, Vec2.x, Vec2.y, Vec3.x, Vec3.y);
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

void CMeshManager::Draw_Color_Triangle(float x1, float y1,
						 float x2, float y2,
						 float x3, float y3)
{
	float Temp;
	int Side;

	if( y2 < y1 )
	{
		SWAP(x2,x1,Temp);
		SWAP(y2,y1,Temp);
	}

	if( y3 < y1 )
	{
		SWAP(x3,x1,Temp);
		SWAP(y3,y1,Temp);
	}

	if( y3 < y2 )
	{
		SWAP(x3,x2,Temp);
		SWAP(y3,y2,Temp);
	}
	
	if( y2 > y1 && y3 > y2)
	{
		float dxdy1 = (x2 - x1) / (y2 - y1);
		float dxdy2 = (x3 - x1) / (y3 - y1);
		Side = dxdy2 > dxdy1;
	}

	if (y1 == y2)
		Side = x1 > x2;

	if (y3 == y2)
		Side = x3 > x2;

	if( !Side ) //левая сторона треугольника длиннее
	{
		m_xl = x1;
		m_dxl = (x3 - x1) / (y3 - y1);

		if(y1 < y2)
		{
			m_xr = x1;
			m_dxr = (x2 - x1) / (y2 - y1);

			Draw_Color_Poly( (int) y1, (int) y2);
		}
		if(y2 < y3)
		{
			m_xr = x2;
			m_dxr = (x3 - x2) / (y3 - y2);

			Draw_Color_Poly( (int) y2, (int) y3);
		}
	}
	else
	{
		m_xr = x1;
		m_dxr = (x3 - x1) / (y3 - y1);

		if(y1 < y2)
		{
			m_xl = x1;
			m_dxl = (x2 - x1) / (y2 - y1);

			Draw_Color_Poly( (int) y1, (int) y2);
		}
		if(y2 < y3)
		{
			m_xl = x2;
			m_dxl = (x3 - x2) / (y3 - y2);

			Draw_Color_Poly( (int) y2, (int) y3);
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

