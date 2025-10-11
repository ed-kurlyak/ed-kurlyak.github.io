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

	delete [] m_TexCoordBuff;
	m_TexCoordBuff = NULL;

	delete [] m_NormalBuffTransformed;
	m_NormalBuffTransformed = NULL;
	
	delete [] m_NormalBuff;
	m_NormalBuff = NULL;


	delete [] m_Res;
	m_Res = NULL;
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

vector3 CMeshManager::Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 &MatIn)
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

vector3 CMeshManager::Vec3_Normalize(vector3 &VecIn)
{
	vector3 VecOut;

	float Len = sqrtf(VecIn.x * VecIn.x + VecIn.y * VecIn.y + VecIn.z * VecIn.z);

	VecOut.x = VecIn.x / Len;
	VecOut.y = VecIn.y / Len;
	VecOut.z = VecIn.z / Len;
	
	return VecOut;

}

float CMeshManager::Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2)
{
	return VecIn1.x*VecIn2.x + VecIn1.y*VecIn2.y + VecIn1.z* VecIn2.z;
}

void CMeshManager::Init_MeshManager(HWND hwnd)
{
	m_Res = NULL;

	m_hWnd = hwnd;

	Create_BackBuffer();

	m_VertCount = 24;
	m_TriangleCount = 12;

	Read_BMP_File(".//texture.bmp");
	
	vector3 VertBuffTemp[24] = {
		{-5.000000,-5.000000,-5.000000},
		{-5.000000,-5.000000,5.000000},
		{5.000000,-5.000000,5.000000},
		{5.000000,-5.000000,-5.000000},
		{-5.000000,5.000000,-5.000000},
		{5.000000,5.000000,-5.000000},
		{5.000000,5.000000,5.000000},
		{-5.000000,5.000000,5.000000},
		{-5.000000,-5.000000,-5.000000},
		{5.000000,-5.000000,-5.000000},
		{5.000000,5.000000,-5.000000},
		{-5.000000,5.000000,-5.000000},
		{5.000000,-5.000000,-5.000000},
		{5.000000,-5.000000,5.000000},
		{5.000000,5.000000,5.000000},
		{5.000000,5.000000,-5.000000},
		{5.000000,-5.000000,5.000000},
		{-5.000000,-5.000000,5.000000},
		{-5.000000,5.000000,5.000000},
		{5.000000,5.000000,5.000000},
		{-5.000000,-5.000000,5.000000},
		{-5.000000,-5.000000,-5.000000},
		{-5.000000,5.000000,-5.000000},
		{-5.000000,5.000000,5.000000} };

	tex_coord2 TexCoordTemp[24] = {
		m_TextureWidth - 1,	m_TextureHeight - 1,
		m_TextureWidth - 1,	0,
		0,					0,
		0,					m_TextureHeight - 1,
		0,					m_TextureHeight - 1,
		m_TextureWidth - 1,	m_TextureHeight - 1,
		m_TextureWidth - 1,	0,
		0,					0,
		0,					m_TextureHeight - 1,
		m_TextureWidth - 1,	m_TextureHeight - 1,
		m_TextureWidth - 1,	0,
		0,					0,
		0,					m_TextureHeight - 1,
		m_TextureWidth - 1,	m_TextureHeight - 1,
		m_TextureWidth - 1,	0,
		0,					0,
		0,					m_TextureHeight - 1,
		m_TextureWidth - 1,	m_TextureHeight - 1,
		m_TextureWidth - 1,	0,
		0,					0,
		0,					m_TextureHeight - 1,
		m_TextureWidth - 1,	m_TextureHeight - 1,
		m_TextureWidth - 1,	0,
		0,					0};

	DWORD IndexBuffTemp[36] = {
		0,2,1,
		2,0,3,
		4,6,5,
		6,4,7,
		8,10,9,
		10,8,11,
		12,14,13,
		14,12,15,
		16,18,17,
		18,16,19,
		20,22,21,
		22,20,23};

	m_VertBuff = NULL;
	m_VertBuff = new vector3[24];

	m_VertBuffTransformed = NULL;
	m_VertBuffTransformed = new vector3[24];

	m_TexCoordBuff = NULL;
	m_TexCoordBuff = new tex_coord2[24];

	m_IndexBuff = NULL;
	m_IndexBuff = new DWORD[36];

	memcpy(m_VertBuff, VertBuffTemp, 24 * sizeof(vector3));
	memcpy(m_TexCoordBuff, TexCoordTemp, 24 * sizeof(tex_coord2));
	memcpy(m_IndexBuff, IndexBuffTemp, 36 * sizeof(DWORD));

	m_NormalBuff = new vector3[24];
	m_NormalBuffTransformed = new vector3[24];

	for ( int i = 0; i < 24; i++ )
	{
		m_NormalBuff[i] = Vec3_Normalize(m_VertBuff[i]);
	}

	for(int j=0; j<256; j++)
	{
		for(int i=0; i<256; i++)
		{
			RGB_Table[i + (j * 256)] = (unsigned short)((i * j) >> 8);
		}
	}

}

void CMeshManager::Update_MeshManager()
{
	static float Angle = 0.0f;

	matrix4x4 MatRotateX={
		1, 0, 0, 0,
        0, cosf(Angle), sinf(Angle), 0,
        0,-sinf(Angle),  cosf(Angle), 0,
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

	Angle += PI / 45.0f;
    if(Angle > PI2)
		Angle = 0.0f;

	matrix4x4 MatWorld={
		1, 0, 0, 0,
		0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 18.0f, 1};

	matrix4x4 MatView={
		1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

	float Fov =PI/2.0f; // FOV 90 degree
    float Aspect = (float)m_ViewWidth/m_ViewHeight;
	float ZFar = 100.0f;
	float ZNear = 0.1f;

	float h, w, Q;

    w = (1.0f/tanf(Fov*0.5f))/Aspect;  
    h = 1.0f/tanf(Fov*0.5f);   
    Q = ZFar/(ZFar - ZNear);
	
	/*
	//полный расчет матрицы проекции нам не нужен
    matrix4x4 MatProj={
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, Q, 1,
        0, 0, -Q*ZNear, 0 };
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

		vector3 VecNormal;
		VecNormal = Vec3_Mat4x4_Mul(m_NormalBuff[i], MatNormal);

		VecNormal = Vec3_Normalize(VecNormal);

		m_NormalBuffTransformed[i] = VecNormal;
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

		float s = (Vec2.xs - Vec1.xs) * (Vec3.ys - Vec1.ys) - (Vec2.ys - Vec1.ys) * (Vec3.xs - Vec1.xs);

		if(s <= 0)
			continue;

		tex_coord2 Tex1 = m_TexCoordBuff[m_IndexBuff[i * 3 + 0]];
		tex_coord2 Tex2 = m_TexCoordBuff[m_IndexBuff[i * 3 + 1]];
		tex_coord2 Tex3 = m_TexCoordBuff[m_IndexBuff[i * 3 + 2]];

		//начинаем расчет освещенности каждой вершины треугольника

		vector3 VecNormal1 = m_NormalBuffTransformed[m_IndexBuff[i * 3 + 0]];
		vector3 VecNormal2 = m_NormalBuffTransformed[m_IndexBuff[i * 3 + 1]];
		vector3 VecNormal3 = m_NormalBuffTransformed[m_IndexBuff[i * 3 + 2]];

		color_rgb Rgb1;
		color_rgb Rgb2;
		color_rgb Rgb3;

		vector3 VecPosLight = { 0.0f, 0.0f, -45.0f };
		color_rgb DiffLightColor= color_rgb(255, 255, 128);

		vector3 VecLightDir1 = VecPosLight - Vec1;
		vector3 VecLightDir2 = VecPosLight - Vec2;
		vector3 VecLightDir3 = VecPosLight - Vec3;

		VecLightDir1 = Vec3_Normalize(VecLightDir1);
		VecLightDir2 = Vec3_Normalize(VecLightDir2);
		VecLightDir3 = Vec3_Normalize(VecLightDir3);
	
		float DotProd1 = Vec3_Dot(VecNormal1,VecLightDir1);
		float DotProd2 = Vec3_Dot(VecNormal2,VecLightDir2);
		float DotProd3 = Vec3_Dot(VecNormal3,VecLightDir3);

		//вершина имеет черный цвет если cos<=0
		if(DotProd1 <= 0)
		{
			Rgb1 = color_rgb(0,0,0);
		}
		else
		{
			Rgb1 = DiffLightColor * DotProd1;
		}
		
		//вершина имеет черный цвет если cos<=0
		if(DotProd2 <= 0)
		{
			Rgb2 = color_rgb(0,0,0);
		}
		else
		{
			Rgb2 = DiffLightColor * DotProd2;
		}
		
		//вершина имеет черный цвет если cos<=0
		if(DotProd3 <= 0)
		{
			Rgb3 = color_rgb(0,0,0);
		}
		else
		{
			Rgb3 = DiffLightColor * DotProd3;
		}
		
		/*
		//добавляем ambient цвет
		Rgb1 = Rgb1 + color_rgb(96, 96, 48);
		
		if(Rgb1.r > 255)
			Rgb1.r = 255;

		if(Rgb1.g > 255)
			Rgb1.g = 255;

		if(Rgb1.b > 255)
			Rgb1.b = 255;

		//добавляем ambient цвет
		Rgb2 = Rgb2 + color_rgb(96, 96, 48);
		
		if(Rgb2.r > 255)
			Rgb2.r = 255;

		if(Rgb2.g > 255)
			Rgb2.g = 255;

		if(Rgb2.b > 255)
			Rgb2.b = 255;

		//добавляем ambient цвет
		Rgb3 = Rgb3 + color_rgb(96, 96, 48);
		
		if(Rgb3.r > 255)
			Rgb3.r = 255;

		if(Rgb3.g > 255)
			Rgb3.g = 255;

		if(Rgb3.b > 255)
			Rgb3.b = 255;
			*/

		Draw_Textured_Triangle(Vec1, Tex1, Rgb1, Vec2, Tex2, Rgb2, Vec3, Tex3, Rgb3 );
     }  

	Present_BackBuffer();

	Sleep(25);
	
}

void CMeshManager::Create_BackBuffer()
{
	RECT Rc;
    ::GetClientRect( m_hWnd, &Rc );

    m_ViewWidth  = Rc.right;
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

void CMeshManager::Present_BackBuffer()
{
	//переворачиваем задний буфер
	DWORD m_dwSize = m_ViewWidth * (BITS_PER_PIXEL >> 3) * m_ViewHeight;
	
	LPBYTE m_DataTemp = (LPBYTE)malloc(m_dwSize * sizeof(BYTE));

	for (UINT h = 0; h < m_ViewHeight; h++ )
	{
		for (UINT w = 0; w < m_ViewWidth; w++)
		{
			int Index = h * 4 * m_ViewWidth + w * 4;

			BYTE b = m_Data[Index + 0]; // blue
			BYTE g = m_Data[Index + 1]; // green
			BYTE r = m_Data[Index + 2]; // red
			
			int IndexTemp = (m_ViewHeight - 1 - h) * 4 * m_ViewWidth + w * 4;
			m_DataTemp[IndexTemp + 0] = b;
			m_DataTemp[IndexTemp + 1] = g;
			m_DataTemp[IndexTemp + 2] = r;
			m_DataTemp[IndexTemp + 3] = 0;
		}
	}

	HDC hDC = GetDC(m_hWnd);
	DrawDibDraw(m_hDD, hDC, 0, 0, m_ViewWidth, m_ViewHeight, &m_Bih, m_DataTemp, 0, 0, m_ViewWidth, m_ViewHeight, 0);
	ReleaseDC(m_hWnd, hDC);

	free(m_DataTemp);
}
	
void CMeshManager::Delete_BackBuffer()
{
	DrawDibClose(m_hDD);

	free(m_Data);
	m_Data = NULL;
}

void CMeshManager::Read_BMP_File(char *szTexFileName)
{
	FILE *fp;

	fopen_s(&fp, szTexFileName,"rb");
	if(fp==NULL) printf("Error Open File");

	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(bfh), 1, fp);

	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(bih), 1, fp);
	
	fseek(fp, bfh.bfOffBits, SEEK_SET);

	m_Res = new unsigned char [bih.biWidth*bih.biHeight*3];
	fread(m_Res,bih.biWidth*bih.biHeight*3,1,fp);
	
	fclose(fp);

	m_TextureWidth = bih.biWidth;
	m_TextureHeight = bih.biHeight;
}

void CMeshManager::Draw_Textured_Triangle(vector3 VecIn1, tex_coord2 TexIn1, color_rgb Color1,
						  vector3 VecIn2, tex_coord2 TexIn2, color_rgb Color2,
						  vector3 VecIn3, tex_coord2 TexIn3, color_rgb Color3)
{
	int Side;
	float x1, x2, x3;
	float y1, y2, y3;
	float iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;
	float FloatTemp;
	int IntTemp;

	x1 = VecIn1.xs;
	y1 = VecIn1.ys;
	x2 = VecIn2.xs;
	y2 = VecIn2.ys;
	x3 = VecIn3.xs;
	y3 = VecIn3.ys;
	
	iz1 = 1.0f / VecIn1.z;
	uiz1 = TexIn1.tu / VecIn1.z;
	viz1 = TexIn1.tv / VecIn1.z;
	
	iz2 = 1.0f / VecIn2.z;
	uiz2 = TexIn2.tu / VecIn2.z;
	viz2 = TexIn2.tv / VecIn2.z;
	
	iz3 = 1.0f / VecIn3.z;
	uiz3 = TexIn3.tu / VecIn3.z;
	viz3 = TexIn3.tv / VecIn3.z;

	//координаты на экране у нас целые числа X и Y
	//поэтому некоторые операции необходимо
	//делать целочисленные (float труднее для
	//сравнения, например)

	int y1i = (int) floor (y1);
	int y2i = (int) floor (y2);
	int y3i = (int) floor (y3);

	if (y1i > y2i)
	{
		SWAP(x1, x2, FloatTemp);
		SWAP(y1, y2, FloatTemp);
		SWAP(iz1, iz2, FloatTemp);
		SWAP(uiz1, uiz2, FloatTemp);
		SWAP(viz1, viz2, FloatTemp);
		SWAP(y1i, y2i, IntTemp);

		SWAP(Color1.r, Color2.r, IntTemp);
		SWAP(Color1.g, Color2.g, IntTemp);
		SWAP(Color1.b, Color2.b, IntTemp);
	}
	if (y1i > y3i)
	{
		SWAP(x1, x3, FloatTemp);
		SWAP(y1, y3, FloatTemp);
		SWAP(iz1, iz3, FloatTemp);
		SWAP(uiz1, uiz3, FloatTemp);
		SWAP(viz1, viz3, FloatTemp);
		SWAP(y1i, y3i, IntTemp);

		SWAP(Color1.r, Color3.r, IntTemp);
		SWAP(Color1.g, Color3.g, IntTemp);
		SWAP(Color1.b, Color3.b, IntTemp);
	}
	if (y2i > y3i)
	{
		SWAP(x2, x3, FloatTemp);
		SWAP(y2, y3, FloatTemp);
		SWAP(iz2, iz3, FloatTemp);
		SWAP(uiz2, uiz3, FloatTemp);
		SWAP(viz2, viz3, FloatTemp);
		SWAP(y2i, y3i, IntTemp);

		SWAP(Color2.r, Color3.r, IntTemp);
		SWAP(Color2.g, Color3.g, IntTemp);
		SWAP(Color2.b, Color3.b, IntTemp);
	}
	
	if(y2i > y1i && y3i > y2i)
	{
		//вычисляем наклон левой и правой стороны
		//треугольника, по знаку величины наклона
		//(+ или -) определяем какая сторона
		//(левая или правая) имеет большее значение по Y
		float dxdy1 = (x2 - x1) / (y2 - y1);
		float dxdy2 = (x3 - x1) / (y3 - y1);
		Side= dxdy2 > dxdy1;
	}

	if (y1i == y2i)
		Side = x1 > x2;
	if (y2i == y3i)
		Side = x3 > x2;

	if (!Side)
	{
		m_xl = x1;
		m_ul = uiz1;
		m_vl = viz1;
		m_zl = iz1;

		m_dxdyl = (x3 - x1) / (y3 - y1);
		m_dudyl = (uiz3 - uiz1) / (y3 - y1);
		m_dvdyl = (viz3 - viz1) / (y3 - y1);
		m_dzdyl = (iz3 - iz1) / (y3 - y1);

		m_redl = (float) Color1.r;
		m_greenl = (float) Color1.g;
		m_bluel = (float) Color1.b;

		m_dredl = (Color3.r - Color1.r) / (y3 - y1);
		m_dgreenl = (Color3.g - Color1.g) / (y3 - y1);
		m_dbluel = (Color3.b - Color1.b) / (y3 - y1);
	
		if (y1i < y2i)
		{
			m_xr = x1;
			m_ur = uiz1;
			m_vr = viz1;
			m_zr = iz1;

			m_dxdyr = (x2 - x1) / (y2 - y1);
			m_dudyr = (uiz2 - uiz1) / (y2 - y1);
			m_dvdyr = (viz2 - viz1) / (y2 - y1);
			m_dzdyr = (iz2 - iz1) / (y2 - y1);

			m_redr = (float) Color1.r;
			m_greenr = (float) Color1.g;
			m_bluer = (float) Color1.b;

			m_dredr = (Color2.r - Color1.r) / (y2 - y1);
			m_dgreenr = (Color2.g - Color1.g) / (y2 - y1);
			m_dbluer = (Color2.b - Color1.b) / (y2 - y1);
			
			Draw_Textured_Poly(y1i, y2i);
		}
		if (y2i < y3i)
		{
			m_xr = x2;
			m_ur = uiz2;
			m_vr = viz2;
			m_zr = iz2;

			m_dxdyr = (x3 - x2) / (y3 - y2);
			m_dudyr = (uiz3 - uiz2) / (y3 - y2);
			m_dvdyr = (viz3 - viz2) / (y3 - y2);
			m_dzdyr = (iz3 - iz2) / (y3 - y2);

			m_redr = (float) Color2.r;
			m_greenr = (float) Color2.g;
			m_bluer = (float) Color2.b;

			m_dredr = (Color3.r - Color2.r) / (y3 - y2);
			m_dgreenr = (Color3.g - Color2.g) / (y3 - y2);
			m_dbluer = (Color3.b - Color2.b) / (y3 - y2);
			
			Draw_Textured_Poly(y2i, y3i);
		}
	}
	else
	{
		m_xr = x1;
		m_ur = uiz1;
		m_vr = viz1;
		m_zr = iz1;
		
		m_dxdyr = (x3 - x1) / (y3 - y1);
		m_dudyr = (uiz3 - uiz1) / (y3 - y1);
		m_dvdyr = (viz3 - viz1) / (y3 - y1);
		m_dzdyr = (iz3 - iz1) / (y3 - y1);

		m_redr = (float) Color1.r;
		m_greenr = (float) Color1.g;
		m_bluer = (float) Color1.b;
		
		m_dredr = (Color3.r - Color1.r) / (y3 - y1);
		m_dgreenr = (Color3.g - Color1.g) / (y3 - y1);
		m_dbluer = (Color3.b - Color1.b) / (y3 - y1);
	
		if (y1i < y2i)
		{
			m_xl = x1;
			m_ul = uiz1;
			m_vl = viz1;
			m_zl = iz1;

			m_dxdyl = (x2 - x1) / (y2 - y1);
			m_dudyl = (uiz2 - uiz1) / (y2 - y1);
			m_dvdyl = (viz2 - viz1) / (y2 - y1);
			m_dzdyl = (iz2 - iz1) / (y2 - y1);

			m_redl = (float) Color1.r;
			m_greenl = (float) Color1.g;
			m_bluel = (float) Color1.b;

			m_dredl = (Color2.r - Color1.r) / (y2 - y1);
			m_dgreenl = (Color2.g - Color1.g) / (y2 - y1);
			m_dbluel = (Color2.b - Color1.b) / (y2 - y1);
		
			Draw_Textured_Poly(y1i, y2i);
		}
		if (y2i < y3i)
		{
			m_xl = x2;
			m_ul = uiz2;
			m_vl = viz2;
			m_zl = iz2;

			m_dxdyl = (x3 - x2) / (y3 - y2);
			m_dudyl = (uiz3 - uiz2) / (y3 - y2);
			m_dvdyl = (viz3 - viz2) / (y3 - y2);
			m_dzdyl = (iz3 - iz2) / (y3 - y2);

			m_redl = (float) Color2.r;
			m_greenl = (float) Color2.g;
			m_bluel = (float) Color2.b;

			m_dredl = (Color3.r - Color2.r) / (y3 - y2);
			m_dgreenl = (Color3.g - Color2.g) / (y3 - y2);
			m_dbluel = (Color3.b - Color2.b) / (y3 - y2);
						
			Draw_Textured_Poly(y2i, y3i);
		}
	}
}

void CMeshManager::Draw_Textured_Poly(int y1, int y2)
{
	float ui, vi, zi;
	float du, dv, dz;
	float ri, gi, bi;
	float dr, dg, db;
	
	for (int y = y1; y<y2; y++)
	{
		ri = m_redl;
		gi = m_greenl;
		bi = m_bluel;

		if ((m_xr - m_xl)>0)
		{
			du = (m_ur - m_ul)/(m_xr - m_xl);
			dv = (m_vr - m_vl)/(m_xr - m_xl);
			dz = (m_zr - m_zl)/(m_xr - m_xl);

			dr = (m_redr - m_redl)/(m_xr - m_xl);
			dg = (m_greenr - m_greenl)/(m_xr - m_xl);
			db = (m_bluer - m_bluel)/(m_xr - m_xl);

			int a = 10;

		}
		else
		{
			du = 0;
			dv = 0;
			dz = 0;

			dr = 0;
			dg = 0;
			db = 0;
		}

		int xln = (int)m_xl;

		float dxt = 1 - (m_xl - xln);

		zi = m_zl + dxt * dz;
		ui = m_ul + dxt * du;
		vi = m_vl + dxt * dv;

		for (int x=(int)m_xl; x<(int)m_xr; x++)
		{
			float z = 1.0f/zi;
			float u = ui * z;
			float v = vi * z;

			UINT t = (int)u  + (((int)v) * m_TextureWidth);

			if( t < 0 || t > (m_TextureWidth * m_TextureHeight - 1) )
				continue;

			t= t*3;


			int Index =  y * 4 * m_ViewWidth + x * 4;
			
#if(0)
			//вариант без RGB_Table
			m_Data[Index + 0] = (BYTE) m_Res[t + 0]; // blue
			m_Data[Index + 1] = (BYTE) m_Res[t + 1]; // green
			m_Data[Index + 2] = (BYTE) m_Res[t + 2]; // red
			m_Data[Index + 3] = 0; 
			
			
			m_Data[Index + 0] = (BYTE) bi; // blue
			m_Data[Index + 1] = (BYTE) gi; // green
			m_Data[Index + 2] = (BYTE) ri; // red
			m_Data[Index + 3] = 0; 
#else			
			
			//вариант с использованием RGB_Table
			BYTE tb = m_Res[t + 0];
			BYTE tg = m_Res[t + 1];
			BYTE tr = m_Res[t + 2];

			int r,g,b,color;

			color = (tr << 8 ) + (int)ri;

			r = RGB_Table[color];

			color = (tg << 8 ) + (int)gi;

			g = RGB_Table[color];

			color = (tb << 8 ) + (int)bi;

			b = RGB_Table[color];

			m_Data[Index + 0] = (BYTE) b; // blue
			m_Data[Index + 1] = (BYTE) g; // green
			m_Data[Index + 2] = (BYTE) r; // red
			m_Data[Index + 3] = 0; 
#endif				
			ui+=du;
			vi+=dv;
			zi+=dz;

			ri+=dr;
			gi+=dg;
			bi+=db;
		}

		m_xl += m_dxdyl;
		m_ul += m_dudyl;
		m_vl += m_dvdyl;
		m_zl += m_dzdyl;

		m_xr += m_dxdyr;
		m_ur += m_dudyr;
		m_vr += m_dvdyr;
		m_zr += m_dzdyr;

		m_redl+= m_dredl;
		m_greenl+=m_dgreenl;
		m_bluel+=m_dbluel;

		m_redr+=m_dredr;
		m_greenr+=m_dgreenr;
		m_bluer+=m_dbluer;

	}
}

