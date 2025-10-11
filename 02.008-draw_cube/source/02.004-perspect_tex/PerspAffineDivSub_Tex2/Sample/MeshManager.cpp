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
			//перспективное отображение текстуры
			Draw_Textured_Triangle_Perspective(Vec1, Tex1, Vec2, Tex2, Vec3, Tex3 );
		}
		else if(GetKeyState('W') & 0xFF00)
		{
			//афинное отображение текстуры
			Draw_Textured_Triangle_Affine(Vec1, Tex1, Vec2, Tex2, Vec3, Tex3 );
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

	float x1, y1, x2, y2, x3, y3;
	float iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;
	float dxdy1 = 0.0f, dxdy2 = 0.0f, dxdy3 = 0.0f;
	float tempf;
	float denom;
	float dy;
	int y1i, y2i, y3i;
	int side;

	x1 = VecIn1.x;
	y1 = VecIn1.y;
	x2 = VecIn2.x;
	y2 = VecIn2.y;
	x3 = VecIn3.x;
	y3 = VecIn3.y;

	iz1 = 1.0f / VecIn1.z;
	iz2 = 1.0f / VecIn2.z;
	iz3 = 1.0f / VecIn3.z;

	uiz1 = TexIn1.tu * iz1;
	viz1 = TexIn1.tv * iz1;
	uiz2 = TexIn2.tu * iz2;
	viz2 = TexIn2.tv * iz2;
	uiz3 = TexIn3.tu * iz3;
	viz3 = TexIn3.tv * iz3;

#define swapfloat(x, y) tempf = x; x = y; y = tempf;
	if (y1 > y2)
	{
		swapfloat(x1, x2);
		swapfloat(y1, y2);
		swapfloat(iz1, iz2);
		swapfloat(uiz1, uiz2);
		swapfloat(viz1, viz2);
	}
	if (y1 > y3)
	{
		swapfloat(x1, x3);
		swapfloat(y1, y3);
		swapfloat(iz1, iz3);
		swapfloat(uiz1, uiz3);
		swapfloat(viz1, viz3);
	}
	if (y2 > y3)
	{
		swapfloat(x2, x3);
		swapfloat(y2, y3);
		swapfloat(iz2, iz3);
		swapfloat(uiz2, uiz3);
		swapfloat(viz2, viz3);
	}
#undef swapfloat

	y1i = (int)y1;
	y2i = (int)y2;
	y3i = (int)y3;

	if ((y1i == y2i && y1i == y3i)
	    || ((int) x1 == (int) x2 && (int) x1 == (int) x3))
		return;

	denom = ((x3 - x1) * (y2 - y1) - (x2 - x1) * (y3 - y1));

	if (!denom)
		return;	

	denom = 1 / denom;
	m_dizdx = ((iz3 - iz1) * (y2 - y1) - (iz2 - iz1) * (y3 - y1)) * denom;
	m_duizdx = ((uiz3 - uiz1) * (y2 - y1) - (uiz2 - uiz1) * (y3 - y1)) * denom;
	m_dvizdx = ((viz3 - viz1) * (y2 - y1) - (viz2 - viz1) * (y3 - y1)) * denom;
	m_dizdy = ((iz2 - iz1) * (x3 - x1) - (iz3 - iz1) * (x2 - x1)) * denom;
	m_duizdy = ((uiz2 - uiz1) * (x3 - x1) - (uiz3 - uiz1) * (x2 - x1)) * denom;
	m_dvizdy = ((viz2 - viz1) * (x3 - x1) - (viz3 - viz1) * (x2 - x1)) * denom;

	if (y2 > y1)
		dxdy1 = (x2 - x1) / (y2 - y1);
	if (y3 > y1)
		dxdy2 = (x3 - x1) / (y3 - y1);
	if (y3 > y2)
		dxdy3 = (x3 - x2) / (y3 - y2);

	side = dxdy2 > dxdy1;

	if (y1 == y2)
		side = x1 > x2;
	if (y2 == y3)
		side = x3 > x2;

	if (!side)
	{
		m_dxdyl = dxdy2;
		m_dizdyl = dxdy2 * m_dizdx + m_dizdy;
		m_duizdyl = dxdy2 * m_duizdx + m_duizdy;
		m_dvizdyl = dxdy2 * m_dvizdx + m_dvizdy;

		dy = 1 - (y1 - y1i);
		m_xl = x1 + dy * m_dxdyl;
		m_izl = iz1 + dy * m_dizdyl;
		m_uizl = uiz1 + dy * m_duizdyl;
		m_vizl = viz1 + dy * m_dvizdyl;

		if (y1i < y2i)
		{
			m_xr = x1 + dy * dxdy1;
			m_dxdyr = dxdy1;

			Draw_Textured_Poly_Perspective(y1i, y2i);
		}
		if (y2i < y3i)
		{
			m_xr = x2 + (1 - (y2 - y2i)) * dxdy3;
			m_dxdyr = dxdy3;

			Draw_Textured_Poly_Perspective(y2i, y3i);
		}
	}
	else
	{
		m_dxdyr = dxdy2;
		dy = 1 - (y1 - y1i);
		m_xr = x1 + dy * m_dxdyr;

		if (y1i < y2i)
		{
			m_dxdyl = dxdy1;
			m_dizdyl = dxdy1 * m_dizdx + m_dizdy;
			m_duizdyl = dxdy1 * m_duizdx + m_duizdy;
			m_dvizdyl = dxdy1 * m_dvizdx + m_dvizdy;
			m_xl = x1 + dy * m_dxdyl;
			m_izl = iz1 + dy * m_dizdyl;
			m_uizl = uiz1 + dy * m_duizdyl;
			m_vizl = viz1 + dy * m_dvizdyl;

			Draw_Textured_Poly_Perspective(y1i, y2i);
		}
		if (y2i < y3i)
		{
			m_dxdyl = dxdy3;
			m_dizdyl = dxdy3 * m_dizdx + m_dizdy;
			m_duizdyl = dxdy3 * m_duizdx + m_duizdy;
			m_dvizdyl = dxdy3 * m_dvizdx + m_dvizdy;
			dy = 1 - (y2 - y2i);
			m_xl = x2 + dy * m_dxdyl;
			m_izl = iz2 + dy * m_dizdyl;
			m_uizl = uiz2 + dy * m_duizdyl;
			m_vizl = viz2 + dy * m_dvizdyl;

			Draw_Textured_Poly_Perspective(y2i, y3i);
		}
	}
}

void CMeshManager::Draw_Textured_Poly_Perspective(int y1, int y2)
{

	int x1, x2;
	float z, u, v, dx;
	float iz, uiz, viz;

	while (y1 < y2)
	{
		x1 = (int)m_xl;
		x2 = (int)m_xr;

		dx = 1 - (m_xl - x1);
		iz = m_izl + dx * m_dizdx;
		uiz = m_uizl + dx * m_duizdx;
		viz = m_vizl + dx * m_dvizdx;

		while (x1++ < x2)
		{
			z = 1 / iz;
			u = uiz * z;
			v = viz * z;

			UINT t = ((((int) v) & 0xff) << 8) + (((int) u) & 0xff);

			if( t < 0 || t > (m_TextureWidth * m_TextureHeight - 1) )
				continue;

			t= t*3;

			SetPixel(m_hBackBuffer, x1, y1, RGB(m_Res[t + 2],m_Res[t + 1],m_Res[t]));

			iz += m_dizdx;
			uiz += m_duizdx;
			viz += m_dvizdx;
		}

		m_xl += m_dxdyl;
		m_xr += m_dxdyr;
		m_izl += m_dizdyl;
		m_uizl += m_duizdyl;
		m_vizl += m_dvizdyl;

		y1++;
	}
}

void CMeshManager::Draw_Textured_Triangle_Affine(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3)
{

	float x1, y1, x2, y2, x3, y3;
	float u1, v1, u2, v2, u3, v3;
	float dxdy1 = 0.0f, dxdy2 = 0.0f, dxdy3 = 0.0f;
	float tempf;
	float denom;
	float dy;
	int y1i, y2i, y3i;
	int side;

	x1 = VecIn1.x;
	y1 = VecIn1.y;
	x2 = VecIn2.x;
	y2 = VecIn2.y;
	x3 = VecIn3.x;
	y3 = VecIn3.y;

	u1 = (float)TexIn1.tu;
	v1 = (float)TexIn1.tv;
	u2 = (float)TexIn2.tu;
	v2 = (float)TexIn2.tv;
	u3 = (float)TexIn3.tu;
	v3 = (float)TexIn3.tv;

#define swapfloat(x, y) tempf = x; x = y; y = tempf;
	if (y1 > y2)
	{
		swapfloat(x1, x2);
		swapfloat(y1, y2);
		swapfloat(u1, u2);
		swapfloat(v1, v2);
	}
	if (y1 > y3)
	{
		swapfloat(x1, x3);
		swapfloat(y1, y3);
		swapfloat(u1, u3);
		swapfloat(v1, v3);
	}
	if (y2 > y3)
	{
		swapfloat(x2, x3);
		swapfloat(y2, y3);
		swapfloat(u2, u3);
		swapfloat(v2, v3);
	}
#undef swapfloat

	y1i = (int)y1;
	y2i = (int)y2;
	y3i = (int)y3;

	if ((y1i == y2i && y1i == y3i)
	    || ((int) x1 == (int) x2 && (int) x1 == (int) x3))
		return;

	denom = ((x3 - x1) * (y2 - y1) - (x2 - x1) * (y3 - y1));

	if (!denom)
		return;	

	denom = 1 / denom;
	dudx = ((u3 - u1) * (y2 - y1) - (u2 - u1) * (y3 - y1)) * denom;
	dvdx = ((v3 - v1) * (y2 - y1) - (v2 - v1) * (y3 - y1)) * denom;
	dudy = ((u2 - u1) * (x3 - x1) - (u3 - u1) * (x2 - x1)) * denom;
	dvdy = ((v2 - v1) * (x3 - x1) - (v3 - v1) * (x2 - x1)) * denom;

	if (y2 > y1)
		dxdy1 = (x2 - x1) / (y2 - y1);
	if (y3 > y1)
		dxdy2 = (x3 - x1) / (y3 - y1);
	if (y3 > y2)
		dxdy3 = (x3 - x2) / (y3 - y2);

	side = dxdy2 > dxdy1;

	if (y1 == y2)
		side = x1 > x2;
	if (y2 == y3)
		side = x3 > x2;

	if (!side)
	{
		m_dxdyl = dxdy2;
		m_dudyl = m_dxdyl * dudx + dudy;
		m_dvdyl = m_dxdyl * dvdx + dvdy;

		dy = 1 - (y1 - y1i);
		m_xl = x1 + dy * m_dxdyl;
		m_ul = u1 + dy * m_dudyl;
		m_vl = v1 + dy * m_dvdyl;

		if (y1i < y2i)
		{
			m_dxdyr = dxdy1;
			m_xr = x1 + dy * m_dxdyr;

			Draw_Textured_Poly_Affine(y1i, y2i);
		}
		if (y2i < y3i)
		{
			m_dxdyr = dxdy3;
			m_xr = x2 + (1 - (y2 - y2i)) * m_dxdyr;

			Draw_Textured_Poly_Affine(y2i, y3i);
		}
	}
	else
	{
		m_dxdyr = dxdy2;
		dy = 1 - (y1 - y1i);
		m_xr = x1 + dy * m_dxdyr;

		if (y1i < y2i)
		{
			m_dxdyl = dxdy1;
			m_dudyl = m_dxdyl * dudx + dudy;
			m_dvdyl = m_dxdyl * dvdx + dvdy;
			m_xl = x1 + dy * m_dxdyl;
			m_ul = u1 + dy * m_dudyl;
			m_vl = v1 + dy * m_dvdyl;

			Draw_Textured_Poly_Affine(y1i, y2i);
		}
		if (y2i < y3i)
		{
			m_dxdyl = dxdy3;
			m_dudyl = m_dxdyl * dudx + dudy;
			m_dvdyl = m_dxdyl * dvdx + dvdy;
			dy = 1 - (y2 - y2i);
			m_xl = x2 + dy * m_dxdyl;
			m_ul = u2 + dy * m_dudyl;
			m_vl = v2 + dy * m_dvdyl;

			Draw_Textured_Poly_Affine(y2i, y3i);
		}
	}
}

void CMeshManager::Draw_Textured_Poly_Affine(int y1, int y2)
{

	int x1, x2;
	float u, v, dx;

	while (y1 < y2)
	{
		x1 = (int)m_xl;
		x2 = (int)m_xr;

		dx = 1 - (m_xl - x1);
		u = m_ul + dx * dudx;
		v = m_vl + dx * dvdx;

		while (x1++ < x2)
		{
			UINT t = ((((int) v) & 0xff) << 8) + (((int) u) & 0xff);

			if( t < 0 || t > (m_TextureWidth * m_TextureHeight - 1) )
				continue;

			t= t*3;

			SetPixel(m_hBackBuffer, x1, y1, RGB(m_Res[t + 2],m_Res[t + 1],m_Res[t]));

			u += dudx;
			v += dvdx;
		}

		m_xl += m_dxdyl;
		m_xr += m_dxdyr;
		m_ul += m_dudyl;
		m_vl += m_dvdyl;

		y1++;
	}
}

void CMeshManager::Draw_Textured_Triangle_Perspective_DivSub(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3)
{

	float x1, y1, x2, y2, x3, y3;
	float iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;
	float dxdy1 = 0.0f, dxdy2 = 0.0f, dxdy3 = 0.0f;
	float tempf;
	float denom;
	float dy;
	int y1i, y2i, y3i;
	int side;

	x1 = VecIn1.x;
	y1 = VecIn1.y;
	x2 = VecIn2.x;
	y2 = VecIn2.y;
	x3 = VecIn3.x;
	y3 = VecIn3.y;

	iz1 = 1.0f / VecIn1.z;
	iz2 = 1.0f / VecIn2.z;
	iz3 = 1.0f / VecIn3.z;

	uiz1 = TexIn1.tu * iz1;
	viz1 = TexIn1.tv * iz1;
	uiz2 = TexIn2.tu * iz2;
	viz2 = TexIn2.tv * iz2;
	uiz3 = TexIn3.tu * iz3;
	viz3 = TexIn3.tv * iz3;

#define swapfloat(x, y) tempf = x; x = y; y = tempf;
	if (y1 > y2)
	{
		swapfloat(x1, x2);
		swapfloat(y1, y2);
		swapfloat(iz1, iz2);
		swapfloat(uiz1, uiz2);
		swapfloat(viz1, viz2);
	}
	if (y1 > y3)
	{
		swapfloat(x1, x3);
		swapfloat(y1, y3);
		swapfloat(iz1, iz3);
		swapfloat(uiz1, uiz3);
		swapfloat(viz1, viz3);
	}
	if (y2 > y3)
	{
		swapfloat(x2, x3);
		swapfloat(y2, y3);
		swapfloat(iz2, iz3);
		swapfloat(uiz2, uiz3);
		swapfloat(viz2, viz3);
	}
#undef swapfloat

	y1i = (int)y1;
	y2i = (int)y2;
	y3i = (int)y3;

	if ((y1i == y2i && y1i == y3i)
	    || ((int) x1 == (int) x2 && (int) x1 == (int) x3))
		return;

	denom = ((x3 - x1) * (y2 - y1) - (x2 - x1) * (y3 - y1));

	if (!denom)
		return;	

	denom = 1 / denom;
	m_dizdx = ((iz3 - iz1) * (y2 - y1) - (iz2 - iz1) * (y3 - y1)) * denom;
	m_duizdx = ((uiz3 - uiz1) * (y2 - y1) - (uiz2 - uiz1) * (y3 - y1)) * denom;
	m_dvizdx = ((viz3 - viz1) * (y2 - y1) - (viz2 - viz1) * (y3 - y1)) * denom;
	m_dizdy = ((iz2 - iz1) * (x3 - x1) - (iz3 - iz1) * (x2 - x1)) * denom;
	m_duizdy = ((uiz2 - uiz1) * (x3 - x1) - (uiz3 - uiz1) * (x2 - x1)) * denom;
	m_dvizdy = ((viz2 - viz1) * (x3 - x1) - (viz3 - viz1) * (x2 - x1)) * denom;

	dizdxn = m_dizdx * SUBDIVSIZE;
	duizdxn = m_duizdx * SUBDIVSIZE;
	dvizdxn = m_dvizdx * SUBDIVSIZE;

	if (y2 > y1)
		dxdy1 = (x2 - x1) / (y2 - y1);
	if (y3 > y1)
		dxdy2 = (x3 - x1) / (y3 - y1);
	if (y3 > y2)
		dxdy3 = (x3 - x2) / (y3 - y2);

	side = dxdy2 > dxdy1;

	if (y1 == y2)
		side = x1 > x2;
	if (y2 == y3)
		side = x3 > x2;

	if (!side)
	{
		m_dxdyl = dxdy2;
		m_dizdyl = dxdy2 * m_dizdx + m_dizdy;
		m_duizdyl = dxdy2 * m_duizdx + m_duizdy;
		m_dvizdyl = dxdy2 * m_dvizdx + m_dvizdy;

		dy = 1 - (y1 - y1i);
		m_xl = x1 + dy * m_dxdyl;
		m_izl = iz1 + dy * m_dizdyl;
		m_uizl = uiz1 + dy * m_duizdyl;
		m_vizl = viz1 + dy * m_dvizdyl;

		if (y1i < y2i)
		{
			m_xr = x1 + dy * dxdy1;
			m_dxdyr = dxdy1;

			Draw_Textured_Poly_Perspective_DivSub(y1i, y2i);
		}
		if (y2i < y3i)
		{
			m_xr = x2 + (1 - (y2 - y2i)) * dxdy3;
			m_dxdyr = dxdy3;

			Draw_Textured_Poly_Perspective_DivSub(y2i, y3i);
		}
	}
	else
	{
		m_dxdyr = dxdy2;
		dy = 1 - (y1 - y1i);
		m_xr = x1 + dy * m_dxdyr;

		if (y1i < y2i)
		{
			m_dxdyl = dxdy1;
			m_dizdyl = dxdy1 * m_dizdx + m_dizdy;
			m_duizdyl = dxdy1 * m_duizdx + m_duizdy;
			m_dvizdyl = dxdy1 * m_dvizdx + m_dvizdy;
			m_xl = x1 + dy * m_dxdyl;
			m_izl = iz1 + dy * m_dizdyl;
			m_uizl = uiz1 + dy * m_duizdyl;
			m_vizl = viz1 + dy * m_dvizdyl;

			Draw_Textured_Poly_Perspective_DivSub(y1i, y2i);
		}
		if (y2i < y3i)
		{
			m_dxdyl = dxdy3;
			m_dizdyl = dxdy3 * m_dizdx + m_dizdy;
			m_duizdyl = dxdy3 * m_duizdx + m_duizdy;
			m_dvizdyl = dxdy3 * m_dvizdx + m_dvizdy;
			dy = 1 - (y2 - y2i);
			m_xl = x2 + dy * m_dxdyl;
			m_izl = iz2 + dy * m_dizdyl;
			m_uizl = uiz2 + dy * m_duizdyl;
			m_vizl = viz2 + dy * m_dvizdyl;

			Draw_Textured_Poly_Perspective_DivSub(y2i, y3i);
		}
	}
}

void CMeshManager::Draw_Textured_Poly_Perspective_DivSub(int y1, int y2)
{

	int x1, x2;
	int x, xcount;
	float z, dx;
	float iz, uiz, viz;
	int u1, v1, u2, v2, u, v, du, dv;

	while (y1 < y2)
	{
		x1 = (int)m_xl;
		x2 = (int)m_xr;

		dx = 1 - (m_xl - x1);
		iz = m_izl + dx * m_dizdx;
		uiz = m_uizl + dx * m_duizdx;
		viz = m_vizl + dx * m_dvizdx;

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
				UINT t = ((((int) v) & 0xff0000) >> 8) + ((((int) u) & 0xff0000) >> 16);

				if( t < 0 || t > (m_TextureWidth * m_TextureHeight - 1) )
				continue;

				t= t*3;

				SetPixel(m_hBackBuffer, x1++, y1, RGB(m_Res[t + 2],m_Res[t + 1],m_Res[t]));

				u += du;
				v += dv;
			}

			xcount -= SUBDIVSIZE;
		}

		if (xcount)
		{
			iz += m_dizdx * xcount;
			uiz += m_duizdx * xcount;
			viz += m_dvizdx * xcount;

			u1 = u2;
			v1 = v2;

			z = 65536 / iz;
			u2 = (int)(uiz * z);
			v2 = (int)(viz * z);

			u = u1;
			v = v1;

			du = (u2 - u1) / xcount;
			dv = (v2 - v1) / xcount;

			while (xcount--)
			{
				UINT t = ((((int) v) & 0xff0000) >> 8) + ((((int) u) & 0xff0000) >> 16);

				if( t < 0 || t > (m_TextureWidth * m_TextureHeight - 1) )
				continue;

				t= t*3;

				SetPixel(m_hBackBuffer, x1++, y1, RGB(m_Res[t + 2],m_Res[t + 1],m_Res[t]));

				u += du;
				v += dv;
			}
		}

		m_xl += m_dxdyl;
		m_xr += m_dxdyr;
		m_izl += m_dizdyl;
		m_uizl += m_duizdyl;
		m_vizl += m_dvizdyl;

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
