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

struct quaternion
{
    float w, x, y, z;

    quaternion(float w = 1, float x = 0, float y = 0, float z = 0) : w(w), x(x), y(y), z(z) {}

    quaternion operator*(const quaternion& q) const
    {
        return quaternion(
            w * q.w - x * q.x - y * q.y - z * q.z,
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y - x * q.z + y * q.w + z * q.x,
            w * q.z + x * q.y - y * q.x + z * q.w
        );
    }

    quaternion conjugate() const
    {
        return quaternion(w, -x, -y, -z);
    }
};

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

vector3 Vec3_RotateY(const vector3& VecIn, float Angle)
{
    // Создаем кватернион вращения вокруг оси Y
    float halfAngle = Angle / 2.0f;
    float sinHalfAngle = sinf(halfAngle);
    quaternion rotation(cosf(halfAngle), 0, sinHalfAngle, 0);

	// Создаем кватернион вращения вокруг оси X
	//quaternion rotation(cosf(halfAngle), sinHalfAngle, 0, 0);

	// Создаем кватернион вращения вокруг оси Z
	//quaternion rotation(cosf(halfAngle), 0, 0, sinHalfAngle);

    // Преобразуем вектор в кватернион
    quaternion vecQuat(0, VecIn.x, VecIn.y, VecIn.z);

    // Выполняем вращение: rotation * vecQuat * conjugate(rotation)
    quaternion resultQuat = rotation * vecQuat * rotation.conjugate();

    // Возвращаем обратно вектор
	vector3 VecOut = {resultQuat.x, resultQuat.y, resultQuat.z};
    
	return VecOut;
}

void Draw_Cube()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	static float Angle = 0.0;

	for ( int i = 0; i < 8; i++ )
	{
		vector3 Vec = Vec3_RotateY(g_VertBuff[i], Angle);

		Vec.z = Vec.z + 15.0f;
		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;
		Vec.x = Vec.x / ((float)Rc.right / Rc.bottom);

		Vec.x = Vec.x * Rc.right / 2.0f + Rc.right / 2.0f;
		Vec.y =-Vec.y * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

		g_VertBuffTransformed[i] = Vec;
	}

	Angle = Angle + PI / 100.0f;
	if(Angle > PI2)
		Angle = 0.0f;


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


