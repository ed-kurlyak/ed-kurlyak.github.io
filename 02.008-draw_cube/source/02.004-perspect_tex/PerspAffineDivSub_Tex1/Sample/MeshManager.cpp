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

	delete [] m_Res;
	m_Res = NULL;
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

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_Res = NULL;

	m_hWnd = hWnd;

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

		
	Angle += PI/75.0f;
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

	float Fov = PI/2.0f; // FOV 90 degree
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
        0, 0, -Q*fZNear, 0 };
	*/
	
    matrix4x4 MatProj={
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };

	float Alpha = 0.5f * m_ViewWidth;
	float Beta  = 0.5f * m_ViewHeight;
	
	matrix4x4 MatScreen = {
		Alpha,  0,	    0,    0, 
		0,      -Beta,  0,    0, 
		0,		0,		1,    0,
		Alpha,  Beta,	0,    1 };

	for (UINT i = 0; i < m_VertCount; i++)
	{
		vector3 VecTemp;

		VecTemp = Vec3_Mat4x4_Mul(m_VertBuff[i], MatRotateX);
		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatRotateY);
		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatRotateZ);
		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatWorld);
		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatView);
		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatProj);

		VecTemp.x = VecTemp.x / VecTemp.z;
		VecTemp.y = VecTemp.y / VecTemp.z;

		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatScreen);

		m_VertBuffTransformed[i] = VecTemp;
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

		tex_coord2 Tex1 = m_TexCoordBuff[m_IndexBuff[i * 3 + 0]];
		tex_coord2 Tex2 = m_TexCoordBuff[m_IndexBuff[i * 3 + 1]];
		tex_coord2 Tex3 = m_TexCoordBuff[m_IndexBuff[i * 3 + 2]];

		float s = (Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);

		if(s <= 0)
			continue;
		
		if(GetKeyState('Q') & 0xFF00)
		{
			//афинное отображение текстуры
			Draw_Textured_Triangle_Affine(Vec1, Tex1, Vec2, Tex2, Vec3, Tex3 );
		}
		else if(GetKeyState('W') & 0xFF00)
		{
			//перспективное отображение текстуры
			Draw_Textured_Triangle_Perspective(Vec1, Tex1, Vec2, Tex2, Vec3, Tex3 );			
		}
		else if(GetKeyState('E') & 0xFF00)
		{
			//scanline subdivision отображение текстуры
			Draw_Textured_Triangle_Perspective_DivSub(Vec1, Tex1, Vec2, Tex2, Vec3, Tex3 );
		}
     }  


	Present_BackBuffer();
	
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

void CMeshManager::Draw_Textured_Triangle_Perspective(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3)
{
	int Side;
	float x1, x2, x3;
	float y1, y2, y3;
	float iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;
	float FloatTemp;
	int IntTemp;

	x1 = VecIn1.x;
	y1 = VecIn1.y;
	x2 = VecIn2.x;
	y2 = VecIn2.y;
	x3 = VecIn3.x;
	y3 = VecIn3.y;
	
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
	}
	if (y1i > y3i)
	{
		SWAP(x1, x3, FloatTemp);
		SWAP(y1, y3, FloatTemp);
		SWAP(iz1, iz3, FloatTemp);
		SWAP(uiz1, uiz3, FloatTemp);
		SWAP(viz1, viz3, FloatTemp);
		SWAP(y1i, y3i, IntTemp);
	}
	if (y2i > y3i)
	{
		SWAP(x2, x3, FloatTemp);
		SWAP(y2, y3, FloatTemp);
		SWAP(iz2, iz3, FloatTemp);
		SWAP(uiz2, uiz3, FloatTemp);
		SWAP(viz2, viz3, FloatTemp);
		SWAP(y2i, y3i, IntTemp);
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
			
			Draw_Textured_Poly_Perspective(y1i, y2i);
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
			
			Draw_Textured_Poly_Perspective(y2i, y3i);
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
			
			Draw_Textured_Poly_Perspective(y1i, y2i);
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
						
			Draw_Textured_Poly_Perspective(y2i, y3i);
		}
	}
}

void CMeshManager::Draw_Textured_Poly_Perspective(int y1, int y2)
{
	float ui, vi, zi;
	float du, dv, dz;
	
	for (int y = y1; y<y2; y++)
	{
		if ((m_xr - m_xl)>0)
		{
			du = (m_ur - m_ul)/(m_xr - m_xl);
			dv = (m_vr - m_vl)/(m_xr - m_xl);
			dz = (m_zr - m_zl)/(m_xr - m_xl);
		}
		else
		{
			du = 0;
			dv = 0;
			dz = 0;
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

			SetPixel(m_hBackBuffer, x, y, RGB(m_Res[t + 2],m_Res[t + 1],m_Res[t]));
				
			ui+=du;
			vi+=dv;
			zi+=dz;
		}

		m_xl += m_dxdyl;
		m_ul += m_dudyl;
		m_vl += m_dvdyl;
		m_zl += m_dzdyl;

		m_xr += m_dxdyr;
		m_ur += m_dudyr;
		m_vr += m_dvdyr;
		m_zr += m_dzdyr;
	}
}

void CMeshManager::Draw_Textured_Triangle_Affine(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3)
{
	int Side = 0;
	float TempFloat;
	int TempInt;

	float x1 = VecIn1.x;
	float y1 = VecIn1.y;
	float tu1 = (float)TexIn1.tu;
	float tv1 = (float)TexIn1.tv;

	float x2 = VecIn2.x;
	float y2 = VecIn2.y;
	float tu2 = (float)TexIn2.tu;
	float tv2 = (float)TexIn2.tv;

	float x3 = VecIn3.x;
	float y3 = VecIn3.y;
	float tu3 = (float)TexIn3.tu;
	float tv3 = (float)TexIn3.tv;

	int y1i = (int) y1;
	int y2i = (int) y2;
	int y3i = (int) y3;

	if(y2i < y1i)
	{
		SWAP(x2, x1, TempFloat);
		SWAP(y2, y1, TempFloat);

		SWAP(tu2, tu1, TempFloat);
		SWAP(tv2, tv1, TempFloat);

		SWAP(y2i, y1i, TempInt);
	}

	if(y3i < y1i)
	{
		SWAP(x3, x1, TempFloat);
		SWAP(y3, y1, TempFloat);

		SWAP(tu3, tu1, TempFloat);
		SWAP(tv3, tv1, TempFloat);

		SWAP(y3i, y1i, TempInt);

	}

	if(y3i < y2i)
	{
		SWAP(x3, x2, TempFloat);
		SWAP(y3, y2, TempFloat);

		SWAP(tu3, tu2, TempFloat);
		SWAP(tv3, tv2, TempFloat);

		SWAP(y3i, y2i, TempInt);

	}

	if( y1i < y2i && y2i < y3i )
	{
		float dxdy1 = (x2 - x1) / (y2 - y1);
		float dxdy2 = (x3 - x1) / (y3 - y1);
		Side = dxdy2 > dxdy1;
	}

	if(y1i == y2i)
		Side = x1 > x2;

	if(y2i == y3i)
		Side = x3 > x2;

	if(!Side)
	{

		m_dxdyl = (x3 - x1) / (y3 - y1);
		m_dudyl = (tu3 - tu1) / (y3 - y1);
		m_dvdyl = (tv3 - tv1) / (y3 - y1);

		m_xl = x1;
		m_ul = tu1;
		m_vl = tv1;
		
		if(y1i < y2i)
		{
			m_dxdyr = (x2 - x1) / (y2 - y1);
			m_dudyr = (tu2 - tu1) / (y2 - y1);
			m_dvdyr = (tv2 - tv1) / (y2 - y1);

			m_xr = x1;
			m_ur = tu1;
			m_vr = tv1;

			Draw_Textured_Poly_Affine(y1i, y2i);
		}
		
		if(y2i < y3i)
		{
			m_dxdyr = (x3 - x2) / (y3 - y2);
			m_dudyr = (tu3 - tu2) / (y3 - y2);
			m_dvdyr = (tv3 - tv2) / (y3 - y2);

			m_xr = x2;
			m_ur = tu2;
			m_vr = tv2;

			Draw_Textured_Poly_Affine(y2i, y3i);
		}
	}
	else
	{
		m_dxdyr = (x3 - x1) / (y3 - y1);
		m_dudyr = (tu3 - tu1) / (y3 - y1);
		m_dvdyr = (tv3 - tv1) / (y3 - y1);

		m_xr = x1;
		m_ur = tu1;
		m_vr = tv1;

		if(y1i < y2i)
		{
			m_dxdyl = (x2 - x1) / (y2 - y1);
			m_dudyl = (tu2 - tu1) / (y2 - y1);
			m_dvdyl = (tv2 - tv1) / (y2 - y1);

			m_xl = x1;
			m_ul = tu1;
			m_vl = tv1;

			Draw_Textured_Poly_Affine(y1i, y2i);
		}
		
		if(y2i < y3i)
		{
			m_dxdyl = (x3 - x2) / (y3 - y2);
			m_dudyl = (tu3 - tu2) / (y3 - y2);
			m_dvdyl = (tv3 - tv2) / (y3 - y2);
			
			m_xl = x2;
			m_ul = tu2;
			m_vl = tv2;

			Draw_Textured_Poly_Affine(y2i, y3i);
		}
		
	}
}

void CMeshManager::Draw_Textured_Poly_Affine(int y1, int y2)
{
	for ( int y = y1; y < y2; y++ )
	{
		float itu, itv;
		float dtu, dtv;

		//смотри ВАРИАНТ 2
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//ВАРИАНТ 1
		itu = m_ul;
		itv = m_vl;
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!

		int dx = int(m_xr - m_xl);

		if(dx > 0)
		{
			dtu = (m_ur - m_ul) / (m_xr - m_xl);
			dtv = (m_vr - m_vl) / (m_xr - m_xl);
		}
		else
		{
			dtu = 0.0f;
			dtv = 0.0f;
		}

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//ВАРИАНТ 2
		/*
		int xln = (int)m_xl;

		float dxt = 1 - (m_xl - xln);

		//iz = zl + dxt * dz;
		itu = m_ul + dxt * dtu;
		itv = m_vl + dxt * dtv;
		*/
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!

		
		for ( int x = (int)m_xl; x < (int)m_xr; x++ )
		{
			float tu = itu;
			float tv = itv;

			int t = (int)tu + (int)tv * m_TextureWidth;

			t = t * 3;

			SetPixel(m_hBackBuffer, x, y, RGB(m_Res[t + 2],m_Res[t + 1],m_Res[t]));
			

			itu += dtu;
			itv += dtv;
		}

		m_xl += m_dxdyl;
		m_ul += m_dudyl;
		m_vl += m_dvdyl;

		m_xr += m_dxdyr;
		m_ur += m_dudyr;
		m_vr += m_dvdyr;
	}
}

void CMeshManager::Draw_Textured_Triangle_Perspective_DivSub(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3)
{
	int Side;
	float x1, x2, x3;
	float y1, y2, y3;
	float iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;
	float FloatTemp;
	int IntTemp;

	x1 = VecIn1.x;
	y1 = VecIn1.y;
	x2 = VecIn2.x;
	y2 = VecIn2.y;
	x3 = VecIn3.x;
	y3 = VecIn3.y;
	
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
	}
	if (y1i > y3i)
	{
		SWAP(x1, x3, FloatTemp);
		SWAP(y1, y3, FloatTemp);
		SWAP(iz1, iz3, FloatTemp);
		SWAP(uiz1, uiz3, FloatTemp);
		SWAP(viz1, viz3, FloatTemp);
		SWAP(y1i, y3i, IntTemp);
	}
	if (y2i > y3i)
	{
		SWAP(x2, x3, FloatTemp);
		SWAP(y2, y3, FloatTemp);
		SWAP(iz2, iz3, FloatTemp);
		SWAP(uiz2, uiz3, FloatTemp);
		SWAP(viz2, viz3, FloatTemp);
		SWAP(y2i, y3i, IntTemp);
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

			Draw_Textured_Poly_Perspective_DivSub(y1i, y2i);
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

			Draw_Textured_Poly_Perspective_DivSub(y2i, y3i);
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

			Draw_Textured_Poly_Perspective_DivSub(y1i, y2i);
			
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

			Draw_Textured_Poly_Perspective_DivSub(y2i, y3i);
		}
	}

}

void CMeshManager::Draw_Textured_Poly_Perspective_DivSub(int y1, int y2)
{

		//char *scr;
		int x1, x2;
		int x, xcount;
		float z, dx;
		float iz, uiz, viz;
		int u1, v1, u2, v2, u, v, du, dv;
		float dizdx, duizdx, dvizdx;
		float dizdxn, duizdxn, dvizdxn;

		while (y1 < y2)
		{

			if ((m_xr - m_xl)>0)
			{
				dizdx = ((m_zr - m_zl)/(m_xr - m_xl));
				duizdx = ((m_ur - m_ul)/(m_xr - m_xl));
				dvizdx = ((m_vr - m_vl)/(m_xr - m_xl));

				dizdxn = dizdx * SUBDIVSIZE;
				duizdxn = duizdx * SUBDIVSIZE;
				dvizdxn = dvizdx * SUBDIVSIZE;
			}
			else
			{	
				dizdx = 0;
				duizdx = 0;
				dvizdx = 0;

				dizdxn = 0;
				duizdxn = 0;
				dvizdxn = 0;
			}

			x1 = (int)m_xl;
			x2 = (int)m_xr;

			dx = 1 - (m_xl - x1);
			iz = m_zl + dx * dizdx;
			uiz = m_ul + dx * duizdx;
			viz = m_vl + dx * dvizdx;

			/*
			iz = m_zl;
			uiz = m_ul;
			viz = m_vl;
			*/

			//scr = &screen[y1 * 320 + x1];

			z = 65536 / iz;
			u2 = (int)(uiz * z);
			v2 = (int)(viz * z);

			xcount = x2 - x1;

			while (xcount >= SUBDIVSIZE)
			{
				iz += dizdxn;
				uiz += duizdxn;
				viz += dvizdxn;

				u1 = u2;
				v1 = v2;

				z = 65536 / iz;
				u2 = (int)(uiz * z);
				v2 = (int)(viz * z);

				u = u1;
				v = v1;

				du = (u2 - u1) >> SUBDIVSHIFT;
				dv = (v2 - v1) >> SUBDIVSHIFT;

				x = SUBDIVSIZE;
				while (x--)
				{
					//*(scr++) = texture[((((int) v) & 0xff0000) >> 8) + ((((int) u) & 0xff0000) >> 16)];

					int t = ((((int) v) & 0xff0000) >> 8) + ((((int) u) & 0xff0000) >> 16);

					t= t*3;

					SetPixel(m_hBackBuffer, x1++, y1, RGB(m_Res[t + 2],m_Res[t + 1],m_Res[t]));

					u += du;
					v += dv;
				}

				xcount -= SUBDIVSIZE;
			}
			

			if (xcount)
			{
				iz += dizdx * xcount;
				uiz += duizdx * xcount;
				viz += dvizdx * xcount;

				u1 = u2;
				v1 = v2;

				z = 65536 / iz;
				u2 = (int)(uiz * z);
				v2 = (int)(viz * z);

				u = u1;
				v = v1;

				du = (u2 - u1) / xcount;
				dv = (v2 - v1) / xcount;

				while (xcount>0)
				{
					//*(scr++) = texture[((((int) v) & 0xff0000) >> 8) + ((((int) u) & 0xff0000) >> 16)];
					
					int t = ((((int) v) & 0xff0000) >> 8) + ((((int) u) & 0xff0000) >> 16);
	
					t= t*3;

					SetPixel(m_hBackBuffer, x1++, y1, RGB(m_Res[t + 2],m_Res[t + 1],m_Res[t]));

					u += du;
					v += dv;

					xcount--;
				}
			}

			m_xl += m_dxdyl;
			m_ul += m_dudyl;
			m_vl += m_dvdyl;
			m_zl += m_dzdyl;

			m_xr += m_dxdyr;
			m_ur += m_dudyr;
			m_vr += m_dvdyr;
			m_zr += m_dzdyr;

			y1++;
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
