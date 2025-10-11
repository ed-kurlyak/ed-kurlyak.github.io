#include <windows.h>

#include ".\scitech\INCLUDE\MGRAPH.H"

#pragma comment (lib, ".\\scitech\\LIB\\WIN32\\VC\\MGLLT.LIB")
#pragma comment(lib, "winmm.lib")

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

extern RGBQUAD RgbPal[256];

extern MGLDC* windc;
extern MGLDC* dibdc;

void Create_BackBuffer();
void Clear_BackBuffer();
void Present_BackBuffer();
void Delete_BackBuffer();
