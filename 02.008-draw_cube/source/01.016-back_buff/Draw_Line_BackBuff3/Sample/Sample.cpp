//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <conio.h>

//для DrawDibDraw
#include <vfw.h>
#pragma comment(lib, "Vfw32.lib")

//глобальные переменные
HWND 	g_hWnd;
LPBYTE g_Data;
LPBYTE g_DataTemp;
BITMAPINFOHEADER g_Bih;
HDRAWDIB g_hDD;

UINT g_ViewWidth;
UINT g_ViewHeight;

#define BITS_PER_PIXEL	32

void Create_Backbuffer()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	g_ViewWidth = Rc.right;
	g_ViewHeight = Rc.bottom;
	
	DWORD m_dwSize = Rc.right * (BITS_PER_PIXEL >> 3) * Rc.bottom;

	//это наш задний буфер
	g_Data = (LPBYTE)malloc(m_dwSize*sizeof(BYTE));
	//это задний буфер для перевернутой картинки
	//так как DrawDibDraw рисует перевернутое изображение
	g_DataTemp = (LPBYTE)malloc(m_dwSize * sizeof(BYTE));

	memset(&g_Bih, 0, sizeof(BITMAPINFOHEADER));
	g_Bih.biSize = sizeof(BITMAPINFOHEADER);
	g_Bih.biWidth = Rc.right;
	g_Bih.biHeight = Rc.bottom;
	g_Bih.biPlanes = 1;
	g_Bih.biBitCount = BITS_PER_PIXEL;
	g_Bih.biCompression = BI_RGB;
	g_Bih.biSizeImage = m_dwSize;
	
	g_hDD = DrawDibOpen();
}


void Clear_Backbuffer()
{
	for ( UINT i = 0; i <  g_ViewHeight; i++)
	{
		for ( UINT j = 0; j < g_ViewWidth; j++ )
		{
			int Indx = i * 4 * g_ViewWidth + j * 4;

			g_Data[Indx] = (BYTE) (255.0 * 0.3f); // blue
			g_Data[Indx + 1] = (BYTE) (255.0 * 0.125f); // green
			g_Data[Indx + 2] = 0; // red

			g_Data[Indx + 3] = 0; 
		}
	}
}

void Present_Backbuffer()
{

	//переворачиваем задний буфер
	for (UINT h = 0; h < g_ViewHeight; h++ )
	{
		for (UINT w = 0; w < g_ViewWidth; w++)
		{
			int Indx = h * 4 * g_ViewWidth + w * 4;
			int IndxTemp = (g_ViewHeight - 1 - h) * 4 * g_ViewWidth + w * 4;

			g_DataTemp[IndxTemp + 0] = g_Data[Indx + 0];
			g_DataTemp[IndxTemp + 1] = g_Data[Indx + 1];
			g_DataTemp[IndxTemp + 2] = g_Data[Indx + 2];
			g_DataTemp[IndxTemp + 3] = 0;
		}
	}

	HDC hDC = GetDC(g_hWnd);
	DrawDibDraw(g_hDD, hDC, 0, 0, g_ViewWidth, g_ViewHeight, &g_Bih, g_DataTemp, 0, 0, g_ViewWidth, g_ViewHeight, 0);
	ReleaseDC(g_hWnd, hDC);
}


void Delete_Backbuffer()
{
	DrawDibClose(g_hDD);

	free(g_Data);
	g_Data = NULL;
}


void Draw_Line()
{
	//очищаем задний буфер от предыдущего изображения
	Clear_Backbuffer();

	//рисуем линиию толщиной 20 пикселей и длинной 150 пикселей
	for ( int y = 50; y < 70; y++)
	{
		for ( int x = 100; x < 250; x++)
		{
			int Indx =  y * 4 * g_ViewWidth + x * 4;
			g_Data[Indx] = 0; // blue
			g_Data[Indx + 1] = 0; // green
			g_Data[Indx + 2] = 255; // red
			g_Data[Indx + 3] = 0; 
		}
	}	
	//выводим готовую картинку на экран
	Present_Backbuffer();
}

int main ()
{
	//получаем HWND консольного окна
	g_hWnd = GetConsoleWindow();

	Create_Backbuffer();

	//главный цикл рисования линии
	while ( !_kbhit() )
	{
		Draw_Line();
	}

	Delete_Backbuffer();

	return 0;
}	