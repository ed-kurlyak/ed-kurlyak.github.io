//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <math.h>

#define CELL_SIZE 4.0f
#define FILED_SIZE 25.0f

struct vector3
{
	float x,y,z;

	vector3 & operator += (const vector3 &VecIn)
	{
		x = x + VecIn.x;
		y = y + VecIn.y;
		z = z + VecIn.z;

		return *this;
	}
};

struct matrix4x4
{
	float Mat[4][4];
};

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();

	void Init_MeshManager(HWND hWnd);
	void Update_MeshManager();
	void Draw_MeshManager();

private:
	
	void Create_BackBuffer();
	void Clear_BackBuffer();
	void Present_BackBuffer();
	void Delete_BackBuffer();

	vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 &MatIn);
	float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2);
	vector3 Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2);
	vector3 Vec3_Normalize(vector3 &VecIn);
	vector3 Vec3_Scale(vector3 &VecIn, float ValIn);
	vector3 Vec3_Add(vector3 &VecIn1, vector3 &VecIn2);
	matrix4x4 Matrix_Rotation_Axis(vector3 &VecIn, float Angle);
	matrix4x4 Get_View_Matrix();
	vector3 Calc_Edge(vector3 &VecIn1, vector3 &VecIn2);
	
	void Timer_Start();
	float Get_Elapsed_Time();

	HWND m_hWnd;

	//контекст окна приложения
	HDC m_hDC;
	//размеры окна приложения
	UINT m_ViewWidth;
	UINT m_ViewHeight;
	//наш задний буфер
	HDC m_hBackBuffer;
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	//кисть для очистки заднего буфера (окна приложения)
	HBRUSH m_hBrush;
	//область для очистки заднего буфера (окна приложения)
	RECT m_Rc;

	//у нас будет 50 линий
	//25 вертикальных
	//25 горизонтальных
	//каждая линия содержит 2 точки начало конец
	//50 линий уномжить на 2 = 100
	//плюс 1 линия в конце по оси X - 2 вершины
	//плюс 1 линия в конце по осиZ - 2 вершины
	vector3 m_VertBuff[100 + 4];

	//трансформированные линии после умножения на матрицы
	//и после деления на Z
	vector3 m_VertBuffTransformed[104];

	//индексного буфера нет

	//timer
	float m_ElapsedTime;
	float m_TimeScale;
	__int64 m_StartTime;
	__int64 m_LastTime;
	__int64 m_PerfFreq;

	//векторы хранят значения для расчета матрицы вида
	vector3 m_VecCamRight;
	vector3 m_VecCamUp;
	vector3 m_VecCamLook;
	vector3 m_VecCamPos;
};

#endif