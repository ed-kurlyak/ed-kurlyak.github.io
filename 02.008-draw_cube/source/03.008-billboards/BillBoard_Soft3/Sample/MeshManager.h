//======================================================================================
//	Ed Kurlyak 2025 Software Rendering
//======================================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <math.h>
#include <stdio.h>

#include <vfw.h> //для DrawDibDraw
#pragma comment(lib, "Vfw32.lib") //для DrawDibDraw

#define PI 3.14159265358979f

#define SWAP(a,b,t) { t = a; a = b; b = t; }

#define EPSILON_E5 (float)(1E-5)

#define BITS_PER_PIXEL	32

struct vector3
{
	float x,y,z;
	float tu,tv;
	
	vector3() {};
	vector3(float ix, float iy, float iz) : x(ix), y(iy), z(iz) {};

	vector3 operator - (const vector3 &VecIn)
	{
		vector3 VecOut;
		
		VecOut.x = x - VecIn.x;
		VecOut.y = y - VecIn.y;
		VecOut.z = z - VecIn.z;
		
		return VecOut;
	}

	vector3 &operator = (const vector3 &VecIn)
	{
		x = VecIn.x;
		y = VecIn.y;
		z = VecIn.z;

		tu = VecIn.tu;
		tv = VecIn.tv;

		return *this;
	}

	vector3 & operator += (const vector3 &VecIn)
	{
		x = x + VecIn.x;
		y = y + VecIn.y;
		z = z + VecIn.z;

		return *this;
	}
};

struct polygon
{
	polygon(){};
	~polygon(){};
	
	int  TexID;
	vector3 Vertex[3];
};

struct list 
{
	list()
	{
		PolygonCurr = 0; PolygonCount = 0; PolyList = NULL;
	};

	~list()
	{
		if(PolyList != NULL)
		{
			free(PolyList);
			PolyList = NULL;
		}
		PolygonCurr = 0;
		PolygonCount = 0;
	}

	void Reset()
	{
		if(PolyList != NULL)
		{
			free(PolyList);
			PolyList = NULL;
		}
		PolygonCurr = 0;
		PolygonCount = 0;
		
	}

	bool Is_Empty_List ();
	polygon *Get_Frog_List();
	void Add_To_List(polygon *p);
	
	polygon* PolyList;
	int PolygonCount;
	int PolygonCurr;
};

struct matrix4x4
{
	union
	{
		float  Mat[4][4];

		struct
		{
			float r1c1; float r1c2; float r1c3; float r1c4;
			float r2c1; float r2c2; float r2c3; float r2c4;
			float r3c1; float r3c2; float r3c3; float r3c4;
			float r4c1; float r4c2; float r4c3; float r4c4;
		};
	};

	matrix4x4 (){};
	~matrix4x4 (){};

	matrix4x4 (	float in_r1c1, float in_r1c2, float in_r1c3, float in_r1c4,
					float in_r2c1, float in_r2c2, float in_r2c3, float in_r2c4,
					float in_r3c1, float in_r3c2, float in_r3c3, float in_r3c4,
					float in_r4c1, float in_r4c2, float in_r4c3, float in_r4c4)
	{
		r1c1 = in_r1c1; r1c2 = in_r1c2; r1c3 = in_r1c3; r1c4 = in_r1c4;
		r2c1 = in_r2c1; r2c2 = in_r2c2; r2c3 = in_r2c3; r2c4 = in_r2c4;
		r3c1 = in_r3c1; r3c2 = in_r3c2; r3c3 = in_r3c3; r3c4 = in_r3c4;
		r4c1 = in_r4c1; r4c2 = in_r4c2; r4c3 = in_r4c3; r4c4 = in_r4c4;
	};
};

struct tex_info
{
	UINT TextureWidth;
	UINT TextureHeight;
};

struct XBUF_PERSP_FP
{
			int	XLeft;
			float UOZleft;
			float VOZleft;
			float OOZleft;
			int	XRight;
			float UOZright;
			float VOZright;
			float OOZright;
};

#define MAX_SCANLINES 1200 //maximum height of screen

void (*Poly_Draw_Routines[])(int *);

#define MAX_POLYGONS 4000


void Init_MeshManager(HWND hwnd);
void Update_MeshManager();
void Draw_MeshManager();
void Destroy_MeshManager();

void Draw_Perspective_Opaque_Tri(int * ptr);
void Draw_Perspective_Transparent_Tri(int * ptr);

void SortPolyList( int number,int buffer[][2]);
void PrintPolyList();

void Do_QuickySorty( int left, int right, int buffer[][2] );

void Create_Backbuffer();
void Clear_Backbuffer();
void Present_Backbuffer();
void Delete_Backbuffer();

vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 &MatIn);
float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2);
vector3 Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2);
vector3 Vec3_Normalize(vector3 &VecIn);
vector3 Vec3_Scale(vector3 &VecIn, float ValIn);
vector3 Vec3_Add(vector3 &VecIn1, vector3 &VecIn2);
void Matrix_Rotation_Axis(vector3 &VecIn, float Angle, matrix4x4 &MatRotate);
void Mat4x4_Inverse(matrix4x4 &MatIn, matrix4x4 &MatOut);
void Get_View_Matrix(matrix4x4 &MatView);
vector3 Calc_Edge(vector3 &VecIn1, vector3 &VecIn2);

//для загрузки текстуры кипричного пола
//текстура пола 24 бит глубина цвета
bool Load_BMP24(char *szFilename, int Tile);

//для загрузки текстуры дерева (билбоард)
//текстура дерева 32 бит глубина цвета
bool Load_BMP32(char *szFilename, int Tile);

int Compute_Side(vector3 VecIn1, vector3 VecIn2, vector3 VecIn3, int & y1i, int & y3i);
int Generate_Slopes(int Side);

void Draw_Textured_Triangle_Opaque(vector3 v0, vector3 v1, vector3 v2);
void Draw_Textured_Poly_Opaque(int g_y1, int g_y2);

void Draw_Textured_Triangle_Transparent(vector3 v0, vector3 v1, vector3 v2);
void Draw_Textured_Poly_Transparent(int g_y1, int g_y2);

int Clip_Vertices_Screen(int Num, vector3 *Source);
int Clip_Vertices_ZNear(int Num, vector3 *Source);

//таймер
void Timer_Start();
float Get_Elapsed_Time();

#endif