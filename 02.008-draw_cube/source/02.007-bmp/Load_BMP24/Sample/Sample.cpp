#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <conio.h>

int main()
{
	//открываем BMP файл 256x256 пикселя 24 бит
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

	//сдвигаемся от начала BMP файла до начала данных
	fseek(fp, bfh.bfOffBits, SEEK_SET);

	//указатель на массив байт полученных из BMP файла
	unsigned char *pRes;
	pRes = new unsigned char[bih.biHeight * bih.biWidth * 3];
	//читаем из файла rgb данные изображения
	fread(pRes, bih.biHeight * bih.biWidth * 3, 1, fp);

	//загрузили текстуру закрываем файл
	fclose(fp);

	HWND hWnd = GetConsoleWindow();
	HDC hDC = GetDC(hWnd);

	int Index = 0;
	for ( int h = 0; h < bih.biHeight; h++ )
	{
		for ( int w = 0; w < bih.biWidth; w++ )
		{
			Index = (w + h * bih.biWidth) * 3;

			SetPixel(hDC, w, h, RGB(pRes[Index + 2], pRes[Index + 1], pRes[Index]));
		}
	}

	while(!_kbhit()) {};

	ReleaseDC(hWnd, hDC);

	delete [] pRes;

	return 0;
}
