//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <conio.h>

//глобальные переменные
HWND 	g_hWnd;

//переменные для заднего буфера
//контекст окна приложения
HDC g_hDC;
//размеры окна приложения
RECT g_Rc;
//наш задний буфер
HDC g_hBackBuffer;
HBITMAP g_hBitmap;
HBITMAP g_hOldBitmap;
//кисть для очистки заднего буфера (окна приложения)
HBRUSH g_hBrush;

void Create_BackBuffer()
{
	//размеры окна приложения (заднего буфера)
	GetClientRect(g_hWnd, &g_Rc);

	g_hDC = GetDC(g_hWnd);

	//кисть для очистки окна приложения (заднего буфера)
	g_hBrush = CreateSolidBrush(RGB(0,0,0));

	//наш задний буфер
	g_hBackBuffer = CreateCompatibleDC(g_hDC);
	g_hBitmap = CreateCompatibleBitmap(g_hDC, g_Rc.right, g_Rc.bottom);
	g_hOldBitmap = (HBITMAP) SelectObject(g_hBackBuffer, g_hBitmap);
}

void Clear_BackBuffer()
{
	//очистка окна приложения
	FillRect(g_hBackBuffer, &g_Rc, g_hBrush);
}

void Present_BackBuffer()
{
	//выводим задний буфер на экран приложения
	BitBlt(g_hDC, 0, 0, g_Rc.right, g_Rc.bottom, g_hBackBuffer, 0, 0, SRCCOPY);
}

void Delete_BackBuffer()
{
	//уничтожение заднего буфера

	SelectObject(g_hBackBuffer, g_hOldBitmap);
	DeleteObject(g_hBitmap);

	DeleteObject(g_hBrush);

	ReleaseDC(g_hWnd, g_hDC);
	DeleteDC(g_hBackBuffer);
}


void Draw_Line()
{
	//очищаем задний буфер от предыдущего изображения
	Clear_BackBuffer();

	//создаем перо, сохраняем старое перо
	HPEN hPen = CreatePen(PS_SOLID, 4, RGB( 255, 255, 127 ) );
	HPEN hOldPen = (HPEN) SelectObject(g_hBackBuffer, hPen);

	//проводим линию на экране
	MoveToEx(g_hBackBuffer, 50, 50, NULL);
	LineTo(g_hBackBuffer, 200, 200);

	//выбираем в контексте старое перо, удаляем старое перо
	SelectObject(g_hBackBuffer, hOldPen);
	DeleteObject(hPen);
	
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