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

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;
	
	Create_BackBuffer();
	
	int Stacks = 10;
	int Slices = 20;
	//float PI2  = 6.28318530717958f;
	//float PI     = 3.14159265358979f;
	float ThetaFac = PI2 / (float)Slices;
	float PhiFac = PI / (float)Stacks;
	float Radius = 15.0f;

	m_VertCount =  Slices * Stacks * 4;
	m_TriangleCount =  Slices * Stacks * 2;

	m_VertBuff = NULL;
	m_VertBuff = new vector3[m_VertCount];

	m_IndexBuff = NULL;
	m_IndexBuff = new DWORD[m_TriangleCount  * 3];

	m_VertBuffTransformed = NULL;
	m_VertBuffTransformed = new vector3[m_VertCount];

	int Index = 0;
	
	for(int l = 0; l < Slices; l++)
	{
		for(int b = 0; b < Stacks; b++)
		{
			float sb = PhiFac * b;
			float sl = ThetaFac * l;

			m_VertBuff[Index + 0].z = Radius * sinf(sb) * cosf(sl);
			m_VertBuff[Index + 0].x = Radius * sinf(sb) * sinf(sl);
			m_VertBuff[Index + 0].y = Radius * cosf(sb);

			//------------------------------

			sb = PhiFac * (b + 1);
			sl = ThetaFac * l;

			m_VertBuff[Index + 1].z = Radius * sinf(sb) * cosf(sl);
			m_VertBuff[Index + 1].x = Radius * sinf(sb) * sinf(sl);
			m_VertBuff[Index + 1].y = Radius * cosf(sb);

			//------------------------------
			
			sb = PhiFac * (b + 1);
			sl = ThetaFac * (l + 1);

			m_VertBuff[Index + 2].z = Radius * sinf(sb) * cosf(sl);
			m_VertBuff[Index + 2].x = Radius * sinf(sb) * sinf(sl);
			m_VertBuff[Index + 2].y = Radius * cosf(sb);

			//------------------------------

			sb = PhiFac * b;
			sl = ThetaFac * (l + 1);

			m_VertBuff[Index + 3].z = Radius * sinf(sb) * cosf(sl);
			m_VertBuff[Index + 3].x = Radius * sinf(sb) * sinf(sl);
			m_VertBuff[Index + 3].y = Radius * cosf(sb);

			Index+=4;
		}
	}

	UINT BaseIndex = 0;
	UINT BaseVert = 0;
		
	while (BaseVert < m_VertCount)
	{
		m_IndexBuff[BaseIndex + 0] =   BaseVert;
		m_IndexBuff[BaseIndex + 1] =   BaseVert+1;
		m_IndexBuff[BaseIndex + 2] =   BaseVert+2;

		m_IndexBuff[BaseIndex + 3] = BaseVert;
		m_IndexBuff[BaseIndex + 4] = BaseVert+2;
		m_IndexBuff[BaseIndex + 5] = BaseVert+3;

		BaseIndex += 6;
		BaseVert += 4;
	}
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
	
	Angle += PI / 200.0f;
    if(Angle > PI2)
		Angle = 0.0f;

	matrix4x4 MatWorld={
		1, 0, 0, 0,
		0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 30, 1};

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

		MatTemp = Mat4x4_Mat4x4_Mul(MatRotateX, MatRotateY);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatRotateZ);
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

		//демонстрация positional light source, gouraud shading

		//находим цвет каждой вершины треугольника

		//находим края треугольника
		//что бы найти нормаль к треугольнику
		//нормаль к треугольнику необходима для
		//рассчета освещенности треугольника
		vector3 VecEdge1 = Vec2 - Vec1;
		vector3 VecEdge2 = Vec3 - Vec1;
		
		VecEdge1 = Vec3_Normalize(VecEdge1);
		VecEdge2 = Vec3_Normalize(VecEdge2);

		//находим нормаль к треугольнику
		vector3 VecCross = Vec3_Cross(VecEdge1, VecEdge2);
		VecCross = Vec3_Normalize(VecCross);

		color_rgb Rgb1;
		color_rgb Rgb2;
		color_rgb Rgb3;

		//позиция источника света
		vector3 VecPosLight = {0.0f, 0.0f, -25.0f};

		//цвет диффузного освещения
		color_rgb DiffLightColor= color_rgb(255, 255, 128);

		//находим три вектора - от каждой вершины треугольника
		//до позиции источника света
		vector3 VecLightDir1 = VecPosLight - Vec1;
		vector3 VecLightDir2 = VecPosLight - Vec2;
		vector3 VecLightDir3 = VecPosLight - Vec3;

		//узнаем длинну вектора для каждой вершины
		float Dist1 = Vec3_Len(VecLightDir1);
		float Dist2 = Vec3_Len(VecLightDir2);
		float Dist3 = Vec3_Len(VecLightDir3);
		
		//нормализуем длинну вектора
		VecLightDir1 = Vec3_Normalize(VecLightDir1);
		VecLightDir2 = Vec3_Normalize(VecLightDir2);
		VecLightDir3 = Vec3_Normalize(VecLightDir3);
	
		//вычисляем косинус
		float DotProd1 = Vec3_Dot(VecCross,VecLightDir1);
		float DotProd2 = Vec3_Dot(VecCross,VecLightDir2);
		float DotProd3 = Vec3_Dot(VecCross,VecLightDir3);

		//вычисляем затухание attenuate
		//суть затухания в том что чем больше дистанция
		//от вершины до источника света тем меньше освещенность вершины
		vector3 Att = {0.0f, 0.025f, 0.0f};

		vector3 AttFac1 = {1.0f, Dist1, Dist1*Dist1};
		vector3 AttFac2 = {1.0f, Dist2, Dist2*Dist2};
		vector3 AttFac3 = {1.0f, Dist3, Dist3*Dist3};

		float Att1 = 1.0f / Vec3_Dot(Att, AttFac1);
		float Att2 = 1.0f / Vec3_Dot(Att, AttFac2);
		float Att3 = 1.0f / Vec3_Dot(Att, AttFac3);

		//на основе косинуса и затухания вычисляем
		//цвет каждой вершины
		Rgb1 = DiffLightColor * Att1 * DotProd1;
		Rgb2 = DiffLightColor * Att2 * DotProd2;
		Rgb3 = DiffLightColor * Att3 * DotProd3;

		if(Rgb1.r > 255)
			Rgb1.r = 255;

		if(Rgb1.g > 255)
			Rgb1.g = 255;

		if(Rgb1.b > 255)
			Rgb1.b = 255;

		if(Rgb2.r > 255)
			Rgb2.r = 255;

		if(Rgb2.g > 255)
			Rgb2.g = 255;

		if(Rgb2.b > 255)
			Rgb2.b = 255;

		if(Rgb3.r > 255)
			Rgb3.r = 255;

		if(Rgb3.g > 255)
			Rgb3.g = 255;

		if(Rgb3.b > 255)
			Rgb3.b = 255;

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
