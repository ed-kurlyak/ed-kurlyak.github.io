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
		
		Vec.x = Vec.x * Rc.bottom / 2.0f / Vec.z + Rc.right / 2.0f;
		Vec.y =-Vec.y * Rc.bottom / 2.0f / Vec.z + Rc.bottom / 2.0f;

		g_VertBuffTransformed[i] = Vec;
	}

	HDC hDC = GetDC(g_hWnd);

	HBRUSH hBrush = CreateSolidBrush( RGB ( 0, 0, 0) );
	HBRUSH hOldBrush = (HBRUSH) SelectObject(hDC, hBrush);

	Rectangle(hDC, 0, 0, Rc.right, Rc.bottom);

	HPEN hPen = CreatePen(PS_SOLID, 4, RGB( 255, 255, 127 ) );
	HPEN hOldPen = (HPEN) SelectObject(hDC, hPen);

	for ( int i = 0; i < 12; i++ )
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

int main ()
{
	g_hWnd = GetConsoleWindow();

	while ( !_kbhit() )
	{
		Draw_Cube();
	}

	return 0;
}


