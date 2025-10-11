#include <windows.h>
#include <math.h>

HWND hWnd;

int nVertCount;
int nTriangleCount;

struct vertex {
	float x,y,z;

	vertex operator - (vertex v)
	{
		vertex t;

		t.x = x - v.x;
		t.y = y - v.y;
		t.z = z - v.z;

		return t;
	}

};

float Vec3_Dot(vertex v1, vertex v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vertex Vec3_Normalize(vertex v)
{
	float len = sqrtf(v.x * v.x + v.y * v.y + v.z *v.z);

	vertex t;

	t.x = v.x / len;
	t.y = v.y / len;
	t.z = v.z / len;

	return t;
}

vertex Vec3_Cross(vertex v1, vertex v2)
{
	vertex t;

	t.x = v1.y * v2.z - v1.z * v2.y;
	t.y = v1.z * v2.x - v1.x * v2.z;
	t.z = v1.x * v2.y - v1.y * v2.x;

	return t;
}

vertex *vert_buff;
vertex *vert_buff_transformed;
unsigned int *index_buff;

typedef float matrix4x4[4][4];

void Delete_Torus()
{
	delete [] vert_buff;
	delete [] vert_buff_transformed;
	delete [] index_buff;

}

HDC hBackBuffer;
HBITMAP hBitmap;
HBITMAP hOldBitmap;

#define FCMP(a,b) ( (fabs(a-b) < 0.005f) ? 1 : 0 )

void Create_BackBuffer()
{
	HDC hDC = GetDC(hWnd);
	hBackBuffer = CreateCompatibleDC(hDC);
	hBitmap = CreateCompatibleBitmap(hDC, 640, 480);
	hOldBitmap = (HBITMAP) SelectObject(hBackBuffer, hBitmap);
	ReleaseDC(hWnd, hDC);
}

void Delete_BackBuffer()
{
	SelectObject(hBackBuffer, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hBackBuffer);
}

void Clear_BackBuffer()
{
	HBRUSH hBrush = CreateSolidBrush( RGB(255,255,255) );
	HBRUSH hOldBrush = (HBRUSH) SelectObject(hBackBuffer, hBrush);
	Rectangle(hBackBuffer, 0, 0, 640, 480);
	SelectObject(hBackBuffer, hOldBrush);
	DeleteObject(hBrush);
}

void Present_BackBuffer()
{
	HDC hDC = GetDC(hWnd);
	BitBlt(hDC, 0, 0, 640, 480, hBackBuffer, 0, 0, SRCCOPY);
	ReleaseDC(hWnd, hDC);
}

vertex Vec3_Mat4x4_Mul ( vertex v, matrix4x4 m)
{
	vertex t;

	t.x =	v.x * m[0][0] +
			v.y * m[1][0] +
			v.z * m[2][0] +
				  m[3][0];

	t.y =	v.x * m[0][1] +
			v.y * m[1][1] +
			v.z * m[2][1] +
				  m[3][1];

	t.z =	v.x * m[0][2] +
			v.y * m[1][2] +
			v.z * m[2][2] +
				  m[3][2];

	return t;
}

void Init_Torus()
{
	
	int rings = 24;
	int sides = 12;

	nTriangleCount = 2 * sides * rings;
    //одно дополнительное кольцо для дублирования первого кольца
	nVertCount  = sides * (rings+1);

	vert_buff = new vertex[nVertCount];
	vert_buff_transformed = new vertex[nVertCount];

	float outerRadius = 8; //внешний радиус (общий)
	float innerRadius = 4; //радиус трубки

	float PI2 = 3.1415926f * 2.0f;

	float ringFactor = PI2 / rings;
	float sideFactor = PI2 / sides;

	int idx = 0;

    for( int ring = 0; ring <= rings; ring++ )
    {
        float u = ring * ringFactor;
        float cu = cosf(u);
        float su = sinf(u);

        for( int side = 0; side < sides; side++ )
        {
            float v = side * sideFactor;
            float cv = cosf(v);
            float sv = sinf(v);
            float r = (outerRadius + innerRadius * cv);
            vert_buff[idx].x = r * cu;
            vert_buff[idx].y = r * su;
            vert_buff[idx].z = innerRadius * sv;
            /*
			norm_buff[idx].x = cv * cu * r;
            norm_buff[idx].y = cv * su * r;
            norm_buff[idx].z = sv * r;

			tex[idx].tu = u / PI2;
			tex[idx].tv = v / PI2;
            
            // Normalize
            float len = sqrtf( norms[idx].x * norms[idx].x +
                              norms[idx].y * norms[idx].y +
                              norms[idx].z * norms[idx].z );
            
			norms[idx].x /= len;
            norms[idx].y /= len;
            norms[idx].z /= len;
			*/
            idx ++;
        }
    }

	index_buff = new unsigned int[nTriangleCount * 3];

    idx = 0;

    for( int ring = 0; ring < rings; ring++ )
    {
        int ringStart = ring * sides;
        int nextRingStart = (ring + 1) * sides;
    
        for( int side = 0; side < sides; side++ )
        {
            int nextSide = (side+1) % sides;
            // квадрат - два треугольника
            index_buff[idx] = (ringStart + side);
            index_buff[idx+1] = (nextRingStart + side);
            index_buff[idx+2] = (nextRingStart + nextSide);
            index_buff[idx+3] = ringStart + side;
            index_buff[idx+4] = nextRingStart + nextSide;
            index_buff[idx+5] = (ringStart + nextSide);
            
            idx += 6;
        }
    }

}

void Draw_Torus()
{
	static float angle = 0.0f;

	matrix4x4 mxRotateY = {
	cosf(angle),	0.0,	sinf(angle),	0.0,
	0.0,			1.0,	0.0,			0.0,
	-sinf(angle),	0.0,	cosf(angle),	0.0,
	0.0,			0.0,	0.0,			1.0f };

	matrix4x4 mxRotateX = {
	1.0,			0.0,			0.0,			0.0,
	0.0,			cosf(angle),	sinf(angle),	0.0,
	0.0,			-sinf(angle),	cosf(angle),	0.0,
	0.0,			0.0,			0.0,			1.0f };

	angle = angle + (3.1415926f / 100.0f);
	if(angle > (3.1415926f * 2.0f) )
		angle = 0.0f;

	for ( int i = 0; i < nVertCount; i++ )
	{
		vertex v = Vec3_Mat4x4_Mul(vert_buff[i], mxRotateX);
		v = Vec3_Mat4x4_Mul(v, mxRotateY);

		v.x = v.x / (640.0f / 480.0f);
		v.z = v.z + 25.0f;
		v.x = v.x / v.z;
		v.y = v.y / v.z;

		v.x = v.x * 640.0f / 2.0f + 640.0f / 2.0f;
		v.y =-v.y * 480.0f / 2.0f + 480.0f / 2.0f;

		vert_buff_transformed[i] = v;
	}

	Clear_BackBuffer();
	
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0) );
	HPEN hOldPen = (HPEN) SelectObject(hBackBuffer, hPen);

	for ( int i = 0; i < nTriangleCount; i++ )
	{
		vertex v1 = vert_buff_transformed[index_buff[i*3]];
		vertex v2 = vert_buff_transformed[index_buff[i*3 + 1]];
		vertex v3 = vert_buff_transformed[index_buff[i*3 + 2]];

		if(FCMP(v1.x,v3.x) && FCMP(v1.y, v3.y))
			continue;
		if(FCMP(v2.x,v3.x) && FCMP(v2.y, v3.y))
			continue;

		vertex edge1, edge2, cross;

		edge1 = v2 - v1;
		edge2 = v3 - v1;

		edge1 = Vec3_Normalize(edge1);
		edge2 = Vec3_Normalize(edge2);

		cross = Vec3_Cross(edge2, edge1);
		cross = Vec3_Normalize(cross);

		vertex look = {0.0f, 0.0f, -1.0f};

		float dot = Vec3_Dot(cross, look);

		if(dot <= 0.0f)
			continue;

		MoveToEx(hBackBuffer, (int) v1.x, (int) v1.y, NULL);
		LineTo(hBackBuffer, (int) v2.x, (int) v2.y);
		LineTo(hBackBuffer, (int) v3.x, (int) v3.y);
		LineTo(hBackBuffer, (int) v1.x, (int) v1.y);
	}

	SelectObject(hBackBuffer, hOldPen);
	DeleteObject(hPen);

	Present_BackBuffer();

	Sleep(50);
}

LRESULT CALLBACK WndProc ( HWND hWnd,
						  UINT uMsg,
						  WPARAM wParam,
						  LPARAM lParam )
{
	switch ( uMsg )
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain ( HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow )
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WNDCLASS wcl;
	wcl.style = CS_HREDRAW | CS_VREDRAW;
	wcl.lpfnWndProc = WndProc;
	wcl.cbClsExtra = 0L;
	wcl.cbWndExtra = 0L;
	wcl.hInstance = hInstance;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wcl.lpszMenuName = NULL;
	wcl.lpszClassName = "Sample";

	if ( !RegisterClass(&wcl) )
		return 0;

	hWnd = CreateWindow("Sample", "Sample Application",
					WS_OVERLAPPEDWINDOW,
					0, 0,
					640, 480,
					NULL,
					NULL,
					hInstance,
					NULL);

	if ( !hWnd )
		return 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	Init_Torus();
	Create_BackBuffer();

	MSG msg;

	while ( true )
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if ( GetKeyState(VK_ESCAPE) & 0xFF00 )
			break;

		Draw_Torus();

	}

	Delete_BackBuffer();
	Delete_Torus();

	DestroyWindow(hWnd);
	UnregisterClass("Sample", hInstance);

	return 0;
}