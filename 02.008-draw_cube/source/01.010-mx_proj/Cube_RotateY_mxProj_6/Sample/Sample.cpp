//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <conio.h>
#include <math.h>

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

HWND g_hWnd;

//максимальная глубина сцены
#define DPQ_END 20480

//таблица хранит заранее расчитанную глубину
//для ускорения расчетов
float ZTable[DPQ_END];

struct vector3
{
	float x, y, z;
};

struct vector4
{
	float x, y, z, w;
};

enum { A, B, C, D, E, F, G, H };

vector3 g_VertBuff[8] = {
	-40.0, -40.0, -40.0,	//A
	 40.0, -40.0, -40.0,	//B
	-40.0,  40.0, -40.0,	//C
	 40.0,  40.0, -40.0,	//D

	-40.0, -40.0,  40.0,	//E
	 40.0, -40.0,  40.0,	//F
	-40.0,  40.0,  40.0,	//G
	 40.0,  40.0,  40.0 };	//H

vector3 g_VertBuffTransformed[8];

unsigned int g_IndexBuff[24] = {
	A, B,
	B, D,
	D, C,
	C, A,

	E, F,
	F, H,
	H, G,
	G, E,

	A, E,
	B, F,
	D, H,
	C, G };

typedef float matrix4x4[4][4];

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

float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2)
{
	return VecIn1.x * VecIn2.x + VecIn1.y * VecIn2.y + VecIn1.z * VecIn2.z;
}

vector3 Vec3_Normalize(vector3 &VecIn)
{
	float len = sqrtf((VecIn.x * VecIn.x) + (VecIn.y * VecIn.y) + (VecIn.z * VecIn.z));
	
	vector3 VecOut = {	VecIn.x / len,
						VecIn.y / len,
						VecIn.z / len };

	return VecOut;
}

vector3 Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut = { VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y,
			VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z,
			VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x };

	return VecOut;
}

vector3 Vec3_Subtract(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut = {	VecIn1.x - VecIn2.x,
						VecIn1.y - VecIn2.y,
						VecIn1.z - VecIn2.z };

	return VecOut;
}

void Init_ZTable()
{
	
	//float Fov = 90.0f / 2.0f; //угол обзора экрана 90 градусов              		
	float Fov = PI / 2.0f;

	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	//косинус делить на синус это конатненс
	//или 1/tan, что соответствует расчету матрицы проекции
	//Rc.bottom / 2 - взято из расчета экранных координат
	//из расчета матрицы проекции - объединяем эти вычисления
	//в одно (здесь в процессе инициализации), что бы сократить вычисления в каждом кадре
	//при отрисовке сцены
	float Persp = Rc.bottom / 2 * cosf(Fov * 0.5f) / sinf(Fov * 0.5f);
	//вычисление выше Persp заменяет вычисление ниже Persp
	//float Persp = Rc.bottom / 2 * (1.0f / tanf(Fov/2)); //тоже работает
	
	int z = 0;

	for (int n = 0; n < DPQ_END; n++)
	{
		//перспективное деление на Z
		//сокращение мат. вычислений в каждом кадре
		ZTable[n] = float(Persp / z);
		z++;
	}
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
	if (Angle > PI2)
		Angle = 0.0f;

	//MATRIX VIEW CALCULATION
	vector3 VecCamRight = { 1.0f, 0.0f, 0.0 };
	vector3 VecCamUp = { 0.0f, 1.0f, 0.0f }; 
	vector3 VecCamPos = { 0.0f, 100.0f, 0.0f };
	vector3 VecModelPos = { 0.0, 0.0, 150.0 };
	vector3 VecCamLook = Vec3_Subtract(VecModelPos, VecCamPos);

	VecCamLook = Vec3_Normalize(VecCamLook);

	VecCamUp = Vec3_Cross(VecCamLook, VecCamRight);
	VecCamUp = Vec3_Normalize(VecCamUp);
	VecCamRight = Vec3_Cross(VecCamUp, VecCamLook);
	VecCamRight = Vec3_Normalize(VecCamRight);

	float xp = -Vec3_Dot(VecCamPos, VecCamRight);
	float yp = -Vec3_Dot(VecCamPos, VecCamUp);
	float zp = -Vec3_Dot(VecCamPos, VecCamLook);

	matrix4x4 MatView = {
		VecCamRight.x,	VecCamUp.x,  VecCamLook.x,	0.0,
		VecCamRight.y, VecCamUp.y,  VecCamLook.y,    0.0,
		VecCamRight.z, VecCamUp.z,  VecCamLook.z,    0.0,
		xp,			yp,		  zp,			1.0 };

	//MATRIX WORLD
	matrix4x4 MatWorld = {
		1.0f, 0.0, 0.0, 0.0,
		0.0, 1.0f, 0.0, 0.0,
		0.0, 0.0, 1.0f, 0.0,
		VecModelPos.x, VecModelPos.y, VecModelPos.z, 1.0f }; 

	//MATRIX PROJECTION CALCULATION
	//расчет матрицы проекции взят из
	//справки DirectX
	float Fov = PI / 2.0f; // FOV 90 degree
	float Aspect = (float)Rc.right / Rc.bottom;
	float ZFar = 100.0f;
	float ZNear = 1.0f;

	float h, w, Q;

	w = (1.0f / tanf(Fov * 0.5f)) / Aspect;
	h = 1.0f / tanf(Fov * 0.5f);
	Q = ZFar / (ZFar - ZNear);

	matrix4x4 MatProj = {
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, Q, 1,
		0, 0, -Q * ZNear, 0 };

	for (int i = 0; i < 8; i++)
	{
		vector3 Vec = Vec3_Mat4x4_Mul(g_VertBuff[i], MatRotateY);
		Vec = Vec3_Mat4x4_Mul(Vec, MatWorld);
		Vec = Vec3_Mat4x4_Mul(Vec, MatView);

#if(1)
		//вариант 1 с использованием ZTable
		//способ через ZTable погрешность в округлении
		//до int Vec.z для индекса в таблице
		//будут искажения в отрисовке куба
		//что бы решить проблему искажений можно увеличить
		//размер вершин, сделать не 4.0f а например 40.0f
		//внутри структуры vector3 g_VertBuff[8]
		//такой метод применялся в старых играх где использовалась fixed point math
		float lz = ZTable[(int)Vec.z];
		//вариант 1 конец

#else
		//вариант 2 без использования ZTable
		float Persp = Rc.bottom / 2 * cosf(Fov * 0.5f) / sinf(Fov * 0.5f);
		//вычисление выше Persp заменяет вычисление ниже Persp
		//float Persp = Rc.bottom / 2 * (1.0f / tanf(Fov/2)); //тоже работает
		float lz = Persp / Vec.z;
		//вариант 2 конец
#endif

		Vec.x = Vec.x * lz + Rc.right / 2;
		Vec.y = -Vec.y * lz + Rc.bottom / 2;


		g_VertBuffTransformed[i] = Vec;
	}

	HDC hDC = GetDC(g_hWnd);

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Rectangle(hDC, 0, 0, Rc.right, Rc.bottom);

	HPEN hPen = CreatePen(PS_SOLID, 4, RGB(255, 255, 127));
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	for (int i = 0; i < 12; i++)
	{
		vector3 Vec1 = g_VertBuffTransformed[ g_IndexBuff[ i * 2 + 0 ] ];
		vector3 Vec2 = g_VertBuffTransformed[ g_IndexBuff[ i * 2 + 1 ] ];

		MoveToEx(hDC, (int)Vec1.x, (int)Vec1.y, NULL);
		LineTo(hDC, (int)Vec2.x, (int)Vec2.y);
	}

	SelectObject(hDC, hOldBrush);
	DeleteObject(hBrush);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);

	ReleaseDC(g_hWnd, hDC);

	Sleep(25);
}

int main()
{
	g_hWnd = GetConsoleWindow();

	Init_ZTable();

	while (!_kbhit())
	{
		Draw_Cube();
	}

	return 0;
}