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

	delete [] m_VertBuffTransformed;
	m_VertBuffTransformed = NULL;

	delete [] m_VertBuff;
	m_VertBuff = NULL;

	delete [] m_IndexBuff;
	m_IndexBuff = NULL;
}

matrix4x4 CMeshManager::Mat4x4_Mat4x4_Mul(matrix4x4 MatIn1, matrix4x4 MatIn2)
{
	matrix4x4 MatOut;

	//row1 * col1
	MatOut.Mat[0][0] = MatIn1.Mat[0][0]*MatIn2.Mat[0][0] + MatIn1.Mat[0][1]*MatIn2.Mat[1][0] + MatIn1.Mat[0][2]*MatIn2.Mat[2][0] + MatIn1.Mat[0][3]*MatIn2.Mat[3][0];
	//row1 * col2
	MatOut.Mat[0][1] = MatIn1.Mat[0][0]*MatIn2.Mat[0][1] + MatIn1.Mat[0][1]*MatIn2.Mat[1][1] + MatIn1.Mat[0][2]*MatIn2.Mat[2][1] + MatIn1.Mat[0][3]*MatIn2.Mat[3][1];
	//row1 * col3
	MatOut.Mat[0][2] = MatIn1.Mat[0][0]*MatIn2.Mat[0][2] + MatIn1.Mat[0][1]*MatIn2.Mat[1][2] + MatIn1.Mat[0][2]*MatIn2.Mat[2][2] + MatIn1.Mat[0][3]*MatIn2.Mat[3][2];
	//row1 * col4
	MatOut.Mat[0][3] = MatIn1.Mat[0][0]*MatIn2.Mat[0][3] + MatIn1.Mat[0][1]*MatIn2.Mat[1][3] + MatIn1.Mat[0][2]*MatIn2.Mat[2][3] + MatIn1.Mat[0][3]*MatIn2.Mat[3][3];

	//row2 * col1
	MatOut.Mat[1][0] = MatIn1.Mat[1][0]*MatIn2.Mat[0][0] + MatIn1.Mat[1][1]*MatIn2.Mat[1][0] + MatIn1.Mat[1][2]*MatIn2.Mat[2][0] + MatIn1.Mat[1][3]*MatIn2.Mat[3][0];
	//row2 * col2
	MatOut.Mat[1][1] = MatIn1.Mat[1][0]*MatIn2.Mat[0][1] + MatIn1.Mat[1][1]*MatIn2.Mat[1][1] + MatIn1.Mat[1][2]*MatIn2.Mat[2][1] + MatIn1.Mat[1][3]*MatIn2.Mat[3][1];
	//row2 * col3
	MatOut.Mat[1][2] = MatIn1.Mat[1][0]*MatIn2.Mat[0][2] + MatIn1.Mat[1][1]*MatIn2.Mat[1][2] + MatIn1.Mat[1][2]*MatIn2.Mat[2][2] + MatIn1.Mat[1][3]*MatIn2.Mat[3][2];
	//row2 * col4
	MatOut.Mat[1][3] = MatIn1.Mat[1][0]*MatIn2.Mat[0][3] + MatIn1.Mat[1][1]*MatIn2.Mat[1][3] + MatIn1.Mat[1][2]*MatIn2.Mat[2][3] + MatIn1.Mat[1][3]*MatIn2.Mat[3][3];

	//row3 * col1
	MatOut.Mat[2][0] = MatIn1.Mat[2][0]*MatIn2.Mat[0][0] + MatIn1.Mat[2][1]*MatIn2.Mat[1][0] + MatIn1.Mat[2][2]*MatIn2.Mat[2][0] + MatIn1.Mat[2][3]*MatIn2.Mat[3][0];
	//row3 * col2
	MatOut.Mat[2][1] = MatIn1.Mat[2][0]*MatIn2.Mat[0][1] + MatIn1.Mat[2][1]*MatIn2.Mat[1][1] + MatIn1.Mat[2][2]*MatIn2.Mat[2][1] + MatIn1.Mat[2][3]*MatIn2.Mat[3][1];
	//row3 * col3
	MatOut.Mat[2][2] = MatIn1.Mat[2][0]*MatIn2.Mat[0][2] + MatIn1.Mat[2][1]*MatIn2.Mat[1][2] + MatIn1.Mat[2][2]*MatIn2.Mat[2][2] + MatIn1.Mat[2][3]*MatIn2.Mat[3][2];
	//row3 * col4
	MatOut.Mat[2][3] = MatIn1.Mat[2][0]*MatIn2.Mat[0][3] + MatIn1.Mat[2][1]*MatIn2.Mat[1][3] + MatIn1.Mat[2][2]*MatIn2.Mat[2][3] + MatIn1.Mat[2][3]*MatIn2.Mat[3][3];

	//row4 * col1
	MatOut.Mat[3][0] = MatIn1.Mat[3][0]*MatIn2.Mat[0][0] + MatIn1.Mat[3][1]*MatIn2.Mat[1][0] + MatIn1.Mat[3][2]*MatIn2.Mat[2][0] + MatIn1.Mat[3][3]*MatIn2.Mat[3][0];
	//row4 * col2
	MatOut.Mat[3][1] = MatIn1.Mat[3][0]*MatIn2.Mat[0][1] + MatIn1.Mat[3][1]*MatIn2.Mat[1][1] + MatIn1.Mat[3][2]*MatIn2.Mat[2][1] + MatIn1.Mat[3][3]*MatIn2.Mat[3][1];
	//row4 * col3
	MatOut.Mat[3][2] = MatIn1.Mat[3][0]*MatIn2.Mat[0][2] + MatIn1.Mat[3][1]*MatIn2.Mat[1][2] + MatIn1.Mat[3][2]*MatIn2.Mat[2][2] + MatIn1.Mat[3][3]*MatIn2.Mat[3][2];
	//row4 * col4
	MatOut.Mat[3][3] = MatIn1.Mat[3][0]*MatIn2.Mat[0][3] + MatIn1.Mat[3][1]*MatIn2.Mat[1][3] + MatIn1.Mat[3][2]*MatIn2.Mat[2][3] + MatIn1.Mat[3][3]*MatIn2.Mat[3][3];

	return MatOut;
}

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

void CMeshManager::Init_MeshManager(HWND hwnd)
{
	m_hWnd = hwnd;
	
	Create_BackBuffer();

	m_VertCount = 8;
	m_TriangleCount = 12;
	
	vector3 vert_buff_temp[8] = {
	-5.0f,  -5.0f, -5.0f, // A
	 5.0f,  -5.0f, -5.0f, // B
	-5.0f,   5.0f, -5.0f, // C
	 5.0f,   5.0f, -5.0f, // D
				
	-5.0f,  -5.0f,  5.0f, // E
	 5.0f,  -5.0f,  5.0f, // F
	-5.0f,   5.0f,  5.0f, // G
	 5.0f,   5.0f,  5.0f};// H

	//строим куб из треугольников
	DWORD index_buff_temp[36] = {
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

	//color array for all 8 vertices
	m_ColorBuff[0] = color_rgb(255, 255, 255);
	m_ColorBuff[1] = color_rgb(0, 0, 0);
	m_ColorBuff[2] = color_rgb(255, 0, 0);
	m_ColorBuff[3] = color_rgb(0, 255, 0);
	m_ColorBuff[4] = color_rgb(0, 0, 255);
	m_ColorBuff[5] = color_rgb(255, 255, 0);
	m_ColorBuff[6] = color_rgb(0, 255, 255);
	m_ColorBuff[7] = color_rgb(255, 0, 255);

	m_VertBuff = NULL;
	m_VertBuff = new vector3[8];

	m_VertBuffTransformed = NULL;
	m_VertBuffTransformed = new vector3[8];

	m_IndexBuff = NULL;
	m_IndexBuff = new DWORD[36];

	memcpy(m_VertBuff, vert_buff_temp, 8 * sizeof(vector3));
	memcpy(m_IndexBuff, index_buff_temp, 36 * sizeof(DWORD));
}

void CMeshManager::Update_MeshManager()
{
	static float Angle = 0.0f;

	matrix4x4 MatRotateX={
		1, 0, 0, 0,
        0, cosf(Angle), sinf(Angle), 0,
        0, -sinf(Angle), cosf(Angle), 0,
        0, 0, 0, 1};
	
	matrix4x4 MatRotateY={
		cosf(Angle), 0, -sinf(Angle), 0,
		0, 1, 0, 0,
		sinf(Angle), 0, cosf(Angle), 0,
		0, 0, 0, 1};

	matrix4x4 MatRotateZ={
		cosf(Angle), sinf(Angle), 0, 0,
		-sinf(Angle), cosf(Angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};
	
	Angle += PI / 100.0f;
    if(Angle > PI2)
		Angle = 0;
	
	matrix4x4 MatWorld={
		1, 0, 0, 0,
		0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 18, 1};

	matrix4x4 MatView={
		1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

	float Fov = PI/2.0f; // FOV 90 degree
    float Aspect = (float) m_ViewWidth / m_ViewHeight;
	float ZFar = 100.0f;
	float ZNear = 0.1f;

	float h, w, Q;

    w = (1.0f/tanf(Fov*0.5f))/Aspect;  
    h = 1.0f/tanf(Fov*0.5f);   
    Q = ZFar/(ZFar - ZNear);
	
	/*
    matrix4x4 mProj = {
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, Q, 1,
        0, 0, -Q*fZNear, 0 };
	*/

	//используем сокращенный вариант матрицы проекции
	//так как значение глубины Z в диапазоне от 0 до 1
	//мы не используем
    matrix4x4 mProj={
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, 1, 1,
        0, 0, 0, 1 };

	float Alpha = 0.5f * m_ViewWidth;
	float Beta  = 0.5f * m_ViewHeight;
	
	matrix4x4 MatScreen = {
		Alpha,  0,	    0,    0, 
		0,      -Beta,  0,    0, 
		0,		0,		1,    0,
		Alpha,  Beta,	0,    1};

	for (UINT i = 0; i < m_VertCount; i++)
	{
		vector3 Vec;

		matrix4x4 Mat;

		Mat = Mat4x4_Mat4x4_Mul(MatRotateX, MatRotateY);
		Mat = Mat4x4_Mat4x4_Mul(Mat, MatRotateZ);
		Mat = Mat4x4_Mat4x4_Mul(Mat, MatWorld);
		Mat = Mat4x4_Mat4x4_Mul(Mat, MatView);
		Mat = Mat4x4_Mat4x4_Mul(Mat, mProj);

		Vec = Vec3_Mat4x4_Mul(m_VertBuff[i], Mat);

		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;

		Vec = Vec3_Mat4x4_Mul(Vec, MatScreen);

		m_VertBuffTransformed[i] = Vec;
	}
}

void CMeshManager::Draw_Color_Poly(int y1, int y2)
{
	float ri, gi, bi;
	float dr, dg, db;

	for ( int y = y1; y < y2; y++ )
	{
		ri = m_redl;
		gi = m_greenl;
		bi = m_bluel;

		if((m_xr - m_xl) > 0) // делить на 0 нельзя
		{
			dr = (m_redr - m_redl)/(m_xr - m_xl);
			dg = (m_greenr - m_greenl)/(m_xr - m_xl);
			db = (m_bluer - m_bluel)/(m_xr - m_xl);
		}
		else
		{
			
			dr = m_redr - m_redl;
			dg = m_greenr - m_greenl;
			db = m_bluer - m_bluel;
		}

		for (int x=(int)m_xl; x<(int)m_xr; x++)
		{
			int Index =  y * 4 * m_ViewWidth + x * 4;

			m_Data[Index + 0] = (BYTE) bi; // blue
			m_Data[Index + 1] = (BYTE) gi; // green
			m_Data[Index + 2] = (BYTE) ri; // red
			m_Data[Index + 3] = 0; 

			ri+=dr;
			gi+=dg;
			bi+=db;
		}

		m_xl += m_dxl;
		m_redl += m_dredl;
		m_greenl += m_dgreenl;
		m_bluel += m_dbluel;

		m_xr += m_dxr;
		m_redr += m_dredr;
		m_greenr += m_dgreenr;
		m_bluer += m_dbluer;
	}

}

void CMeshManager::Draw_Color_Triangle(float x1,float y1,
					   float x2,float y2,
					   float x3,float y3,
					   color_rgb color1,
					   color_rgb color2,
					   color_rgb color3)
{
	float Temp;
	int ColorTemp;
	int Side;

	if (y2 < y1)
	{
		SWAP(x2,x1,Temp);
		SWAP(y2,y1,Temp);
		
		SWAP(color2.r, color1.r, ColorTemp);
		SWAP(color2.g, color1.g, ColorTemp);
		SWAP(color2.b, color1.b, ColorTemp);
	}

	if (y3 < y1)
	{
		SWAP(x3,x1,Temp);
		SWAP(y3,y1,Temp);

		SWAP(color3.r, color1.r, ColorTemp);
		SWAP(color3.g, color1.g, ColorTemp);
		SWAP(color3.b, color1.b, ColorTemp);
	}

	if (y3 < y2)
	{
		SWAP(x3,x2,Temp);
		SWAP(y3,y2,Temp);

		SWAP(color3.r, color2.r, ColorTemp);
		SWAP(color3.g, color2.g, ColorTemp);
		SWAP(color3.b, color2.b, ColorTemp);
	}

	//определяем какая сторона треугольника длинее
	if (y2 > y1 && y3 > y2)
	{
		float dxdy1 = (x2 - x1) / (y2 - y1);
		float dxdy2 = (x3 - x1) / (y3 - y1);
		Side = dxdy2 > dxdy1;
	}

	if (y1 == y2)
		Side = x1 > x2;
	if (y2 == y3)
		Side = x3 > x2;

	if (!Side) //длинее левая сторона
	{
		m_xl = x1;
		m_redl = (float) color1.r;
		m_greenl = (float) color1.g;
		m_bluel = (float) color1.b;

		m_dxl = (x3 - x1) / (y3 - y1);
		m_dredl = (color3.r - color1.r) / (y3 - y1);
		m_dgreenl = (color3.g - color1.g) / (y3 - y1);
		m_dbluel = (color3.b - color1.b) / (y3 - y1);
	
		if ( y1 < y2)
		{
			m_xr = x1;
			m_redr = (float) color1.r;
			m_greenr = (float) color1.g;
			m_bluer = (float) color1.b;

			m_dxr = (x2 - x1) / (y2 - y1);
			m_dredr = (color2.r - color1.r) / (y2 - y1);
			m_dgreenr = (color2.g - color1.g) / (y2 - y1);
			m_dbluer = (color2.b - color1.b) / (y2 - y1);

			Draw_Color_Poly((int)y1, (int)y2);	
		}
		if(y2 < y3)
		{
			m_xr = x2;
			m_redr = (float) color2.r;
			m_greenr = (float) color2.g;
			m_bluer = (float) color2.b;

			m_dxr = (x3 - x2) / (y3 - y2);
			m_dredr = (color3.r - color2.r) / (y3 - y2);
			m_dgreenr = (color3.g - color2.g) / (y3 - y2);
			m_dbluer = (color3.b - color2.b) / (y3 - y2);

			Draw_Color_Poly((int)y2, (int)y3);
		}
	}
	else
	{
		m_xr = x1;
		m_redr = (float) color1.r;
		m_greenr = (float) color1.g;
		m_bluer = (float) color1.b;

		m_dxr = (x3 - x1) / (y3 - y1);
		m_dredr = (color3.r - color1.r) / (y3 - y1);
		m_dgreenr = (color3.g - color1.g) / (y3 - y1);
		m_dbluer = (color3.b - color1.b) / (y3 - y1);

		if (y1 < y2)
		{
			m_xl = x1;
			m_redl = (float) color1.r;
			m_greenl = (float) color1.g;
			m_bluel = (float) color1.b;

			m_dxl = (x2 - x1) / (y2 - y1);
			m_dredl = (color2.r - color1.r) / (y2 - y1);
			m_dgreenl = (color2.g - color1.g) / (y2 - y1);
			m_dbluel = (color2.b - color1.b) / (y2 - y1);

			Draw_Color_Poly((int)y1, (int)y2);
		}
		if (y2 < y3)
		{
			m_xl = x2;
			m_redl = (float) color2.r;
			m_greenl = (float) color2.g;
			m_bluel = (float) color2.b;

			m_dxl = (x3 - x2) / (y3 - y2);
			m_dredl = (color3.r - color2.r) / (y3 - y2);
			m_dgreenl = (color3.g - color2.g) / (y3 - y2);
			m_dbluel = (color3.b - color2.b) / (y3 - y2);
			
			Draw_Color_Poly((int)y2, (int)y3);
		}
	}
}

void CMeshManager::Draw_MeshManager ()
{
	 Clear_BackBuffer();

    for (UINT i = 0; i < m_TriangleCount; i++)
	 {
		vector3 Vec1 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 0]];
		vector3 Vec2 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 1]];
		vector3 Vec3 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 2]];
		
		//сейчас вершины в экранных координатах
		//используем псевдоскалярное (косое) умножение векторов
		//для отбрасывания задних поверхностей
		float s = (Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);

		if (s <= 0)
			continue;
	
		color_rgb rgb1 = m_ColorBuff[m_IndexBuff[i * 3 + 0]];
		color_rgb rgb2 = m_ColorBuff[m_IndexBuff[i * 3 + 1]];
		color_rgb rgb3 = m_ColorBuff[m_IndexBuff[i * 3 + 2]];

		Draw_Color_Triangle(Vec1.x, Vec1.y, Vec2.x, Vec2.y, Vec3.x, Vec3.y,
			rgb1, rgb2, rgb3);
     }  

	Present_BackBuffer();
	
}

void CMeshManager::Create_BackBuffer()
{
	RECT rc;
	GetClientRect(m_hWnd, &rc);

	m_ViewWidth = rc.right;
	m_ViewHeight = rc.bottom;
	
	DWORD m_dwSize = m_ViewWidth * (BITS_PER_PIXEL >> 3) * m_ViewHeight;

	m_Data = (LPBYTE)malloc(m_dwSize*sizeof(BYTE));

	memset(&m_Bih, 0, sizeof(BITMAPINFOHEADER));
	m_Bih.biSize = sizeof(BITMAPINFOHEADER);
	m_Bih.biWidth = m_ViewWidth;
	m_Bih.biHeight = m_ViewHeight;
	m_Bih.biPlanes = 1;
	m_Bih.biBitCount = BITS_PER_PIXEL;
	m_Bih.biCompression = BI_RGB;
	m_Bih.biSizeImage = m_dwSize;
	
	m_hDD = DrawDibOpen();
}

void CMeshManager::Clear_BackBuffer()
{
	for ( UINT i = 0; i <  m_ViewHeight; i++)
	{
		for ( UINT j = 0; j < m_ViewWidth; j++ )
		{
			int Index = i * 4 * m_ViewWidth + j * 4;

			m_Data[Index + 0] = (BYTE) (255.0 * 0.3f); // blue
			m_Data[Index + 1] = (BYTE) (255.0 * 0.125f); // green
			m_Data[Index + 2] = 0; // red
			m_Data[Index + 3] = 0; 
		}
	}
}

void CMeshManager::Present_BackBuffer( )
{ 
	//функция DrawDibDraw рисует перевернутое изображение
	//так что перед выводом на экран нужно переворачивать картинку
	//в буфере, но для модели куба это не так важно
	HDC hDC = GetDC(m_hWnd);
	DrawDibDraw(m_hDD, hDC, 0, 0, m_ViewWidth, m_ViewHeight, &m_Bih, m_Data, 0, 0, m_ViewWidth, m_ViewHeight, 0);
	ReleaseDC(m_hWnd, hDC);
}

void CMeshManager::Delete_BackBuffer()
{
	DrawDibClose(m_hDD);

	free(m_Data);
	m_Data = NULL;
}
