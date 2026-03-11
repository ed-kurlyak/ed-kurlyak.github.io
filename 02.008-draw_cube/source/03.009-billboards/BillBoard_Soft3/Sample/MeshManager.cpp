//======================================================================================
//	Ed Kurlyak 2025 Software Rendering
//======================================================================================

#include "MeshManager.h"

XBUF_PERSP_FP 	XBuffer[MAX_SCANLINES];

int	*sort3dptr;
int *info3dptr;

int sort3d_buffer[MAX_POLYGONS][2];
int info3d_buffer[MAX_POLYGONS*30];

int SurfaceNum;

unsigned char **g_pLevelTile24;
unsigned char **g_pLevelTile32;

UCHAR * g_Res;

tex_info * Tex;

ULONG g_ViewWidth;
ULONG g_ViewHeight;
	
HWND g_hWnd;

vector3 g_VecCamRight;
vector3 g_VecCamUp;
vector3 g_VecCamPos;
vector3 g_VecCamLook;

float **g_ZBuff;
	
//полигоны кирпичного пола
list g_PolygonsFloor;

//полигоны дерева (билбоард)
list g_PolygonsBillboard;

UINT g_TextureWidth;
UINT g_TextureHeight;

//Добавим задний буфер
int g_BackLpitch;
LPBYTE g_Data;
BITMAPINFOHEADER g_Bih;
HDRAWDIB g_hDD;
LPBYTE g_DataTemp;

//timer
float g_ElapsedTime;
float g_TimeScale;
__int64 g_StartTime;
__int64 g_LastTime;
__int64 g_PerfFreq;

//ближняя плоскость отсечения
float g_ZNear;

float g_x1, g_x2, g_x3;
float g_y1, g_y2, g_y3;
float g_iz1, gu_iz1, gv_iz1, g_iz2, gu_iz2, gv_iz2, g_iz3, gu_iz3, gv_iz3;

void (*Poly_Draw_Routines[])(int* buff) = {
		Draw_Perspective_Opaque_Tri,
		Draw_Perspective_Transparent_Tri };

void Destroy_MeshManager()
{
	Delete_Backbuffer();

	delete [] g_pLevelTile24[0];
	g_pLevelTile24[0] = NULL;

	delete [] g_pLevelTile24;
	g_pLevelTile24 = NULL;

	delete [] g_pLevelTile32[0];
	g_pLevelTile32[0] = NULL;

	delete [] g_pLevelTile32;
	g_pLevelTile32 = NULL;

	delete [] Tex;
}

float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2)
{
	return VecIn1.x*VecIn2.x + VecIn1.y*VecIn2.y + VecIn1.z* VecIn2.z;
}

vector3 Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut;

	VecOut.x = VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y;
	VecOut.y = VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z;
	VecOut.z = VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x;

	return VecOut;
}

vector3 Vec3_Normalize(vector3 &VecIn)
{
	vector3 VecOut;

	float Len = sqrtf(VecIn.x * VecIn.x + VecIn.y * VecIn.y + VecIn.z * VecIn.z);

	VecOut.x = VecIn.x / Len;
	VecOut.y = VecIn.y / Len;
	VecOut.z = VecIn.z / Len;
	
	return VecOut;

}

vector3 Vec3_Scale(vector3 &VecIn, float ValIn)
{
	vector3 VecOut;

	VecOut.x = VecIn.x * ValIn;
	VecOut.y = VecIn.y * ValIn;
	VecOut.z = VecIn.z * ValIn;

	return VecOut;
}

vector3 Vec3_Add(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut;

	VecOut.x = VecIn1.x + VecIn2.x;
	VecOut.y = VecIn1.y + VecIn2.y;
	VecOut.z = VecIn1.z + VecIn2.z;

	return VecOut;
}

vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 &MatIn)
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

	VecOut.tu = VecIn.tu;
	VecOut.tv = VecIn.tv;

	return VecOut;
}

void Matrix_Rotation_Axis(vector3 &VecIn, float Angle, matrix4x4 &MatRotate)
{
	float x = VecIn.x;
	float y = VecIn.y;
	float z = VecIn.z;
	
	float s = sin(Angle);
	float c = cos(Angle);
	float omc = 1.0f - c;

	float xomc = x * omc;
	float yomc = y * omc;
	float zomc = z * omc;

	float xxomc = x * xomc;
	float xyomc = x * yomc;
	float xzomc = x * zomc;
		
	float yyomc = y * yomc;
	float yzomc = y * zomc;
	float zzomc = z * zomc;

	float xs = x * s;
	float ys = y * s;
	float zs = z * s;

	MatRotate = matrix4x4 (xxomc + c,  xyomc + zs, xzomc - ys, 0.0f,
		xyomc - zs, yyomc + c,  yzomc + xs, 0.0f,
		xzomc + ys, yzomc - xs, zzomc + c, 0.0f,
		0.0f ,0.0f, 0.0f, 1.0f);
}

bool list::Is_Empty_List ()
{
		if(PolygonCount == 0)
			return true;

		return false;
}

void list::Add_To_List(polygon *p)
{
	if ( PolygonCount == 0)
	{	
		PolygonCount++;
		PolyList=(polygon*)malloc(PolygonCount*sizeof(polygon));
	}
	else
	{
		PolygonCount++;
		PolyList=(polygon*)realloc(PolyList, PolygonCount*sizeof(polygon));
	}

	for(int i=0;i<3;i++)
		PolyList[PolygonCount-1].Vertex[i] = p->Vertex[i];

	PolyList[PolygonCount-1].TexID = p->TexID;
}

polygon* list::Get_Frog_List ()
{
	polygon *plg;
	plg = &PolyList[PolygonCurr];
	PolygonCurr++;
	if(PolygonCurr > PolygonCount)
		return NULL;

	return plg;
}

void Draw_Textured_Triangle_Opaque(vector3 VecIn1, vector3 VecIn2, vector3 VecIn3)
{
	int y1i, y3i;

	int Side = Compute_Side(VecIn1, VecIn2, VecIn3, y1i, y3i);

	if(Generate_Slopes(Side))
		Draw_Textured_Poly_Opaque(y1i, y3i);
}

int Compute_Side(vector3 VecIn1, vector3 VecIn2, vector3 VecIn3, int & y1i, int & y3i)
{
	int Side;
	float FloatTemp;
	int IntTemp;

	g_x1 = VecIn1.x;
	g_y1 = VecIn1.y;
	g_x2 = VecIn2.x;
	g_y2 = VecIn2.y;
	g_x3 = VecIn3.x;
	g_y3 = VecIn3.y;
	
	g_iz1 = VecIn1.z;
	gu_iz1 = VecIn1.tu;
	gv_iz1 = VecIn1.tv;

	g_iz2 = VecIn2.z;
	gu_iz2 = VecIn2.tu;
	gv_iz2 = VecIn2.tv;
	
	g_iz3 = VecIn3.z;
	gu_iz3 = VecIn3.tu;
	gv_iz3 = VecIn3.tv;


	//координаты на экране у нас целые числа X и Y
	//поэтому некоторые операции необходимо
	//делать целочисленные (float труднее для
	//сравнения, например)

	y1i = (int) floor (g_y1);
	int y2i = (int) floor (g_y2);
	y3i = (int) floor (g_y3);

	if (y1i > y2i)
	{
		SWAP(g_x1, g_x2, FloatTemp);
		SWAP(g_y1, g_y2, FloatTemp);
		SWAP(g_iz1, g_iz2, FloatTemp);
		SWAP(gu_iz1, gu_iz2, FloatTemp);
		SWAP(gv_iz1, gv_iz2, FloatTemp);
		SWAP(y1i, y2i, IntTemp);
	}
	if (y1i > y3i)
	{
		SWAP(g_x1, g_x3, FloatTemp);
		SWAP(g_y1, g_y3, FloatTemp);
		SWAP(g_iz1, g_iz3, FloatTemp);
		SWAP(gu_iz1, gu_iz3, FloatTemp);
		SWAP(gv_iz1, gv_iz3, FloatTemp);
		SWAP(y1i, y3i, IntTemp);
	}
	if (y2i > y3i)
	{
		SWAP(g_x2, g_x3, FloatTemp);
		SWAP(g_y2, g_y3, FloatTemp);
		SWAP(g_iz2, g_iz3, FloatTemp);
		SWAP(gu_iz2, gu_iz3, FloatTemp);
		SWAP(gv_iz2, gv_iz3, FloatTemp);
		SWAP(y2i, y3i, IntTemp);
	}
	
	if(y2i > y1i && y3i > y2i)
	{
		//вычисляем наклон левой и правой стороны
		//треугольника, по знаку величины наклона
		//(+ или -) определяем какая сторона
		//(левая или правая) имеет большее значение по Y
		float dxdy1 = (g_x2 - g_x1) / (g_y2 - g_y1);
		float dxdy2 = (g_x3 - g_x1) / (g_y3 - g_y1);
		Side= dxdy2 > dxdy1;
	}

	if (y1i == y2i)
		Side = g_x1 > g_x2;
	if (y2i == y3i)
		Side = g_x3 > g_x2;

	return Side;
}

int Generate_Slopes(int Side)
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
		xl = g_x1;
		ul = gu_iz1;
		vl = gv_iz1;
		zl = g_iz1;

		dxdyl = (g_x3 - g_x1) / (g_y3 - g_y1);
		dudyl = (gu_iz3 - gu_iz1) / (g_y3 - g_y1);
		dvdyl = (gv_iz3 - gv_iz1) / (g_y3 - g_y1);
		dzdyl = (g_iz3 - g_iz1) / (g_y3 - g_y1);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)g_y1;

		for ( int y = (int)g_y1; y < (int)g_y3; y++, XPtr++ )
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

		xr = g_x1;
		ur = gu_iz1;
		vr = gv_iz1;
		zr = g_iz1;

		dxdyr = (g_x2 - g_x1) / (g_y2 - g_y1);
		dudyr = (gu_iz2 - gu_iz1) / (g_y2 - g_y1);
		dvdyr = (gv_iz2 - gv_iz1) / (g_y2 - g_y1);
		dzdyr = (g_iz2 - g_iz1) / (g_y2 - g_y1);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)g_y1;

		for ( int y = (int)g_y1; y < (int)g_y2; y++, XPtr++ )
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

		xr = g_x2;
		ur = gu_iz2;
		vr = gv_iz2;
		zr = g_iz2;

		dxdyr = (g_x3 - g_x2) / (g_y3 - g_y2);
		dudyr = (gu_iz3 - gu_iz2) / (g_y3 - g_y2);
		dvdyr = (gv_iz3 - gv_iz2) / (g_y3 - g_y2);
		dzdyr = (g_iz3 - g_iz2) / (g_y3 - g_y2);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)g_y2;

		for ( int y = (int)g_y2; y < (int)g_y3; y++, XPtr++)
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
		xr = g_x1;
		ur = gu_iz1;
		vr = gv_iz1;
		zr = g_iz1;
		
		dxdyr = (g_x3 - g_x1) / (g_y3 - g_y1);
		dudyr = (gu_iz3 - gu_iz1) / (g_y3 - g_y1);
		dvdyr = (gv_iz3 - gv_iz1) / (g_y3 - g_y1);
		dzdyr = (g_iz3 - g_iz1) / (g_y3 - g_y1);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)g_y1;

		for ( int y = (int)g_y1; y < (int)g_y3; y++, XPtr++ )
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

		xl = g_x1;
		ul = gu_iz1;
		vl = gv_iz1;
		zl = g_iz1;

		dxdyl = (g_x2 - g_x1) / (g_y2 - g_y1);
		dudyl = (gu_iz2 - gu_iz1) / (g_y2 - g_y1);
		dvdyl = (gv_iz2 - gv_iz1) / (g_y2 - g_y1);
		dzdyl = (g_iz2 - g_iz1) / (g_y2 - g_y1);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)g_y1;

		for ( int y = (int)g_y1; y < (int)g_y2; y++, XPtr++ )
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

		xl = g_x2;
		ul = gu_iz2;
		vl = gv_iz2;
		zl = g_iz2;

		dxdyl = (g_x3 - g_x2) / (g_y3 - g_y2);
		dudyl = (gu_iz3 - gu_iz2) / (g_y3 - g_y2);
		dvdyl = (gv_iz3 - gv_iz2) / (g_y3 - g_y2);
		dzdyl = (g_iz3 - g_iz2) / (g_y3 - g_y2);

		XPtr = ((XBUF_PERSP_FP *)XBuffer) + (int)g_y2;

		for ( int y = (int)g_y2; y < (int)g_y3; y++, XPtr++)
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


void Draw_Perspective_Opaque_Tri(int * ptr)
{

	g_Res  = (UCHAR *) g_pLevelTile24[*ptr];	
	
	g_TextureWidth = Tex[0].TextureWidth;
	g_TextureHeight = Tex[0].TextureHeight;

	ptr++;

	vector3 * Vec1 = (vector3 *) ptr;
	ptr += 5;

	vector3 * Vec2 = (vector3 *) ptr;
	ptr += 5;

	vector3 * Vec3 = (vector3 *) ptr;
	ptr += 5;

	Draw_Textured_Triangle_Opaque(*Vec1, *Vec2, *Vec3);

}

void Draw_Perspective_Transparent_Tri(int* ptr)
{
	g_Res  = (UCHAR *) g_pLevelTile32[*ptr];	
	
	g_TextureWidth = Tex[1].TextureWidth;
	g_TextureHeight = Tex[1].TextureHeight;

	ptr++;

	vector3 * Vec1 = (vector3 *) ptr;
	ptr += 5;

	vector3 * Vec2 = (vector3 *) ptr;
	ptr += 5;

	vector3 * Vec3 = (vector3 *) ptr;
	ptr += 5;

	Draw_Textured_Triangle_Transparent(*Vec1, *Vec2, *Vec3);
}

void Init_MeshManager(HWND hwnd)
{
	ShowCursor(FALSE);

	g_hWnd = hwnd;

	Timer_Start();
    
	Create_Backbuffer();

	//массивы для хранения двух текстур
	//1 текстура кипричного пола
	g_pLevelTile24 = new unsigned char *[1];
	//1 текстура дерева 
	g_pLevelTile32 = new unsigned char *[1];

	//структура для хранения высоты и ширины
	//каждой текстуры
	Tex = new tex_info[2];

	//загружаем текстуру 24 бита BMP
	Load_BMP24((char*)"texture1.bmp", 0);
	//загружаем текстуру 32 бита BMP с альфа каналом
	//текстура дерева
	Load_BMP32((char*)"tree.bmp", 0);

	polygon Poly;

	//-----------
	//создаем пол для сцены

	//первый треугольник пола
	//левый нижний угол
	Poly.Vertex[0].x = -50.0f;
	Poly.Vertex[0].y = 0.0f;
	Poly.Vertex[0].z = -50.0f;
	Poly.Vertex[0].tu = 0.0f;
	Poly.Vertex[0].tv = (float)Tex[0].TextureHeight * 5 - 1;

	//левый верхний угол
	Poly.Vertex[1].x = -50.0f;
	Poly.Vertex[1].y = 0.0f;
	Poly.Vertex[1].z = 50.0f;
	Poly.Vertex[1].tu = 0.0f;
	Poly.Vertex[1].tv = 0.0f;

	//правый верхний угол
	Poly.Vertex[2].x = 50.0f;
	Poly.Vertex[2].y = 0.0f;
	Poly.Vertex[2].z = 50.0f;
	Poly.Vertex[2].tu = (float)Tex[0].TextureWidth * 5 - 1;
	Poly.Vertex[2].tv = 0.0f;
	
	Poly.TexID = 0;

	g_PolygonsFloor.Add_To_List(&Poly);

	//второй треугольник пола
	//левый нижний угол
	Poly.Vertex[0].x = -50.0f;
	Poly.Vertex[0].y = 0.0f;
	Poly.Vertex[0].z = -50.0f;
	Poly.Vertex[0].tu = 0.0f;
	Poly.Vertex[0].tv = (float)Tex[0].TextureHeight * 5 - 1;

	//правый верхний угол
	Poly.Vertex[1].x = 50.0f;
	Poly.Vertex[1].y = 0.0f;
	Poly.Vertex[1].z = 50.0f;
	Poly.Vertex[1].tu = (float)Tex[0].TextureWidth * 5 - 1;
	Poly.Vertex[1].tv = 0.0f;

	//правый нижний угол
	Poly.Vertex[2].x = 50.0f;
	Poly.Vertex[2].y = 0.0f;
	Poly.Vertex[2].z = -50.0f;
	Poly.Vertex[2].tu = (float)Tex[0].TextureWidth * 5 - 1;
	Poly.Vertex[2].tv = (float)Tex[0].TextureHeight * 5 - 1;
	
	Poly.TexID = 0;

	g_PolygonsFloor.Add_To_List(&Poly);

	//-------------
	//создаем billboard

	//первый треугольник billboard
	//левый нижний угол
	Poly.Vertex[0].x = -25.0f;
	Poly.Vertex[0].y = 0.0f;
	Poly.Vertex[0].z = 0.0f;
	Poly.Vertex[0].tu = 0.0f;
	Poly.Vertex[0].tv = (float)Tex[1].TextureHeight - 1;

	//левый верхний угол
	Poly.Vertex[1].x = -25.0f;
	Poly.Vertex[1].y = 50.0f;
	Poly.Vertex[1].z = 0.0f;
	Poly.Vertex[1].tu = 0.0f;
	Poly.Vertex[1].tv = 0.0f;

	//правый верхний угол
	Poly.Vertex[2].x = 25.0f;
	Poly.Vertex[2].y = 50.0f;
	Poly.Vertex[2].z = 0.0f;
	Poly.Vertex[2].tu = (float)Tex[1].TextureWidth - 1;
	Poly.Vertex[2].tv = 0.0f;
	
	Poly.TexID = 0;

	g_PolygonsBillboard.Add_To_List(&Poly);

	//второй треугольник billboard
	//левый нижний угол
	Poly.Vertex[0].x = -25.0f;
	Poly.Vertex[0].y = 0.0f;
	Poly.Vertex[0].z = 0.0f;
	Poly.Vertex[0].tu = 0.0f;
	Poly.Vertex[0].tv = (float)Tex[1].TextureHeight - 1;

	//правый верхний угол
	Poly.Vertex[1].x = 25.0f;
	Poly.Vertex[1].y = 50.0f;
	Poly.Vertex[1].z = 0.0f;
	Poly.Vertex[1].tu = (float)Tex[1].TextureWidth - 1;
	Poly.Vertex[1].tv = 0.0f;

	//правый нижний угол
	Poly.Vertex[2].x = 25.0f;
	Poly.Vertex[2].y = 0.0f;
	Poly.Vertex[2].z = 0.0f;
	Poly.Vertex[2].tu = (float)Tex[1].TextureWidth - 1;
	Poly.Vertex[2].tv = (float)Tex[1].TextureHeight - 1;
	
	Poly.TexID = 0;

	g_PolygonsBillboard.Add_To_List(&Poly);

	//--------------

	//инициализируем векторы камеры
	g_VecCamUp = vector3(0.0, 1.0, 0.0);
	g_VecCamLook = vector3(0.0, 0.0, 1.0);

	//камера может смотреть в направлении +Z и -Z
	if(g_VecCamLook.z > 0)
	{
		g_VecCamRight = vector3(1.0, 0.0, 0.0);
	}

	if(g_VecCamLook.z < 0)
	{
		g_VecCamRight = vector3(-1.0, 0.0, 0.0);
	}

	g_VecCamPos = vector3(0.0f, 5.0f, -60.0f);

	//создаем Z буфер - массив размером окна приложения
	//будет содержать глубину каждого пикселя на экране
	/*
	g_ZBuff = new float*[g_ViewHeight];

	for (UINT i = 0; i < g_ViewHeight; i++)
	{
		g_ZBuff[i] = new float[g_ViewWidth];
	}
	*/
}

void Get_View_Matrix(matrix4x4 &MatView)
{
	float Time = Get_Elapsed_Time();

	UINT ScreenWidth = g_ViewWidth;
	UINT ScreenHeight = g_ViewHeight;

	POINT MousePos;
	GetCursorPos(&MousePos);

	SetCursorPos(ScreenWidth/2, ScreenHeight/2);
    
	int DeltaX=ScreenWidth/2-MousePos.x;
	int DeltaY=ScreenHeight/2-MousePos.y;

	//что бы перемещать камеру вертикально
	//поставте значение g_RotationScalerY
	//больше нуля
	//float g_RotationScalerY = 0.1f;
	float g_RotationScalerY = 0.0f;
	float g_RotationScalerX = 0.075f;

	//реакция на движение мышью

	//вертикальное смещение камеры (вокруг оси X)
	if(DeltaY<0) g_RotationScalerY = g_RotationScalerY;
	else if(DeltaY>0) g_RotationScalerY = -g_RotationScalerY;
	else if(DeltaY==0) g_RotationScalerY = 0;

	matrix4x4 MatRotRight;
	Matrix_Rotation_Axis(g_VecCamRight , g_RotationScalerY, MatRotRight);
	
	g_VecCamRight = Vec3_Mat4x4_Mul(g_VecCamRight, MatRotRight);
	g_VecCamUp = Vec3_Mat4x4_Mul(g_VecCamUp, MatRotRight);
	g_VecCamLook = Vec3_Mat4x4_Mul(g_VecCamLook, MatRotRight);
	
	//горизонтальное смещение камеры (вокруг оси Y)
	if(DeltaX<0) g_RotationScalerX = g_RotationScalerX;
	else if(DeltaX>0) g_RotationScalerX = -g_RotationScalerX;
	else if(DeltaX==0) g_RotationScalerX = 0;

	vector3 VecUpTemp = vector3( 0.0f, 1.0f, 0.0f );
	matrix4x4 MatRotUp;
	Matrix_Rotation_Axis(VecUpTemp , g_RotationScalerX, MatRotUp);
	
	g_VecCamRight = Vec3_Mat4x4_Mul(g_VecCamRight, MatRotUp);
	g_VecCamUp = Vec3_Mat4x4_Mul(g_VecCamUp, MatRotUp);
	g_VecCamLook = Vec3_Mat4x4_Mul(g_VecCamLook, MatRotUp);
	
	//реакция на клавиши W,S,A,D
	float RatioMove = 50;
	vector3 VecAccel = vector3(0,0,0);
	
	if(GetAsyncKeyState('W')& 0xFF00) 
	{
		vector3 VecTemp = vector3(g_VecCamLook.x,0,g_VecCamLook.z);

		VecAccel += Vec3_Scale(VecTemp, Time * RatioMove);
	}

	if(GetAsyncKeyState('S')& 0xFF00) 
	{
		vector3 VecTemp = vector3(g_VecCamLook.x,0,g_VecCamLook.z);

		VecAccel += Vec3_Scale(VecTemp, Time * -RatioMove);
	}

	if(GetAsyncKeyState('D')& 0xFF00) 
	{
		vector3 VecTemp = vector3(g_VecCamRight.x,0,g_VecCamRight.z);

		VecAccel += Vec3_Scale(VecTemp, Time * RatioMove);
	}

	if(GetAsyncKeyState('A')& 0xFF00) 
	{
		vector3 VecTemp = vector3(g_VecCamRight.x,0,g_VecCamRight.z);
		
		VecAccel += Vec3_Scale(VecTemp, Time * -RatioMove);
	}

	g_VecCamPos = Vec3_Add(g_VecCamPos,VecAccel);

	g_VecCamLook = Vec3_Normalize(g_VecCamLook);
	g_VecCamUp = Vec3_Cross(g_VecCamLook,g_VecCamRight);
	g_VecCamUp = Vec3_Normalize(g_VecCamUp);
	g_VecCamRight = Vec3_Cross(g_VecCamUp,g_VecCamLook);
	g_VecCamRight = Vec3_Normalize(g_VecCamRight);

	float xp = -Vec3_Dot(g_VecCamPos, g_VecCamRight);
	float yp = -Vec3_Dot(g_VecCamPos, g_VecCamUp);
	float zp = -Vec3_Dot(g_VecCamPos, g_VecCamLook);

	MatView = matrix4x4(
        g_VecCamRight.x, g_VecCamUp.x, g_VecCamLook.x, 0,
		g_VecCamRight.y, g_VecCamUp.y, g_VecCamLook.y, 0,
		g_VecCamRight.z, g_VecCamUp.z, g_VecCamLook.z, 0,
		xp,		  yp,	 zp,	  1 );
}

vector3 Calc_Edge(vector3 &VecIn1, vector3 &VecIn2)
{
	//вершина перед передней плоскостью v1
	//вершина за передней проскостью v2

	vector3 VecOut;

	float d = (g_ZNear - VecIn1.z) / (VecIn2.z - VecIn1.z);

    VecOut.x = VecIn1.x + d * (VecIn2.x - VecIn1.x);
    VecOut.y = VecIn1.y + d * (VecIn2.y - VecIn1.y);
    VecOut.z = VecIn1.z + d * (VecIn2.z - VecIn1.z);

	//VecOut.z = g_ZNear;

	VecOut.tu = VecIn1.tu + d * (VecIn2.tu - VecIn1.tu);
    VecOut.tv = VecIn1.tv + d * (VecIn2.tv - VecIn1.tv);

	return VecOut;
}

int Clip_Vertices_Screen(int Num, vector3 *Source)
{
	float MinClipX = 0;
	float MaxClipX = (float)g_ViewWidth - 1;

	float MinClipY = 0;
	float MaxClipY = (float)g_ViewHeight - 1;

    float Scale;
    vector3 Vertices[8];

	//последняя вершина
	vector3 *l = &Source[Num - 1];
    int j = 0;

    for (int i = 0; i < Num; i++)
	{
		//сюда записываем результат
        vector3 *v1 = &Vertices[j];
        //последняя вершина v2
		vector3 *v2 = l;
        //нулевая (первая) вершина l
		l = &Source[i];

        if (v2->x < MinClipX)
		{
            if (l->x < MinClipX)
			{
                continue;
            }
            Scale = (MinClipX - l->x) / (v2->x - l->x);
            v1->x = MinClipX;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }
		else if (v2->x > MaxClipX)
		{
            if (l->x > MaxClipX)
			{
                continue;
            }
            Scale = (MaxClipX - l->x) / (v2->x - l->x);
            v1->x = MaxClipX;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }

        if (l->x < MinClipX)
		{
            Scale = (MinClipX - l->x) / (v2->x - l->x);
            v1->x = MinClipX;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }
		else if (l->x > MaxClipX)
		{
            Scale = (MaxClipX - l->x) / (v2->x - l->x);
            v1->x = MaxClipX;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }
		else
		{
            v1->x = l->x;
            v1->y = l->y;
            v1->z = l->z;
            v1->tu = l->tu;
            v1->tv = l->tv;
			v1 = &Vertices[++j];
        }
    }

    if (j < 3)
	{
        return 0;
    }

    Num = j;
    l = &Vertices[j - 1];
    j = 0;

    for (int i = 0; i < Num; i++)
	{
        vector3 *v1 = &Source[j];
        vector3 *v2 = l;
        l = &Vertices[i];

        if (v2->y < MinClipY)
		{
            if (l->y < MinClipY)
			{
                continue;
            }
            Scale = (MinClipY - l->y) / (v2->y - l->y);
            v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = MinClipY;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Source[++j];
        }
		else if (v2->y > MaxClipY)
		{
            if (l->y > MaxClipY)
			{
                continue;
            }
            Scale = (MaxClipY - l->y) / (v2->y - l->y);
            v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = MaxClipY;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Source[++j];
        }

        if (l->y < MinClipY)
		{
            Scale = (MinClipY - l->y) / (v2->y - l->y);
            v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = MinClipY;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Source[++j];
        }
		else if (l->y > MaxClipY)
		{
            Scale = (MaxClipY - l->y) / (v2->y - l->y);
            v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = MaxClipY;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
            v1 = &Source[++j];
        }
		else
		{
            v1->x = l->x;
            v1->y = l->y;
            v1->z = l->z;
            v1->tu = l->tu;
            v1->tv = l->tv;
			v1 = &Source[++j];
        }
    }
	
    if (j < 3)
	{
        return 0;
    }

	return j;

}

int Clip_Vertices_ZNear(int Num, vector3 *Source)
{
    float Scale;
    vector3 Vertices[8];

	//последняя вершина
	vector3 *l = &Source[Num - 1];
    int j = 0;

    for (int i = 0; i < Num; i++)
	{
		//сюда записываем результат
        vector3 *v1 = &Vertices[j];
        //последняя вершина v2
		vector3 *v2 = l;
        //нулевая (первая) вершина l
		l = &Source[i];

        if (v2->z < g_ZNear)
		{
            if (l->z < g_ZNear)
			{
                continue;
            }
            Scale = (g_ZNear - l->z) / (v2->z - l->z);
			v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }

		if (l->z < g_ZNear)
		{
            Scale = (g_ZNear - l->z) / (v2->z - l->z);
            v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }
		else
		{
            v1->x = l->x;
            v1->y = l->y;
            v1->z = l->z;
            v1->tu = l->tu;
            v1->tv = l->tv;
			v1 = &Vertices[++j];
        }
    }

    if (j < 3)
	{
        return 0;
    }

	for ( int i = 0; i < j; i++ )
	{
		Source[i] = Vertices[i];
	}

	return j;
}

void Update_MeshManager()
{
	sort3dptr = (int*) sort3d_buffer;	
	info3dptr = (int*) info3d_buffer;    

	SurfaceNum = 0;

	matrix4x4 MatWorld = matrix4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

	matrix4x4 MatView;
    Get_View_Matrix(MatView);

	float Fov = PI / 2.0f; // FOV 90 degree
    float Aspect = (float) g_ViewWidth/g_ViewHeight;
	float ZFar = 100.0f;
	g_ZNear = 0.01f;

	float h, w, Q;

    w = (float)(1.0f/tanf(Fov*0.5f))/Aspect;  
    h = (float)1.0f/tanf(Fov*0.5f);   
    Q = ZFar/(ZFar - g_ZNear);

	/*
    matrix4x4 MatProj(
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, Q, 1,
        0, 0, -Q*g_ZNear, 0);

	*/

	//так как нам Z от 0 до 1 не надо
	//мы не будем заполнять этими значениями
	//буфер глубины, на W делить Z не будем,
	//то возьмем упрощенный
	//вариант матрицы проекции
    matrix4x4 MatProj(
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

	float Alpha = 0.5f * g_ViewWidth;
	float Beta  = 0.5f * g_ViewHeight;
	
	matrix4x4 MatScreen(
		Alpha,  0,	    0,    0, 
		0,      -Beta,  0,    0, 
		0,		0,		1,    0,
		Alpha,  Beta,	0,    1);
    
	for ( int j =0; j < g_PolygonsFloor.PolygonCount; j++)
	{
		vector3 Vec1 = g_PolygonsFloor.PolyList[j].Vertex[0];
		vector3 Vec2 = g_PolygonsFloor.PolyList[j].Vertex[1];
		vector3 Vec3 = g_PolygonsFloor.PolyList[j].Vertex[2];

		Vec1 = Vec3_Mat4x4_Mul(Vec1, MatWorld);
		Vec2 = Vec3_Mat4x4_Mul(Vec2, MatWorld);
		Vec3 = Vec3_Mat4x4_Mul(Vec3, MatWorld);
		
		Vec1 = Vec3_Mat4x4_Mul(Vec1, MatView);
		Vec2 = Vec3_Mat4x4_Mul(Vec2, MatView);
		Vec3 = Vec3_Mat4x4_Mul(Vec3, MatView);
		//---------------------------------
		//backface culling

		vector3 VecEdge1,VecEdge2,VecCross;

		VecEdge1 = Vec2 - Vec1;
		VecEdge2 = Vec3 - Vec1;

		VecEdge1 = Vec3_Normalize(VecEdge1);
		VecEdge2 = Vec3_Normalize(VecEdge2);

		VecCross = Vec3_Cross(VecEdge1, VecEdge2);

		VecCross = Vec3_Normalize(VecCross);

		float Dot = Vec3_Dot(VecCross, Vec1);
		
		if(Dot >= 0.0) //backface culling
			continue;

		//---------------------------------

		//начинаем отсечение по передней плоскости
		//отсечение производится в пространстве вида
		//то есть после умножения вершин на матрицу вида
		//в результате отсечения накапливаем новые
		//треугольники в FrontList
		
		vector3 ClippedVertices[8];

		memset(ClippedVertices, 0, sizeof(vector3) * 8);

		ClippedVertices[0].x = Vec1.x;
		ClippedVertices[0].y = Vec1.y;
		ClippedVertices[0].z = Vec1.z;
		ClippedVertices[0].tu = Vec1.tu;
		ClippedVertices[0].tv = Vec1.tv;

		ClippedVertices[1].x = Vec2.x;
		ClippedVertices[1].y = Vec2.y;
		ClippedVertices[1].z = Vec2.z;
		ClippedVertices[1].tu = Vec2.tu;
		ClippedVertices[1].tv = Vec2.tv;

		ClippedVertices[2].x = Vec3.x;
		ClippedVertices[2].y = Vec3.y;
		ClippedVertices[2].z = Vec3.z;
		ClippedVertices[2].tu = Vec3.tu;
		ClippedVertices[2].tv = Vec3.tv;

		int Verts = Clip_Vertices_ZNear(3, &ClippedVertices[0]);

		list FrontList;

		if (Verts == 3 )
		{
			polygon Poly;

			Poly.Vertex[0] = ClippedVertices[0];
			Poly.Vertex[1] = ClippedVertices[1];
			Poly.Vertex[2] = ClippedVertices[2];

			Poly.TexID = g_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);
		}
		else if (Verts == 4 )
		{
			polygon Poly1;

			Poly1.Vertex[0] = ClippedVertices[0];
			Poly1.Vertex[1] = ClippedVertices[1];
			Poly1.Vertex[2] = ClippedVertices[2];

			Poly1.TexID = g_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly1);
			
			polygon Poly2;

			Poly2.Vertex[0] = ClippedVertices[0];
			Poly2.Vertex[1] = ClippedVertices[2];
			Poly2.Vertex[2] = ClippedVertices[3];

			Poly2.TexID = g_PolygonsFloor.PolyList[j].TexID; 

			//вот этот треугольник не выводиться на экран
			FrontList.Add_To_List(&Poly2);
			
		}

		//отсечение в пространстве вида сделано
		//можно дальше трансформировать вершины
		//умножаем вершины на матрицу проекции
		//и далее деление на z

		for ( int i = 0; i < FrontList.PolygonCount; i++)
		{
			vector3 Vec1, Vec2, Vec3;

			Vec1 = FrontList.PolyList[i].Vertex[0];
			Vec2 = FrontList.PolyList[i].Vertex[1];
			Vec3 = FrontList.PolyList[i].Vertex[2];
			
			Vec1 = Vec3_Mat4x4_Mul(Vec1, MatProj);
			Vec1.x = Vec1.x / Vec1.z;
			Vec1.y = Vec1.y / Vec1.z;

			Vec2 = Vec3_Mat4x4_Mul(Vec2, MatProj);
			Vec2.x = Vec2.x / Vec2.z;
			Vec2.y = Vec2.y / Vec2.z;

			Vec3 = Vec3_Mat4x4_Mul(Vec3, MatProj);
			Vec3.x = Vec3.x / Vec3.z;
			Vec3.y = Vec3.y / Vec3.z;

			Vec1 = Vec3_Mat4x4_Mul(Vec1, MatScreen);
			Vec2 = Vec3_Mat4x4_Mul(Vec2, MatScreen);
			Vec3 = Vec3_Mat4x4_Mul(Vec3, MatScreen);

			vector3 Vertex[8];

			memset(Vertex, 0, sizeof(vector3) * 8);

			Vertex[0].x = Vec1.x;
			Vertex[0].y = Vec1.y;
			Vertex[0].z = 1.0f / Vec1.z;
			Vertex[0].tu = Vec1.tu * Vertex[0].z;
			Vertex[0].tv = Vec1.tv * Vertex[0].z;

			Vertex[1].x = Vec2.x;
			Vertex[1].y = Vec2.y;
			Vertex[1].z = 1.0f / Vec2.z;
			Vertex[1].tu = Vec2.tu * Vertex[1].z;
			Vertex[1].tv = Vec2.tv * Vertex[1].z;

			Vertex[2].x = Vec3.x;
			Vertex[2].y = Vec3.y;
			Vertex[2].z = 1.0f / Vec3.z;
			Vertex[2].tu = Vec3.tu * Vertex[2].z;
			Vertex[2].tv = Vec3.tv * Vertex[2].z;

			int Verts = Clip_Vertices_Screen(3, &Vertex[0]);

			if(Verts == 0)
				continue;

			float depth = Vec1.z;
						
			if(depth < Vec2.z)
			{
				depth = Vec2.z;
			}

			if(depth < Vec3.z)
			{
				depth = Vec3.z;
			}

			for ( int k = 1; k < Verts - 1; k++ )
			{
				polygon pt1;
					
				pt1.Vertex[0] = Vertex[0];
				pt1.Vertex[1] = Vertex[k];
				pt1.Vertex[2] = Vertex[k+1];

				int * sort = sort3dptr;
				int * info = info3dptr;
		
				sort[0] = (int)info;
				sort[1] = (int)depth;

				sort3dptr += 2;

				info[0] = 0; //draw type (opaque,transparent tri)
				info[1] = FrontList.PolyList[i].TexID; //tex page

				info += 2;

				*(float*)&info[0] = pt1.Vertex[0].x;
				*(float*)&info[1] = pt1.Vertex[0].y;
				*(float*)&info[2] = pt1.Vertex[0].z;
				*(float*)&info[3] = pt1.Vertex[0].tu;
				*(float*)&info[4] = pt1.Vertex[0].tv;

				info += 5;

				*(float*)&info[0] = pt1.Vertex[1].x;
				*(float*)&info[1] = pt1.Vertex[1].y;
				*(float*)&info[2] = pt1.Vertex[1].z;
				*(float*)&info[3] = pt1.Vertex[1].tu;
				*(float*)&info[4] = pt1.Vertex[1].tv;

				info += 5;

				*(float*)&info[0] = pt1.Vertex[2].x;
				*(float*)&info[1] = pt1.Vertex[2].y;
				*(float*)&info[2] = pt1.Vertex[2].z;
				*(float*)&info[3] = pt1.Vertex[2].tu;
				*(float*)&info[4] = pt1.Vertex[2].tv;

				info += 5;
							
				info3dptr = info;
				SurfaceNum++;
			}
		}

		FrontList.Reset();
	}

	//-----------
	//billboard

	for ( int j =0; j < g_PolygonsBillboard.PolygonCount; j++)
	{
		vector3 Vec1 = g_PolygonsBillboard.PolyList[j].Vertex[0];
		vector3 Vec2 = g_PolygonsBillboard.PolyList[j].Vertex[1];
		vector3 Vec3 = g_PolygonsBillboard.PolyList[j].Vertex[2];

		matrix4x4 MatWorldBillboard;

		//первый вариант начало кода

		Mat4x4_Inverse(MatView, MatWorldBillboard);

		//позиция билбоарда в мировых координатах
		MatWorldBillboard.Mat[3][0] = 0.0f;
		MatWorldBillboard.Mat[3][1] = 0.0f;
		MatWorldBillboard.Mat[3][2] = 0.0f;

		//Y координата всегда вверха у билбоарда
		MatWorldBillboard.Mat[1][0] = 0.0f;
		MatWorldBillboard.Mat[1][1] = 1.0f;
		MatWorldBillboard.Mat[1][2] = 0.0f;

		//первый вариант конец кода
		
		/*
		//второй вариант начало кода
		vector3 vLook, vRight, vUp, vPos;

		//билбоард можно разместить в любом месте
		//сцены но мы разместим в координатах (0, 0, 0)
		vPos = vector3(0.0f, 0.0f, 0.0f);
		
		//вектор который направлен от позиции билбоарда
		//в сторону позиции камеры
		vLook = Vec3_Subtract(g_VecCamPos, vPos);
		
		//верхний вектор - билбоард всегда
		//расположен вертикально
		vUp = vector3(0.0f, 1.0f, 0.0f);
		
		//нормализуем вектор куда смотрит билбоард
		vLook = Vec3_Normalize(vLook);
		//vUp = Vec3_Cross(vLook,vRight);
		//vUp = Vec3_Normalize(vUp);
		
		//находим правый вектор матрицы
		vRight = Vec3_Cross(vUp,vLook);
		vRight = Vec3_Normalize(vRight);

		//теперь у нас есть три вектора и позиция
		//заполняем матрицу мира для билбоарда
		MatWorldBillboard.m16[M00] = vRight.x;
		MatWorldBillboard.m16[M01] = vRight.y;
		MatWorldBillboard.m16[M02] = vRight.z;

		MatWorldBillboard.m16[M10] = vUp.x;
		MatWorldBillboard.m16[M11] = vUp.y;
		MatWorldBillboard.m16[M12] = vUp.z;

		MatWorldBillboard.m16[M20] = vLook.x;
		MatWorldBillboard.m16[M21] = vLook.y;
		MatWorldBillboard.m16[M22] = vLook.z;

		MatWorldBillboard.m16[M30] = vPos.x;
		MatWorldBillboard.m16[M31] = vPos.y;
		MatWorldBillboard.m16[M32] = vPos.z;

		MatWorldBillboard.m16[M03] = 0.0f;
		MatWorldBillboard.m16[M13] = 0.0f;
		MatWorldBillboard.m16[M23] = 0.0f;
		MatWorldBillboard.m16[M33] = 1.0f;
		
		//второй вариант конец кода
		*/

		//умножаем вершины билбоарда на 
		//матрицу мира для билбоарда
		Vec1 = Vec3_Mat4x4_Mul(Vec1, MatWorldBillboard);
		Vec2 = Vec3_Mat4x4_Mul(Vec2, MatWorldBillboard);
		Vec3 = Vec3_Mat4x4_Mul(Vec3, MatWorldBillboard);

		//дальше как обычно умножение на матрицу
		//вида, проекции, деление на z, экранные координаты
		Vec1 = Vec3_Mat4x4_Mul(Vec1, MatView);
		Vec2 = Vec3_Mat4x4_Mul(Vec2, MatView);
		Vec3 = Vec3_Mat4x4_Mul(Vec3, MatView);

		//если все вершины билбоарда за камерой
		//переходим к началу цикла - пропускаем полигон
		if( (Vec1.z < 0) || (Vec2.z < 0) || (Vec3.z < 0) )
			continue;

		Vec1 = Vec3_Mat4x4_Mul(Vec1, MatProj);
		Vec2 = Vec3_Mat4x4_Mul(Vec2, MatProj);
		Vec3 = Vec3_Mat4x4_Mul(Vec3, MatProj);
			
		Vec1.x = Vec1.x / Vec1.z;
		Vec1.y = Vec1.y / Vec1.z;
	
		Vec2.x = Vec2.x / Vec2.z;
		Vec2.y = Vec2.y / Vec2.z;

		Vec3.x = Vec3.x / Vec3.z;
		Vec3.y = Vec3.y / Vec3.z;

		Vec1 = Vec3_Mat4x4_Mul(Vec1, MatScreen);
		Vec2 = Vec3_Mat4x4_Mul(Vec2, MatScreen);
		Vec3 = Vec3_Mat4x4_Mul(Vec3, MatScreen);
			
		vector3 Vertex[8];

		memset(Vertex, 0, sizeof(vector3) * 8);

		Vertex[0].x = Vec1.x;
		Vertex[0].y = Vec1.y;
		Vertex[0].z = 1.0f / Vec1.z;
		Vertex[0].tu = Vec1.tu * Vertex[0].z;
		Vertex[0].tv = Vec1.tv * Vertex[0].z;

		Vertex[1].x = Vec2.x;
		Vertex[1].y = Vec2.y;
		Vertex[1].z = 1.0f / Vec2.z;
		Vertex[1].tu = Vec2.tu * Vertex[1].z;
		Vertex[1].tv = Vec2.tv * Vertex[1].z;

		Vertex[2].x = Vec3.x;
		Vertex[2].y = Vec3.y;
		Vertex[2].z = 1.0f / Vec3.z;
		Vertex[2].tu = Vec3.tu * Vertex[2].z;
		Vertex[2].tv = Vec3.tv * Vertex[2].z;

		int Verts = Clip_Vertices_Screen(3, &Vertex[0]);

		if(Verts == 0)
			continue;

		float depth = Vec1.z;
						
		if(depth < Vec2.z)
		{
			depth = Vec2.z;
		}

		if(depth < Vec3.z)
		{
			depth = Vec3.z;
		}

		for ( int k = 1; k < Verts - 1; k++ )
		{
			polygon pt1;
					
			pt1.Vertex[0] = Vertex[0];
			pt1.Vertex[1] = Vertex[k];
			pt1.Vertex[2] = Vertex[k+1];

			int * sort = sort3dptr;
			int * info = info3dptr;
		
			sort[0] = (int)info;
			sort[1] = (int)depth;

			sort3dptr += 2;

			info[0] = 1; //draw type
			info[1] = g_PolygonsBillboard.PolyList[j].TexID; //tex page

			info += 2;

			*(float*)&info[0] = pt1.Vertex[0].x;
			*(float*)&info[1] = pt1.Vertex[0].y;
			*(float*)&info[2] = pt1.Vertex[0].z;
			*(float*)&info[3] = pt1.Vertex[0].tu;
			*(float*)&info[4] = pt1.Vertex[0].tv;

			info += 5;

			*(float*)&info[0] = pt1.Vertex[1].x;
			*(float*)&info[1] = pt1.Vertex[1].y;
			*(float*)&info[2] = pt1.Vertex[1].z;
			*(float*)&info[3] = pt1.Vertex[1].tu;
			*(float*)&info[4] = pt1.Vertex[1].tv;

			info += 5;

			*(float*)&info[0] = pt1.Vertex[2].x;
			*(float*)&info[1] = pt1.Vertex[2].y;
			*(float*)&info[2] = pt1.Vertex[2].z;
			*(float*)&info[3] = pt1.Vertex[2].tu;
			*(float*)&info[4] = pt1.Vertex[2].tv;

			info += 5;

			info3dptr = info;
			SurfaceNum++;
		}
	}
	
		
	//очищаем Z-Buffer
	/*
	for (UINT i = 0; i < g_ViewHeight; i++)
	{
		for(UINT j = 0; j < g_ViewWidth; j++)
		{
			//дальняя плоскость отсечения 25000.0f
			//это максимальная глубина Z
			//мы этой глубиной очищаем Z-Buffer
			g_ZBuff[i][j] = 25000.0f;
		}
	}
	*/
}

void SortPolyList(int Number,int Buffer[][2])
{
	if (Number)
	{
		/*
		for (int i=0; i<Number;i++) // eliminate polygon flicker
			Buffer[i][1] += i;
		*/

		Do_QuickySorty( 0, Number-1, Buffer);
	}
}

void Do_QuickySorty(int Left, int Right, int Buffer[][2])
{
	int i,j;
	int Compare, Swap;
	
	i = Left;
	j = Right;
	Compare = Buffer[(Left+Right)/2][1];

	do
	{
		while (Buffer[i][1]>Compare && i<Right)
			++i;
		while ( Compare>Buffer[j][1] && j>Left )
			--j;
		if ( i<=j )
		{
			Swap = Buffer[i][1];
			Buffer[i][1] = Buffer[j][1];
			Buffer[j][1] = Swap;

			Swap = Buffer[i][0];
			Buffer[i][0] = Buffer[j][0];
			Buffer[j][0] = Swap;
						
			i++;
			j--;
		}
	}
	while (i<=j);
		
	if(Left<j)
		Do_QuickySorty(Left,j,Buffer);
	if(i<Right)
		Do_QuickySorty(i,Right,Buffer);
}

void PrintPolyList()
{

	int	i;
	int *Sptr;
	int *Iptr;
	int Routine;
	
	Sptr = (int *)sort3d_buffer;

	for ( i=SurfaceNum; i>0; i-- )
	{
		Iptr = (int *)( *Sptr );
		Routine = *(Iptr++);
		(*Poly_Draw_Routines[Routine])( Iptr );
		Sptr += 2;
	}
}

void Draw_MeshManager()
{
	SortPolyList(SurfaceNum,sort3d_buffer);

	Clear_Backbuffer();

	PrintPolyList();

	Present_Backbuffer();
}

void Draw_Textured_Poly_Opaque(int g_y1, int g_y2)
{
	XBUF_PERSP_FP *DataPtr;

	DataPtr = ((XBUF_PERSP_FP *)XBuffer)+g_y1;

	float ui, vi, zi;
	float du, dv, dz;
	
	for (int y = g_y1; y<g_y2; y++, DataPtr++)
	{
		if ((DataPtr->XRight - DataPtr->XLeft)>0)
		{
			du = (DataPtr->UOZright - DataPtr->UOZleft)/(DataPtr->XRight - DataPtr->XLeft);
			dv = (DataPtr->VOZright - DataPtr->VOZleft)/(DataPtr->XRight - DataPtr->XLeft);
			dz = (DataPtr->OOZright - DataPtr->OOZleft)/(DataPtr->XRight - DataPtr->XLeft);
		}
		else
		{
			du = 0;
			dv = 0;
			dz = 0;
		}

		/*
		int xln = (int)g_xl;

		float dxt = 1 - (g_xl - xln);

		zi = g_zl + dxt * dz;
		ui = g_ul + dxt * du;
		vi = g_vl + dxt * dv;
		*/
		
		zi = DataPtr->OOZleft;
		ui = DataPtr->UOZleft;
		vi = DataPtr->VOZleft;

		for (int x=(int)DataPtr->XLeft; x<(int)DataPtr->XRight; x++)
		{

			//float fZVal = g_ZBuff[y][x];

			float z = 1.0f/zi;
			float uf = ui * z;
			float vf = vi * z;

			UINT u = (UINT)uf;
			UINT v = (UINT)vf;

			u = u % g_TextureWidth;
			v = v % g_TextureHeight;


			//если глубина fZVal меньше
			//чем глубина пикселя Z
			//не рисуем пиксель пропускаем его
			/*
			if( fZVal < z )
			{	
				ui+=du;
				vi+=dv;
				zi+=dz;
						
				continue;
			}

			g_ZBuff[y][x] = z;
			*/

			UINT t = (int)u  + (((int)v) * g_TextureWidth);

			if( t < 0 || t > (g_TextureWidth * g_TextureHeight - 1) )
				continue;

			int Index = y * g_BackLpitch + x * 4;

			//умножаем индекс на 3 так как 
			//изображение в массиве содержит три
			//три цветовых компоненты r,g,b
			t= t*3;
			
			//просто берем значения пикселей из текстуры
			g_Data[Index + 0] = g_Res[t + 0];
			g_Data[Index + 1] = g_Res[t + 1];
			g_Data[Index + 2] = g_Res[t + 2];
			g_Data[Index + 3] = 0;
				
			ui+=du;
			vi+=dv;
			zi+=dz;
		}
	}
}

void Draw_Textured_Triangle_Transparent(vector3 VecIn1, vector3 VecIn2, vector3 VecIn3)
{
	int y1i, y3i;

	int Side = Compute_Side(VecIn1, VecIn2, VecIn3, y1i, y3i);

	if(Generate_Slopes(Side))
		Draw_Textured_Poly_Transparent(y1i, y3i);
}


void Draw_Textured_Poly_Transparent(int g_y1, int g_y2)
{

	XBUF_PERSP_FP *DataPtr;

	DataPtr = ((XBUF_PERSP_FP *)XBuffer)+g_y1;

	float ui, vi, zi;
	float du, dv, dz;
	
	for (int y = g_y1; y<g_y2; y++, DataPtr++)
	{
		if ((DataPtr->XRight - DataPtr->XLeft)>0)
		{
			du = (DataPtr->UOZright - DataPtr->UOZleft)/(DataPtr->XRight - DataPtr->XLeft);
			dv = (DataPtr->VOZright - DataPtr->VOZleft)/(DataPtr->XRight - DataPtr->XLeft);
			dz = (DataPtr->OOZright - DataPtr->OOZleft)/(DataPtr->XRight - DataPtr->XLeft);
		}
		else
		{
			du = 0;
			dv = 0;
			dz = 0;
		}

		/*
		int xln = (int)g_xl;

		float dxt = 1 - (g_xl - xln);

		zi = g_zl + dxt * dz;
		ui = g_ul + dxt * du;
		vi = g_vl + dxt * dv;
		*/
		
		zi = DataPtr->OOZleft;
		ui = DataPtr->UOZleft;
		vi = DataPtr->VOZleft;

		for (int x=(int)DataPtr->XLeft; x<(int)DataPtr->XRight; x++)
		{

			//float fZVal = g_ZBuff[y][x];

			float z = 1.0f/zi;
			float uf = ui * z;
			float vf = vi * z;

			UINT u = (UINT)uf;
			UINT v = (UINT)vf;

			u = u % g_TextureWidth;
			v = v % g_TextureHeight;


			//если глубина fZVal меньше
			//чем глубина пикселя Z
			//не рисуем пиксель пропускаем его
			/*
			if( fZVal < z )
			{	
				ui+=du;
				vi+=dv;
				zi+=dz;
						
				continue;
			}

			g_ZBuff[y][x] = z;
			*/

			UINT t = (int)u  + (((int)v) * g_TextureWidth);

			if( t < 0 || t > (g_TextureWidth * g_TextureHeight - 1) )
				continue;

			int Index = y * g_BackLpitch + x * 4;

						//текущаая текстура дерева
			t= t*4;

			//выполняем альфа смешивание
			//для текстуры дерева
			//смешиваем пиксель текстуры дерева
			//с пикселем фона (того что уже есть на сцене)

			//получаем пиксель из текстуры дерева
			//и преобразуем в диапазон от 0 до 1
			float r = g_Res[t + 2] / 255.0f;
			float g = g_Res[t + 1] / 255.0f;
			float b = g_Res[t + 0] / 255.0f;
			float a = g_Res[t + 3] / 255.0f;

			//умножаем пиксель текстуры дерева
			//на его альфа канал
			float SrcR = r * a;
			float SrcG = g * a;
			float SrcB = b * a;

			//получаем пиксель бак- буфера (фона)
			//и преобразуем в диапазон от 0 до 1
			float rb = g_Data[Index + 2] / 255.0f;
			float gb = g_Data[Index + 1] / 255.0f;
			float bb = g_Data[Index + 0] / 255.0f;

			//умножаем пиксель из бак- буфера
			//на 1 - альфа канал текстуры дерева
			float DestR = rb * (1.0f - a);
			float DestG = gb * (1.0f - a);
			float DestB = bb * (1.0f - a);

			//складываем значения и умрожаем на 255
			float ResR = (SrcR + DestR) * 255;
			float ResG = (SrcG + DestG) * 255;
			float ResB = (SrcB + DestB) * 255;
			
			//заполняем бак- буфер
			g_Data[Index + 0] = (int)ResB;
			g_Data[Index + 1] = (int)ResG;
			g_Data[Index + 2] = (int)ResR;
			g_Data[Index + 3] = 0;

				
			ui+=du;
			vi+=dv;
			zi+=dz;
		}
	}
}

bool Load_BMP24(char *szFilename, int Tile)
{
	FILE *fp;

	fopen_s(&fp, szFilename,"rb");
	if(fp==NULL) printf("Error Open File");

	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(bfh), 1, fp);

	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(bih), 1, fp);

	g_pLevelTile24[Tile] = new unsigned char [bih.biWidth*bih.biHeight*3];

	fread(g_pLevelTile24[Tile],bih.biWidth*bih.biHeight*3,1,fp);

	//первый элемент массива Tex хранит размер
	//текстуры пола (кирпичная текстура)
	Tex[0].TextureWidth = bih.biWidth;
	Tex[0].TextureHeight = bih.biHeight;

	return true;
}

bool Load_BMP32(char *szFilename, int Tile)
{
	FILE *fp;

	fopen_s(&fp, szFilename,"rb");
	if(fp==NULL) printf("Error Open File");

	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(bfh), 1, fp);

	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(bih), 1, fp);

	g_pLevelTile32[Tile] = new unsigned char [bih.biWidth*bih.biHeight*4];

	fread(g_pLevelTile32[Tile],bih.biWidth*bih.biHeight*4,1,fp);

	//второй элемент массива Tex хранит размер
	//текстуры дерева (билбоард)
	Tex[1].TextureWidth = bih.biWidth;
	Tex[1].TextureHeight = bih.biHeight;

	return true;

}

void Timer_Start()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)& g_PerfFreq);
	QueryPerformanceCounter((LARGE_INTEGER*)& g_LastTime);
	g_StartTime = g_LastTime;
	g_TimeScale = 1.0f / g_PerfFreq;
}

float Get_Elapsed_Time()
{
	__int64 nowTime;

	QueryPerformanceCounter((LARGE_INTEGER*)& nowTime);
	g_ElapsedTime = (nowTime - g_StartTime) * g_TimeScale;
	g_StartTime = nowTime;

	return g_ElapsedTime;
}

void Mat4x4_Inverse(matrix4x4 &MatIn, matrix4x4 &MatOut)
{
	//computes the inverse of a 4x4, assumes that the last
	//column is [0 0 0 1]t

	float det =  ( MatIn.Mat[0][0] * ( MatIn.Mat[1][1] * MatIn.Mat[2][2] - MatIn.Mat[1][2] * MatIn.Mat[2][1] ) -
		           MatIn.Mat[0][1] * ( MatIn.Mat[1][0] * MatIn.Mat[2][2] - MatIn.Mat[1][2] * MatIn.Mat[2][0] ) +
			       MatIn.Mat[0][2] * ( MatIn.Mat[1][0] * MatIn.Mat[2][1] - MatIn.Mat[1][1] * MatIn.Mat[2][0] ) );

	// test determinate to see if it's 0
	if (fabs(det) < EPSILON_E5)
		return;

	float det_inv  = 1.0f / det;

	MatOut.Mat[0][0] =  det_inv * ( MatIn.Mat[1][1] * MatIn.Mat[2][2] - MatIn.Mat[1][2] * MatIn.Mat[2][1] );
	MatOut.Mat[0][1] = -det_inv * ( MatIn.Mat[0][1] * MatIn.Mat[2][2] - MatIn.Mat[0][2] * MatIn.Mat[2][1] );
	MatOut.Mat[0][2] =  det_inv * ( MatIn.Mat[0][1] * MatIn.Mat[1][2] - MatIn.Mat[0][2] * MatIn.Mat[1][1] );
	MatOut.Mat[0][3] = 0.0f; // always 0

	MatOut.Mat[1][0] = -det_inv * ( MatIn.Mat[1][0] * MatIn.Mat[2][2] - MatIn.Mat[1][2] * MatIn.Mat[2][0] );
	MatOut.Mat[1][1] =  det_inv * ( MatIn.Mat[0][0] * MatIn.Mat[2][2] - MatIn.Mat[0][2] * MatIn.Mat[2][0] );
	MatOut.Mat[1][2] = -det_inv * ( MatIn.Mat[0][0] * MatIn.Mat[1][2] - MatIn.Mat[0][2] * MatIn.Mat[1][0] );
	MatOut.Mat[1][3] = 0.0f; // always 0

	MatOut.Mat[2][0] =  det_inv * ( MatIn.Mat[1][0] * MatIn.Mat[2][1] - MatIn.Mat[1][1] * MatIn.Mat[2][0] );
	MatOut.Mat[2][1] = -det_inv * ( MatIn.Mat[0][0] * MatIn.Mat[2][1] - MatIn.Mat[0][1] * MatIn.Mat[2][0] );
	MatOut.Mat[2][2] =  det_inv * ( MatIn.Mat[0][0] * MatIn.Mat[1][1] - MatIn.Mat[0][1] * MatIn.Mat[1][0] );
	MatOut.Mat[2][3] = 0.0f; // always 0

	MatOut.Mat[3][0] = -( MatIn.Mat[3][0] * MatOut.Mat[0][0] + MatIn.Mat[3][1] * MatOut.Mat[1][0] + MatIn.Mat[3][2] * MatOut.Mat[2][0] );
	MatOut.Mat[3][1] = -( MatIn.Mat[3][0] * MatOut.Mat[0][1] + MatIn.Mat[3][1] * MatOut.Mat[1][1] + MatIn.Mat[3][2] * MatOut.Mat[2][1] );
	MatOut.Mat[3][2] = -( MatIn.Mat[3][0] * MatOut.Mat[0][2] + MatIn.Mat[3][1] * MatOut.Mat[1][2] + MatIn.Mat[3][2] * MatOut.Mat[2][2] );
	MatOut.Mat[3][3] = 1.0f; // always 0
}

void Create_Backbuffer()
{
	RECT Rc;
    ::GetClientRect( g_hWnd, &Rc );

	g_BackLpitch = Rc.right * 4;
    
	g_ViewWidth  = Rc.right;
    g_ViewHeight = Rc.bottom;

	DWORD g_dwSize = g_ViewWidth * (BITS_PER_PIXEL >> 3) * g_ViewHeight;

	g_Data = (LPBYTE)malloc(g_dwSize * sizeof(BYTE));
	g_DataTemp = (LPBYTE)malloc(g_dwSize * sizeof(BYTE));

	memset(&g_Bih, 0, sizeof(BITMAPINFOHEADER));
	g_Bih.biSize = sizeof(BITMAPINFOHEADER);
	g_Bih.biWidth = g_ViewWidth;
	g_Bih.biHeight = g_ViewHeight;
	g_Bih.biPlanes = 1;
	g_Bih.biBitCount = BITS_PER_PIXEL;
	g_Bih.biCompression = BI_RGB;
	g_Bih.biSizeImage = g_dwSize;

	g_hDD = DrawDibOpen();
}

void Clear_Backbuffer()
{
	for (UINT h = 0; h < g_ViewHeight; h++)
	{
		for (UINT w = 0; w < g_ViewWidth; w++)
		{
			int Index = h * 4 * g_ViewWidth + w * 4;

			g_Data[Index + 0] = (BYTE)(255.0 * 0.3f); // blue
			g_Data[Index + 1] = (BYTE)(255.0 * 0.125f); // green
			g_Data[Index + 2] = 0; // red
			g_Data[Index + 3] = 0;
		}
	}
}

void Present_Backbuffer()
{
	//переворачиваем задний буфер
	//так как DrawDibDraw рисует перевернутое изображение
	
	//DWORD g_dwSize = g_ViewWidth * (BITS_PER_PIXEL >> 3) * g_ViewHeight;
	//LPBYTE g_DataTemp = (LPBYTE)malloc(g_dwSize * sizeof(BYTE));

	for (UINT h = 0; h < g_ViewHeight; h++ )
	{
		for (UINT w = 0; w < g_ViewWidth; w++)
		{
			int Index = h * 4 * g_ViewWidth + w * 4;

			BYTE b = g_Data[Index + 0]; // blue
			BYTE g = g_Data[Index + 1]; // green
			BYTE r = g_Data[Index + 2]; // red

			int IndexTemp = (g_ViewHeight - 1 - h) * 4 * g_ViewWidth + w * 4;

			g_DataTemp[IndexTemp + 0] = b;
			g_DataTemp[IndexTemp + 1] = g;
			g_DataTemp[IndexTemp + 2] = r;
			g_DataTemp[IndexTemp + 3] = 0;
		}
	}

	//выводим задний буфер на экран
	HDC hDC = GetDC(g_hWnd);
	DrawDibDraw(g_hDD, hDC, 0, 0, g_ViewWidth, g_ViewHeight, &g_Bih, g_DataTemp, 0, 0, g_ViewWidth, g_ViewHeight, 0);
	ReleaseDC(g_hWnd, hDC);
}

void Delete_Backbuffer()
{
	DrawDibClose(g_hDD);

	free(g_Data);
	free(g_DataTemp);

	g_Data = NULL;
	g_DataTemp = NULL;
}
