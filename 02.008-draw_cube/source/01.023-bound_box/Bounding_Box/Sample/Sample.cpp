#include <windows.h>
#include <math.h>

struct vertex
{
		float x,y,z;
};

enum { A, B, C, D, E, F, G, H };

vertex vert_buff[8] = {
	-4.0, -4.0, -4.0,	//A
	 4.0, -4.0, -4.0,	//B
	-4.0,  4.0, -4.0,	//C
	 4.0,  4.0, -4.0,	//D

 	-4.0, -4.0,  4.0,	//E
	 4.0, -4.0,  4.0,	//F
	-4.0,  4.0,  4.0,	//G
	 4.0,  4.0,  4.0 };	//H

UINT index_buff[36] = {

/*
front side & back side
	C - D     G - H
	|   |     |   |
	A - B     E - F
*/

A, C, D,
A, D, B,

F, H, G,
F, G, E,

/*
left side & right side
	G - C     D - H
	|   |     |   |
	E - A     B - F
*/

E, G, C,
E, C, A,

B, D, H,
B, H, F,

/*
top side & bottom side
	G - H     E - F
	|   |     |   |
	C - D     A - B
*/

C, G, H,
C, H, D,

E, A, B,
E, B, F };

vertex m_BoundsMin = { 999999.0f, 999999.0f, 999999.0f };
vertex m_BoundsMax = { -999999.0f, -999999.0f, -999999.0f };

void GetBoundingBox(vertex v)
{

	if ( v.x < m_BoundsMin.x ) m_BoundsMin.x = v.x;
    if ( v.y < m_BoundsMin.y ) m_BoundsMin.y = v.y;
    if ( v.z < m_BoundsMin.z ) m_BoundsMin.z = v.z;
    if ( v.x > m_BoundsMax.x ) m_BoundsMax.x = v.x;
    if ( v.y > m_BoundsMax.y ) m_BoundsMax.y = v.y;
    if ( v.z > m_BoundsMax.z ) m_BoundsMax.z = v.z;

}

int PASCAL WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	//начинаем расчет bounding box

	//без индексного буфера 8 вершин
	for ( int i = 0; i < 8; i++ )
	{
		vertex v0 = vert_buff[i];
		GetBoundingBox(v0);
	}

	vertex box_min1 = m_BoundsMin;
	vertex box_max1 = m_BoundsMax;

	/*
	//для индексного буфера 12 * 3 индексов
	for ( int i = 0; i < 12; i++ )
	{
		vertex v0 = vert_buff[index_buff[i * 3 + 0]];
		vertex v1 = vert_buff[index_buff[i * 3 + 1]];
		vertex v2 = vert_buff[index_buff[i * 3 + 2]];

		GetBoundingBox(v0);
		GetBoundingBox(v1);
		GetBoundingBox(v2);
	}

	vertex box_min2 = m_BoundsMin;
	vertex box_max2 = m_BoundsMax;
	*/

	//окончили расчет bounding box

	return 0;
}