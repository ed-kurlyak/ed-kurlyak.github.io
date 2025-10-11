//======================================================================================
//	Ed Kurlyak 2025 Software Rendering
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

// Получить индекс в таблице из угла
int CMeshManager::AngleToIndex(int angle)
{
    // Угол по модулю 360 градусов (65536)
    angle &= (PHD_360 - 1);

    // Индекс = угол / 64 (шаг)
    return angle / ANGLE_STEP;
}

int CMeshManager::Math_Cos(int Angle)
{
    int idx = AngleToIndex(Angle);
    return cos_table[idx];
}

int CMeshManager::Math_Sin(int Angle)
{
    // Синус(theta) = косинус(90° - theta)
    int shifted_angle = (PHD_90 - Angle) & (PHD_360 - 1);
    return Math_Cos(shifted_angle);
}

void CMeshManager::Build_Cos_Table()
{
	for (int i = 0; i < TABLE_SIZE; i++)
    {
        double angle = 2.0 * PI * i / TABLE_SIZE;
        double cos_val = cos(angle);
        short int fixed_val = static_cast<short int>(cos_val * FIXED_SCALE);
        cos_table[i] = fixed_val;
    }
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

	Build_Cos_Table();

	Create_BackBuffer();

	m_VertCount = 24;
	m_TriangleCount = 12;

	Read_BMP_File(".//texture.bmp");
	
	vector3 VertBuffTemp[24] = {
		{-50,-50,-50},
		{-50,-50,50},
		{50,-50,50},
		{50,-50,-50},
		{-50,50,-50},
		{50,50,-50},
		{50,50,50},
		{-50,50,50},
		{-50,-50,-50},
		{50,-50,-50},
		{50,50,-50},
		{-50,50,-50},
		{50,-50,-50},
		{50,-50,50},
		{50,50,50},
		{50,50,-50},
		{50,-50,50},
		{-50,-50,50},
		{-50,50,50},
		{50,50,50},
		{-50,-50,50},
		{-50,-50,-50},
		{-50,50,-50},
		{-50,50,50} };

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
	static short int Angle = 0;

	matrix4x4 MatRotateX={
		1, 0, 0, 0,
        0, Math_Cos(Angle), Math_Sin(Angle), 0,
        0,-Math_Sin(Angle),  Math_Cos(Angle), 0,
        0, 0, 0, 1};

	matrix4x4 MatRotateY={
		Math_Cos(Angle), 0, -Math_Sin(Angle), 0,
		0, 1, 0, 0,
		Math_Sin(Angle), 0, Math_Cos(Angle), 0,
		0, 0, 0, 1};

	matrix4x4 MatRotateZ={
		Math_Cos(Angle), Math_Sin(Angle), 0, 0,
		-Math_Sin(Angle), Math_Cos(Angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

	Angle = Angle + 182;
	Angle = Angle & 0xFFFF;

	matrix4x4 MatWorld={
		1, 0, 0, 0,
		0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, (180 << 14), 1};

	matrix4x4 MatView={
		1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

	// 65536 / 360 = 182 
	int Fov = 182 * 90; //угол обзора экрана 90 градусов              		
	int Persp = m_ViewHeight / 2 * Math_Cos(Fov / 2) / Math_Sin(Fov / 2);
	
	for (UINT i = 0; i < m_VertCount; i++)
	{
		vector3 VecTemp;

		VecTemp = Vec3_Mat4x4_Mul(m_VertBuff[i], MatRotateX);
		
		VecTemp.x = VecTemp.x;
		VecTemp.y = VecTemp.y >> 14;
		VecTemp.z = VecTemp.z >> 14;
		
		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatRotateY);

		VecTemp.y = VecTemp.y << 14;
		VecTemp.z = VecTemp.z + (180 << 14);
		
		VecTemp.x = (int)(VecTemp.x * ((float)Persp / VecTemp.z)) + m_ViewWidth / 2;
		VecTemp.y = (int)(-VecTemp.y * ((float)Persp / VecTemp.z)) + m_ViewHeight / 2;

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

		int s = ((Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x));

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

	m_TextureWidth = bih.biWidth << 8;
	m_TextureHeight = bih.biHeight << 8;
}

void CMeshManager::Draw_Textured_Triangle(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3)
{
	int Side;
	int x1, x2, x3;
	int y1, y2, y3;
	float iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;
	int IntTemp;
	float FloatTemp;

	x1 = VecIn1.x;
	y1 = VecIn1.y;
	x2 = VecIn2.x;
	y2 = VecIn2.y;
	x3 = VecIn3.x;
	y3 = VecIn3.y;
	
	iz1 = One / VecIn1.z;
	uiz1 = (TexIn1.tu) * iz1;
	viz1 = (TexIn1.tv) * iz1;
	
	iz2 = One / VecIn2.z;
	uiz2 = (TexIn2.tu) * iz2;
	viz2 = (TexIn2.tv) * iz2;
	
	iz3 = One / VecIn3.z;
	uiz3 = (TexIn3.tu) * iz3;
	viz3 = (TexIn3.tv) * iz3;

	if (y1 > y2)
	{
		SWAP(x1, x2, IntTemp);
		SWAP(y1, y2, IntTemp);
		SWAP(iz1, iz2, FloatTemp);
		SWAP(uiz1, uiz2, FloatTemp);
		SWAP(viz1, viz2, FloatTemp);
	}
	if (y1 > y3)
	{
		SWAP(x1, x3, IntTemp);
		SWAP(y1, y3, IntTemp);
		SWAP(iz1, iz3, FloatTemp);
		SWAP(uiz1, uiz3, FloatTemp);
		SWAP(viz1, viz3, FloatTemp);
	}
	if (y2 > y3)
	{
		SWAP(x2, x3, IntTemp);
		SWAP(y2, y3, IntTemp);
		SWAP(iz2, iz3, FloatTemp);
		SWAP(uiz2, uiz3, FloatTemp);
		SWAP(viz2, viz3, FloatTemp);
	}
	
	if(y2 > y1 && y3 > y2)
	{
		//вычисляем наклон левой и правой стороны
		//треугольника, по знаку величины наклона
		//(+ или -) определяем какая сторона
		//(левая или правая) имеет большее значение по Y
		int dxdy1 = ((x2 - x1) << 16) / (y2 - y1);
		int dxdy2 = ((x3 - x1) << 16) / (y3 - y1);
		Side= dxdy2 > dxdy1;
	}

	if (y1 == y2)
		Side = x1 > x2;
	if (y2 == y3)
		Side = x3 > x2;

	if (!Side)
	{
		m_xl = x1 << 16;
		m_ul = uiz1;
		m_vl = viz1;
		m_zl = iz1;

		
		m_dxdyl = ((x3 - x1) << 16) / (y3 - y1);
		m_dudyl = (uiz3 - uiz1) / (y3 - y1);
		m_dvdyl = (viz3 - viz1) / (y3 - y1);
		m_dzdyl = (iz3 - iz1) / (y3 - y1);
	
		if (y1 < y2)
		{
			m_xr = x1 << 16;
			m_ur = uiz1;
			m_vr = viz1;
			m_zr = iz1;

			m_dxdyr = ((x2 - x1) << 16) / (y2 - y1);
			m_dudyr = (uiz2 - uiz1) / (y2 - y1);
			m_dvdyr = (viz2 - viz1) / (y2 - y1);
			m_dzdyr = (iz2 - iz1) / (y2 - y1);
			
			Draw_Textured_Poly(y1, y2);
		}
		if (y2 < y3)
		{
			m_xr = x2 << 16;
			m_ur = uiz2;
			m_vr = viz2;
			m_zr = iz2;

			m_dxdyr = ((x3 - x2) << 16) / (y3 - y2);
			m_dudyr = (uiz3 - uiz2) / (y3 - y2);
			m_dvdyr = (viz3 - viz2) / (y3 - y2);
			m_dzdyr = (iz3 - iz2) / (y3 - y2);
			
			Draw_Textured_Poly(y2, y3);
		}
	}
	else
	{
		m_xr = x1 << 16;
		m_ur = uiz1;
		m_vr = viz1;
		m_zr = iz1;
		
		float val = ((float)(x3 - x1)) / (y3 - y1);

		m_dxdyr = ((x3 - x1) << 16) / (y3 - y1);
		m_dudyr = (uiz3 - uiz1) / (y3 - y1);
		m_dvdyr = (viz3 - viz1) / (y3 - y1);
		m_dzdyr = (iz3 - iz1) / (y3 - y1);
		
		if (y1 < y2)
		{
			m_xl = x1 << 16;
			m_ul = uiz1;
			m_vl = viz1;
			m_zl = iz1;

			m_dxdyl = ((x2 - x1) << 16) / (y2 - y1);
			m_dudyl = (uiz2 - uiz1) / (y2 - y1);
			m_dvdyl = (viz2 - viz1) / (y2 - y1);
			m_dzdyl = (iz2 - iz1) / (y2 - y1);
			
			Draw_Textured_Poly(y1, y2);
		}
		if (y2 < y3)
		{
			m_xl = x2 << 16;
			m_ul = uiz2;
			m_vl = viz2;
			m_zl = iz2;

			m_dxdyl = ((x3 - x2) << 16) / (y3 - y2);
			m_dudyl = (uiz3 - uiz2) / (y3 - y2);
			m_dvdyl = (viz3 - viz2) / (y3 - y2);
			m_dzdyl = (iz3 - iz2) / (y3 - y2);
						
			Draw_Textured_Poly(y2, y3);
		}
	}
}

void CMeshManager::Draw_Textured_Poly(int y1, int y2)
{
	float ui, vi, zi;
	float du, dv, dz;
	
	for (int y = y1; y < y2; y++)
	{
		int m_xlt = m_xl >> 16;
		int m_xrt = m_xr >> 16;

		if ((m_xrt - m_xlt)>0)
		{
			du = (m_ur - m_ul) / (m_xrt - m_xlt);
			dv = (m_vr - m_vl) / (m_xrt - m_xlt);
			dz = (m_zr - m_zl) / (m_xrt - m_xlt);
		}
		else
		{
			du = 0;
			dv = 0;
			dz = 0;
		}

		
		zi = m_zl;
		ui = m_ul;
		vi = m_vl;

		for (int x=m_xlt; x<m_xrt; x++)
		{
			/*
			float z = 256.0f / zi;
			int u = (int)(ui * z);
			u = (u >> 16);
			int v = (int)(vi * z);
			v = (v >> 16);
			*/

			float z = 1.0f / zi;
			int u = (int)(ui * z);
			u = (u >> 8);
			int v = (int)(vi * z);
			v = (v >> 8);

			UINT t = (int)u  + (((int)v) * (m_TextureWidth >> 8));

			if( t < 0 || t > (((m_TextureWidth >> 8) * m_TextureHeight >> 8) - 1) )
				continue;

			t= t*3;

			SetPixel(m_hBackBuffer, x, y, RGB(m_Res[t + 2],m_Res[t + 1],m_Res[t]));
			//SetPixel(m_hBackBuffer, x, y, RGB(255,255,127));
				
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
