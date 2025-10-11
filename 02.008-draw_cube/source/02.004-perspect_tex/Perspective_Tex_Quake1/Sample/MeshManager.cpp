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

void CMeshManager::Init_MeshManager(HWND hWnd, HINSTANCE hInstance)
{
	m_Res = NULL;

	m_hWnd = hWnd;

	m_hInst = hInstance;

	Read_BMP_File(".//texture.bmp");

	Create_BackBuffer();
	Create_Palette();

	m_VertCount = 24;
	m_TriangleCount = 12;

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

		Draw_Textured_Triangle(Vec1, Tex1,
								Vec2, Tex2,
								Vec3, Tex3);
     }  

	Sleep(25);

	Present_BackBuffer();
	
}

void CMeshManager::Read_BMP_File(char *szTexFileName)
{

	//открываем BMP файл 64x64 пикселя 8 бит
	//глубина цвета для чтения в бинарном режиме
	FILE *fp = NULL;
	fopen_s(&fp, "texture.bmp", "rb");
	if(fp==NULL) printf("Error Open File");

	//читаем заголовок файла текстуры
	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);

	//читаем заголовок файла текстуры
	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(BITMAPINFOHEADER), 1, fp);

	//одни элемент палитны занимает 4 байта
	//3 байта для R,G,B и один зарезервированный
	//fread(&RgbPal, 256 * 4,1,fp);
	fread(&RgbPal, 256 * sizeof(RGBQUAD),1,fp);

	//сдвигаемся от начала BMP файла до начала данных
	fseek(fp, bfh.bfOffBits, SEEK_SET);

	//указатель на массив байт полученных из BMP файла
	m_Res = new unsigned char[bih.biHeight * bih.biWidth];
	
	//читаем из файла цветовые данные изображения
	fread(m_Res, bih.biHeight * bih.biWidth, 1, fp);

	//загрузили текстуру закрываем файл
	fclose(fp);

		m_TextureWidth = bih.biWidth;
	m_TextureHeight = bih.biHeight;


}

void CMeshManager::DrawSpan(int y, Edge left, Edge right)
{
    int x0 = int(left.x);
    int x1 = int(right.x);
    int count = x1 - x0;
    if (count <= 0 || y < 0 || y >= m_ViewHeight || x1 < 0 || x0 >= m_ViewWidth)
        return;

    float ds_over_z = (right.s_over_z - left.s_over_z) / count;
    float dt_over_z = (right.t_over_z - left.t_over_z) / count;
    float done_over_z = (right.one_over_z - left.one_over_z) / count;

    float s_over_z = left.s_over_z;
    float t_over_z = left.t_over_z;
    float one_over_z = left.one_over_z;

    if (x0 < 0)
    {
        int skip = -x0;
        s_over_z += ds_over_z * skip;
        t_over_z += dt_over_z * skip;
        one_over_z += done_over_z * skip;
        x0 = 0;
    }
    if (x1 > m_ViewWidth) x1 = m_ViewWidth;

    for (int x = x0; x < x1; ++x)
    {
        float z = 1.0f / one_over_z;
        float s = s_over_z * z;
        float t = t_over_z * z;

        int u = (s < 0) ? 0 : (s > 63 ? 63 : int(s));
        int v = (t < 0) ? 0 : (t > 63 ? 63 : int(t));

        unsigned char texel = m_Res[v * 64 + u];

        m_BackBuffer[y * m_ViewWidth + x] = texel;

        s_over_z += ds_over_z;
        t_over_z += dt_over_z;
        one_over_z += done_over_z;
    }
}

Edge CMeshManager::InterpolateEdge(const Vertex& a, const Vertex& b, float y)
{
    float dy = b.y - a.y;
    float t = (dy == 0) ? 0.0f : (y - a.y) / dy;
    float x = a.x + (b.x - a.x) * t;
    float s_over_z = (a.s / a.z) + ((b.s / b.z) - (a.s / a.z)) * t;
    float t_over_z = (a.t / a.z) + ((b.t / b.z) - (a.t / a.z)) * t;
    float one_over_z = (1.0f / a.z) + ((1.0f / b.z) - (1.0f / a.z)) * t;
    Edge e = { x, s_over_z, t_over_z, one_over_z };
    return e;
}

void CMeshManager::Draw_Textured_Triangle(vector3 VecIn1, tex_coord2 Tex1,
										  vector3 VecIn2, tex_coord2 Tex2,
										  vector3 VecIn3, tex_coord2 Tex3)
{
	Vertex v0 = { VecIn1.x, VecIn1.y, VecIn1.z, (float)Tex1.tu, (float)Tex1.tv};
	Vertex v1 = { VecIn2.x, VecIn2.y, VecIn2.z, (float)Tex2.tu, (float)Tex2.tv};
	Vertex v2 = { VecIn3.x, VecIn3.y, VecIn3.z, (float)Tex3.tu, (float)Tex3.tv};

    // сортировка по y
    const Vertex* vt[3] = { &v0, &v1, &v2 };

    if (vt[1]->y < vt[0]->y) std::swap(vt[0], vt[1]);
    if (vt[2]->y < vt[1]->y) std::swap(vt[1], vt[2]);
    if (vt[1]->y < vt[0]->y) std::swap(vt[0], vt[1]);

    int y0 = (int)std::ceil(vt[0]->y);
	if (y0 < 0)
		y0 = 0;

	int y1 = (int)std::ceil(vt[1]->y);
	if (y1 > m_ViewHeight)
		y1 = m_ViewHeight;

	int y2 = (int)std::ceil(vt[2]->y);
	if (y2 > m_ViewHeight)
		y2 = m_ViewHeight;

    for (int y = y0; y < y1; ++y)
    {
        Edge left = InterpolateEdge(*vt[0], *vt[2], (float)y);
        Edge right = InterpolateEdge(*vt[0], *vt[1], (float)y);
        if (left.x > right.x) std::swap(left, right);
        DrawSpan(y, left, right);
    }

    for (int y = y1; y < y2; ++y)
    {
        Edge left = InterpolateEdge(*vt[0], *vt[2], (float)y);
        Edge right = InterpolateEdge(*vt[1], *vt[2], (float)y);
        if (left.x > right.x) std::swap(left, right);
        DrawSpan(y, left, right);
    }
}

void CMeshManager::Create_Palette()
{
	
	palette_t Pal[256];

	for (int i = 0; i < 256; i++)
	{
		Pal[i].red = RgbPal[i].rgbRed;
		Pal[i].green = RgbPal[i].rgbGreen;
		Pal[i].blue = RgbPal[i].rgbBlue;
	}

	MGL_setPalette(dibdc, Pal, 256, 0);
	MGL_realizePalette(dibdc, 256, 0, false);
}

void CMeshManager::Create_BackBuffer()
{
	RECT Rc;
	GetClientRect(m_hWnd, &Rc);

	m_ViewWidth = Rc.right;
	m_ViewHeight = Rc.bottom;

	pixel_format_t pf;

	MGL_setAppInstance(m_hInst);

	MGL_registerDriver(MGL_PACKED8NAME, PACKED8_driver);

	MGL_initWindowed("");

	if (!MGL_changeDisplayMode(grWINDOWED))
		MGL_fatalError("Unable to use window mode!");

	if ((windc = MGL_createWindowedDC(m_hWnd)) == NULL)
		MGL_fatalError("Unable to create Windowed DC!");

	MGL_getPixelFormat(windc, &pf);

	if ((dibdc = MGL_createMemoryDC(m_ViewWidth, m_ViewHeight, 8, &pf)) ==
		NULL)
		MGL_fatalError("Unable to create Memory DC!");
}

void CMeshManager::Clear_BackBuffer()
{
	MGL_beginDirectAccess();

	m_BackBuffer = (unsigned char *)dibdc->surface;
	
	for (int x = 0; x < m_ViewWidth; x++)
	{
		for (int y = 0; y < m_ViewHeight; y++)
		{
			int Index = y * m_ViewWidth + x;

			m_BackBuffer[Index + 0] = 0;
		}
	}
}

void CMeshManager::Present_BackBuffer()
{
	MGL_endDirectAccess();

	// MGL present back buffer
	HDC hdcScreen = GetDC(m_hWnd);
	MGL_setWinDC(windc, hdcScreen);

	MGL_bitBltCoord(windc, dibdc, 0, 0, m_ViewWidth, m_ViewHeight, 0, 0,
					MGL_REPLACE_MODE);

	ReleaseDC(m_hWnd, hdcScreen);
}

void CMeshManager::Delete_BackBuffer()
{
	if (windc)
		MGL_destroyDC(windc);
	if (dibdc)
		MGL_destroyDC(dibdc);

	windc = dibdc = NULL;
}
