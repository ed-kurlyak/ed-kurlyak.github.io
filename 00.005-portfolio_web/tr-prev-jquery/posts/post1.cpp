#include <windows.h>
#include <math.h>

HWND hwnd;

typedef struct slib_vector3
{
	float x,y,z;
} lib_vector3;

typedef struct slib_4x4_matrix
{
float m[4][4];
} lib_4x4_matrix;

lib_vector3 Mat4x4_Vect3_Mul( lib_4x4_matrix mat, lib_vector3 v )
{
lib_vector3 temp;

temp.x = v.x * mat.m[0][0] + v.y * mat.m[1][0] + v.z * mat.m[2][0] + mat.m[3][0];
temp.y = v.x * mat.m[0][1] + v.y * mat.m[1][1] + v.z * mat.m[2][1] + mat.m[3][1];
temp.z = v.x * mat.m[0][2] + v.y * mat.m[1][2] + v.z * mat.m[2][2] + mat.m[3][2];

return temp;
}

void Draw_Cube()
{

	//float angle = 3.1415926f / 4.0f;
	static float angle = 0.0;

	angle+=(float)(3.1415926/180);
    if(angle>3.1415926*2) angle =0;

	UINT nVertCount = 24;

lib_vector3 vert_buff[24] = {-5.000000, 5.000000,-5.000000,
	5.000000, 5.000000,-5.000000,
	5.000000, -5.000000,-5.000000,
	-5.000000, -5.000000,-5.000000,
	-5.000000, 5.000000, 5.000000,
	5.000000, 5.000000, 5.000000,
	5.000000, 5.000000,-5.000000,
	-5.000000, 5.000000,-5.000000,
	-5.000000, -5.000000,5.000000,
	5.000000, -5.000000,5.000000,
	5.000000, 5.000000, 5.000000,
	-5.000000, 5.000000, 5.000000,
	-5.000000, -5.000000,-5.000000,
	5.000000, -5.000000,-5.000000,
	5.000000, -5.000000, 5.000000,
	-5.000000, -5.000000, 5.000000,
	-5.000000, 5.000000, 5.000000,
	-5.000000, 5.000000,-5.000000,
	-5.000000,-5.000000,-5.000000,
	-5.000000,-5.000000, 5.000000,
	5.000000, 5.000000,-5.000000,
	5.000000, 5.000000, 5.000000,
	5.000000,-5.000000, 5.000000,
	5.000000,-5.000000,-5.000000};


	lib_4x4_matrix mWorldRotX={
    1, 0, 0, 0,
    0, cos(angle), sin(angle), 0,
    0,-sin(angle),  cos(angle), 0,
    0, 0, 0, 1};

    lib_4x4_matrix mWorldRotY={
    cos(angle), 0, -sin(angle), 0,
    0, 1, 0, 0,
    sin(angle), 0, cos(angle), 0,
    0, 0, 0, 1};

    lib_4x4_matrix mWorldRotZ={
    cos(angle), sin(angle), 0, 0,
    -sin(angle), cos(angle), 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1};

    lib_4x4_matrix mWorld={
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 15.0, 1};

	

	
	RECT rc;
    GetClientRect(hwnd, &rc);


	
	long nViewWidth  = rc.right - rc.left;
    long nViewHeight = rc.bottom - rc.top;

	long nViewX      = rc.left;
    long nViewY      = rc.top;

	float fAspect = (float) nViewWidth/nViewHeight;
    
	for ( UINT i = 0; i < nVertCount; i++)
	{
		vert_buff[i] = Mat4x4_Vect3_Mul(mWorldRotX, vert_buff[i]);
		vert_buff[i] = Mat4x4_Vect3_Mul(mWorldRotY, vert_buff[i]);
		vert_buff[i] = Mat4x4_Vect3_Mul(mWorld, vert_buff[i]);
	
		vert_buff[i].x = vert_buff[i].x / fAspect;
	
		vert_buff[i].x = vert_buff[i].x / vert_buff[i].z;
		vert_buff[i].y = vert_buff[i].y / vert_buff[i].z;

		vert_buff[i].x =   vert_buff[i].x * nViewWidth  / 2 + nViewX + nViewWidth  / 2;
	    vert_buff[i].y =  -vert_buff[i].y * nViewHeight / 2 + nViewY + nViewHeight / 2;
	}
	
	HDC hDC = ::GetDC( hwnd );

	LOGBRUSH logBrush;
	HBRUSH   hBrush = NULL;
	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = 0 ;
	hBrush = ::CreateBrushIndirect( &logBrush );
	FillRect(hDC,&rc,hBrush);

	for ( int i = 0; i < 6; i++)
	{

		HPEN hPen = CreatePen(PS_SOLID, 5, RGB(128, 0, 0));
		HPEN hOldPen = (HPEN)SelectObject (hDC, hPen);

		MoveToEx(hDC,(int)vert_buff[i * 4].x, (int)vert_buff[i * 4].y,NULL);
        
		LineTo(hDC,(int)vert_buff[i * 4 + 1].x, (int)vert_buff[i * 4 + 1].y);
		LineTo(hDC,(int)vert_buff[i * 4 + 2].x, (int)vert_buff[i * 4 + 2].y);
		LineTo(hDC,(int)vert_buff[i * 4 + 3].x, (int)vert_buff[i * 4 + 3].y);
		LineTo(hDC,(int)vert_buff[i * 4].x, (int)vert_buff[i * 4].y);

		SelectObject(hDC,hOldPen);
		DeleteObject (hPen) ;
	}

}




int  main (int argc, char* argv[])
{

	char str[256];
	SetConsoleTitle("Sowtware rendering engine (no OpenGL/DirectX)");
	GetConsoleTitle(str, 256);
	hwnd = FindWindow(NULL, str);

		MSG         msg ;
     while (TRUE)
          {
          if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
               {
               if (msg.message == WM_QUIT)
                    break ;

			   TranslateMessage (&msg) ;
               DispatchMessage (&msg) ;
               }
          else
			  {
               Draw_Cube () ;
			   Sleep(15);
			   }
          }


	return 0;
}
