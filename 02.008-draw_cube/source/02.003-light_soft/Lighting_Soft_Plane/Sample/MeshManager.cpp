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
}

matrix4x4 CMeshManager::Mat4x4_Mat4x4_Mul(matrix4x4 &MatIn1, matrix4x4 &MatIn2)
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

float CMeshManager::Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2)
{
	return VecIn1.x*VecIn2.x + VecIn1.y*VecIn2.y + VecIn1.z* VecIn2.z;
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

float CMeshManager::Vec3_Len(vector3 &VecIn)
{
	return sqrtf(VecIn.x * VecIn.x + VecIn.y * VecIn.y + VecIn.z * VecIn.z);
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

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;
	
	Create_BackBuffer();

	m_NumCellsPerRow = 25;
	m_NumCellsPerCol = 25;
	m_CellSpacing = 2.0;

	m_NumVertsPerRow = m_NumCellsPerRow + 1;
	m_NumVertsPerCol = m_NumCellsPerCol + 1;

	//всего треугольников и вершин
	m_TriangleCount = m_NumCellsPerRow * m_NumCellsPerCol * 2;
	m_VertCount = m_NumVertsPerRow * m_NumVertsPerCol;

	//общие массивы
	m_VertBuff = new vector3[m_VertCount];
	m_VertBuffTransformed = new vector3[m_VertCount];
	m_IndexBuff = new unsigned int[m_TriangleCount * 3];

	Build_Side(m_VertBuff, m_IndexBuff);
}

void CMeshManager::Update_MeshManager()
{
	float AngleX = -(PI / 2);
	
	matrix4x4 MatRotateX={
		1, 0, 0, 0,
        0, cosf(AngleX), sinf(AngleX), 0,
        0, -sinf(AngleX), cosf(AngleX), 0,
        0, 0, 0, 1};
	
	float AngleY = -(PI / 8);
		
	matrix4x4 MatRotateY={
		cosf(AngleY), 0, -sinf(AngleY), 0,
		0, 1, 0, 0,
		sinf(AngleY), 0, cosf(AngleY), 0,
		0, 0, 0, 1};

		/*
	matrix4x4 MatRotateZ={
		cosf(Angle), sinf(Angle), 0, 0,
		-sinf(Angle), cosf(Angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};
	
	Angle += PI / 200.0f;
    if(Angle > PI2)
		Angle = 0.0f;
*/
	matrix4x4 MatWorld={
		1, 0, 0, 0,
		0, 1, 0, 0,
        0, 0, 1, 0,
        -5, 0, 60, 1};

	matrix4x4 MatView={
		1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

	float Fov = PI/2.0f; // FOV 90 degree
    float Aspect = (float)m_ViewWidth / m_ViewHeight;
	float ZFar = 100.0f;
	float ZNear = 0.1f;

	float h, w, Q;

    w = (1.0f/tanf(Fov*0.5f))/Aspect;  
    h = 1.0f/tanf(Fov*0.5f);   
    Q = ZFar/(ZFar - ZNear);

	/*
    matrix4x4 MatProj={
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, Q, 1,
        0, 0, -Q*fZNear, 0 };
	*/

    matrix4x4 MatProj={
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, 1, 1,
        0, 0, 0, 1 };

	for (UINT i = 0; i < m_VertCount; i++)
	{
		matrix4x4 MatTemp;

		/*
		MatTemp = Mat4x4_Mat4x4_Mul(MatRotateX, MatRotateY);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatRotateZ);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatWorld);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatView);
		*/

		//MatTemp = Mat4x4_Mat4x4_Mul(MatRotateX, MatWorld);
		MatTemp = Mat4x4_Mat4x4_Mul(MatRotateX, MatRotateY);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatWorld);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatView);
		
		vector3 VecTemp;

		VecTemp = Vec3_Mat4x4_Mul(m_VertBuff[i], MatTemp);

		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatProj);

		vector3 VecScreenTemp;
		
		VecScreenTemp = Vec3_Mat4x4_Mul(m_VertBuff[i], MatTemp);

		VecScreenTemp.xs = VecScreenTemp.x / VecScreenTemp.z;
		VecScreenTemp.ys = VecScreenTemp.y / VecScreenTemp.z;

		VecScreenTemp.xs = VecScreenTemp.xs * m_ViewWidth / 2.0f + m_ViewWidth / 2.0f;
		VecScreenTemp.ys = -VecScreenTemp.ys * m_ViewHeight / 2.0f + m_ViewHeight / 2.0f;

		VecTemp.xs = VecScreenTemp.xs;
		VecTemp.ys = VecScreenTemp.ys;

		m_VertBuffTransformed[i] = VecTemp;
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

		m_xl+=m_dxl;
		m_redl+=m_dredl;
		m_greenl+=m_dgreenl;
		m_bluel+=m_dbluel;

		m_xr+=m_dxr;
		m_redr+=m_dredr;
		m_greenr+=m_dgreenr;
		m_bluer+=m_dbluer;
	}

}

void CMeshManager::Draw_Color_Triangle(float x1,float y1,
					   float x2,float y2,
					   float x3,float y3,
					   color_rgb Color1,
					   color_rgb Color2,
					   color_rgb Color3)
{
	float t;
	int ct;
	int side;

	if (y2 < y1)
	{
		SWAP(x2,x1,t);
		SWAP(y2,y1,t);
		
		SWAP(Color2.r, Color1.r, ct);
		SWAP(Color2.g, Color1.g, ct);
		SWAP(Color2.b, Color1.b, ct);
	}

	if (y3 < y1)
	{
		SWAP(x3,x1,t);
		SWAP(y3,y1,t);

		SWAP(Color3.r, Color1.r, ct);
		SWAP(Color3.g, Color1.g, ct);
		SWAP(Color3.b, Color1.b, ct);
	}

	if (y3 < y2)
	{
		SWAP(x3,x2,t);
		SWAP(y3,y2,t);

		SWAP(Color3.r, Color2.r, ct);
		SWAP(Color3.g, Color2.g, ct);
		SWAP(Color3.b, Color2.b, ct);
	}

	if (y2 > y1 && y3 > y2)
	{
		float dxdy1 = (x2 - x1) / (y2 - y1);
		float dxdy2 = (x3 - x1) / (y3 - y1);
		side = dxdy2 > dxdy1;
	}

	if (y1 == y2)
		side = x1 > x2;
	if (y2 == y3)
		side = x3 > x2;

	if (!side)	//длиннее левая сторона
	{
		m_xl = x1;
		m_redl = (float) Color1.r;
		m_greenl = (float) Color1.g;
		m_bluel = (float) Color1.b;

		m_dxl = (x3 - x1) / (y3 - y1);
		m_dredl = (Color3.r - Color1.r) / (y3 - y1);
		m_dgreenl = (Color3.g - Color1.g) / (y3 - y1);
		m_dbluel = (Color3.b - Color1.b) / (y3 - y1);
		
		if ( y1 < y2)
		{
			m_xr = x1;
			m_redr = (float) Color1.r;
			m_greenr = (float) Color1.g;
			m_bluer = (float) Color1.b;

			m_dxr = (x2 - x1) / (y2 - y1);
			m_dredr = (Color2.r - Color1.r) / (y2 - y1);
			m_dgreenr = (Color2.g - Color1.g) / (y2 - y1);
			m_dbluer = (Color2.b - Color1.b) / (y2 - y1);

			Draw_Color_Poly((int)y1, (int)y2);	
		}
		if(y2 < y3)
		{
			m_xr = x2;
			m_redr = (float) Color2.r;
			m_greenr = (float) Color2.g;
			m_bluer = (float) Color2.b;

			m_dxr = (x3 - x2) / (y3 - y2);
			m_dredr = (Color3.r - Color2.r) / (y3 - y2);
			m_dgreenr = (Color3.g - Color2.g) / (y3 - y2);
			m_dbluer = (Color3.b - Color2.b) / (y3 - y2);
			
			Draw_Color_Poly((int)y2, (int)y3);
		}
	}
	else
	{
		m_xr = x1;
		m_redr = (float) Color1.r;
		m_greenr = (float) Color1.g;
		m_bluer = (float) Color1.b;
		
		m_dxr = (x3 - x1) / (y3 - y1);
		m_dredr = (Color3.r - Color1.r) / (y3 - y1);
		m_dgreenr = (Color3.g - Color1.g) / (y3 - y1);
		m_dbluer = (Color3.b - Color1.b) / (y3 - y1);

		if (y1 < y2)
		{
			m_xl = x1;
			m_redl = (float) Color1.r;
			m_greenl = (float) Color1.g;
			m_bluel = (float) Color1.b;

			m_dxl = (x2 - x1) / (y2 - y1);
			m_dredl = (Color2.r - Color1.r) / (y2 - y1);
			m_dgreenl = (Color2.g - Color1.g) / (y2 - y1);
			m_dbluel = (Color2.b - Color1.b) / (y2 - y1);

			Draw_Color_Poly((int)y1, (int)y2);
		}
		if (y2 < y3)
		{
			m_xl = x2;
			m_redl = (float) Color2.r;
			m_greenl = (float) Color2.g;
			m_bluel = (float) Color2.b;

			m_dxl = (x3 - x2) / (y3 - y2);
			m_dredl = (Color3.r - Color2.r) / (y3 - y2);
			m_dgreenl = (Color3.g - Color2.g) / (y3 - y2);
			m_dbluel = (Color3.b - Color2.b) / (y3 - y2);
			
			Draw_Color_Poly((int)y2, (int)y3);
		}
	}
}

float CMeshManager::Calc_Attenuation(float d, float FalloffStart, float FalloffEnd)
{
	float Att = (FalloffEnd-d) / (FalloffEnd - FalloffStart);

	if (Att > 1.0f)
		return 1.0f;
	else if(Att < 0.0f)
		return 0.0f;

    return Att;
}

float CMeshManager::Max(float a, float b)
{
    return (a > b) ? a : b;
}

void CMeshManager::Draw_MeshManager ()
{
	 Clear_BackBuffer();

    for (UINT i = 0; i < m_TriangleCount; i++)
    {
		vector3 Vec1 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 0]];
		vector3 Vec2 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 1]];
		vector3 Vec3 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 2]];

		//используем псевдоскалярное (косое) умножение векторов
		//для отбрасывания задних поверхностей
		float s = (Vec2.xs - Vec1.xs) * (Vec3.ys - Vec1.ys) - (Vec2.ys - Vec1.ys) * (Vec3.xs - Vec1.xs);

		//здесь значение 0.1f используется что бы отбрасывать вырожденные
		//треугольники у которых площадь равна нулю (или прилизительно ноль)
		if (s <= 0.1f)
			continue;

		//демонстрация spot light source, gouraud shading

		//находим цвет каждой вершины треугольника

		float SpotPower = 25.0f;
		vector3 LightDirection = { 0.0f, 0.0f, 1.0f };
		//позиция источника света
		vector3 VecPosLight = {0.0f, 0.0f, 0.0f};
		//цвет диффузного освещения
		color_rgb DiffLightColor= color_rgb(255, 255, 128);

		vector3 LightToVertPos1 = Vec1 - VecPosLight;
		vector3 LightToVertPos2 = Vec2 - VecPosLight;
		vector3 LightToVertPos3 = Vec3 - VecPosLight;

		LightToVertPos1 = Vec3_Normalize(LightToVertPos1);
		LightToVertPos2 = Vec3_Normalize(LightToVertPos2);
		LightToVertPos3 = Vec3_Normalize(LightToVertPos3);

		float SpotFactor1 = pow(Max(Vec3_Dot(LightToVertPos1, LightDirection), 0.0f), SpotPower);
		float SpotFactor2 = pow(Max(Vec3_Dot(LightToVertPos2, LightDirection), 0.0f), SpotPower);
		float SpotFactor3 = pow(Max(Vec3_Dot(LightToVertPos3, LightDirection), 0.0f), SpotPower);

		color_rgb Rgb1 = DiffLightColor * SpotFactor1;
		color_rgb Rgb2 = DiffLightColor * SpotFactor2;
		color_rgb Rgb3 = DiffLightColor * SpotFactor3;

		Draw_Color_Triangle(Vec1.xs, Vec1.ys, Vec2.xs, Vec2.ys, Vec3.xs, Vec3.ys,
			Rgb1, Rgb2, Rgb3);
     }  

	Present_BackBuffer();
}

void CMeshManager::Create_BackBuffer()
{
	RECT Rc;
	GetClientRect(m_hWnd, &Rc);

	m_ViewWidth = Rc.right;
	m_ViewHeight = Rc.bottom;

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

			m_Data[Index + 0] = (BYTE)(255.0 * 0.3f); // blue
			m_Data[Index + 1] = (BYTE)(255.0 * 0.125f); // green
			m_Data[Index + 2] = 0; // red
			m_Data[Index + 3] = 0; 
		}
	}
}

void CMeshManager::Present_BackBuffer( )
{    
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
