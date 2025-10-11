#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <conio.h>

int main()
{
	//загружаем изображение 256x256 пикселей
	//BMP 24 бит глубина цвета
	HBITMAP hbmBitmap = (HBITMAP)LoadImage(NULL, "texture.bmp", 
					IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
					//IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	BITMAP bm;
    GetObject( hbmBitmap, sizeof(BITMAP), &bm );

	//равно 24 бит глубина цвета
	int Bits = bm.bmBitsPixel;

	unsigned char *pRes = (unsigned char *)bm.bmBits;

	HWND hWnd = GetConsoleWindow();
	HDC hDC = GetDC(hWnd);

	int Index = 0;
	for ( int h = 0; h < bm.bmHeight; h++ )
	{
		for ( int w = 0; w < bm.bmWidth; w++ )
		{
			Index = (h * bm.bmWidth + w) * 3;
			
			BYTE r = pRes[Index + 2];
			BYTE g = pRes[Index + 1];
			BYTE b = pRes[Index + 0];
			
			SetPixel(hDC, w, h, RGB(r, g, b));
		}
	}

	while(!_kbhit()) {};

	ReleaseDC(hWnd, hDC);

	return 0;
}
