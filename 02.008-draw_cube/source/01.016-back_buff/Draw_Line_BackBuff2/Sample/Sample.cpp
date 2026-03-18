//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <conio.h>

HWND g_hWnd = NULL;

HDC g_hBackBuffer; // Контекст устройства памяти
HBITMAP g_hBitmap; // Хранение изображения буфера
void* pBits; // Указатель на пиксели
RECT g_Rc; //размеры окна приложения


//Создать задний буфер
bool Create_BackBuffer()
{
	//размеры окна приложения (заднего буфера)
	GetClientRect(g_hWnd, &g_Rc);

    // Получить контекст устройства окна
    HDC hdcWindow = GetDC(g_hWnd);

    // Создать контекст устройства памяти
    g_hBackBuffer = CreateCompatibleDC(hdcWindow);

    if (!g_hBackBuffer)
        return false;

    // Создать совместимый битмап
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = g_Rc.right;
	bmi.bmiHeader.biHeight = -g_Rc.bottom; // Отрицательная высота для переворота
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Выделение памяти для пикселей
    g_hBitmap = CreateDIBSection(g_hBackBuffer, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

    if (!g_hBitmap)
    {
        DeleteDC(g_hBackBuffer);
        return false;
    }

    // Выбрать битмап в контекст устройства
    SelectObject(g_hBackBuffer, g_hBitmap);

    // Освободить контекст устройства окна
    ReleaseDC(g_hWnd, hdcWindow);

    return true;
}

//Очистить задний буфер
void Clear_BackBuffer()
{
    if (pBits)
    {
        // Заливка буфера указанным цветом
		for (int y = 0; y < g_Rc.bottom; y++)
        {
			DWORD* pPixel = (DWORD*)((BYTE*)pBits + y * g_Rc.right * 4);
			for (int x = 0; x < g_Rc.right; x++)
            {
                *pPixel++ = RGB(0, 0, 0);
            }
        }
    }
}

//Вывести задний буфер на экран
void Present_BackBuffer()
{
    HDC hdcWindow = GetDC(g_hWnd);

    // Копирование буфера на экран
	BitBlt(hdcWindow, 0, 0, g_Rc.right, g_Rc.bottom, g_hBackBuffer, 0, 0, SRCCOPY);

    ReleaseDC(g_hWnd, hdcWindow);
}

//Удалить задний буфер
void Delete_BackBuffer()
{
    if (g_hBitmap)
    {
        DeleteObject(g_hBitmap);
        g_hBitmap = NULL;
    }
    if (g_hBackBuffer)
    {
        DeleteDC(g_hBackBuffer);
        g_hBackBuffer = NULL;
    }
    pBits = NULL;

}


void Draw_Line()
{
	//очищаем задний буфер от предыдущего изображения
	Clear_BackBuffer();


	/*
	//вариант #1

	//создаем перо, сохраняем старое перо
	HPEN hPen = CreatePen(PS_SOLID, 4, RGB( 255, 255, 127 ) );
	HPEN hOldPen = (HPEN) SelectObject(g_hBackBuffer, hPen);
	
	//проводим линию на экране
	MoveToEx(g_hBackBuffer, 50, 50, NULL);
	LineTo(g_hBackBuffer, 200, 200);
	
	//выбираем в контексте старое перо, удаляем старое перо
	SelectObject(g_hBackBuffer, hOldPen);
	DeleteObject(hPen);
	*/
	

	//вариант #2

	BYTE* pPixel = (BYTE*)pBits;
	
	//рисуем линиию толщиной 20 пикселей и длинной 150 пикселей
	for ( int y = 50; y < 70; y++)
	{
		for ( int x = 100; x < 300; x++)
		{
			int Indx =  y * 4 * g_Rc.right + x * 4;
			
			pPixel[Indx + 0] = 127; //blue
			pPixel[Indx + 1] = 255; //green
			pPixel[Indx + 2] = 255; //red
			pPixel[Indx + 3] = 0;
		}
	}

	//выводим готовую картинку на экран
	Present_BackBuffer();
}

int main ()
{
	//получаем HWND консольного окна
	g_hWnd = GetConsoleWindow();

	Create_BackBuffer();

	//главный цикл рисования линии
	while ( !_kbhit() )
	{
		Draw_Line();
	}

	Delete_BackBuffer();

	return 0;
}	