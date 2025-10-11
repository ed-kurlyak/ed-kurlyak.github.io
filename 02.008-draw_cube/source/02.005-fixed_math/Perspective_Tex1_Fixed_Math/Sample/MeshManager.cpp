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

	for (int j = 0; j < 3; j++)
	{
		float Sum = 0.0f;
		int i;

		for (i = 0; i < 3; i++)
		{
			Sum += VecIn.Vec[i] * MatIn.Mat[i][j];
		}

		Sum += MatIn.Mat[i][j];
		VecOut.Vec[j] = Sum;
	}

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

	float Fov = PI/2.0f; // FOV 90 degree
    float Aspect = (float)m_ViewWidth / m_ViewHeight;
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

	float Alpha = 0.5f * m_ViewWidth;
	float Beta  = 0.5f * m_ViewHeight;
	
	matrix4x4 MatScreen = {
		Alpha,  0,	    0,    0, 
		0,      -Beta,  0,    0, 
		0,		0,		1,    0,
		Alpha,  Beta,	0,    1};

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

		Draw_Textured_Triangle(Vec1, Tex1, Vec2, Tex2, Vec3, Tex3 );
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

void CMeshManager::Draw_Textured_Triangle(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3)
{
	int IntTemp;
	int Side;
	int x1, x2, x3;
	int y1, y2, y3;
	int z1, z2, z3;
	int tu1, tu2, tu3;
	int tv1, tv2, tv3;
	int iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;

	x1 = (int)VecIn1.x;
	y1 = (int)VecIn1.y;
	z1 = (int)VecIn1.z;
	tu1 = TexIn1.tu;
	tv1 = TexIn1.tv;

	x2 = (int)VecIn2.x;
	y2 = (int)VecIn2.y;
	z2 = (int)VecIn2.z;
	tu2 = TexIn2.tu;
	tv2 = TexIn2.tv;
	
	x3 = (int)VecIn3.x;
	y3 = (int)VecIn3.y;
	z3 = (int)VecIn3.z;
	tu3 = TexIn3.tu;
	tv3 = TexIn3.tv;

	if (y1 > y2)
	{
		SWAP(x1, x2, IntTemp);
		SWAP(y1, y2, IntTemp);
		SWAP(z1, z2, IntTemp);
		SWAP(tu1, tu2, IntTemp);
		SWAP(tv1, tv2, IntTemp);
	}
	if (y1 > y3)
	{
		SWAP(x1, x3, IntTemp);
		SWAP(y1, y3, IntTemp);
		SWAP(z1, z3, IntTemp);
		SWAP(tu1, tu3, IntTemp);
		SWAP(tv1, tv3, IntTemp);
	}
	if (y2 > y3)
	{
		SWAP(x2, x3, IntTemp);
		SWAP(y2, y3, IntTemp);
		SWAP(z2, z3, IntTemp);
		SWAP(tu2, tu3, IntTemp);
		SWAP(tv2, tv3, IntTemp);
	}
	
	iz1 = (1 << FIXP26_SHIFT) / z1;
	iz2 = (1 << FIXP26_SHIFT) / z2;
	iz3 = (1 << FIXP26_SHIFT) / z3;
	
	uiz1 = (tu1 << FIXP22_SHIFT) / z1;
	viz1 = (tv1 << FIXP22_SHIFT) / z1;
	uiz2 = (tu2 << FIXP22_SHIFT) / z2;
	viz2 = (tv2 << FIXP22_SHIFT) / z2;
	uiz3 = (tu3 << FIXP22_SHIFT) / z3;
	viz3 = (tv3 << FIXP22_SHIFT) / z3;

	if(y2 > y1 && y3 > y2)
	{
		float dxdy1 = float(x2 - x1) / float(y2 - y1);
		float dxdy2 = float(x3 - x1) / float(y3 - y1);
		Side= dxdy2 > dxdy1;
	}

	if (y1 == y2)
		Side = x1 > x2;
	if (y2 == y3)
		Side = x3 > x2;

	if (!Side)
	{
		m_xl = x1 << FIXP16_SHIFT;
		m_ul = uiz1;
		m_vl = viz1;
		m_zl = iz1;

		m_dxdyl = ((x3 - x1) << FIXP16_SHIFT) / (y3 - y1);
		m_dudyl = (uiz3 - uiz1) / (y3 - y1);
		m_dvdyl = (viz3 - viz1) / (y3 - y1);
		m_dzdyl = (iz3 - iz1) / (y3 - y1);
	
		if (y1 < y2)
		{
			m_xr = x1 << FIXP16_SHIFT;
			m_ur = uiz1;
			m_vr = viz1;
			m_zr = iz1;

			m_dxdyr = ((x2 - x1) << FIXP16_SHIFT) / (y2 - y1);
			m_dudyr = (uiz2 - uiz1) / (y2 - y1);
			m_dvdyr = (viz2 - viz1) / (y2 - y1);
			m_dzdyr = (iz2 - iz1) / (y2 - y1);
			
			Draw_Textured_Poly((int)y1, (int)y2);
		}
		if (y2 < y3)
		{
			m_xr = x2 << FIXP16_SHIFT;
			m_ur = uiz2;
			m_vr = viz2;
			m_zr = iz2;

			m_dxdyr = ((x3 - x2) << FIXP16_SHIFT) / (y3 - y2);
			m_dudyr = (uiz3 - uiz2) / (y3 - y2);
			m_dvdyr = (viz3 - viz2) / (y3 - y2);
			m_dzdyr = (iz3 - iz2) / (y3 - y2);
			
			Draw_Textured_Poly((int)y2, (int)y3);
		}
		
	}
	else
	{
		m_xr = x1 << FIXP16_SHIFT;
		m_ur = uiz1;
		m_vr = viz1;
		m_zr = iz1;
		
		m_dxdyr = ((x3 - x1) << FIXP16_SHIFT) / (y3 - y1);
		m_dudyr = (uiz3 - uiz1) / (y3 - y1);
		m_dvdyr = (viz3 - viz1) / (y3 - y1);
		m_dzdyr = (iz3 - iz1) / (y3 - y1);
		
		if (y1 < y2)
		{
			m_xl = x1 << FIXP16_SHIFT;
			m_ul = uiz1;
			m_vl = viz1;
			m_zl = iz1;

			m_dxdyl = ((x2 - x1) << FIXP16_SHIFT) / (y2 - y1);
			m_dudyl = (uiz2 - uiz1) / (y2 - y1);
			m_dvdyl = (viz2 - viz1) / (y2 - y1);
			m_dzdyl = (iz2 - iz1) / (y2 - y1);
			
			Draw_Textured_Poly((int)y1, (int)y2);
		}
		if (y2 < y3)
		{
			m_xl = x2 << FIXP16_SHIFT;
			m_ul = uiz2;
			m_vl = viz2;
			m_zl = iz2;

			m_dxdyl = ((x3 - x2) << FIXP16_SHIFT) / (y3 - y2);
			m_dudyl = (uiz3 - uiz2) / (y3 - y2);
			m_dvdyl = (viz3 - viz2) / (y3 - y2);
			m_dzdyl = (iz3 - iz2) / (y3 - y2);
						
			Draw_Textured_Poly((int)y2, (int)y3);
		}
	}
}

void CMeshManager::Draw_Textured_Poly(int y1, int y2)
{
	int ui, vi, zi;
	int du, dv, dz;
	
	for (int y = y1; y<y2; y++)
	{
		int x1 = m_xl >> FIXP16_SHIFT;
		int x2 = m_xr >> FIXP16_SHIFT;

		ui = m_ul;
		vi = m_vl;
		zi = m_zl;

		if ((x2 - x1)>0)
		{
			du = (m_ur - m_ul)/(x2 - x1);
			dv = (m_vr - m_vl)/(x2 - x1);
			dz = (m_zr - m_zl)/(x2 - x1);
		}
		else
		{
			du = 0;
			dv = 0;
			dz = 0;
		}

		for (int x=x1; x<x2; x++)
		{
			int u = (ui << (FIXP26_SHIFT - FIXP22_SHIFT)) / zi;
			int v = (vi << (FIXP26_SHIFT - FIXP22_SHIFT)) / zi;

			UINT t = u  + (v * m_TextureWidth);

			if( t < 0 || t > (m_TextureWidth * m_TextureHeight - 1) )
				continue;

			t= t*3;

			SetPixel(m_hBackBuffer, x, y, RGB(m_Res[t + 2],m_Res[t + 1],m_Res[t + 0]));
				
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
