//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <conio.h>
#include <math.h>
#include <stdio.h>

#define SWAP(a,b,t) {t = a; a = b; b = t;}

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

double LdFreq;

unsigned char *Res = NULL;

float g_xl, g_tul, g_tvl;
float g_xr, g_tur, g_tvr;
float g_dxdyl, g_dtudyl, g_dtvdyl;
float g_dxdyr, g_dtudyr, g_dtvdyr;

UINT g_TexWidth = 255;
UINT g_TexHeight = 255;

//контекст окна приложения
HDC g_hDC;
//размеры окна приложения
RECT g_Rc;
//наш задний буфер
HDC g_hBackBuffer;
HBITMAP g_hBitmap;
HBITMAP g_hOldBitmap;
//кисть для очистки заднего буфера (окна приложения)
HBRUSH g_hBrush;

HWND g_hWnd;

struct vector3
{
	float x,y,z;
};

struct tex_coord
{
	float tu, tv;
};

enum { A, B, C, D, E, F, G, H };

/*
	CUBE VERTICES

	FONT SIDE	BACK SIDE
	C - D		G - H
	|	|		|	|
	A - B		E - F
*/

vector3 g_VertBuff[8] = {
	-4.0, -4.0, -4.0,	//A
	 4.0, -4.0, -4.0,	//B
	-4.0,  4.0, -4.0,	//C
	 4.0,  4.0, -4.0,	//D

 	-4.0, -4.0,  4.0,	//E
	 4.0, -4.0,  4.0,	//F
	-4.0,  4.0,  4.0,	//G
	 4.0,  4.0,  4.0 };	//H

vector3 g_VertBuffTransformed[8];

unsigned int g_IndexBuff[24] = {
	//FRONT SIDE LINES
	A, B,
	B, D,
	D, C,
	C, A,

	//BACK SIDE LINES
	E, F,
	F, H,
	H, G,
	G, E,

	//LINES CONNECT FRONT AND BACK
	A, E,
	B, F,
	D, H,
	C, G };

typedef float matrix4x4[4][4];

float Calculate_FPS()
{

	LARGE_INTEGER PerfFreq;
	QueryPerformanceFrequency(&PerfFreq);
	
	LARGE_INTEGER CurrentTime;
	QueryPerformanceCounter(&CurrentTime);

	static double LastTime = (double)CurrentTime.QuadPart;
	static int Count = 0;
	static float FloatFPS = 0.0f;
	
	Count++;
    if(Count == 30)
	{
        double Time;
		
		double ThisTime = (double)CurrentTime.QuadPart;
		Time = (ThisTime - LastTime)/ PerfFreq.QuadPart;
        LastTime = ThisTime;
        FloatFPS = (float)(Count/Time);
		Count = 0;
	}

	return FloatFPS;
}

int Sync(void)
{
	static double LdSync;
	int Frames;
	LARGE_INTEGER CurrentTime;
	double dCounter;
	
	QueryPerformanceCounter(&CurrentTime);

	dCounter=(double)CurrentTime.LowPart+(double)CurrentTime.HighPart*(double)0x100000000;	// WARNING: inaccuracy as HighPart increases and double loses low bit precision?
	dCounter/=LdFreq;

	Frames=long(dCounter)-long(LdSync);	// Use the non-fractional bits to get the frame count

	LdSync=dCounter;
	return Frames;
}

void Init_Limit_FPS(float LimitFPS)
{
	LARGE_INTEGER PerfFreq;
	
	if (!QueryPerformanceFrequency(&PerfFreq))
		return;

	LdFreq=(double)PerfFreq.LowPart+(double)PerfFreq.HighPart*(double)0xffffffff;
	LdFreq/=LimitFPS;
}


void Limit_FPS()
{
	int	nframes;

	nframes = Sync();

	while (!nframes)	
	{
		nframes = Sync();
	}

}

vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn)
{
	vector3 VecOut;

	VecOut.x =	VecIn.x * MatIn[0][0] +
				VecIn.y * MatIn[1][0] +
				VecIn.z * MatIn[2][0] +
						  MatIn[3][0];

	VecOut.y =	VecIn.x * MatIn[0][1] +
				VecIn.y * MatIn[1][1] +
				VecIn.z * MatIn[2][1] +
						  MatIn[3][1];

	VecOut.z =	VecIn.x * MatIn[0][2] +
				VecIn.y * MatIn[1][2] +
				VecIn.z * MatIn[2][2] +
						  MatIn[3][2];

	return VecOut;
}

void Load_BMP24()
{

	//открываем BMP файл для чтения в бинарном режиме
	FILE *fp = NULL;
	//fopen_s(&fp, "TextFont.bmp", "rb");
	fopen_s(&fp, "ExportedFont.bmp", "rb");
	if(fp==NULL) printf("Error Open File");

	//читаем заголовок файла текстуры
	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);

	//читаем заголовок файла текстуры
	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(BITMAPINFOHEADER), 1, fp);

	//сдвигаемся от начала BMP файла до начала данных
	fseek(fp, bfh.bfOffBits, SEEK_SET);

	//указатель на массив байт полученных из BMP файла
	unsigned char * ResTemp = new unsigned char[bih.biHeight * bih.biWidth * 3];
	Res = new unsigned char[bih.biHeight * bih.biWidth * 3];
	
	//читаем из файла rgb данные изображения
	//один пиксель изображения хранит три компоненты R,G,B
	//каждая компонента цвета занимает 1 байт
	fread(ResTemp, bih.biHeight * bih.biWidth * 3, 1, fp);

	//загрузили текстуру закрываем файл
	fclose(fp);

	for (int h = 0; h < bih.biHeight; h++)
		{
			for (int w = 0; w < bih.biWidth; w++)
			{
				int Index1 = (h * bih.biWidth + w) * 3;
				int Index2 = ((bih.biHeight - 1 - h) * bih.biWidth + w) * 3;

				//меняем RGB местами
				//переворачиваем BMP изображение по вертикали
				Res[Index2 + 0] = ResTemp[Index1 + 2];
				Res[Index2 + 1] = ResTemp[Index1 + 1];
				Res[Index2 + 2] = ResTemp[Index1 + 0];
			}
		}
	
	g_TexWidth = bih.biWidth;
	g_TexHeight = bih.biHeight;

	delete [] ResTemp;
}

void Create_BackBuffer()
{
	//размеры окна приложения (заднего буфера)
	GetClientRect(g_hWnd, &g_Rc);

	g_hDC = GetDC(g_hWnd);

	//кисть для очистки окна приложения (заднего буфера)
	g_hBrush = CreateSolidBrush(RGB(0,0,0));

	//наш задний буфер
	g_hBackBuffer = CreateCompatibleDC(g_hDC);
	g_hBitmap = CreateCompatibleBitmap(g_hDC, g_Rc.right, g_Rc.bottom);
	g_hOldBitmap = (HBITMAP) SelectObject(g_hBackBuffer, g_hBitmap);
}

void Clear_BackBuffer()
{
	//очистка окна приложения
	FillRect(g_hBackBuffer, &g_Rc, g_hBrush);
}

void Present_BackBuffer()
{
	//выводим задний буфер на экран приложения
	BitBlt(g_hDC, 0, 0, g_Rc.right, g_Rc.bottom, g_hBackBuffer, 0, 0, SRCCOPY);
}

void Delete_BackBuffer()
{
	//уничтожение заднего буфера

	SelectObject(g_hBackBuffer, g_hOldBitmap);
	DeleteObject(g_hBitmap);

	DeleteObject(g_hBrush);

	ReleaseDC(g_hWnd, g_hDC);
	DeleteDC(g_hBackBuffer);
}

void Draw_Textured_Poly(int y1, int y2)
{
	for ( int y = y1; y < y2; y++ )
	{
		float itu, itv;
		float dtu, dtv;

		//смотри ВАРИАНТ 2
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//ВАРИАНТ 1
		itu = g_tul;
		itv = g_tvl;
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!

		int dx = int(g_xr - g_xl);

		if(dx > 0)
		{
			dtu = (g_tur - g_tul) / (g_xr - g_xl);
			dtv = (g_tvr - g_tvl) / (g_xr - g_xl);
		}
		else
		{
			dtu = 0.0f;
			dtv = 0.0f;
		}

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//ВАРИАНТ 2
		/*
		int xln = (int)g_xl;

		float dxt = 1 - (g_xl - xln);

		//iz = zl + dxt * dz;
		itu = g_tul + dxt * dtu;
		itv = g_tvl + dxt * dtv;
		*/
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!

		
		for ( int x = (int)g_xl; x < (int)g_xr; x++ )
		{
			float tu = itu;
			float tv = itv;

			int t = (int)tu + (int)tv * g_TexWidth;

			BYTE r = Res[t * 3 + 0];
			BYTE g = Res[t * 3 + 1];
			BYTE b = Res[t * 3 + 2];

			SetPixel(g_hBackBuffer,x, y, RGB(r, g, b));

			itu += dtu;
			itv += dtv;
		}

		g_xl += g_dxdyl;
		g_tul += g_dtudyl;
		g_tvl += g_dtvdyl;

		g_xr += g_dxdyr;
		g_tur += g_dtudyr;
		g_tvr += g_dtvdyr;
	}
}

void Draw_Textured_Triangle_Affine(vector3 v1, tex_coord t1,
							vector3 v2, tex_coord t2,
							vector3 v3, tex_coord t3)
{
	int Side = 0;
	float TempFloat;
	int TempInt;

	float x1 = v1.x;
	float y1 = v1.y;
	float tu1 = t1.tu * (g_TexWidth - 1);
	float tv1 = t1.tv * (g_TexHeight - 1);

	float x2 = v2.x;
	float y2 = v2.y;
	float tu2 = t2.tu * (g_TexWidth - 1);
	float tv2 = t2.tv * (g_TexHeight - 1);

	float x3 = v3.x;
	float y3 = v3.y;
	float tu3 = t3.tu * (g_TexWidth - 1);
	float tv3 = t3.tv * (g_TexHeight - 1);

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

		g_dxdyl = (x3 - x1) / (y3 - y1);
		g_dtudyl = (tu3 - tu1) / (y3 - y1);
		g_dtvdyl = (tv3 - tv1) / (y3 - y1);

		g_xl = x1;
		g_tul = tu1;
		g_tvl = tv1;
		/*
		if(y1i < y2i)
		{
			g_dxdyr = (x2 - x1) / (y2 - y1);
			g_dtudyr = (tu2 - tu1) / (y2 - y1);
			g_dtvdyr = (tv2 - tv1) / (y2 - y1);

			g_xr = x1;
			g_tur = tu1;
			g_tvr = tv1;


			Draw_Textured_Poly(y1i, y2i);
		}
		*/
		if(y2i < y3i)
		{
			g_dxdyr = (x3 - x2) / (y3 - y2);
			g_dtudyr = (tu3 - tu2) / (y3 - y2);
			g_dtvdyr = (tv3 - tv2) / (y3 - y2);

			g_xr = x2;
			g_tur = tu2;
			g_tvr = tv2;

			Draw_Textured_Poly(y2i, y3i);
		}
	}
	else
	{
		g_dxdyr = (x3 - x1) / (y3 - y1);
		g_dtudyr = (tu3 - tu1) / (y3 - y1);
		g_dtvdyr = (tv3 - tv1) / (y3 - y1);

		g_xr = x1;
		g_tur = tu1;
		g_tvr = tv1;

		if(y1i < y2i)
		{
			g_dxdyl = (x2 - x1) / (y2 - y1);
			g_dtudyl = (tu2 - tu1) / (y2 - y1);
			g_dtvdyl = (tv2 - tv1) / (y2 - y1);

			g_xl = x1;
			g_tul = tu1;
			g_tvl = tv1;

			Draw_Textured_Poly(y1i, y2i);
		}
		/*
		if(y2i < y3i)
		{
			g_dxdyl = (x3 - x2) / (y3 - y2);
			g_dtudyl = (tu3 - tu2) / (y3 - y2);
			g_dtvdyl = (tv3 - tv2) / (y3 - y2);
			
			g_xl = x2;
			g_tul = tu2;
			g_tvl = tv2;


			Draw_Textured_Poly(y2i, y3i);
		}
		*/
	}
}

void Print_Text(char * Text, float x, float y, float SizeX, float SizeY)
{
	
	UINT Length = (UINT)strlen(Text);

	//каждая буква два треугольника 6 вершин
	UINT BuffSize = Length * 6;

	vector3 * VertBuff = new vector3[BuffSize];
	tex_coord * TexBuff = new tex_coord[BuffSize];

	UINT Index = 0;

	for ( unsigned int i=0 ; i<Length ; i++ )
	{
		vector3 VertexUpLeft    = { x+i*SizeX,			y,			0};
		vector3 VertexUpRight   = { x+i*SizeX+SizeX,	y,			0};
		vector3 VertexDownRight = { x+i*SizeX+SizeX,	y+SizeY,	0};
		vector3 VertexDownLeft  = { x+i*SizeX,			y+SizeY,	0};

		VertBuff[Index + 0] = VertexDownLeft;
		VertBuff[Index + 1] = VertexUpLeft;
		VertBuff[Index + 2] = VertexUpRight;

		VertBuff[Index + 3] = VertexDownLeft;
		VertBuff[Index + 4] = VertexUpRight;
		VertBuff[Index + 5] = VertexDownRight;
		
		char character = Text[i];
		float uv_x = (character%16)/16.0f;
		float uv_y = (character/16)/16.0f;

		//отступ перед буквой в клетке 3 пикселя
		float Tw1 = 3.0f / g_TexWidth;
		//ширина буквы 9 пикселей
		float Tw2 = 9.0f / g_TexWidth;

		//ширина клетки 16 пикселей

		/*
		//код без учета начала и ширины символа
		tex_coord UvUpLeft  = { uv_x           , uv_y };
		tex_coord UvUpRight = { uv_x+1.0f/16.0f, uv_y };
		tex_coord UvDownRight = { uv_x+1.0f/16.0f, (uv_y + 1.0f/16.0f) };
		tex_coord UvDownLeft  = { uv_x           , (uv_y + 1.0f/16.0f) };
		*/

		//код учитывает начало и ширину символа
		tex_coord UvUpLeft    =	{ uv_x + Tw1,		uv_y };
		tex_coord UvUpRight	  =	{ uv_x + Tw1 + Tw2,	uv_y };
		tex_coord UvDownRight =	{ uv_x + Tw1 + Tw2,	uv_y + 1.0f/16.0f};
		tex_coord UvDownLeft  =	{ uv_x + Tw1,		uv_y + 1.0f/16.0f };

		TexBuff[Index + 0] = UvDownLeft;
		TexBuff[Index + 1] = UvUpLeft;
		TexBuff[Index + 2] = UvUpRight;

		TexBuff[Index + 3] = UvDownLeft;
		TexBuff[Index + 4] = UvUpRight;
		TexBuff[Index + 5] = UvDownRight;

		Index += 6;

	}

	//каждая буква 2 треугольника
	UINT TriangleCount = Length * 2;

	for ( UINT i = 0; i < TriangleCount; i++ )
	{
		vector3 v1 = VertBuff[i * 3 + 0];
		vector3 v2 = VertBuff[i * 3 + 1];
		vector3 v3 = VertBuff[i * 3 + 2];

		tex_coord t1 = TexBuff[i * 3 + 0];
		tex_coord t2 = TexBuff[i * 3 + 1];
		tex_coord t3 = TexBuff[i * 3 + 2];

		Draw_Textured_Triangle_Affine(v1, t1, v2, t2, v3, t3);
	}

	delete [] VertBuff;
	delete [] TexBuff;
}

void Draw_Cube()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	static float Angle = 0.0;

	matrix4x4 MatRotateY = {
		cosf(Angle),	0.0,	-sinf(Angle),	0.0,
		0.0,			1.0,	0.0,			0.0,
		sinf(Angle),	0.0,	cosf(Angle),	0.0,
		0.0,			0.0,	0.0,			1.0 };

	Angle = Angle + PI / 100.0f;
	if(Angle > PI2)
		Angle = 0.0f;
	
	for ( int i = 0; i < 8; i++ )
	{
		vector3 Vec = Vec3_Mat4x4_Mul(g_VertBuff[i], MatRotateY);

		Vec.z = Vec.z + 15.0f;
		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;
		Vec.x = Vec.x / ((float)Rc.right / Rc.bottom);

		Vec.x = Vec.x * Rc.right / 2.0f + Rc.right / 2.0f;
		Vec.y =-Vec.y * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

		g_VertBuffTransformed[i] = Vec;
	}

	HPEN hPen = CreatePen(PS_SOLID, 4, RGB( 255, 255, 127 ) );
	HPEN hOldPen = (HPEN) SelectObject(g_hBackBuffer, hPen);

	Clear_BackBuffer();

	for ( int i = 0; i < 12; i++ )
	{
		vector3 Vec1 = g_VertBuffTransformed[ g_IndexBuff[ i * 2 + 0 ] ];
		vector3 Vec2 = g_VertBuffTransformed[ g_IndexBuff[ i * 2 + 1 ] ];

		MoveToEx(g_hBackBuffer, (int)Vec1.x, (int)Vec1.y, NULL);
		LineTo(g_hBackBuffer, (int)Vec2.x, (int)Vec2.y);
	}

	//limit 30 frames per second
	Limit_FPS();
	float FloatFPS = Calculate_FPS();

	char SzBuffer[32];
	sprintf_s(SzBuffer, 32, "FPS:%05.02f", FloatFPS);

	Print_Text(SzBuffer, 10.0f, 10.0f, 12.0f, 20.0f);

	Present_BackBuffer();

	SelectObject(g_hBackBuffer, hOldPen);
	DeleteObject(hPen);
}

int main ()
{
	g_hWnd = GetConsoleWindow();

	Init_Limit_FPS(30.0f);

	Load_BMP24();

	Create_BackBuffer();

	while ( !_kbhit() )
	{
		Draw_Cube();
	}

	Delete_BackBuffer();

	delete [] Res;

	return 0;
}


