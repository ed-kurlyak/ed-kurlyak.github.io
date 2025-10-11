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

	Angle += PI/45.0f;
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

int CMeshManager::Generate_Slopes(int Side)
{

	//начальная позиция для левой стороны треугольника
	float ul, vl, zl, xl;
	
	//начальная позиция для правой стороны треугольника
	float ur, vr, zr, xr;

	//приращение координат для левой стороны треугольника
	float  dudyl, dvdyl, dzdyl, dxdyl;

	//приращение координат для правой стороны треугольника
	float dudyr, dvdyr, dzdyr, dxdyr;

	XBUF_PERSP_FP *XPtr;

	if (!Side) //левая сторона треугольника длинее
	{
		xl = x1;
		ul = uiz1;
		vl = viz1;
		zl = iz1;

		dxdyl = (x3 - x1) / (y3 - y1);
		dudyl = (uiz3 - uiz1) / (y3 - y1);
		dvdyl = (viz3 - viz1) / (y3 - y1);
		dzdyl = (iz3 - iz1) / (y3 - y1);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)y1;

		for ( int y = (int)y1; y < (int)y3; y++, XPtr++ )
		{
			XPtr->XLeft = (int)xl;
			XPtr->UOZleft = ul;
			XPtr->VOZleft = vl;
			XPtr->OOZleft = zl;

			xl += dxdyl;
			ul += dudyl;
			vl += dvdyl;
			zl += dzdyl;

		}

		xr = x1;
		ur = uiz1;
		vr = viz1;
		zr = iz1;

		dxdyr = (x2 - x1) / (y2 - y1);
		dudyr = (uiz2 - uiz1) / (y2 - y1);
		dvdyr = (viz2 - viz1) / (y2 - y1);
		dzdyr = (iz2 - iz1) / (y2 - y1);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)y1;

		for ( int y = (int)y1; y < (int)y2; y++, XPtr++ )
		{
			XPtr->XRight = (int)xr;
			XPtr->UOZright = ur;
			XPtr->VOZright = vr;
			XPtr->OOZright = zr;

			xr += dxdyr;
			ur += dudyr;
			vr += dvdyr;
			zr += dzdyr;
		}

		xr = x2;
		ur = uiz2;
		vr = viz2;
		zr = iz2;

		dxdyr = (x3 - x2) / (y3 - y2);
		dudyr = (uiz3 - uiz2) / (y3 - y2);
		dvdyr = (viz3 - viz2) / (y3 - y2);
		dzdyr = (iz3 - iz2) / (y3 - y2);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)y2;

		for ( int y = (int)y2; y < (int)y3; y++, XPtr++)
		{
			XPtr->XRight = (int)xr;
			XPtr->UOZright = ur;
			XPtr->VOZright = vr;
			XPtr->OOZright = zr;

			xr += dxdyr;
			ur += dudyr;
			vr += dvdyr;
			zr += dzdyr;
		}
	}
	else
	{
		xr = x1;
		ur = uiz1;
		vr = viz1;
		zr = iz1;
		
		dxdyr = (x3 - x1) / (y3 - y1);
		dudyr = (uiz3 - uiz1) / (y3 - y1);
		dvdyr = (viz3 - viz1) / (y3 - y1);
		dzdyr = (iz3 - iz1) / (y3 - y1);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)y1;

		for ( int y = (int)y1; y < (int)y3; y++, XPtr++ )
		{
			XPtr->XRight = (int)xr;
			XPtr->UOZright = ur;
			XPtr->VOZright = vr;
			XPtr->OOZright = zr;

			xr += dxdyr;
			ur += dudyr;
			vr += dvdyr;
			zr += dzdyr;
		}

		xl = x1;
		ul = uiz1;
		vl = viz1;
		zl = iz1;

		dxdyl = (x2 - x1) / (y2 - y1);
		dudyl = (uiz2 - uiz1) / (y2 - y1);
		dvdyl = (viz2 - viz1) / (y2 - y1);
		dzdyl = (iz2 - iz1) / (y2 - y1);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)y1;

		for ( int y = (int)y1; y < (int)y2; y++, XPtr++ )
		{
			XPtr->XLeft = (int)xl;
			XPtr->UOZleft = ul;
			XPtr->VOZleft = vl;
			XPtr->OOZleft = zl;

			xl += dxdyl;
			ul += dudyl;
			vl += dvdyl;
			zl += dzdyl;
		}

		xl = x2;
		ul = uiz2;
		vl = viz2;
		zl = iz2;

		dxdyl = (x3 - x2) / (y3 - y2);
		dudyl = (uiz3 - uiz2) / (y3 - y2);
		dvdyl = (viz3 - viz2) / (y3 - y2);
		dzdyl = (iz3 - iz2) / (y3 - y2);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)y2;

		for ( int y = (int)y2; y < (int)y3; y++, XPtr++)
		{
			XPtr->XLeft = (int)xl;
			XPtr->UOZleft = ul;
			XPtr->VOZleft = vl;
			XPtr->OOZleft = zl;

			xl += dxdyl;
			ul += dudyl;
			vl += dvdyl;
			zl += dzdyl;
		}
	}

	return 1;
}

void CMeshManager::Draw_Textured_Triangle(vector3 VecIn1, tex_coord2 TexIn1,
						  vector3 VecIn2, tex_coord2 TexIn2,
						  vector3 VecIn3, tex_coord2 TexIn3)
{
	int Side;
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

	if(Generate_Slopes(Side))
		Draw_Textured_Poly(y1i, y3i);
}

void CMeshManager::Draw_Textured_Poly(int y1, int y2)
{
	XBUF_PERSP_FP *DataPtr;
	float OoZ,UoZ,VoZ,Scale;
	float OoZAdd16,UoZAdd16,VoZAdd16;
	float EndU,EndV;
	float StartU,StartV;
	float AddU,AddV;
	int StartX,EndX;
	int x;
	int Color;
	int IndexX;

	DataPtr = ((XBUF_PERSP_FP *)XBuffer)+y1;

	for (int y = y1; y < y2; y++, DataPtr++)
	{
		StartX = DataPtr->XLeft;
		EndX = DataPtr->XRight;

		x = EndX - StartX;

		if (x <= 0)
		{
			continue;
		}
		
		if ( x>16 )
		{
			Scale = 16.0f/x;

			UoZ = DataPtr->UOZleft;
			UoZAdd16 = (DataPtr->UOZright-UoZ) * Scale;
			VoZ = DataPtr->VOZleft;
			VoZAdd16 = (DataPtr->VOZright-VoZ) * Scale;
			OoZ = DataPtr->OOZleft;
			OoZAdd16 = (DataPtr->OOZright-OoZ) * Scale;

			Scale = 1.0f/OoZ;
			EndU = (UoZ * Scale);
			EndV = (VoZ * Scale);

			IndexX = StartX;
		
			OoZ += OoZAdd16;
			Scale = 1.0f/OoZ;

			for ( ; x>=16; x-=16)
			{
				StartU = EndU;
				UoZ += UoZAdd16;
				EndU = (UoZ*Scale);
				AddU = (EndU-StartU)/16.0f;

				StartV = EndV;
				VoZ += VoZAdd16;
				EndV = (VoZ*Scale);
				AddV = (EndV-StartV)/16.0f;

				OoZ += OoZAdd16;
				Scale = 1.0f/OoZ;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 0, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
					
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 1, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 2, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 3, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 4, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 5, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 6, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 7, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 8, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 9, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 10, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 11, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 12, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 13, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;

				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 14, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				StartU += AddU;
				StartV += AddV;
	
				Color = (((int)StartV)<<8) + ((int)StartU);
				Color *= 3;
				SetPixel(m_hBackBuffer, IndexX + 15, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
				
				IndexX += 16;
			}

			if (!x)
			{
				continue;
			}

			Scale = 1.0f/DataPtr->OOZright;

			StartU = EndU;
			EndU = (DataPtr->UOZright*Scale);
			AddU = (EndU-StartU)/x;

			StartV = EndV;
			EndV = (DataPtr->VOZright*Scale);
			AddV = (EndV-StartV)/x;

		}
		else
		{

			Scale = 1.0f/DataPtr->OOZleft;
			StartU = (DataPtr->UOZleft * Scale);
			StartV = (DataPtr->VOZleft * Scale);

			Scale = 1.0f/DataPtr->OOZright;
			EndU = (DataPtr->UOZright * Scale);
			EndV = (DataPtr->VOZright * Scale);

			AddU = (EndU-StartU)/x;
			AddV = (EndV-StartV)/x;

			IndexX = StartX;

		}
		
		for ( ; x>1; x-=2, IndexX += 2)
		{

			Color = (((int)StartV)<<8) + ((int)StartU);
			Color *= 3;
			SetPixel(m_hBackBuffer, IndexX + 0, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
			
			StartU += AddU;
			StartV += AddV;

			Color = (((int)StartV)<<8) + ((int)StartU);
			Color *= 3;
			SetPixel(m_hBackBuffer, IndexX + 1, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));

			StartU += AddU;
			StartV += AddV;
		
		}
		
		if (x)
		{
			Color = (((int)StartV)<<8) + ((int)StartU);
			Color *= 3;
			SetPixel(m_hBackBuffer, IndexX + 0, y, RGB(m_Res[Color + 2],m_Res[Color + 1],m_Res[Color + 0]));
		}
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
