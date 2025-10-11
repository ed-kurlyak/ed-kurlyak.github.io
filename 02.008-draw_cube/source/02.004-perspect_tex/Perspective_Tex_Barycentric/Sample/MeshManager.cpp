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

float CMeshManager::fminf(float a, float b)
{
    return (a < b) ? a : b;
}

float CMeshManager::fmaxf(float a, float b)
{
    return (a > b) ? a : b;
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

		Draw_Textured_Barycentric_Triangle(Vec1, Tex1, Vec2, Tex2, Vec3, Tex3 );
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


void CMeshManager::Draw_Textured_Barycentric_Triangle(vector3 VecIn1, tex_coord2 TexIn1,
                                                      vector3 VecIn2, tex_coord2 TexIn2,
                                                      vector3 VecIn3, tex_coord2 TexIn3)
{
    float x1 = VecIn1.x, y1 = VecIn1.y, z1 = VecIn1.z;
    float x2 = VecIn2.x, y2 = VecIn2.y, z2 = VecIn2.z;
    float x3 = VecIn3.x, y3 = VecIn3.y, z3 = VecIn3.z;

    // проверяем ориентацию треугольника
    float cross = (x2 - x1)*(y3 - y1) - (y2 - y1)*(x3 - x1);
    if (cross < 0.0f)
    {
        std::swap(VecIn2, VecIn3);
        std::swap(TexIn2, TexIn3);

        x2 = VecIn2.x; y2 = VecIn2.y; z2 = VecIn2.z;
        x3 = VecIn3.x; y3 = VecIn3.y; z3 = VecIn3.z;
    }

    unsigned int minX = (int)floorf(fminf(fminf(x1, x2), x3));
    unsigned int maxX = (int)ceilf(fmaxf(fmaxf(x1, x2), x3));
    unsigned int minY = (int)floorf(fminf(fminf(y1, y2), y3));
    unsigned int maxY = (int)ceilf(fmaxf(fmaxf(y1, y2), y3));

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= m_ViewWidth) maxX = m_ViewWidth - 1;
    if (maxY >= m_ViewHeight) maxY = m_ViewHeight - 1;

    float denom = ( (y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3) );
    if (fabs(denom) < 1e-6f)
        return;

    float texWidthF  = (float)(m_TextureWidth - 1);
	float texHeightF = (float)(m_TextureHeight - 1);

	float u1 = (TexIn1.tu / texWidthF) / z1;
	float v1 = (TexIn1.tv / texHeightF) / z1;
	float u2 = (TexIn2.tu / texWidthF) / z2;
	float v2 = (TexIn2.tv / texHeightF) / z2;
	float u3 = (TexIn3.tu / texWidthF) / z3;
	float v3 = (TexIn3.tv / texHeightF) / z3;

    float oneOverZ1 = 1.0f / z1;
    float oneOverZ2 = 1.0f / z2;
    float oneOverZ3 = 1.0f / z3;

    for (unsigned int y = minY; y <= maxY; ++y)
    {
        for (unsigned int x = minX; x <= maxX; ++x)
        {
            float lambda1 = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / denom;
            float lambda2 = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / denom;
            float lambda3 = 1.0f - lambda1 - lambda2;

            // допуск на численные ошибки
            if (lambda1 < -1e-4f || lambda2 < -1e-4f || lambda3 < -1e-4f)
                continue;

            float oneOverZ = lambda1 * oneOverZ1 + lambda2 * oneOverZ2 + lambda3 * oneOverZ3;
            if (oneOverZ < 1e-6f)
                continue;

            float u = (lambda1 * u1 + lambda2 * u2 + lambda3 * u3) / oneOverZ;
            float v = (lambda1 * v1 + lambda2 * v2 + lambda3 * v3) / oneOverZ;

            if (u < 0.0f) u = 0.0f;
            if (u > 1.0f) u = 1.0f;
            if (v < 0.0f) v = 0.0f;
            if (v > 1.0f) v = 1.0f;

            unsigned int TexX = (int)(u * (m_TextureWidth - 1));
            unsigned int TexY = (int)(v * (m_TextureHeight - 1));

            if (TexX < 0 || TexX >= m_TextureWidth || TexY < 0 || TexY >= m_TextureHeight)
                continue;

            unsigned int Index = (TexY * m_TextureWidth + TexX) * 3;
            if (Index + 2 >= m_TextureWidth * m_TextureHeight * 3)
                continue;

            BYTE r = m_Res[Index + 2];
            BYTE g = m_Res[Index + 1];
            BYTE b = m_Res[Index + 0];

            SetPixel(m_hBackBuffer, x, y, RGB(r, g, b));
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
