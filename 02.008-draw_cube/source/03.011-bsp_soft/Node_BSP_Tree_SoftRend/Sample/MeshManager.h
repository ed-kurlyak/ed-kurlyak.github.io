//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#ifndef _LEVEL_LOADER_
#define _LEVEL_LOADER_

#include <windows.h>
#include <math.h>
#include <stdio.h>

#include <vfw.h> //для DrawDibDraw
#pragma comment(lib, "Vfw32.lib") //для DrawDibDraw

#define PI 3.14159265358979f

#define SWAP(a,b,t) { t = a; a = b; b = t; }

#define BITS_PER_PIXEL	32

enum {COINCIDENT, IN_BACK_OF, IN_FRONT_OF, SPANNING};

struct matrix4x4;

struct vector4
{
	vector4();
	vector4(float ix, float iy, float iz);
	~vector4();

	float x;
    float y;
    float z;
	float w;

	float tu;
    float tv;

	//vector dot
	float operator | (const vector4 & VecIn);

	vector4 &operator = (const vector4 & VecIn);
	
	//vector subtract
	vector4  operator - (const vector4 & VecIn);

	//vector scale
	vector4  operator * (const float & VecIn);

	//vector add
	vector4 operator + (const vector4 & VecIn);

	vector4 & operator += (const vector4 & VecIn);

		
};

struct polygon;

struct plane
{
	float a,b,c,d;

	int Classify_Polygon (polygon *Poly);
	float Classify_Point (vector4 Eye);
	vector4 Normal();
};

struct polygon
{
	polygon(){};
	~polygon(){};
	
	vector4 Vertex[3];
	int  TexID;

	vector4 GetNormal();
	plane Get_Plane();
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

	//конструктор копирования что бы передавать
	//list как параметр функции
	list(const list & l)
	{
			PolyList = NULL;
			PolygonCount = 0;
			PolygonCurr = 0;

			PolygonCount = l.PolygonCount;
			PolygonCurr = l.PolygonCurr;
			PolyList=(polygon*)malloc(PolygonCount*sizeof(polygon));

			memcpy(&PolyList[0], &l.PolyList[0], PolygonCount*sizeof(polygon));

	}

	//оператор присваивания копированием
	list & operator = (list & l)
	{
		if(PolyList != NULL)
		{
			delete [] PolyList;
			PolygonCount = 0;
			PolygonCurr = 0;
		}

		PolygonCount = l.PolygonCount;
		PolygonCurr = l.PolygonCurr;

		PolyList=(polygon*)malloc(PolygonCount*sizeof(polygon));

		memcpy(&PolyList[0], &l.PolyList[0], PolygonCount*sizeof(polygon));

		return *this;
	}

	bool Is_Empty_List ();
	polygon *Get_From_List();
	void Add_To_List(polygon *p);
	
	polygon* PolyList;
	int PolygonCount;
	int PolygonCurr;
};

struct matrix4x4
{
	union
	{
		float Mat16[16];
		float Mat[4][4];
	};

	matrix4x4 ();
	~matrix4x4 ();
	
	matrix4x4(float in_r1c1, float in_r1c2, float in_r1c3, float in_r1c4,
		float in_r2c1, float in_r2c2, float in_r2c3, float in_r2c4,
		float in_r3c1, float in_r3c2, float in_r3c3, float in_r3c4,
		float in_r4c1, float in_r4c2, float in_r4c3, float in_r4c4);
	
	//matrix matrix mul
	matrix4x4 operator * (const matrix4x4 m2);
};

struct	BSP_tree
{
	BSP_tree(){ Front = NULL; Back = NULL; };
	~BSP_tree()
	{
		if(Front != NULL)
		{
			delete Front;
			Front = NULL;
		}

		if(Back != NULL)
		{
			delete Back;
			Back = NULL;
		}
	};

	plane     Partition;
	list      Polygons;
	list      TransformedPolygons;
	BSP_tree  *Front,
             *Back;
};

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();

	void Init_MeshManager(HWND hWnd) ;
	void Update_MeshManager();
	void Draw_MeshManager();

private:

	void Create_Backbuffer();
	void Clear_Backbuffer();
	void Present_Backbuffer();
	void Delete_Backbuffer();

	vector4 Vec4_Mat4x4_Mul(vector4 &VecIn, matrix4x4 &MatIn);
	vector4 Vec4_Normalize(vector4 &VecIn);
	vector4 Vec4_Scale(vector4 &VecIn, float ValIn);
	vector4 Vec4_Add(vector4 &VecIn1, vector4 &VecIn2);
	vector4 Vec4_Cross(vector4 &VecIn1, vector4 &VecIn2);
	float Vec4_Dot(vector4 &VecIn1, vector4 &VecIn2);
	void Matrix_Rotation_Axis(vector4 &VecIn, float Angle, matrix4x4 &MatOut);
	
	vector4 Calc_Edge(vector4 &VecIn1, vector4 &VecIn2);

	int Get_TextureID(char * szFilename);

	bool Load_BMP(char *szFilename, int Tile);

	void Build_BSP_Tree(BSP_tree *Tree, list Polygons);
	void Split_Polygon(polygon *Poly, plane *Part, polygon *&Front, polygon *&Back, int &cf, int &cb);
	void Delete_BSP(BSP_tree *Tree);
	void Transform_BSP_Tree(BSP_tree *Tree);
	void Draw_BSP_Tree(BSP_tree *Tree, vector4 Eye);

	void Extract_Frustum();
	bool Polygon_In_Frustum(int NumPoints, vector4* PointList);

	void Draw_Polygon_List(list Polygons);
	void Draw_Textured_Triangle(vector4 VecIn1, vector4 VecIn2, vector4 VecIn3);
	void Draw_Textured_Poly(int y1, int y2);
	int Clip_Vertices_Screen(int Num, vector4 *Source);

	void Timer_Start();
	float Get_Elapsed_Time();

	void Get_View_Matrix(matrix4x4 &MatOut);
	
	unsigned char **m_pLevelTile;

    ULONG m_ViewWidth;
	ULONG m_ViewHeight;
	
	HWND m_hWnd;

	vector4 m_VecCamRight;
	vector4 m_VecCamUp;
	vector4 m_VecCamLook;
	vector4 m_VecCamPos;

	list m_Polygons;

	BSP_tree* m_Root;

	matrix4x4 m_MatWorld;
	matrix4x4 m_MatView;
	matrix4x4 m_MatProj;
	matrix4x4 m_MatRes;

	//шесть плоскостей отсечения
	//левая, правая, передняя, задняя, верхняя, нижняя
	float m_Frustum[6][4];
	
	//начальная позиция для левой стороны треугольника
	float m_ul, m_vl, m_zl, m_xl;
	//приращение координат для левой стороны треугольника
	float  m_dudyl, m_dvdyl, m_dzdyl, m_dxdyl;

	//начальная позиция для правой стороны треугольника
	float m_ur, m_vr, m_zr, m_xr;
	//приращение координат для правой стороны треугольника
	float m_dudyr, m_dvdyr, m_dzdyr, m_dxdyr;

	UCHAR *m_Res;

	//Добавим задний буфер
	int m_BackLpitch;
	LPBYTE m_Data;
	LPBYTE m_DataTemp;
	BITMAPINFOHEADER m_Bih;
	HDRAWDIB m_hDD;

	//timer
	float m_ElapsedTime;
	float m_TimeScale;
	__int64 m_StartTime;
	__int64 m_LastTime;
	__int64 m_PerfFreq;

	UINT m_TextureWidth;
	UINT m_TextureHeight;

	float m_ZNear;
};



#endif