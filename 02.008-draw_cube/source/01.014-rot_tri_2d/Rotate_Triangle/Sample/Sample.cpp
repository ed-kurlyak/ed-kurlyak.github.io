#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <math.h>
#include <conio.h>

#define PI 3.1415926535897932384626433f

#define PI2 (PI * 2.0f)

HWND g_hWnd;

struct Point
{
	float x,y;
};

typedef float matrix2x2[2][2];

Point Point_Mat2x2_Mul(Point &PointIn, matrix2x2 MatIn)
{
	Point PointOut;

	PointOut.x =	PointIn.x * MatIn[0][0] +
					PointIn.y * MatIn[1][0];

	PointOut.y =	PointIn.x * MatIn[0][1] +
					PointIn.y * MatIn[1][1];

	return PointOut;
}

void Rotate_Triangle()
{
	static float Angle = 0.0;

	matrix2x2 MatRotateZ = {
	cosf(Angle), sinf(Angle),
	-sinf(Angle), cosf(Angle) };

	Angle = Angle + (PI / 100.0f);
	if(Angle > PI2)
		Angle = 0.0f;
	
	float Side = 240.0f;
	float Height = sqrt(3.0f) / 2.0f * Side;

	Point PointSource1 = { 0, (2.0f / 3.0f * Height) };     // верхн€€ вершина
	Point PointSource2 = { -(Side / 2), -(1.0f / 3.0f * Height) };  // нижн€€ лева€
	Point PointSource3 = { (Side / 2), -(1.0f / 3.0f * Height) };   // нижн€€ права€

	/*
	Point PointSource1 = { 0,      +138 };  // 2/3 * Height
	Point PointSource2 = { -120,   -69 };   // -1/3 * Height
	Point PointSource3 = { +120,   -69 };
	*/

	Point PointTransformed1 = Point_Mat2x2_Mul(PointSource1, MatRotateZ);
	Point PointTransformed2 = Point_Mat2x2_Mul(PointSource2, MatRotateZ);
	Point PointTransformed3 = Point_Mat2x2_Mul(PointSource3, MatRotateZ);

	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	POINT PointScreen[3];

	PointScreen[0].x = (long) (PointTransformed1.x + Rc.right / 2.0f);
	PointScreen[0].y = (long) (-PointTransformed1.y + Rc.bottom / 2.0f);

	PointScreen[1].x = (long) (PointTransformed2.x + Rc.right / 2.0f);
	PointScreen[1].y = (long) (-PointTransformed2.y + Rc.bottom / 2.0f);

	PointScreen[2].x = (long) (PointTransformed3.x + Rc.right / 2.0f);
	PointScreen[2].y = (long) (-PointTransformed3.y + Rc.bottom / 2.0f);
	
	HDC hDC = GetDC(g_hWnd);

	//очистка экрана черным цветом
	HBRUSH hBrushClear = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(hDC, &Rc, hBrushClear);

	//создаем кисть и перо дл€ заливки треугольника
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 127));
	HPEN hOldBrush = (HPEN) SelectObject(hDC, hBrush);

	HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	HPEN hOldPen = (HPEN) SelectObject(hDC, hPen);

	Polygon(hDC, PointScreen, 3);

	SelectObject(hDC, hPen);
	DeleteObject(hPen);
	SelectObject(hDC, hOldBrush);
	DeleteObject(hBrush);

	DeleteObject(hBrushClear);

	ReleaseDC(g_hWnd, hDC);

	Sleep(25);
}

int main(void) 
{ 

	g_hWnd = GetConsoleWindow();

	while(!_kbhit())
	{
		Rotate_Triangle();
	}

	return 0;
}



