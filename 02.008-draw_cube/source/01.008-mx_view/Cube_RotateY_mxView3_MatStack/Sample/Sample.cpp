//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <conio.h>
#include <math.h>

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

HWND g_hWnd = NULL;
HDC g_hDC = NULL;

struct vector3
{
	float x,y,z;
};

enum { A, B, C, D, E, F, G, H };

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

/*
	CUBE VERTICES

	FONT SIDE	BACK SIDE
	C - D		G - H
	|   |		|   |
	A - B		E - F
*/

unsigned int g_IndexBuff[24] = {
	//LINES CONNECT FRONT SIDE VERTICES
	A, B,
	B, D,
	D, C,
	C, A,

	//LINES CONNECT BACK SIDE VERTICES
	E, F,
	F, H,
	H, G,
	G, E,

	//LINES CONNECT FRONT AND BACK SIDE
	A, E,
	B, F,
	D, H,
	C, G };

struct matrix4x4
{
	float Mat[4][4];
};

matrix4x4 * g_MatrixPtr;

#define MAX_MATRICES 40

matrix4x4 m_MatrixStack[MAX_MATRICES] = { 0 };

void ResetMatrixStack()
{
    g_MatrixPtr = &m_MatrixStack[0];
}

void PushMatrix()
{
    g_MatrixPtr++;
    g_MatrixPtr[0] = g_MatrixPtr[-1];
}

void PushUnitMatrix()
{
	matrix4x4 MatrixUnit = {
		1.0f,	0.0f,	0.0f,	0.0f,
		0.0f,	1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f };

    g_MatrixPtr++;
    g_MatrixPtr[0] = MatrixUnit;
}

void PopMatrix()
{
    g_MatrixPtr--;
}

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

float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2)
{
	return VecIn1.x * VecIn2.x + VecIn1.y * VecIn2.y + VecIn1.z * VecIn2.z;
}

vector3 Vec3_Normalize(vector3 &VecIn1)
{
	float Len = sqrtf( (VecIn1.x * VecIn1.x) + (VecIn1.y * VecIn1.y) + (VecIn1.z * VecIn1.z) );
	
	vector3 t = { VecIn1.x / Len, VecIn1.y / Len, VecIn1.z / Len };

	return t;
}

vector3 Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut = { VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y,
		VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z,
		VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x };

	return VecOut;
}

void Get_Matrix_View(vector3 &VecCubePos)
{
	//MATRIX VIEW CALCULATION
	vector3 VecCamRight = { 1.0f, 0.0f, 0.0 };
	vector3 VecCamUp = { 0.0f, 1.0f, 0.0f };
	vector3 VecCamPos = { 0.0f, 0.0f, 0.0f };
	
	//камера смотрит в центр экрана по оси X
	vector3 VecCamLook = { 0.0f,
							VecCubePos.y - VecCamPos.y,
							VecCubePos.z - VecCamPos.z};

	VecCamLook = Vec3_Normalize(VecCamLook);

	VecCamUp = Vec3_Cross(VecCamLook, VecCamRight);
	VecCamUp = Vec3_Normalize(VecCamUp);
	VecCamRight = Vec3_Cross(VecCamUp, VecCamLook);
	VecCamRight = Vec3_Normalize(VecCamRight);

	float Px = -Vec3_Dot(VecCamPos, VecCamRight);
	float Py = -Vec3_Dot(VecCamPos, VecCamUp);
	float Pz = -Vec3_Dot(VecCamPos, VecCamLook);

	matrix4x4 MatView = {
		VecCamRight.x,		VecCamUp.x,	VecCamLook.x,		0.0f,
		VecCamRight.y,		VecCamUp.y,	VecCamLook.y,		0.0f,
		VecCamRight.z,		VecCamUp.z,	VecCamLook.z,		0.0f,
		Px,				Py,			Pz,				1.0f };

	memcpy(g_MatrixPtr, &MatView, sizeof(float) * 16);

}

void Matrix_Rotate()
{
	static float Angle = 0.0;

	matrix4x4 MatRotateY = {
		cosf(Angle),	0.0,	-sinf(Angle),	0.0,
		0.0,			1.0,	0.0,			0.0,
		sinf(Angle),	0.0,	cosf(Angle),	0.0,
		0.0,			0.0,	0.0,			1.0 };

	Angle = Angle + PI / 100.0f;
	if(Angle > PI2)
		Angle = 0.0f;

	*g_MatrixPtr = Mat4x4_Mat4x4_Mul(MatRotateY, *g_MatrixPtr);
}

void Matrix_Translate(vector3 &VecCubePos)
{
	
	//MATRIX WORLD
	matrix4x4 MatWorld = {
		1.0f, 0.0, 0.0, 0.0,
		0.0, 1.0f, 0.0, 0.0,
		0.0, 0.0, 1.0f, 0.0,
		VecCubePos.x, VecCubePos.y, VecCubePos.z, 1.0f }; 

	*g_MatrixPtr = Mat4x4_Mat4x4_Mul(MatWorld, *g_MatrixPtr);
}

void Draw_Cube_Main()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	//берем Fov не слишком большое что бы
	//по углам экрана небыло проекционных искажений
	//моделей куба
	float Fov = PI / 3.0f; // 60 градусов

	for ( int i = 0; i < 8; i++ )
	{
		vector3 Vec = Vec3_Mat4x4_Mul(g_VertBuff[i], *g_MatrixPtr);

		//учитываем Fov - угол обзора камеры
		//это вычисление равнозначно Vec.x * 1.0 / tan(Fov / 2.0f);
		//так как cos делить на sin будет 1 / tan
		Vec.x = Vec.x * (cosf(Fov / 2.0f) / sinf(Fov / 2.0f));
		Vec.y = Vec.y * (cosf(Fov / 2.0f) / sinf(Fov / 2.0f));

		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;

		Vec.x = Vec.x / ( (float) Rc.right / Rc.bottom );

		Vec.x = Vec.x * Rc.right / 2.0f + Rc.right / 2.0f;
		Vec.y =-Vec.y * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

		g_VertBuffTransformed[i] = Vec;
	}


	for ( int i = 0; i < 12; i++ )
	{
		vector3 Vec1 = g_VertBuffTransformed[ g_IndexBuff[ i * 2 + 0] ];
		vector3 Vec2 = g_VertBuffTransformed[ g_IndexBuff[ i * 2 + 1 ] ];

		MoveToEx(g_hDC, (int)Vec1.x, (int)Vec1.y, NULL);
		LineTo(g_hDC, (int)Vec2.x, (int)Vec2.y);
	}


}

void Draw_Cube()
{

	//готовим контекст рисования
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	g_hDC = GetDC(g_hWnd);

	HBRUSH hBrush = CreateSolidBrush( RGB ( 0, 0, 0) );
	HBRUSH hOldBrush = (HBRUSH) SelectObject(g_hDC, hBrush);

	Rectangle(g_hDC, 0, 0, Rc.right, Rc.bottom);

	HPEN hPen = CreatePen(PS_SOLID, 4, RGB( 255, 255, 127 ) );
	HPEN hOldPen = (HPEN) SelectObject(g_hDC, hPen);

	//задаем позицию для первого и второго куба
	vector3 VecCube1Pos = {-12.0f, -7.0f, 25.0f};
	vector3 VecCube2Pos = {12.0f, -7.0f, 25.0f};

	//начинаем манипуляции с матрицей вида
	ResetMatrixStack();

	Get_Matrix_View(VecCube1Pos);

	//рисуем куб 1

	PushMatrix();

	//в этом примере другой порядок умножения матриц
	//поэтому сначала перемещаем куб затем вращаем
	
	//перемещаем куб на сцене
	Matrix_Translate(VecCube1Pos);	

	//вращаем куб на сцене
	Matrix_Rotate();

	Draw_Cube_Main();

	PopMatrix();

	//рисуем куб 2

	PushMatrix();

	//в этом примере другой порядок умножения матриц
	//поэтому сначала перемещаем куб затем вращаем

	//перемещаем куб на сцене
	Matrix_Translate(VecCube2Pos);	

	//вращаем куб на сцене
	Matrix_Rotate();

	Draw_Cube_Main();

	PopMatrix();

	//очиста контекста рисования
	SelectObject(g_hDC, hOldBrush);
	DeleteObject(hBrush);
	
	SelectObject(g_hDC, hOldPen);
	DeleteObject(hPen);
	
	ReleaseDC(g_hWnd, g_hDC);

	//делаем паузу между кадрами
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