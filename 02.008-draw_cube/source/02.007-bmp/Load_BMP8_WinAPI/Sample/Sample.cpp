#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <conio.h>


int main()
{
	//загружаем изображение 64x64 пикселя BMP 8 бит глубина цвета
	HBITMAP hbmBitmap = (HBITMAP)LoadImage(NULL, "texture.bmp", 
					IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
					//IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	BITMAP bm;
    GetObject( hbmBitmap, sizeof(BITMAP), &bm );

	//получаем палитру из изображения BMP 8 бит
	RGBQUAD RgbPal[256] = {0};
	HDC hMemDC = CreateCompatibleDC(NULL);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hbmBitmap);
	GetDIBColorTable(hMemDC, 0, 256, (RGBQUAD*)RgbPal);
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);

	//равно 8 бит глубина цвета
	int Bits = bm.bmBitsPixel;

	unsigned char *pRes = (unsigned char *)bm.bmBits;

	HWND hWnd = GetConsoleWindow();
	HDC hDC = GetDC(hWnd);

	int Index = 0;

	for ( int h = 0; h < bm.bmHeight; h++ )
	{
		for ( int w = 0; w < bm.bmWidth; w++ )
		{
			//берем следующий пиксель изображения
			Index = (h * bm.bmWidth + w);

			//из массива изображения берем
			//номер цвета который значится в палитре
			unsigned char Color = pRes[Index];

			//по номеру цвета извлекаем
			//компонеты R,G,B из палитры
			BYTE r = RgbPal[Color].rgbRed;
			BYTE g = RgbPal[Color].rgbGreen;
			BYTE b = RgbPal[Color].rgbBlue;

			SetPixel(hDC, w, h, RGB(r, g, b));
		}
	}

	while(!_kbhit()) {};

	ReleaseDC(hWnd, hDC);

	return 0;
}
