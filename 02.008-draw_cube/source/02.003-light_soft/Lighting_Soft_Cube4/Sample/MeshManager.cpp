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

	delete [] m_NormalBuffTransformed;
	m_NormalBuffTransformed = NULL;

	delete [] m_NormalBuff;
	m_NormalBuff = NULL;
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

vector3 CMeshManager::Vec3_Reflect(vector3 &VecIn, vector3 &VecNorm)
{
	vector3 VecOut;

	//float s = 2.0f * ( VecIn.x * VecNorm.x + VecIn.y * VecNorm.y + VecIn.z * VecNorm.z );
	float s = 2.0f * Vec3_Dot(VecIn, VecNorm);

	VecOut.x = VecIn.x - s * VecNorm.x;
	VecOut.y = VecIn.y - s * VecNorm.y;
	VecOut.z = VecIn.z - s * VecNorm.z;

	return VecOut;
}

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;

	Create_BackBuffer();

	m_VertCount =  8;
	m_TriangleCount =  12;

	m_VertBuff = new vector3[8];
	m_VertBuffTransformed = new vector3[8];

	m_VertBuff[0] = vector3(-4.0f, -4.0f, -4.0f);
	m_VertBuff[1] = vector3( 4.0f, -4.0f, -4.0f);
	m_VertBuff[2] = vector3(-4.0f,  4.0f, -4.0f);
	m_VertBuff[3] = vector3( 4.0f,  4.0f, -4.0f);

	m_VertBuff[4] = vector3(-4.0f, -4.0f,  4.0f);
	m_VertBuff[5] = vector3( 4.0f, -4.0f,  4.0f);
	m_VertBuff[6] = vector3(-4.0f,  4.0f,  4.0f);
	m_VertBuff[7] = vector3( 4.0f,  4.0f,  4.0f);

	m_NormalBuff = new vector3[8];
	m_NormalBuffTransformed = new vector3[8];
	
	m_NormalBuff[0] = Vec3_Normalize(m_VertBuff[0]);
	m_NormalBuff[1] = Vec3_Normalize(m_VertBuff[1]);
	m_NormalBuff[2] = Vec3_Normalize(m_VertBuff[2]);
	m_NormalBuff[3] = Vec3_Normalize(m_VertBuff[3]);

	m_NormalBuff[4] = Vec3_Normalize(m_VertBuff[4]);
	m_NormalBuff[5] = Vec3_Normalize(m_VertBuff[5]);
	m_NormalBuff[6] = Vec3_Normalize(m_VertBuff[6]);
	m_NormalBuff[7] = Vec3_Normalize(m_VertBuff[7]);

	WORD IndexTemp[36] = {
	//front face	
	A, C, D,
	A, D, B,
	
	//back face
	G, E, F,
	G, F, H,

	//left face
	E, G, C,
	E, C, A,

	//right face
	B, D, H,
	B, H, F,

	//top face
	C, G, H,
	C, H, D,

	//bottom face
	E, A, B,
	E, B, F};

	m_IndexBuff = new WORD[36];
	memcpy(m_IndexBuff, IndexTemp, 36 * sizeof(WORD));
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
	
	Angle += PI / 50.0f;
    if(Angle > PI2)
		Angle = 0.0f;

	matrix4x4 MatWorld={
		1, 0, 0, 0,
		0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 15, 1};

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
        0, 0, 1, 0,
        0, 0, 0, 1 };

	for (UINT i = 0; i < m_VertCount; i++)
	{
		matrix4x4 MatTemp;
		
		//трансформируем вершины
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

		//трансформируем нормали
		MatTemp = Mat4x4_Mat4x4_Mul(MatRotateX, MatRotateY);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatRotateZ);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatWorld);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatView);
		
		matrix4x4 MatNormal = {
			MatTemp.Mat[0][0], MatTemp.Mat[0][1], MatTemp.Mat[0][2], 0.0f,
			MatTemp.Mat[1][0], MatTemp.Mat[1][1], MatTemp.Mat[1][2], 0.0f,
			MatTemp.Mat[2][0], MatTemp.Mat[2][1], MatTemp.Mat[2][2], 0.0f,
			0.0f,	0.0f,	0.0f,	1.0f };

		vector3 Normal;
		Normal = Vec3_Mat4x4_Mul(m_NormalBuff[i], MatNormal);

		Normal = Vec3_Normalize(Normal);

		m_NormalBuffTransformed[i] = Normal;
	}
}

void CMeshManager::Draw_Color_Poly(int y1, int y2)
{
	float nxi, nyi, nzi;
	float dnx, dny, dnz;

	float vWxi, vWyi, vWzi;
	float dvWx, dvWy, dvWz;

	for ( int y = y1; y < y2; y++ )
	{
		nxi = m_nxl;
		nyi = m_nyl;
		nzi = m_nzl;

		vWxi = m_vWxl;
		vWyi = m_vWyl;
		vWzi = m_vWzl;

		if((m_xr - m_xl) > 0) // делить на 0 нельзя
		{
			dnx = (m_nxr - m_nxl)/(m_xr - m_xl);
			dny = (m_nyr - m_nyl)/(m_xr - m_xl);
			dnz = (m_nzr - m_nzl)/(m_xr - m_xl);

			dvWx = (m_vWxr - m_vWxl)/(m_xr - m_xl);
			dvWy = (m_vWyr - m_vWyl)/(m_xr - m_xl);
			dvWz = (m_vWzr - m_vWzl)/(m_xr - m_xl);
		}
		else
		{
			dnx = m_nxr - m_nxl;
			dny = m_nyr - m_nyl;
			dnz = m_nzr - m_nzl;

			dvWx = m_vWxr - m_vWxl;
			dvWy = m_vWyr - m_vWyl;
			dvWz = m_vWzr - m_vWzl;
		}

		for (int x=(int)m_xl; x<(int)m_xr; x++)
		{
			//получаем интерполированную нормаль и
			//нормализуем ее
			vector3 Normal = vector3(nxi, nyi, nzi);
			Normal = Vec3_Normalize(Normal);

			//интерполированная позиция вершины в видовом пространстве
			//т.е. после умножения на матрицу вида
			vector3 vW = vector3(vWxi, vWyi, vWzi);

			//позиция иточника света
			vector3 VecPosLight = vector3(0.0f, 0.0f, -30.0f);

			//позиция наблюдателя
			vector3 VecCamPos = vector3(0.0f, 0.0f, 0.0f);

			//цвет диффузного освещения
			color_rgb DiffLightColor = color_rgb(255, 255, 128);

			//вычисляем вектор от позиции вершины (в видовых координатах)
			//до позиции источника света, нормализуем этот вектор
			//вектор необходим для расчета диффузного освещения
			vector3 VecLightDirInv = VecPosLight - vW;
			float dist = Vec3_Len(VecLightDirInv);
			VecLightDirInv = Vec3_Normalize(VecLightDirInv);

			//вычисляем вектор  от позиции источника света
			//до вершины, нормализуем этот вектор
			//этот вектор необходим для вычисления зеркального освещения
			vector3 VecLightDir = vW - VecPosLight;
			VecLightDir = Vec3_Normalize(VecLightDir);

			//вычисляем вектор отражения
			//есть вектор куда направлен свет
			//и нормаль к треугольнику - этот ветор необходим
			//для расчета зеркального освещения
			vector3 VecReflect = Vec3_Reflect(VecLightDir, Normal);
			VecReflect = Vec3_Normalize(VecReflect);
			
			//вычисляем вектор от наблюдателя до вершины
			//этот вектор необходим для расчета зеркального освещения
			vector3 VecViewer = VecCamPos - vW;
			VecViewer = Vec3_Normalize(VecViewer);
			
			//вычисляем диффузное освещение
			float DotProdDiff = Vec3_Dot(VecLightDirInv, Normal);

			//вычисляем затухание attenuate
			vector3 AttVal = vector3(0.0f, 0.025f, 0.0f);
			vector3 AttFac = vector3(1.0f, dist, dist*dist);
			float Att = 1.0f / Vec3_Dot(AttVal, AttFac);

			//три цвета - амбиент (окружающий свет), блики (зеркальное освещение),
			//и диффузное освещение
			color_rgb RgbAmbient, RgbSpec, RgbDiff;

			RgbAmbient = color_rgb(128, 128, 64);

			RgbDiff = color_rgb(0,0,0);

			RgbSpec = color_rgb(0,0,0);
			
			if(DotProdDiff > 0)
			{
				//вычисляем цвет диффузного освещения
				RgbDiff = DiffLightColor * Att * DotProdDiff;
				
				//вычисляем зеркальное освещение
				float fDotProdSpec = Vec3_Dot(VecReflect, VecViewer);

				fDotProdSpec = pow(fDotProdSpec, 0.5f);

				if(fDotProdSpec > 0)
				{	
					//вычисляем цвет- зеркальное освещение
					RgbSpec = DiffLightColor * fDotProdSpec;
				}

			}

			//добавляем ambient цвет и складываем все цвета
			color_rgb Rgb = RgbAmbient + RgbDiff + RgbSpec;
			
			//делаем clamp
			if(Rgb.r > 255)
				Rgb.r = 255;

			if(Rgb.g > 255)
				Rgb.g = 255;

			if(Rgb.b > 255)
				Rgb.b = 255;
			
			int indx =  y * 4 * m_ViewWidth + x * 4;

			m_Data[indx + 0] = (BYTE) Rgb.b; // blue
			m_Data[indx + 1] = (BYTE) Rgb.g; // green
			m_Data[indx + 2] = (BYTE) Rgb.r; // red
			m_Data[indx + 3] = 0; 
				
			nxi+=dnx;
			nyi+=dny;
			nzi+=dnz;

			vWxi+=dvWx;
			vWyi+=dvWy;
			vWzi+=dvWz;
		}

		m_xl+=m_dxl;
		m_nxl+=m_dnxl;
		m_nyl+=m_dnyl;
		m_nzl+=m_dnzl;

		m_vWxl+=m_dvWxl;
		m_vWyl+=m_dvWyl;
		m_vWzl+=m_dvWzl;

		m_xr+=m_dxr;
		m_nxr+=m_dnxr;
		m_nyr+=m_dnyr;
		m_nzr+=m_dnzr;

		m_vWxr+=m_dvWxr;
		m_vWyr+=m_dvWyr;
		m_vWzr+=m_dvWzr;
	}
}

void CMeshManager::Draw_Color_Triangle(vector3 VecIn1,
							vector3 VecIn2,
							vector3 VecIn3,
					   vector3 Normal1,
					   vector3 Normal2,
					   vector3 Normal3)
{
	float t;
	int side;

	float x1 = VecIn1.x;
	float y1 = VecIn1.y;
	float z1 = VecIn1.z;
	float xs1 = VecIn1.xs;
	float ys1 = VecIn1.ys;


	float x2 = VecIn2.x;
	float y2 = VecIn2.y;
	float z2 = VecIn2.z;
	float xs2 = VecIn2.xs;
	float ys2 = VecIn2.ys;


	float x3 = VecIn3.x;
	float y3 = VecIn3.y;
	float z3 = VecIn3.z;
	float xs3 = VecIn3.xs;
	float ys3 = VecIn3.ys;


	if (ys2 < ys1)
	{
		SWAP(x2,x1,t);
		SWAP(y2,y1,t);
		SWAP(z2,z1,t);

		SWAP(xs2,xs1,t);
		SWAP(ys2,ys1,t);

		
		SWAP(Normal2.x, Normal1.x, t);
		SWAP(Normal2.y, Normal1.y, t);
		SWAP(Normal2.z, Normal1.z, t);
		
	}

	if (ys3 < ys1)
	{
		SWAP(x3,x1,t);
		SWAP(y3,y1,t);
		SWAP(z3,z1,t);

		SWAP(xs3,xs1,t);
		SWAP(ys3,ys1,t);


		SWAP(Normal3.x, Normal1.x, t);
		SWAP(Normal3.y, Normal1.y, t);
		SWAP(Normal3.z, Normal1.z, t);

	}

	if (ys3 < ys2)
	{
		SWAP(x3,x2,t);
		SWAP(y3,y2,t);
		SWAP(z3,z2,t);

		SWAP(xs3,xs2,t);
		SWAP(ys3,ys2,t);


		SWAP(Normal3.x, Normal2.x, t);
		SWAP(Normal3.y, Normal2.y, t);
		SWAP(Normal3.z, Normal2.z, t);

	}

	if (ys2 > ys1 && ys3 > ys2)
	{
		float dxdy1 = (xs2 - xs1) / (ys2 - ys1);
		float dxdy2 = (xs3 - xs1) / (ys3 - ys1);
		side = dxdy2 > dxdy1;
	}

	if (ys1 == ys2)
		side = xs1 > xs2;
	if (ys2 == ys3)
		side = xs3 > xs2;

	if (!side)	//длинее левая сторона
	{
		m_xl = xs1;
		m_nxl = (float) Normal1.x;
		m_nyl = (float) Normal1.y;
		m_nzl = (float) Normal1.z;

		m_vWxl = (float) x1;
		m_vWyl = (float) y1;
		m_vWzl = (float) z1;

		m_dxl = (xs3 - xs1) / (ys3 - ys1);
		m_dnxl = (Normal3.x - Normal1.x) / (ys3 - ys1);
		m_dnyl = (Normal3.y - Normal1.y) / (ys3 - ys1);
		m_dnzl = (Normal3.z - Normal1.z) / (ys3 - ys1);

		m_dvWxl = (x3 - x1) / (ys3 - ys1);
		m_dvWyl = (y3 - y1) / (ys3 - ys1);
		m_dvWzl = (z3 - z1) / (ys3 - ys1);

		if ( ys1 < ys2)
		{
			m_xr = xs1;
			m_nxr = Normal1.x;
			m_nyr = Normal1.y;
			m_nzr = Normal1.z;

			m_vWxr = x1;
			m_vWyr = y1;
			m_vWzr = z1;

			m_dxr = (xs2 - xs1) / (ys2 - ys1);
			m_dnxr = (Normal2.x - Normal1.x) / (ys2 - ys1);
			m_dnyr = (Normal2.y - Normal1.y) / (ys2 - ys1);
			m_dnzr = (Normal2.z - Normal1.z) / (ys2 - ys1);

			m_dvWxr = (x2 - x1) / (ys2 - ys1);
			m_dvWyr = (y2 - y1) / (ys2 - ys1);
			m_dvWzr = (z2 - z1) / (ys2 - ys1);

			Draw_Color_Poly((int)ys1, (int)ys2);	
		}
		if(ys2 < ys3)
		{
			m_xr = xs2;
			m_nxr = Normal2.x;
			m_nyr = Normal2.y;
			m_nzr = Normal2.z;

			m_vWxr = x2;
			m_vWyr = y2;
			m_vWzr = z2;

			m_dxr = (xs3 - xs2) / (ys3 - ys2);
			m_dnxr = (Normal3.x - Normal2.x) / (ys3 - ys2);
			m_dnyr = (Normal3.y - Normal2.y) / (ys3 - ys2);
			m_dnzr = (Normal3.z - Normal2.z) / (ys3 - ys2);

			m_dvWxr = (x3 - x2) / (ys3 - ys2);
			m_dvWyr = (y3 - y2) / (ys3 - ys2);
			m_dvWzr = (z3 - z2) / (ys3 - ys2);

			Draw_Color_Poly((int)ys2, (int)ys3);
		}
	}
	else
	{
		m_xr = xs1;
		m_nxr = Normal1.x;
		m_nyr = Normal1.y;
		m_nzr = Normal1.z;
		
		m_vWxr = x1;
		m_vWyr = y1;
		m_vWzr = z1;
		
		m_dxr = (xs3 - xs1) / (ys3 - ys1);
		m_dnxr = (Normal3.x - Normal1.x) / (ys3 - ys1);
		m_dnyr = (Normal3.y - Normal1.y) / (ys3 - ys1);
		m_dnzr = (Normal3.z - Normal1.z) / (ys3 - ys1);

		m_dvWxr = (x3 - x1) / (ys3 - ys1);
		m_dvWyr = (y3 - y1) / (ys3 - ys1);
		m_dvWzr = (z3 - z1) / (ys3 - ys1);

		if (ys1 < ys2)
		{
			m_xl = xs1;
			m_nxl = Normal1.x;
			m_nyl = Normal1.y;
			m_nzl = Normal1.z;

			m_vWxl = x1;
			m_vWyl = y1;
			m_vWzl = z1;

			m_dxl = (xs2 - xs1) / (ys2 - ys1);
			m_dnxl = (Normal2.x - Normal1.x) / (ys2 - ys1);
			m_dnyl = (Normal2.y - Normal1.y) / (ys2 - ys1);
			m_dnzl = (Normal2.z - Normal1.z) / (ys2 - ys1);

			m_dvWxl = (x2 - x1) / (ys2 - ys1);
			m_dvWyl = (y2 - y1) / (ys2 - ys1);
			m_dvWzl = (z2 - z1) / (ys2 - ys1);

			Draw_Color_Poly((int)ys1, (int)ys2);
		}
		if (ys2 < ys3)
		{
			m_xl = xs2;
			m_nxl = Normal2.x;
			m_nyl = Normal2.y;
			m_nzl = Normal2.z;

			m_vWxl = x2;
			m_vWyl = y2;
			m_vWzl = z2;

			m_dxl = (xs3 - xs2) / (ys3 - ys2);
			m_dnxl = (Normal3.x - Normal2.x) / (ys3 - ys2);
			m_dnyl = (Normal3.y - Normal2.y) / (ys3 - ys2);
			m_dnzl = (Normal3.z - Normal2.z) / (ys3 - ys2);
			
			m_dvWxl = (x3 - x2) / (ys3 - ys2);
			m_dvWyl = (y3 - y2) / (ys3 - ys2);
			m_dvWzl = (z3 - z2) / (ys3 - ys2);
			
			Draw_Color_Poly((int)ys2, (int)ys3);
		}
	}
}

void CMeshManager::Draw_MeshManager()
{
	 Clear_BackBuffer();

    for (UINT i = 0; i < m_TriangleCount; i++)
    {
		vector3 Vec1 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 0]];
		vector3 Vec2 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 1]];
		vector3 Vec3 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 2]];

		float s = (Vec2.xs - Vec1.xs) * (Vec3.ys - Vec1.ys) - (Vec2.ys - Vec1.ys) * (Vec3.xs - Vec1.xs);

		if (s <= 0)
			continue;

		//positional light source, specular lighting - зеркальное освещение
		//модель освещения по Фонгу - т.е. 
		//ambient + diffuse + specular lighing 
		//(фоновое освещение + диффузное освещение + зеркальное освещение),
		//и модель затенения по Фонгу - т.е. интерполяция нормалей
		//у каждой из 3х вершин треугольника своя нормаль

		vector3 Normal1 = m_NormalBuffTransformed[m_IndexBuff[i * 3 + 0]];
		vector3 Normal2 = m_NormalBuffTransformed[m_IndexBuff[i * 3 + 1]];
		vector3 Normal3 = m_NormalBuffTransformed[m_IndexBuff[i * 3 + 2]];

		Draw_Color_Triangle(Vec1, Vec2, Vec3,
						Normal1, Normal2, Normal3);
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
