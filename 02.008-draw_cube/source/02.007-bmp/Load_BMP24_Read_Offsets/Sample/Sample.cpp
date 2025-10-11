#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <conio.h>

int main()
{
	unsigned char Header[54];
	unsigned int DataPos;
	unsigned int ImageSize;
	unsigned int Width, Height;
	
	//открываем BMP файл 256x256 пикселя 24 бит
	//глубина цвета для чтения в бинарном режиме
	FILE *fp = NULL;
	fopen_s(&fp, "texture.bmp", "rb");
	if(fp==NULL) printf("Error Open File");

	//читаем заголовок файла текстуры
	fread(Header, 54, 1, fp);

	//убедимся что это BMP файл
	if ( Header[0]!='B' || Header[1]!='M' )
	{
		printf("Not a correct BMP file\n");
		return 0;
	}

	//убедимся что это 24 bpp BMP файл
	if ( *(int*)&(Header[0x1E])!=0  )
	{
		printf("Not a correct BMP file\n");
		return 0;
	}
	if ( *(int*)&(Header[0x1C])!=24 )
	{
		printf("Not a correct BMP file\n");
		return 0;
	}

	DataPos    = *(int*)&(Header[0x0A]);
	Width      = *(int*)&(Header[0x12]);
	Height     = *(int*)&(Header[0x16]);
	ImageSize  = *(int*)&(Header[0x22]);	

	if (DataPos == 0)
		DataPos=54;

	//сдвигаемся от начала BMP файла до начала данных
	fseek(fp, DataPos, SEEK_SET);

	//указатель на массив байт полученных из BMP файла
	unsigned char *pRes;
	pRes = new unsigned char[Width * Height * 3];
	//читаем из файла rgb данные изображения
	fread(pRes, Width * Height * 3, 1, fp);

	/*
	// Create a buffer
	data = new unsigned char [ImageSize];

	// Read the actual data from the file into the buffer
	fread(data,1,ImageSize,file);
	*/

	//загрузили текстуру закрываем файл
	fclose(fp);

	HWND hWnd = GetConsoleWindow();
	HDC hDC = GetDC(hWnd);

	int Index = 0;
	for ( unsigned int h = 0; h < Height; h++ )
	{
		for ( unsigned int w = 0; w < Width; w++ )
		{
			Index = (w + h * Width) * 3;

			SetPixel(hDC, w, h, RGB(pRes[Index + 2], pRes[Index + 1], pRes[Index]));
		}
	}

	while(!_kbhit()) {};

	ReleaseDC(hWnd, hDC);

	delete [] pRes;

	return 0;
}
