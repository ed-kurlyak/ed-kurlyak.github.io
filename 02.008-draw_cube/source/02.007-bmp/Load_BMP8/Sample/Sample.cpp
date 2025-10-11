#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <conio.h>

int main()
{
	//открываем BMP файл 64x64 пикселя 8 бит
	//глубина цвета для чтения в бинарном режиме
	FILE *fp = NULL;
	fopen_s(&fp, "texture.bmp", "rb");
	if(fp==NULL) printf("Error Open File");

	//читаем заголовок файла текстуры
	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);

	//читаем заголовок файла текстуры
	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(BITMAPINFOHEADER), 1, fp);

	//одни элемент палитны занимает 4 байта
	//3 байта для R,G,B и один зарезервированный
	RGBQUAD RgbPal[256];
	//fread(&RgbPal, 256 * 4,1,fp);
	fread(&RgbPal, 256 * sizeof(RGBQUAD),1,fp);

	//сдвигаемся от начала BMP файла до начала данных
	fseek(fp, bfh.bfOffBits, SEEK_SET);

	//указатель на массив байт полученных из BMP файла
	unsigned char *pRes;
	pRes = new unsigned char[bih.biHeight * bih.biWidth];
	
	//читаем из файла цветовые данные изображения
	fread(pRes, bih.biHeight * bih.biWidth, 1, fp);

	//загрузили текстуру закрываем файл
	fclose(fp);

	HWND hWnd = GetConsoleWindow();
	HDC hDC = GetDC(hWnd);

	int Index = 0;

	for ( int h = 0; h < bih.biHeight; h++ )
	{
		for ( int w = 0; w < bih.biWidth; w++ )
		{
			//берем следующий пиксель изображения
			Index = w + h * bih.biWidth;

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

	delete [] pRes;

	return 0;
}
