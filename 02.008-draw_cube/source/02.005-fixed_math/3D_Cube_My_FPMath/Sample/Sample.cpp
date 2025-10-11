#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <conio.h>
#include <math.h>

//65536 = 360 градусов
//65536 / 4 = 90 градусов
//65536 / 2 = 180 градусов
//1 градус = 65536 / 360 = 182
//sin, cos 16384 = 1.0

#define PI 3.14159265358979
#define PHD_360 65536
#define PHD_180 32768
#define PHD_90 16384
#define TABLE_SIZE 1024
#define ANGLE_STEP (PHD_360 / TABLE_SIZE) // 64
#define FIXED_SCALE (1 << 14) // 16384

short int cos_table[TABLE_SIZE];

HWND g_hWnd;
//максимальная глубина сцены
#define DPQ_END 20480

//таблица хранит заранее расчитанную глубину
//для ускорения расчетов
float ZTable[DPQ_END];

struct vector3
{
	int x, y, z;
};

enum { A, B, C, D, E, F, G, H };

/*
	FONT SIDE	BACK SIDE
	C - D		G - H
	|	|		|	|
	A - B		E - F
*/

//чем больше x,y,z (мы взяли 40)
//тем плавнее внимация, так как
//глубина для расчета берется из ZTable
//и есть погрешность округления до int
//int это индекс в массиве ZTable
vector3 g_VertBuff[8] = {
	-40, -40, -40,	//A
	 40, -40, -40,	//B
	-40,  40, -40,	//C
	 40,  40, -40,	//D

	-40, -40,  40,	//E
	 40, -40,  40,	//F
	-40,  40,  40,	//G
	 40,  40,  40};	//H

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

typedef int matrix4x4[4][4];

vector3 Vec3_Mat4x4_Mul(vector3 VecIn, matrix4x4 MatIn)
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

// Получить индекс в таблице из угла
int AngleToIndex(int angle)
{
    // Угол по модулю 360 градусов (65536)
    angle &= (PHD_360 - 1);

    // Индекс = угол / 64 (шаг)
    return angle / ANGLE_STEP;
}

int Math_Cos(int Angle)
{
    int idx = AngleToIndex(Angle);
    return cos_table[idx];
}

int Math_Sin(int Angle)
{
    // Синус(theta) = косинус(90° - theta)
    int shifted_angle = (PHD_90 - Angle) & (PHD_360 - 1);
    return Math_Cos(shifted_angle);
}

void Build_Cos_Table()
{
	for (int i = 0; i < TABLE_SIZE; i++)
    {
        double angle = 2.0 * PI * i / TABLE_SIZE;
        double cos_val = cos(angle);
        short int fixed_val = static_cast<short int>(cos_val * FIXED_SCALE);
        cos_table[i] = fixed_val;
    }
}

void Init_ZTable()
{
	// 65536 / 360 = 182 
	int Fov = 182 * 90; //угол обзора экрана 90 градусов              		

	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	//косинус делить на синус это конатненс
	//или 1/tan, что соответствует расчету матрицы проекции
	//Rc.bottom / 2 - взято из расчета экранных координат
	//Math_Cos(Fov) / Math_Sin(Fov) или котангенс взято
	//из расчета матрицы проекции - объединяем эти вычисления
	//в одно (здесь в процессе инициализации), что бы сократить вычисления в каждом кадре
	//при отрисовке сцены
	int val1 = Math_Sin(Fov / 2);
	int val2 = Math_Cos(Fov / 2);

	int Persp = Rc.bottom / 2 * Math_Cos(Fov / 2) / Math_Sin(Fov / 2);
	
	int z = 0;

	for (int n = 0; n < DPQ_END; n++)
	{
		//перспективное деление на Z
		//сокращение мат. вычислений в каждом кадре
		ZTable[n] = float(Persp / float(z << 14));
		z++;
	}
}

void Draw_Cube()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	static int Angle = 0;

	matrix4x4 MatRotateX={
		1, 0, 0, 0,
        0, Math_Cos(Angle), Math_Sin(Angle), 0,
        0,-Math_Sin(Angle),  Math_Cos(Angle), 0,
        0, 0, 0, 1};

	matrix4x4 MatRotateY = {
		Math_Cos(Angle),	0,	-Math_Sin(Angle),	0,
		0,			1,	0,			0,
		Math_Sin(Angle),	0,	Math_Cos(Angle),	0,
		0,			0,	0,			1 };

	Angle = Angle + 182;
	Angle = Angle & 0xffff;

	//у куба 8 вершин
	for (int i = 0; i < 8; i++)
	{
		vector3 VecTemp = Vec3_Mat4x4_Mul(g_VertBuff[i], MatRotateY);

		//при умножении на MatRotateY координата Y не масштабировалась
		//масштабируем координату Y самостоятельно
		VecTemp.y = VecTemp.y << 14;

		//приводим к нормальному виду перед умножением на сл.матрицу
		VecTemp.x = VecTemp.x >> 14;
		VecTemp.y = VecTemp.y >> 14;
		VecTemp.z = VecTemp.z >> 14;

		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatRotateX);

		//при умножении на MatRotateX координата X не масштабировалась
		//масштабируем координату X самостоятельно
		VecTemp.x = VecTemp.x << 14;

		//умножение на матрицу проекции не требуется
		//так как у нас есть заранее вычисленные значения
		//которые храняться в ZTable (так же учет аспекта)
		//деление на Z тоже не требуется

		//отодвигаем куб в глубину на 150 единиц
		VecTemp.z = VecTemp.z + (150 << 14);

		//используем ZTable для сокращения
		//мат. вычислений в каждом кадре
		float lz = ZTable[VecTemp.z >> 14];

		//экранные координаты, перспективное деление,
		//угол обзора FOV из мат. проекции - это lz
		VecTemp.x = int(VecTemp.x * lz) + Rc.right / 2;
		VecTemp.y = int(-VecTemp.y * lz) + Rc.bottom / 2;

		g_VertBuffTransformed[i] = VecTemp;
	}

	HDC hDC = GetDC(g_hWnd);

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Rectangle(hDC, 0, 0, Rc.right, Rc.bottom);

	HPEN hPen = CreatePen(PS_SOLID, 4, RGB(255, 255, 127));
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	//у куба 12 линий, т.е. 24 вершины для этих линий
	for (int i = 0; i < 12; i++)
	{
		vector3 Vec1 = g_VertBuffTransformed[g_IndexBuff[i * 2 + 0]];
		vector3 Vec2 = g_VertBuffTransformed[g_IndexBuff[i * 2 + 1]];

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

	Build_Cos_Table();
	Init_ZTable();

	while (!_kbhit())
	{
		Draw_Cube();
	}

	return 0;
}
