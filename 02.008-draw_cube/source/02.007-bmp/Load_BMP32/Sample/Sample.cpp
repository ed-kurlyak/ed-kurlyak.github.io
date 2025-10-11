#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <conio.h>

int main()
{
	//открываем BMP файл 256x256 пикселя 32 бит
	//глубина цвета для чтения в бинарном режиме
	FILE *fp = NULL;
	fopen_s(&fp, "tree.bmp", "rb");
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
	pRes = new unsigned char[bih.biHeight * bih.biWidth * 4];
	//читаем из файла rgb данные изображения
	fread(pRes, bih.biHeight * bih.biWidth * 4, 1, fp);

	//загрузили текстуру закрываем файл
	fclose(fp);

	HWND hWnd = GetConsoleWindow();
	HDC hDC = GetDC(hWnd);

	RECT rc;
	GetClientRect(hWnd, &rc);

	//можно попробовать поменять цвет фона
	//что бы убедиться что альфа смешивание
	//проводится правильно

	//фон белого цвета
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	
	//фон черного цвета
	//HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	
	FillRect(hDC, &rc, hBrush);

	int Index = 0;
	for ( int h = 0; h < bih.biHeight; h++ )
	{
		for ( int w = 0; w < bih.biWidth; w++ )
		{
			Index = (w + h * bih.biWidth) * 4;

			//вводная информация

			//Dest - то что есть на экране (фон)
			//Src - то что будет рисоваться на экране (дерево)

			//Для дерева расчет SourceBlend
			//Коэффициент blend имеет значение (As, As, As, As, As).

			//Для фона расчет DestBlend
			//Коэффициент смешения ( 1 - As, 1 - As, 1 - As, 1 - As).

			//Color = TexelColor x SourceBlend + CurrentPixelColor x DestBlend

			//В DirectX 9 этот тип смешивания выглядит так:
			//m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			//m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			//получаем цвет пикселя дерева
			//которое будет накладывать на фон
			//при помощи альфа смешивания
			float r = pRes[Index + 2] / 255.0f;
			float g = pRes[Index + 1] / 255.0f;
			float b = pRes[Index + 0] / 255.0f;
			//получаем значение альфы из изображения
			float a = pRes[Index + 3] / 255.0f;

			//вычисляем смешивание для источника
			//умножаем пиксель текстуры дерева
			//на его альфа канал
			float SrcR = r * a;
			float SrcG = g * a;
			float SrcB = b * a;

			//получаем цвет фона необходимый для сешивания
			COLORREF Color = GetPixel(hDC, w,h );

			float rb = GetRValue(Color) / 255.0f;
			float gb = GetGValue(Color) / 255.0f;
			float bb = GetBValue(Color) / 255.0f;

			//вычисляем смешивание для приемника
			//умножаем пиксель из бак- буфера
			//на 1 - альфа канал текстуры дерева
			float DestR = rb * (1.0f - a);
			float DestG = gb * (1.0f - a);
			float DestB = bb * (1.0f - a);

			//вычисляем результирующий цвет пиклселя
			//это цвет будет выведен на экран
			//или в задний буфер
			//складываем значения и умрожаем на 255
			float ResR = (SrcR + DestR) * 255;
			float ResG = (SrcG + DestG) * 255;
			float ResB = (SrcB + DestB) * 255;
			
			SetPixel(hDC, w, h, RGB(ResR, ResG, ResB));
		}
	}

	while(!_kbhit()) {};

	ReleaseDC(hWnd, hDC);

	delete [] pRes;

	return 0;
}
