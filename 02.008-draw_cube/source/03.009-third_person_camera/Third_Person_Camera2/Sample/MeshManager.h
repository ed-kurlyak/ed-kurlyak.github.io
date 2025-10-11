//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
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

	vector3 operator * (const float &ValIn)
	{
		vector3 VecOut;
		
		VecOut.x = x * ValIn;
		VecOut.y = y * ValIn;
		VecOut.z = z * ValIn;
		
		return VecOut;
	}

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

struct vertex
{
	float x,y,z;
};

struct tex_coord2
{
	int tu, tv;
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

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();

	void Init_MeshManager(HWND hwnd) ;
	void Update_MeshManager();
	void Draw_MeshManager();

private:

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
	float Vec3_Length(vector3 &VecIn);
	void Matrix_Rotation_Axis(vector3 &VecIn, float Angle, matrix4x4 &MatRotate);
	void Mat4x4_Inverse(matrix4x4 &MatIn, matrix4x4 &MatOut);
	void Get_View_Matrix(matrix4x4 &MatView);
	vector3 Calc_Edge(vector3 &VecIn1, vector3 &VecIn2);
	void FrameMove_Cube(matrix4x4 &MatWorldCube);

	//для загрузки текстуры кипричного пола
	//текстура пола 24 бит глубина цвета
	bool Load_BMP24(char *szFilename, int Tile);

	//рисуем пол
	void Draw_Polygon_List_Floor();

	//рисуем куб
	void Draw_Polygon_List_Cube();

	void Draw_Textured_Triangle(vector3 VecIn1, vector3 VecIn2, vector3 VecIn3);
	void Draw_Textured_Poly(int y1, int y2);
	int Clip_Vertices_Screen(int Num, vector3 *Source);

	//таймер
	void Timer_Start();
	float Get_Elapsed_Time();

	unsigned char **m_pLevelTile24;

	UCHAR * m_Res;

	tex_info * Tex;
	
    ULONG m_ViewWidth;
	ULONG m_ViewHeight;
	
	HWND m_hWnd;

	vector3 m_VecCamRight;
	vector3 m_VecCamUp;
	vector3 m_VecCamPos;
	vector3 m_VecCamLook;

	vector3 m_VecCubeRight;
    vector3 m_VecCubeUp;
    vector3 m_VecCubeLook;
	vector3 m_VecCubePos;

	float m_AngleCube;

	float **m_ZBuff;
	
	//полигоны кирпичного пола
	list m_PolygonsFloor;
	list m_TransformedPolyFloor;

	//полигоны куба
	list m_PolygonsCube;
	list m_TransformedPolyCube;

	UINT m_TextureWidth;
	UINT m_TextureHeight;

	//начальная позиция для левой стороны треугольника
	float m_ul, m_vl, m_zl, m_xl;
	//приращение координат для левой стороны треугольника
	float  m_dudyl, m_dvdyl, m_dzdyl, m_dxdyl;

	//начальная позиция для правой стороны треугольника
	float m_ur, m_vr, m_zr, m_xr;
	//приращение координат для правой стороны треугольника
	float m_dudyr, m_dvdyr, m_dzdyr, m_dxdyr;

	//Добавим задний буфер
	int m_BackLpitch;
	LPBYTE m_Data;
	BITMAPINFOHEADER m_Bih;
	HDRAWDIB m_hDD;
	LPBYTE m_DataTemp;

	//timer
	float m_ElapsedTime;
	float m_TimeScale;
	__int64 m_StartTime;
	__int64 m_LastTime;
	__int64 m_PerfFreq;

	//ближняя плоскость отсечения
	float m_ZNear;

};

#endif