//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#include <windows.h>
#include <stdio.h>

#define SWAP(a,b,t) {t=a; a=b; b=t;}

void Side_Triangle()
{

	float t;
	int side = 0;

	float x1 = 400, y1 = 100;
	float x2 = 550, y2 = 250;
	float x3 = 250, y3 = 400;

	if( y2 < y1 )
	{
		SWAP(x2, x1, t);
		SWAP(y2, y1, t);
	}

	if( y3 < y1 )
	{
		SWAP(x3, x1, t);
		SWAP(y3, y1, t);
	}

	if( y3 < y2 )
	{
		SWAP(x3, x2, t);
		SWAP(y3, y2, t);
	}

	//вычисляем какая сторона треугольника длиннее
	//общего вида треугольник с неравными сторонами
	if ( y2 > y1 && y3 > y2 )
	{
		float dxdy1 = (x2 - x1) / (y2 - y1);
		float dxdy2 = (x3 - x1) / (y3 - y1);
		side = dxdy2 > dxdy1;
	}
	
	//треугольник с плоским верхом
	if (y1 == y2)
		side = x1 > x2;

	//треугольник с плоским низом
	if (y3 == y2)
		side = x3 > x2;

	if( !side )
	{
		printf("Longer is left side\n");
	}
	else
	{
		printf("Longer is right side\n");
	}

}

int main()
{
	Side_Triangle();
	
	return 0;
}