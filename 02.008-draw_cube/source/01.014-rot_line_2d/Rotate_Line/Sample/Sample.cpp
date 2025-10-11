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

void Rotate_Line()
{
	static float Angle = 0.0;

	matrix2x2 MatRotateZ = {
	cosf(Angle), sinf(Angle),
	-sinf(Angle), cosf(Angle) };

	Angle = Angle + (PI / 100.0f);
	if(Angle > PI2)
		Angle = 0.0f;
	
	Point Point1 = {0, 20}; //start point
	Point Point2 = {0, 150}; //end point
	
	//умножение на матрицу поворота
	Point PointTransformed1 = Point_Mat2x2_Mul(Point1, MatRotateZ);
	Point PointTransformed2 = Point_Mat2x2_Mul(Point2, MatRotateZ);

	//преобразование в экранные координаты
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);
	int StartX = (int)(Rc.right/2 + PointTransformed1.x);
	int StartY = (int)(Rc.bottom/2 - PointTransformed1.y);
	int EndX = (int)(Rc.right/2 + PointTransformed2.x);
	int EndY = (int)(Rc.bottom/2  - PointTransformed2.y);

	//рисуем линию
	HDC hDC = GetDC(g_hWnd);

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(hDC, &Rc, hBrush);

	HPEN hPen = CreatePen(PS_SOLID, 8, RGB(255, 255, 127));
	HPEN hOldPen = (HPEN) SelectObject(hDC, hPen);

	MoveToEx(hDC, StartX, StartY, NULL);
	LineTo(hDC, EndX, EndY);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);

	DeleteObject(hBrush);

	ReleaseDC(g_hWnd, hDC);

	Sleep(25);
	

}

int main(void) 
{ 

	g_hWnd = GetConsoleWindow();

	while(!_kbhit())
	{
		Rotate_Line();
	}

	return 0;
}



