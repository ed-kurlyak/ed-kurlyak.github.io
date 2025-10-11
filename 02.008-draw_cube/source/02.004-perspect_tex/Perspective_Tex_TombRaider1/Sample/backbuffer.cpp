#include "backbuffer.h"


int SCREEN_WIDTH;
int SCREEN_HEIGHT;

MGLDC* windc = NULL;
MGLDC* dibdc = NULL;

void Create_Palette()
{
		palette_t	pal[256];
		
		//0 цвет черный
		//pal[0].red = 255;
		//pal[0].green = 0;
		//pal[0].blue = 0;


	for (int i = 0; i < 256; i++)
	{
		pal[i].red = RgbPal[i].rgbRed;
		pal[i].green = RgbPal[i].rgbGreen;
		pal[i].blue = RgbPal[i].rgbBlue;
	}

	MGL_setPalette(dibdc, pal, 256, 0);
	MGL_realizePalette(dibdc, 256, 0, false);

}

void Create_BackBuffer()
{
	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	SCREEN_WIDTH = Rc.right;
	SCREEN_HEIGHT = Rc.bottom;
	
	pixel_format_t	pf;

	MGL_setAppInstance(g_hInst);

	MGL_registerDriver(MGL_PACKED8NAME, PACKED8_driver);

	//для оконного режима
	MGL_initWindowed("");

	if (!MGL_changeDisplayMode(grWINDOWED))
		MGL_fatalError("Unable to use window mode!");

	if ((windc = MGL_createWindowedDC(g_hWnd)) == NULL)
		MGL_fatalError("Unable to create Windowed DC!");

	MGL_getPixelFormat(windc, &pf);

	if ((dibdc = MGL_createMemoryDC(SCREEN_WIDTH, SCREEN_HEIGHT, 8, &pf)) == NULL)
		MGL_fatalError("Unable to create Memory DC!");

	Create_Palette();

}


void Clear_BackBuffer()
{
	MGL_beginDirectAccess();

	unsigned char * phd_winptr_my = NULL;

	phd_winptr_my = (unsigned char*)dibdc->surface;

	//очищаем m_BackBuffer (экран)
	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			int Index = y * SCREEN_WIDTH + x;

			phd_winptr_my[Index + 0] = 255;
		}
	}

}

void Present_BackBuffer()
{
	MGL_endDirectAccess();

	//MGL present back buffer
	HDC hdcScreen = GetDC(g_hWnd);
	MGL_setWinDC(windc, hdcScreen);

	MGL_bitBltCoord(windc, dibdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, MGL_REPLACE_MODE);

	ReleaseDC(g_hWnd, hdcScreen);

}

void Delete_BackBuffer()
{
	if (windc)
		MGL_destroyDC(windc);
	if (dibdc)
		MGL_destroyDC(dibdc);

	windc = dibdc = NULL;
}

