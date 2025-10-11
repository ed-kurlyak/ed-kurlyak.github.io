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

struct vector3
{
	float x,y,z;
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

	
struct matrix4x4
{
	float Mat[4][4];
};

vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn)
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

matrix4x4 Mat4x4_Mat4x4_Mul(matrix4x4 MatIn1, matrix4x4 MatIn2)
{
	matrix4x4 MatOut;

	//row1 * col1
	MatOut.Mat[0][0] = MatIn1.Mat[0][0]*MatIn2.Mat[0][0] + MatIn1.Mat[0][1]*MatIn2.Mat[1][0] + MatIn1.Mat[0][2]*MatIn2.Mat[2][0] + MatIn1.Mat[0][3]*MatIn2.Mat[3][0];
	//row1 * col2
	MatOut.Mat[0][1] = MatIn1.Mat[0][0]*MatIn2.Mat[0][1] + MatIn1.Mat[0][1]*MatIn2.Mat[1][1] + MatIn1.Mat[0][2]*MatIn2.Mat[2][1] + MatIn1.Mat[0][3]*MatIn2.Mat[3][1];
	//row1 * col3
	MatOut.Mat[0][2] = MatIn1.Mat[0][0]*MatIn2.Mat[0][2] + MatIn1.Mat[0][1]*MatIn2.Mat[1][2] + MatIn1.Mat[0][2]*MatIn2.Mat[2][2] + MatIn1.Mat[0][3]*MatIn2.Mat[3][2];
	//row1 * col4
	MatOut.Mat[0][3] = MatIn1.Mat[0][0]*MatIn2.Mat[0][3] + MatIn1.Mat[0][1]*MatIn2.Mat[1][3] + MatIn1.Mat[0][2]*MatIn2.Mat[2][3] + MatIn1.Mat[0][3]*MatIn2.Mat[3][3];

	//row2 * col1
	MatOut.Mat[1][0] = MatIn1.Mat[1][0]*MatIn2.Mat[0][0] + MatIn1.Mat[1][1]*MatIn2.Mat[1][0] + MatIn1.Mat[1][2]*MatIn2.Mat[2][0] + MatIn1.Mat[1][3]*MatIn2.Mat[3][0];
	//row2 * col2
	MatOut.Mat[1][1] = MatIn1.Mat[1][0]*MatIn2.Mat[0][1] + MatIn1.Mat[1][1]*MatIn2.Mat[1][1] + MatIn1.Mat[1][2]*MatIn2.Mat[2][1] + MatIn1.Mat[1][3]*MatIn2.Mat[3][1];
	//row2 * col3
	MatOut.Mat[1][2] = MatIn1.Mat[1][0]*MatIn2.Mat[0][2] + MatIn1.Mat[1][1]*MatIn2.Mat[1][2] + MatIn1.Mat[1][2]*MatIn2.Mat[2][2] + MatIn1.Mat[1][3]*MatIn2.Mat[3][2];
	//row2 * col4
	MatOut.Mat[1][3] = MatIn1.Mat[1][0]*MatIn2.Mat[0][3] + MatIn1.Mat[1][1]*MatIn2.Mat[1][3] + MatIn1.Mat[1][2]*MatIn2.Mat[2][3] + MatIn1.Mat[1][3]*MatIn2.Mat[3][3];

	//row3 * col1
	MatOut.Mat[2][0] = MatIn1.Mat[2][0]*MatIn2.Mat[0][0] + MatIn1.Mat[2][1]*MatIn2.Mat[1][0] + MatIn1.Mat[2][2]*MatIn2.Mat[2][0] + MatIn1.Mat[2][3]*MatIn2.Mat[3][0];
	//row3 * col2
	MatOut.Mat[2][1] = MatIn1.Mat[2][0]*MatIn2.Mat[0][1] + MatIn1.Mat[2][1]*MatIn2.Mat[1][1] + MatIn1.Mat[2][2]*MatIn2.Mat[2][1] + MatIn1.Mat[2][3]*MatIn2.Mat[3][1];
	//row3 * col3
	MatOut.Mat[2][2] = MatIn1.Mat[2][0]*MatIn2.Mat[0][2] + MatIn1.Mat[2][1]*MatIn2.Mat[1][2] + MatIn1.Mat[2][2]*MatIn2.Mat[2][2] + MatIn1.Mat[2][3]*MatIn2.Mat[3][2];
	//row3 * col4
	MatOut.Mat[2][3] = MatIn1.Mat[2][0]*MatIn2.Mat[0][3] + MatIn1.Mat[2][1]*MatIn2.Mat[1][3] + MatIn1.Mat[2][2]*MatIn2.Mat[2][3] + MatIn1.Mat[2][3]*MatIn2.Mat[3][3];

	//row4 * col1
	MatOut.Mat[3][0] = MatIn1.Mat[3][0]*MatIn2.Mat[0][0] + MatIn1.Mat[3][1]*MatIn2.Mat[1][0] + MatIn1.Mat[3][2]*MatIn2.Mat[2][0] + MatIn1.Mat[3][3]*MatIn2.Mat[3][0];
	//row4 * col2
	MatOut.Mat[3][1] = MatIn1.Mat[3][0]*MatIn2.Mat[0][1] + MatIn1.Mat[3][1]*MatIn2.Mat[1][1] + MatIn1.Mat[3][2]*MatIn2.Mat[2][1] + MatIn1.Mat[3][3]*MatIn2.Mat[3][1];
	//row4 * col3
	MatOut.Mat[3][2] = MatIn1.Mat[3][0]*MatIn2.Mat[0][2] + MatIn1.Mat[3][1]*MatIn2.Mat[1][2] + MatIn1.Mat[3][2]*MatIn2.Mat[2][2] + MatIn1.Mat[3][3]*MatIn2.Mat[3][2];
	//row4 * col4
	MatOut.Mat[3][3] = MatIn1.Mat[3][0]*MatIn2.Mat[0][3] + MatIn1.Mat[3][1]*MatIn2.Mat[1][3] + MatIn1.Mat[3][2]*MatIn2.Mat[2][3] + MatIn1.Mat[3][3]*MatIn2.Mat[3][3];

	return MatOut;
}

void Draw_Cube()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	//угол поворота
	static float Angle = 0.0;

	//матрица вращения по оси X
	matrix4x4 MatRotateX = {
		1.0,		0.0,			0.0,			0.0,
		0.0,		cosf(Angle),	sinf(Angle),	0.0,
		0.0,		-sinf(Angle),	cosf(Angle),	0.0,
		0.0,		0.0,			0.0,			1.0 };

	//матрица вращения по оси Y
	matrix4x4 MatRotateY = {
		cosf(Angle),	0.0,	-sinf(Angle),	0.0,
		0.0,			1.0,	0.0,			0.0,
		sinf(Angle),	0.0,	cosf(Angle),	0.0,
		0.0,			0.0,	0.0,			1.0 };

	//добавляем угол поворота
	Angle = Angle + PI / 100.0f;
	if(Angle > PI2)
		Angle = 0.0f;
	
	//куб имеет 8 вершин
	for ( int i = 0; i < 8; i++ )
	{
		matrix4x4 MatRes = Mat4x4_Mat4x4_Mul(MatRotateX, MatRotateY);
		vector3 Vec = Vec3_Mat4x4_Mul(g_VertBuff[i], MatRes);

		Vec.z = Vec.z + 15.0f;
		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;
		Vec.x = Vec.x / ((float)Rc.right / Rc.bottom);

		Vec.x = Vec.x * Rc.right / 2.0f + Rc.right / 2.0f;
		Vec.y =-Vec.y * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

		g_VertBuffTransformed[i] = Vec;
	}

	HDC hDC = GetDC(g_hWnd);

	//черным цветом будем очищать экран- создаем кисть
	HBRUSH hBrush = CreateSolidBrush( RGB ( 0, 0, 0) );

	//очищаем экран для следующего кадра
	FillRect(hDC, &Rc, hBrush);

	//выбираем толщину и цвет линий куба - создаем перо
	HPEN hPen = CreatePen(PS_SOLID, 4, RGB( 255, 255, 127 ) );
	HPEN hOldPen = (HPEN) SelectObject(hDC, hPen);

	//куб состоит из 12 линий
	for ( int i = 0; i < 12; i++ )
	{
		//получаем две точки из буфера
		vector3 Vec1 = g_VertBuffTransformed[ g_IndexBuff[ i * 2 + 0 ] ];
		vector3 Vec2 = g_VertBuffTransformed[ g_IndexBuff[ i * 2 + 1 ] ];

		//проводим линию
		MoveToEx(hDC, (int)Vec1.x, (int)Vec1.y, NULL);
		LineTo(hDC, (int)Vec2.x, (int)Vec2.y);
	}

	DeleteObject(hBrush);
	
	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
	
	ReleaseDC(g_hWnd, hDC);

	//делаем задержку перед выводом следующего кадра
	Sleep(25);
}

int main ()
{
	g_hWnd = GetConsoleWindow();

	while ( !_kbhit() )
	{
		Draw_Cube();
	}

	return 0;
}


