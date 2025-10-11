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

	delete [] m_TexCoordBuff;
	m_TexCoordBuff = NULL;

	delete [] m_Res;
	m_Res = NULL;
}

vector3 CMeshManager::Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn)
{
	vector3 VecOut;

	VecOut.x =	VecIn.x * MatIn[M00] +
				VecIn.y * MatIn[M10] +
				VecIn.z * MatIn[M20] +
						MatIn[M30];

	VecOut.y =	VecIn.x * MatIn[M01] +
				VecIn.y * MatIn[M11] +
				VecIn.z * MatIn[M21] +
						MatIn[M31];

	VecOut.z =	VecIn.x * MatIn[M02] +
				VecIn.y * MatIn[M12] +
				VecIn.z * MatIn[M22] +
						MatIn[M32];


	return VecOut;
}

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_Res = NULL;
	m_TextureWidth = 0;
	m_TextureHeight = 0;

	m_hWnd = hWnd;

	Create_BackBuffer();

	Read_BMP_File(".//texture.bmp");
	
	int Stacks = 10;
	int Slices = 20;
	//float PI2	   = 6.28318530717958f;
	//float PI     = 3.14159265358979f;
	float ThetaFac = PI2 / (float)Slices;
	float PhiFac = PI / (float)Stacks;
	float Radius = 8.0;

	m_VertCount =  Slices * Stacks * 4;
	m_TriangleCount =  Slices * Stacks * 2;

	m_VertBuff = NULL;
	m_VertBuff = new vector3[m_VertCount];

	m_TexCoordBuff = NULL;
	m_TexCoordBuff = new tex_coord2[m_VertCount];

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

			//переворачиваем текстурные координаты tu
			//так как на сферу текстура накладывается
			//справа на лево а в текстуре рисунок лежит
			//слева на право
			m_TexCoordBuff[Index + 0].tu = (m_TextureWidth - 1) - (int) (( (float)l / (float)(Slices) ) * (float)(m_TextureWidth - 1) );
			m_TexCoordBuff[Index + 0].tv = (int) (( (float)b / (float)(Stacks) ) * (float)(m_TextureHeight - 1) );

			sb = PhiFac * (b + 1);
			sl = ThetaFac * l;

			m_VertBuff[Index + 1].z = Radius * sinf(sb) * cosf(sl);
			m_VertBuff[Index + 1].x = Radius * sinf(sb) * sinf(sl);
			m_VertBuff[Index + 1].y = Radius * cosf(sb);
			
			m_TexCoordBuff[Index + 1].tu = (m_TextureWidth - 1) - (int) (( (float)l / (float)(Slices) ) * (float)(m_TextureWidth - 1) );
			m_TexCoordBuff[Index + 1].tv = (int) (( (float)(b + 1) / (float)(Stacks) ) * (float)(m_TextureHeight - 1) );
			
			sb = PhiFac * (b + 1);
			sl = ThetaFac * (l + 1);

			m_VertBuff[Index + 2].z = Radius * sinf(sb) * cosf(sl);
			m_VertBuff[Index + 2].x = Radius * sinf(sb) * sinf(sl);
			m_VertBuff[Index + 2].y = Radius * cosf(sb);

			m_TexCoordBuff[Index + 2].tu = (m_TextureWidth - 1) - (int) (( (float) (l + 1) / (float)(Slices) ) * (float)(m_TextureWidth - 1) );
			m_TexCoordBuff[Index + 2].tv = (int) (( (float) (b + 1) / (float)(Stacks) ) * (float)(m_TextureHeight - 1) );

			sb = PhiFac * b;
			sl = ThetaFac * (l + 1);

			m_VertBuff[Index + 3].z = Radius * sinf(sb) * cosf(sl);
			m_VertBuff[Index + 3].x = Radius * sinf(sb) * sinf(sl);
			m_VertBuff[Index + 3].y = Radius * cosf(sb);

			m_TexCoordBuff[Index + 3].tu = (m_TextureWidth - 1) - (int) (( (float) (l + 1) / (float)(Slices) ) * (float)(m_TextureWidth - 1) );
			m_TexCoordBuff[Index + 3].tv = (int) (( (float)b / (float)(Stacks) ) * (float)(m_TextureHeight - 1) );

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
        0, 0, 15.0f, 1};

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

		//используем псевдоскалярное (косое) умножение векторов
		//для отбрасывания задних поверхностей
		float s = (Vec2.x - Vec1.x) * ( Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);

		//здесь значение 0.1f используется что бы отбрасывать вырожденные
		//треугольники у которых площадь равна нулю (или прилизительно ноль)
		if(s <= 0.1f)
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
	
	if (y2i > y1i && y3i > y2i)
	{
		float dxdy1 = (x2 - x1) / (y2 - y1);
		float dxdy2 = (x3 - x1) / (y3 - y1);
		Side = dxdy2 > dxdy1;
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
		
			Draw_Textured_Poly(y2i, y3i);
		}
	}
}

void CMeshManager::Draw_Textured_Poly(int y1, int y2)
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

			//размер текстуры сферы 256 пикс. высота и 512 пикс. ширина
			if( t < 0 || t > (m_TextureWidth * m_TextureHeight - 1) )
				continue;

			t= t*3;

			SetPixel(m_hBackBuffer, x, y, RGB(m_Res[t + 2], m_Res[t + 1], m_Res[t]));
				
			ui+=du;
			vi+=dv;
			zi+=dz;
		}

		m_xl+=m_dxdyl;
		m_ul+=m_dudyl;
		m_vl+=m_dvdyl;
		m_zl+=m_dzdyl;

		m_xr+=m_dxdyr;
		m_ur+=m_dudyr;
		m_vr+=m_dvdyr;
		m_zr+=m_dzdyr;
				
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
