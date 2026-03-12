#include <windows.h>
#include <math.h>

#include <vfw.h> //для DrawDibDraw
#pragma comment(lib, "Vfw32.lib") //для DrawDibDraw

#define BITS_PER_PIXEL	32


HWND g_hWnd;

BITMAPINFOHEADER m_Bih;
HDRAWDIB m_hDD;
LPBYTE m_Data;
LPBYTE m_DataTemp;

ULONG m_ViewWidth;
ULONG m_ViewHeight;

void Create_BackBuffer()
{
	RECT Rc;
    ::GetClientRect( g_hWnd, &Rc );

    m_ViewWidth  = Rc.right;
    m_ViewHeight = Rc.bottom;

	DWORD m_dwSize = m_ViewWidth * (BITS_PER_PIXEL >> 3) * m_ViewHeight;

	m_Data = (LPBYTE)malloc(m_dwSize*sizeof(BYTE));

	memset(&m_Bih, 0, sizeof(BITMAPINFOHEADER));
	m_Bih.biSize = sizeof(BITMAPINFOHEADER);
	m_Bih.biWidth = m_ViewWidth;
	m_Bih.biHeight = m_ViewHeight;
	m_Bih.biPlanes = 1;
	m_Bih.biBitCount = BITS_PER_PIXEL;
	m_Bih.biCompression = BI_RGB;
	m_Bih.biSizeImage = m_dwSize;

	m_hDD = DrawDibOpen();
}

void Clear_BackBuffer()
{
	for ( UINT i = 0; i <  m_ViewHeight; i++)
	{
		for ( UINT j = 0; j < m_ViewWidth; j++ )
		{
			int Index = i * 4 * m_ViewWidth + j * 4;

			m_Data[Index + 0] = (BYTE)(255.0 * 0.3f); // blue
			m_Data[Index + 1] = (BYTE)(255.0 * 0.125f); // green
			m_Data[Index + 2] = 0; // red
			m_Data[Index + 3] = 0; 
		}
	}
}

void Present_BackBuffer()
{
	//переворачиваем задний буфер
	DWORD m_dwSize = m_ViewWidth * (BITS_PER_PIXEL >> 3) * m_ViewHeight;
	
	LPBYTE m_DataTemp = (LPBYTE)malloc(m_dwSize * sizeof(BYTE));

	for (UINT h = 0; h < m_ViewHeight; h++ )
	{
		for (UINT w = 0; w < m_ViewWidth; w++)
		{
			int Index = h * 4 * m_ViewWidth + w * 4;

			BYTE b = m_Data[Index + 0]; // blue
			BYTE g = m_Data[Index + 1]; // green
			BYTE r = m_Data[Index + 2]; // red
			
			int IndexTemp = (m_ViewHeight - 1 - h) * 4 * m_ViewWidth + w * 4;
			m_DataTemp[IndexTemp + 0] = b;
			m_DataTemp[IndexTemp + 1] = g;
			m_DataTemp[IndexTemp + 2] = r;
			m_DataTemp[IndexTemp + 3] = 0;
		}
	}

	HDC hDC = GetDC(g_hWnd);
	DrawDibDraw(m_hDD, hDC, 0, 0, m_ViewWidth, m_ViewHeight, &m_Bih, m_DataTemp, 0, 0, m_ViewWidth, m_ViewHeight, 0);
	ReleaseDC(g_hWnd, hDC);

	free(m_DataTemp);
}
	
void Delete_BackBuffer()
{
	DrawDibClose(m_hDD);

	free(m_Data);
	m_Data = NULL;
}



//---------------------------- GAME 

#define MAP_X 12
#define MAP_Y 12

int map[12][12] =
{
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1}
};

double playerX = 2.5;
double playerY = 2.5;
double playerAngle = 0.0;

void Draw_Scene()
{
    //HDC hDC = GetDC(g_hWnd);

    RECT Rc;
    GetClientRect(g_hWnd, &Rc);

    int screenWidth  = Rc.right;
    int screenHeight = Rc.bottom;

    double FOV = 60.0 * 3.14159265 / 180.0;

    for(int x = 0; x < screenWidth; x++)
    {
        double rayAngle = playerAngle - FOV/2.0 + (FOV * x) / screenWidth;

		//направление луча в координатах карты
        double rayDirX = cos(rayAngle);
        double rayDirY = sin(rayAngle);

        double rayX = playerX;
        double rayY = playerY;

        double step = 0.02;
        int hit = 0;

        while(!hit)
        {
			//добавляем к текущей позиции луча малый шаг в направлении луча
			//цель — определить, где луч столкнётся со стеной.
            rayX += rayDirX * step;
            rayY += rayDirY * step;

            int mapX = (int)rayX;
            int mapY = (int)rayY;

            if(mapX < 0 || mapX >= MAP_X || mapY < 0 || mapY >= MAP_Y)
                break;

            if(map[mapY][mapX] == 1)
                hit = 1;
        }

        double dx = rayX - playerX;
        double dy = rayY - playerY;

        double distance = sqrt(dx*dx + dy*dy);

        distance *= cos(rayAngle - playerAngle);

        if(distance < 0.1)
            distance = 0.1;

        int wallHeight = (int)(screenHeight / distance);

        int start = screenHeight/2 - wallHeight/2;
        int end   = screenHeight/2 + wallHeight/2;

        if(start < 0) start = 0;
        if(end >= screenHeight) end = screenHeight-1;

        for(int y = start; y <= end; y++)
        {
            //SetPixel(hDC, x, y, RGB(255,255,255));

			int Index =  y * 4 * m_ViewWidth + x * 4;

			m_Data[Index + 0] = (BYTE) 127; // blue
			m_Data[Index + 1] = (BYTE) 255; // green
			m_Data[Index + 2] = (BYTE) 255; // red
			m_Data[Index + 3] = 0; 
			
        }
    }

    //ReleaseDC(g_hWnd, hDC);
}

void Update_Player()
{
    double moveSpeed = 0.05;
    double rotSpeed  = 0.04;

    //поворот
    if(GetAsyncKeyState(VK_LEFT))
    {
        playerAngle -= rotSpeed;
    }

    if(GetAsyncKeyState(VK_RIGHT))
    {
        playerAngle += rotSpeed;
    }

    double dirX = cos(playerAngle);
    double dirY = sin(playerAngle);

    //вперед
    if(GetAsyncKeyState(VK_UP))
    {
        double nx = playerX + dirX * moveSpeed;
        double ny = playerY + dirY * moveSpeed;

        if(map[(int)ny][(int)nx] == 0)
        {
            playerX = nx;
            playerY = ny;
        }
    }

    //назад
    if(GetAsyncKeyState(VK_DOWN))
    {
        double nx = playerX - dirX * moveSpeed;
        double ny = playerY - dirY * moveSpeed;

        if(map[(int)ny][(int)nx] == 0)
        {
            playerX = nx;
            playerY = ny;
        }
    }
}


//---------------------------- GAME



LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT uMsg,
						 WPARAM wParam,
						 LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
	
}

int PASCAL WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	/*
	WNDCLASS wcl = {0};
	//wcl.style = CS_HREDRAW | CS_VREDRAW;
	wcl.lpfnWndProc = WndProc;
	//wcl.cbClsExtra = 0L;
	//wcl.cbWndExtra = 0L;
	wcl.hInstance = hInstance;
	//wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	//wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wcl.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	//wcl.lpszMenuName = NULL;
	wcl.lpszClassName = "Sample";
	*/

	/*
	WNDCLASS wcl = { CS_HREDRAW|CS_VREDRAW, WndProc,
			0, 0, hInstance, NULL, LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_WINDOW+1),
			NULL, "Sample"};

	*/

	/*
	WNDCLASS wcl = { 0, WndProc, 0, 0, hInstance, 0, 0, 0, 0, "Sample"};


	if(!RegisterClass(&wcl))
		return 0;

	hWnd = CreateWindow("Sample", "Sample Application",
					WS_OVERLAPPEDWINDOW,
					0, 0,
					640, 480,
					NULL,
					NULL,
					hInstance,
					NULL);
	if(!hWnd)
		return 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	*/


	WNDCLASS wcl = { 0, WndProc, 0, 0, hInstance, 0, 0, 0, 0, "Sample"};

	if(!RegisterClass(&wcl))
		return 0;
    
	g_hWnd = CreateWindow("Sample", "Sample App", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
                             NULL, NULL, hInstance, NULL);
    
	ShowWindow(g_hWnd, nCmdShow);



	Create_BackBuffer();

	MSG msg;

	while(true)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message ==	WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(GetKeyState(VK_ESCAPE) & 0xFF00)
			break;

		Update_Player();
		Clear_BackBuffer();
        Draw_Scene();
		Present_BackBuffer();


        Sleep(16);


	}
	
	Delete_BackBuffer();

	DestroyWindow(g_hWnd);
	//UnregisterClass("Sample", hInstance);
	//UnregisterClass(wcl.lpszClassName, wcl.hInstance);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return (int)msg.wParam;
}