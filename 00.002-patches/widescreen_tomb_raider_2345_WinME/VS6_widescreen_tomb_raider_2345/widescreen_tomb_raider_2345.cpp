//=============================================================
//	Ed Kurlyak 2023 Tomb Raider 2-3-4-5 Widescreen Patch
//=============================================================

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <process.h>
#include <stdio.h>
#include "resource.h"

#pragma comment (lib, "Comctl32.lib")

#define WINDOW_WIDTH 570
//#define WINDOW_HEIGHT 298
#define WINDOW_HEIGHT 305

#define LONG_PTR long

HINSTANCE g_hInst;
HWND g_hDlg;

HWND g_hWndCheckBox_Dist;
HWND g_hWndCheckBox_FOV;
HWND g_hWndCheckBox_Aspect;
HWND g_hWndCheckBox_60fps;

#define IDC_MYCHECKBOX_DIST 101
#define IDC_MYCHECKBOX_FOV 102
#define IDC_MYCHECKBOX_ASPECT 103
#define IDC_MYCHECKBOX_60FPS 104

bool g_lButtonDown_CheckBox_Dist = false;
bool g_lButtonDown_CheckBox_FOV = false;
bool g_lButtonDown_CheckBox_Aspect = true;
bool g_lButtonDown_CheckBox_60fps = false;

HWND g_hWndStaticText_Dist;
HWND g_hWndStaticText_FOV;
HWND g_hWndStaticText_Aspect;
HWND g_hWndStaticText_AspectWH;
HWND g_hWndStaticText_Result;
HWND g_hWndStaticText_60fps;

#define IDC_MYSTATICTEXT_DIST 108
#define IDC_MYSTATICTEXT_FOV 109
#define IDC_MYSTATICTEXT_ASPECT 110
#define IDC_MYSTATICTEXT_ASPECTWH 111
#define IDC_MYSTATICTEXT_RESULT 116
#define IDC_MYSTATICTEXT_60FPS 117

HWND g_hWndEdit_Dist;
HWND g_hWndEdit_FOV;
HWND g_hWndEdit_AspectW;
HWND g_hWndEdit_AspectH;

#define IDC_MYEDIT_DIST 104
#define IDC_MYEDIT_FOV 105
#define IDC_MYEDIT_ASPECTW 106
#define IDC_MYEDIT_ASPECTH 107

HWND g_hWndButton_Apply;
#define IDC_MYBUTTON_APPLY 112

HWND g_hWndButton_Exit;
#define IDC_MYBUTTON_EXIT 114

HWND g_hWndTextCaption;
#define IDC_MYSTATICTEXT_CAPTION 110

HWND g_hWndButtonExit_Title;
#define IDC_MYBUTTON_EXIT_TITLE 115

enum {FIRST_SCREEN, AFTER_APPLY_RESULT};
UINT Status = FIRST_SCREEN;

struct THREADPARMS
{
    float Aspect;
	short int CamDist;
	int Fov;

	char Widescreen_Res[256];
	char CamDist_Res[256];
	char Fov_Res[256];
	char FPS60_Res[256];

	char File_Name[256];
};

THREADPARMS* pThreadParms;

LRESULT CALLBACK CheckBox_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	HBRUSH hBrush = NULL;
	RECT Rect;

	bool Redraw = false;

    WNDPROC WndProc=(WNDPROC)(LONG_PTR)GetWindowLong(hWnd,GWL_USERDATA);
    //if(WndProc)
    //{
        switch(uMsg)
        {
		case WM_PAINT:
			hDC = BeginPaint(hWnd, &Ps);

			GetClientRect(hWnd, &Rect);

			hBrush = ::CreateSolidBrush( RGB( 87, 114, 144) );
			if (!hBrush) return 0;

			::FillRect(hDC, &Rect, hBrush);

		    ::DeleteObject( hBrush );
			
			Rect.top = Rect.top + 3;
			Rect.left = Rect.left + 3;
			Rect.right = Rect.right - 3;
			Rect.bottom = Rect.bottom - 3;

			hBrush = ::CreateSolidBrush( RGB( 184, 190, 202) );
			if (!hBrush) return 0;

			::FillRect(hDC, &Rect, hBrush);

		    ::DeleteObject( hBrush );

			if(g_lButtonDown_CheckBox_Dist && hWnd == g_hWndCheckBox_Dist)
			{
				Redraw = true;					
			}

			if(g_lButtonDown_CheckBox_FOV && hWnd == g_hWndCheckBox_FOV)
			{
				Redraw = true;
			}

			if(g_lButtonDown_CheckBox_Aspect && hWnd == g_hWndCheckBox_Aspect)
			{
				Redraw = true;
			}

			if (g_lButtonDown_CheckBox_60fps && hWnd == g_hWndCheckBox_60fps)
			{
				Redraw = true;
			}

			if(Redraw)
			{
				Rect.top = Rect.top + 3;
				Rect.left = Rect.left + 3;
				Rect.right = Rect.right - 3;
				Rect.bottom = Rect.bottom - 3;

				hBrush = ::CreateSolidBrush( RGB( 87, 114, 144) );
				if (!hBrush) return 0;
	
				::FillRect(hDC, &Rect, hBrush);

				::DeleteObject( hBrush );
			}
			
			EndPaint(hWnd, &Ps);

			break;
		
		case WM_LBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
			if(hWnd == g_hWndCheckBox_Dist)
			{
				g_lButtonDown_CheckBox_Dist = g_lButtonDown_CheckBox_Dist ? 0 : 1;
				GetClientRect(hWnd, &Rect);
				::InvalidateRect(hWnd, &Rect, TRUE);
			}

			if(hWnd == g_hWndCheckBox_FOV)
			{
				g_lButtonDown_CheckBox_FOV = g_lButtonDown_CheckBox_FOV ? 0 : 1;
				GetClientRect(hWnd, &Rect);
				::InvalidateRect(hWnd, &Rect, TRUE);

			}

			if(hWnd == g_hWndCheckBox_Aspect)
			{
				g_lButtonDown_CheckBox_Aspect = g_lButtonDown_CheckBox_Aspect ? 0 : 1;
				GetClientRect(hWnd, &Rect);
				::InvalidateRect(hWnd, &Rect, TRUE);
			}

			if (hWnd == g_hWndCheckBox_60fps)
			{
				g_lButtonDown_CheckBox_60fps = g_lButtonDown_CheckBox_60fps ? 0 : 1;
				GetClientRect(hWnd, &Rect);
				::InvalidateRect(hWnd, &Rect, TRUE);
			}

        break;

        case WM_NCDESTROY:
            SetWindowLong(hWnd,GWL_WNDPROC,(LONG)(LONG_PTR)WndProc);
            SetWindowLong(hWnd,GWL_USERDATA,0);
            break;
        }
        return CallWindowProc(WndProc,hWnd,uMsg,wParam,lParam);
    //}
    //return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

LRESULT CALLBACK Edit_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	HBRUSH   hBrush = NULL;
	RECT Rect;
	const char *Buff = "";

    WNDPROC WndProc=(WNDPROC)(LONG_PTR)GetWindowLong(hWnd,GWL_USERDATA);
    //if(WndProc)
    //{
        switch(uMsg)
        {
		case WM_NCPAINT:
				hDC = GetWindowDC(hWnd);
			    
				GetWindowRect(hWnd, &Rect);
				::OffsetRect(&Rect, -Rect.left, -Rect.top);
				
				hBrush = ::CreateSolidBrush( RGB( 87, 114, 144) );
				if (!hBrush) return 0;
				
				::FillRect(hDC, &Rect, hBrush);
				::DeleteObject( hBrush );
				ReleaseDC(hWnd, hDC);
				
				return 0;

		case WM_PAINT:
			hDC = BeginPaint(hWnd, &Ps);
			
			if(hWnd == g_hWndEdit_Dist)
			{
				Buff="1536";
				SendMessage(hWnd, WM_SETTEXT, 0, LPARAM(Buff));
			}

			if(hWnd == g_hWndEdit_FOV)
			{
				Buff="undef";
				SendMessage(hWnd, WM_SETTEXT, 0, LPARAM(Buff));
			}

			if(hWnd == g_hWndEdit_AspectW)
			{
				Buff="16";
				SendMessage(hWnd, WM_SETTEXT, 0, LPARAM(Buff));
			}

			if(hWnd == g_hWndEdit_AspectH)
			{
				Buff="9";
				SendMessage(hWnd, WM_SETTEXT, 0, LPARAM(Buff));
			}
 
			EndPaint(hWnd, &Ps);
			break;
 
        case WM_NCDESTROY:
            SetWindowLong(hWnd,GWL_WNDPROC,(LONG)(LONG_PTR)WndProc);
            SetWindowLong(hWnd,GWL_USERDATA,0);
            break;
        }
        return CallWindowProc(WndProc,hWnd,uMsg,wParam,lParam);
    //}
    //return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

LRESULT CALLBACK TextCaption_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HBRUSH hBrush = NULL, hOldBrush = NULL;

	POINT Pt;
	RECT Rect;

    WNDPROC WndProc=(WNDPROC)(LONG_PTR)GetWindowLong(hWnd,GWL_USERDATA);
    //if(WndProc)
    //{
        switch(uMsg)
        {
        case WM_LBUTTONDOWN:
			Pt.x = LOWORD(lParam);
			Pt.y = HIWORD(lParam);

			Rect.left = 0;
			Rect.top = 0;
			Rect.right = 570;
			Rect.bottom = 22;

			if(PtInRect(&Rect, Pt))
			{
				SendMessage(g_hDlg, WM_SYSCOMMAND, SC_MOVE|0x0002, 0);
			}

		break;

		case WM_LBUTTONUP:

		break;
 
        case WM_NCDESTROY:
            SetWindowLong(hWnd,GWL_WNDPROC,(LONG)(LONG_PTR)WndProc);
            SetWindowLong(hWnd,GWL_USERDATA,0);
            break;
        }
        return CallWindowProc(WndProc,hWnd,uMsg,wParam,lParam);
    //}
    //return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void __cdecl Thread_Func1(void * pParam)
{
	THREADPARMS* pThreadParms = (THREADPARMS* )pParam;

	float Aspect =   pThreadParms->Aspect;
	short int CamDist =   pThreadParms->CamDist;
	int Fov =   pThreadParms->Fov;

	bool WideScreen = false;
	bool Dist = false;
	bool FOV = false;
	bool FPS60 = false;
	
	FILE *Fp = NULL;

	Fp = fopen("Tomb2.exe", "r+b");
	if( Fp != 0 )
    {
		fclose(Fp);
		strcpy(pThreadParms->File_Name, "Tomb2.exe");
	}

	Fp = fopen("t2gold.exe", "r+b");
	if( Fp != 0 )
    {
		fclose(Fp);
		strcpy(pThreadParms->File_Name, "t2gold.exe");
	}

	Fp = fopen("tomb3.exe", "r+b");
	if( Fp != 0 )
    {
		fclose(Fp);
		strcpy(pThreadParms->File_Name, "tomb3.exe");
	}

	Fp = fopen("tr3gold.exe", "r+b");
	if( Fp != 0 )
    {
		fclose(Fp);
		strcpy(pThreadParms->File_Name, "tr3gold.exe");
	}

	Fp = fopen("tomb4.exe", "r+b");
	if( Fp != 0 )
    {
		fclose(Fp);
		strcpy(pThreadParms->File_Name, "tomb4.exe");
	}

	Fp = fopen("PCTomb5.exe", "r+b");
	if( Fp != 0 )
    {
		fclose(Fp);
		strcpy(pThreadParms->File_Name, "PCTomb5.exe");
	}

	if( strlen(pThreadParms->File_Name) == 0 )
    {
		_endthread();
	}
	
	Fp  = fopen(pThreadParms->File_Name, "r+b");
	if( Fp != 0 )
    {
	}

	UINT SeekPos = 0;

	fseek(Fp, 0, SEEK_END);
	
	DWORD FileSize = ftell(Fp);

	rewind(Fp);

	BYTE *Buff = new BYTE[FileSize];

	fread((BYTE*)Buff, FileSize, 1, Fp);

	
	FILE* FpBak = NULL;
	char FileName[256];
	strcpy(FileName, pThreadParms->File_Name);
	strcat(FileName, ".bak");

	FpBak = fopen(FileName, "wb");
	if (Fp != 0)
	{
		fwrite(Buff, FileSize, 1, FpBak);
		fclose(FpBak);
	}

	

	if(g_lButtonDown_CheckBox_Aspect && Aspect!=0.0f)
	{
		UINT i = 0;
		do
		{

			if (Buff[i] == 0xAB &&
				Buff[i + 1] == 0xAA &&
				Buff[i + 2] == 0xAA &&
				Buff[i + 3] == 0x3F)
			{

				BYTE Data[4];
				memcpy(Data, &Aspect, sizeof(Aspect));

				Buff[i] = Data[0];
				Buff[i + 1] = Data[1];
				Buff[i + 2] = Data[2];
				Buff[i + 3] = Data[3];

				strcpy(pThreadParms->Widescreen_Res, "Widescreen correction OK");
				WideScreen = true;

				break;
			}
			i++;

		} while (i < FileSize - 4);
	}

	if(g_lButtonDown_CheckBox_Dist && CamDist != 0)
	{
		
		UINT i = 0;

		do 
		{
			if (Buff[i] == 0xC7 &&
				Buff[i + 1] == 0x05)
			{
				if (Buff[i + 6] == 0x00 &&
					Buff[i + 6 + 1] == 0x06)
				{

					BYTE Data[2];
					memcpy(Data, &CamDist, sizeof(CamDist));

					Buff[i + 6] = Data[0];
					Buff[i + 6 + 1] = Data[1];
			
					strcpy(pThreadParms->CamDist_Res, "Camera dist correction OK");
					Dist = true;
				}
			}

			i++;

		}while (i < FileSize - 7);
	}

	if(g_lButtonDown_CheckBox_FOV && Fov !=0 )
	{
		UINT i = 0;

		do 
		{
			if (Buff[i] == 0xA1)
			{
				if (Buff[i + 5] == 0x99)
				{
					float Fa = 4.0f / 3.0f;

					float FFov = (float) Fov;

					float ResFov = FFov / Fa;

					int IntFov = (int)ResFov;

					int TFov = IntFov << 8 |  0xB8;

					BYTE Data[4];
					memcpy(Data, &TFov, sizeof (TFov));

					Buff[i] = Data[0];
					Buff[i + 1] = Data[1];
					Buff[i + 2] = Data[2];
					Buff[i + 3] = Data[3];
					
					strcpy(pThreadParms->Fov_Res, "FOV correction OK");
					FOV = true;

					//break;
				}
			}

			i++;

		}while (i < FileSize - 5);

	}

	if(!strncmp(pThreadParms->File_Name, "Tomb2.exe", 256) ||
		
		!strncmp(pThreadParms->File_Name, "t2gold.exe", 256) ||
		
		!strncmp(pThreadParms->File_Name, "tomb3.exe", 256) ||
		
		!strncmp(pThreadParms->File_Name, "tr3gold.exe", 256) )

	{
	
	if (g_lButtonDown_CheckBox_60fps)
	{
		UINT i = 0;
		do
		{

			if (Buff[i] == 0x8B &&
				Buff[i + 1] == 0xF8 &&
				Buff[i + 2] == 0x83 &&
				Buff[i + 3] == 0xFF &&
				Buff[i + 4] == 0x2)
			{
				Buff[i + 4] = 0x0;

				strcpy(pThreadParms->FPS60_Res, "60 FPS Unlock OK");
				FPS60 = true;

				break;
			}

			i++;

		} while (i < FileSize);
	}

	}
 
	rewind(Fp);
	fwrite(Buff, FileSize , 1, Fp);
	fclose(Fp);
	
	if(!WideScreen)
		strcpy(pThreadParms->Widescreen_Res, "Widescreen correction not used");
	if(!Dist)
		strcpy(pThreadParms->CamDist_Res, "Camera dist correction not used");
	if (!FOV)
		strcpy(pThreadParms->Fov_Res, "FOV correction not used");
	if (!FPS60)
		strcpy(pThreadParms->FPS60_Res, "Unlock 60 FPS not used");

	_endthread();
}

INT_PTR CALLBACK WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	RECT Rect;
	HBRUSH   hBrush = NULL;
	POINT Pt[6];
	HRGN hRgn;
	POINT Pt1;
	//HANDLE FontHandle;
	HRSRC Res;
	HFONT hFont;
	const char *Buff = "";
	LPDRAWITEMSTRUCT DrawItemStruct;
	UINT State;
	int Mode;
	int Color;
	SIZE Size;
	HPEN hPen, hOldPen;

    switch(uMsg)
    {
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				//обрабатываем нажатие на кнопку закрытия
				//в правом верхнем углу заголовка окна
				case IDC_MYBUTTON_EXIT_TITLE:
						DestroyWindow(hDlg);
						break;
				
				//обрабатываем нажатие на кнопку exit
				//появляется после нажатия на кнопку apply
				case IDC_MYBUTTON_EXIT:
						DestroyWindow(hDlg);
						break;
				
				//обрабатываем нажатие на кнопку apply
				//в главном окне приложения
				case IDC_MYBUTTON_APPLY:
				
				pThreadParms = new THREADPARMS;
				ZeroMemory(pThreadParms, sizeof(THREADPARMS));
				
				pThreadParms->File_Name[0] = 0;
				pThreadParms->Widescreen_Res[0] = 0;
				pThreadParms->CamDist_Res[0] = 0;
				pThreadParms->Fov_Res[0] = 0;
				pThreadParms->FPS60_Res[0] = 0;

				char BuffWidth[256];
				::SendMessage(g_hWndEdit_AspectW, WM_GETTEXT, 256, LPARAM(BuffWidth));

				char BuffHeight[256];
				::SendMessage(g_hWndEdit_AspectH, WM_GETTEXT, 256, LPARAM(BuffHeight));

				char BuffDist[256];
				::SendMessage(g_hWndEdit_Dist, WM_GETTEXT, 256, LPARAM(BuffDist));

				char BuffFov[256];
				::SendMessage(g_hWndEdit_FOV, WM_GETTEXT, 256, LPARAM(BuffFov));

				UINT IntWidth = atoi(BuffWidth);
				UINT IntHeight = atoi(BuffHeight);

				if( strlen(BuffWidth) == 0 || strlen(BuffHeight) == 0 )
					pThreadParms->Aspect = 0.0f;
				else
					pThreadParms->Aspect =  (float) IntWidth / IntHeight;
				
				
				if(strlen(BuffDist) == 0)
					pThreadParms->CamDist = 0;
				else
					pThreadParms->CamDist = atoi(BuffDist);
				
				if(_strcmpi(BuffFov, "undef"))
					pThreadParms->Fov = atoi(BuffFov);
				else
					pThreadParms->Fov = 0;
	
				HANDLE hThread;

				hThread = (HANDLE)_beginthread(Thread_Func1, 0, (LPVOID)pThreadParms);
				WaitForSingleObject(hThread, INFINITE);
				//CloseHandle(hThread);

				DestroyWindow(g_hWndStaticText_Dist);
				DestroyWindow(g_hWndStaticText_FOV);
				DestroyWindow(g_hWndStaticText_Aspect);
				DestroyWindow(g_hWndStaticText_AspectWH);
				DestroyWindow(g_hWndStaticText_60fps);

				DestroyWindow(g_hWndEdit_Dist);
				DestroyWindow(g_hWndEdit_FOV);
				DestroyWindow(g_hWndEdit_AspectW);
				DestroyWindow(g_hWndEdit_AspectH);
				
				DestroyWindow(g_hWndCheckBox_Dist);
				DestroyWindow(g_hWndCheckBox_FOV);
				DestroyWindow(g_hWndCheckBox_Aspect);
				DestroyWindow(g_hWndCheckBox_60fps);
				
				DestroyWindow(g_hWndButton_Apply);
				
				//создаем фонт для кнопки exit
				hFont = CreateFont(-15,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,FALSE,FALSE,
                //FALSE,0, FALSE,"Exo 2 Light");
				FALSE,0, FALSE,"Arial");
				//FALSE,0, FALSE,"Times New Roman");
				//FALSE,0, FALSE,"Consolas");

				//создаем кнопку закрытия окна на месте кнопки appy
				//обрабатываем нажатие в WM_COMMAND
				g_hWndButton_Exit=CreateWindowEx(0,"Button", "",
					WS_CHILD|WS_TABSTOP|WS_VISIBLE|BS_OWNERDRAW|BS_PUSHBUTTON,
					436, 233, 107, 42,hDlg,
					(HMENU)IDC_MYBUTTON_EXIT,g_hInst,NULL);
 
				if(!g_hWndButton_Exit)
				{
					MessageBox(NULL,"Button создать не удалось", "Info",MB_OK);
					return -1;
				}

				//устанавливаем фонт для кнопки exit
				SendMessage(g_hWndButton_Exit, WM_SETFONT, WPARAM (hFont), TRUE);

				if(strlen(pThreadParms->File_Name) == 0)
				{
					Buff = "No Tomb Raider 2345 files\r\nMaybe file is busy";
				}
				else
				{
					char BuffTemp[256];
					sprintf(BuffTemp, "Done %s!\r\n\r\n%s\r\n%s\r\n%s\r\n%s",
					pThreadParms->File_Name,
					pThreadParms->Widescreen_Res,
					pThreadParms->CamDist_Res,
					pThreadParms->Fov_Res,
					pThreadParms->FPS60_Res);

					Buff = BuffTemp;
				}
					
				g_hWndStaticText_Result=CreateWindowEx(0,"Static", "",
					WS_CHILD|WS_TABSTOP|WS_VISIBLE | SS_LEFT | SS_NOTIFY,
					73,60,280,120,hDlg,(HMENU)(UINT_PTR)IDC_MYSTATICTEXT_RESULT,g_hInst,NULL);
 
				if(!g_hWndStaticText_Result)
				{
						MessageBox(NULL,"Static создать не удалось", "Info",MB_OK);
						return -1;
				}
    
				SendMessage(g_hWndStaticText_Result, WM_SETFONT, WPARAM (hFont), TRUE);

				SendMessage(g_hWndStaticText_Result, WM_SETTEXT, 0, LPARAM(Buff));
				
				//рисуем окно результата в WM_PAINT
				Status = AFTER_APPLY_RESULT;

				::InvalidateRect(hDlg, NULL, true);
			}

			return (INT_PTR)TRUE;

		//создаем фонт для текста и edit
		//создаем check box три штуки
		//создаем static для текстовой подписи каждого check box
		//создаем надпись в заголовке окна
		//создаем кнопку закрытия в заголовке окна
		//создаем кнопку apply
		case WM_INITDIALOG:
			//в ресурсах ищем фонт
			Res = FindResource(g_hInst, MAKEINTRESOURCE(IDR_MY_FONT1),"MY_FONT");

			//создаем фонт из ресурсов программы
			/*
			if (Res) 
			{
				HGLOBAL Mem = LoadResource(g_hInst, Res);
				void *Data = LockResource(Mem);
				size_t Len = SizeofResource(g_hInst, Res);

				DWORD NumFonts;
				FontHandle = AddFontMemResourceEx(
				Data,       	// font resource
				(int) Len,       	// number of bytes in font resource 
				NULL,          	// Reserved. Must be 0.
				&NumFonts      	// number of fonts installed
				);

				if(FontHandle==0)
				{
					MessageBox(hDlg, "Font add fails", "Error", MB_OK);
				}
			}
			*/

			//создаем окно check box для dist на базе static
			g_hWndCheckBox_Dist=CreateWindowEx(0,"Static", "",
			    WS_CHILD|WS_TABSTOP|WS_VISIBLE | SS_LEFT | SS_NOTIFY,
				18, 35, 25, 25,hDlg,(HMENU)(UINT_PTR)IDC_MYCHECKBOX_DIST,g_hInst,NULL);
 
			if(!g_hWndCheckBox_Dist)
			{
				MessageBox(hDlg,"Static создать не удалось", "Info",MB_OK);
				return -1;
			}
 
			// Установить новую функцию обработки окна check box dist
			SetWindowLong(g_hWndCheckBox_Dist,GWL_USERDATA,GetWindowLong(g_hWndCheckBox_Dist,GWL_WNDPROC));
			SetWindowLong(g_hWndCheckBox_Dist,GWL_WNDPROC,(LONG)(LONG_PTR)CheckBox_WndProc);
	
			//создаем окно check box для FOV на базе static
			g_hWndCheckBox_FOV=CreateWindowEx(0,"Static", "",
		       WS_CHILD|WS_TABSTOP|WS_VISIBLE | SS_LEFT | SS_NOTIFY,
				18, 132, 25, 25,hDlg,(HMENU)(UINT_PTR)IDC_MYCHECKBOX_FOV,g_hInst,NULL);
 
			if(!g_hWndCheckBox_FOV)
			{
				MessageBox(NULL,"Static создать не удалось", "Info",MB_OK);
				return -1;
			}
 
			// Установить новую функцию окна
			SetWindowLong(g_hWndCheckBox_FOV,GWL_USERDATA,GetWindowLong(g_hWndCheckBox_FOV,GWL_WNDPROC));
			SetWindowLong(g_hWndCheckBox_FOV,GWL_WNDPROC,(LONG)(LONG_PTR)CheckBox_WndProc);
 
			//создаем окно check box для aspect на базе static
			g_hWndCheckBox_Aspect=CreateWindowEx(0,"Static", "",
		       WS_CHILD|WS_TABSTOP|WS_VISIBLE | SS_LEFT | SS_NOTIFY,
				310, 35, 25, 25,hDlg,(HMENU)(UINT_PTR)IDC_MYCHECKBOX_ASPECT,g_hInst,NULL);
 
			if(!g_hWndCheckBox_Aspect)
			{
				MessageBox(NULL,"Static создать не удалось", "Info",MB_OK);
				return -1;
			}
 
			// Установить новую функцию окна
			SetWindowLong(g_hWndCheckBox_Aspect,GWL_USERDATA,GetWindowLong(g_hWndCheckBox_Aspect,GWL_WNDPROC));
			SetWindowLong(g_hWndCheckBox_Aspect,GWL_WNDPROC,(LONG)(LONG_PTR)CheckBox_WndProc);
 
			//создаем окно check box для 60 fps на базе static
			g_hWndCheckBox_60fps = CreateWindowEx(0, "Static", "",
				WS_CHILD | WS_TABSTOP | WS_VISIBLE | SS_LEFT | SS_NOTIFY,
				18, 250, 25, 25, hDlg, (HMENU)(UINT_PTR)IDC_MYCHECKBOX_60FPS, g_hInst, NULL);

			if (!g_hWndCheckBox_60fps)
			{
				MessageBox(NULL, "Static создать не удалось", "Info", MB_OK);
				return -1;
			}

			// Установить новую функцию окна
			SetWindowLong(g_hWndCheckBox_60fps, GWL_USERDATA, GetWindowLong(g_hWndCheckBox_60fps, GWL_WNDPROC));
			SetWindowLong(g_hWndCheckBox_60fps, GWL_WNDPROC, (LONG)(LONG_PTR)CheckBox_WndProc);


			//создаем фонт что бы выводить текст в static окнах
			//dist, fov, aspect hw
			hFont = CreateFont(-15,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,FALSE,FALSE,
                //FALSE,0, FALSE,"Exo 2 Light");
				FALSE,0, FALSE,"Arial");
				//FALSE,0, FALSE,"Times New Roman");
				//FALSE,0, FALSE,"Consolas");
			
			//на базе static создаем текст для dist
			//в сообщении WM_CTLCOLORSTATIC закрасим цвет фона для текста
			g_hWndStaticText_Dist=CreateWindowEx(0,"Static", "",
		        WS_CHILD|WS_TABSTOP|WS_VISIBLE | SS_LEFT | SS_NOTIFY,
				68, 35, 220, 40,hDlg,(HMENU)(UINT_PTR)IDC_MYSTATICTEXT_DIST,g_hInst,NULL);
 
			if(!g_hWndStaticText_Dist)
			{
				MessageBox(NULL,"Static создать не удалось", "Info",MB_OK);
				return -1;
			}
			
			//устанавливаем фонт для static текста dist
			SendMessage(g_hWndStaticText_Dist, WM_SETFONT, WPARAM (hFont), TRUE);
	
			//устанавливаем текст для static dist
			Buff = "Camera distance from Lara; default game value is 1536";
			SendMessage(g_hWndStaticText_Dist, WM_SETTEXT, 0, LPARAM(Buff));

			//на базе static создаем текст для FOV
			//в сообщении WM_CTLCOLORSTATIC закрасим цвет фона для текста
			g_hWndStaticText_FOV=CreateWindowEx(0,"Static", "",
				WS_CHILD|WS_TABSTOP|WS_VISIBLE | SS_LEFT | SS_NOTIFY,
				68, 132, 220, 60,hDlg,(HMENU)(UINT_PTR)IDC_MYSTATICTEXT_FOV,g_hInst,NULL);
 
			if(!g_hWndStaticText_FOV)
			{
				MessageBox(NULL,"Static создать не удалось", "Info",MB_OK);
				return -1;
			}
			//устанавливаем фонт для static текста FOV
			SendMessage(g_hWndStaticText_FOV, WM_SETFONT, WPARAM (hFont), TRUE);

			//устанавливаем текст для static FOV
			Buff = "Field Of View (FOV); enter screen resolution WIDTH used in game setup";
			SendMessage(g_hWndStaticText_FOV, WM_SETTEXT, 0, LPARAM(Buff));

			//на базе static создаем текст для Aspect
			//в сообщении WM_CTLCOLORSTATIC закрасим цвет фона для текста
			g_hWndStaticText_Aspect=CreateWindowEx(0,"Static", "",
				WS_CHILD|WS_TABSTOP|WS_VISIBLE | SS_LEFT | SS_NOTIFY,
				360, 35, 180, 53,hDlg,(HMENU)(UINT_PTR)IDC_MYSTATICTEXT_ASPECT,g_hInst,NULL);
 
			if(!g_hWndStaticText_Aspect)
			{
				MessageBox(NULL, "Static создать не удалось", "Info",MB_OK);
				return -1;
			}
 
			//устанавливаем фонт для static текста Aspect
			SendMessage(g_hWndStaticText_Aspect, WM_SETFONT, WPARAM (hFont), TRUE);
	
			//устанавливаем текст для static Aspect
			Buff = "Widescreen aspect\r\nratio default game value is 4:3";
			SendMessage(g_hWndStaticText_Aspect, WM_SETTEXT, 0, LPARAM(Buff));
	
			//на базе static создаем текст для AspectWH
			//в сообщении WM_CTLCOLORSTATIC закрасим цвет фона для текста
		    g_hWndStaticText_AspectWH=CreateWindowEx(0,"Static", "",
		        WS_CHILD|WS_TABSTOP|WS_VISIBLE | SS_LEFT | SS_NOTIFY,
				367, 92, 125, 70,hDlg,(HMENU)(UINT_PTR)IDC_MYSTATICTEXT_ASPECTWH,g_hInst,NULL);
 
			if(!g_hWndStaticText_AspectWH)
			{
				MessageBox(NULL,"Static создать не удалось", "Info",MB_OK);
				return -1;
			}
			
			//устанавливаем фонт для static текста AspectWH
			SendMessage(g_hWndStaticText_AspectWH, WM_SETFONT, WPARAM (hFont), TRUE);

			//устанавливаем текст для static AspectWH
			Buff = "Display width\r\n\r\nDisplay height";
			SendMessage(g_hWndStaticText_AspectWH, WM_SETTEXT, 0, LPARAM(Buff));

			//на базе static создаем текст для 60fps
			//в сообщении WM_CTLCOLORSTATIC закрасим цвет фона для текста
			g_hWndStaticText_60fps = CreateWindowEx(0, "Static", "",
				WS_CHILD | WS_TABSTOP | WS_VISIBLE | SS_LEFT | SS_NOTIFY,
				68, 250, 130, 45, hDlg, (HMENU)(UINT_PTR)IDC_MYSTATICTEXT_60FPS, g_hInst, NULL);

			if (!g_hWndStaticText_60fps)
			{
				MessageBox(NULL, "Static создать не удалось", "Info", MB_OK);
				return -1;
			}
			//устанавливаем фонт для static текста FOV
			SendMessage(g_hWndStaticText_60fps, WM_SETFONT, WPARAM(hFont), TRUE);

			//устанавливаем текст для static FOV
			Buff = "Unlock 60 FPS\r\nTR23 Only";
			SendMessage(g_hWndStaticText_60fps, WM_SETTEXT, 0, LPARAM(Buff));

			//создаем edit для dist
			//в сообщении WM_CTLCOLOREDIT закрашиваем фон для edit dist
		    g_hWndEdit_Dist=CreateWindowEx(WS_EX_CLIENTEDGE,"Edit", "1536",
				WS_CHILD|WS_TABSTOP|WS_VISIBLE | WS_BORDER|ES_LEFT,
				75, 78, 65,32,hDlg,(HMENU)IDC_MYEDIT_DIST,g_hInst,NULL);
 
			if(!g_hWndEdit_Dist)
			{
				MessageBox(NULL,"Edit создать не удалось", "Info",MB_OK);
				return -1;
			}
 
			// Установить функцию обработки сообщений для edit dist
			SetWindowLong(g_hWndEdit_Dist,GWL_USERDATA,GetWindowLong(g_hWndEdit_Dist,GWL_WNDPROC));
			SetWindowLong(g_hWndEdit_Dist,GWL_WNDPROC,(LONG)(LONG_PTR)Edit_WndProc);
		
			//установить фонт для edit dist
			SendMessage(g_hWndEdit_Dist, WM_SETFONT, WPARAM (hFont), TRUE);

			//создаем edit для FOV
			//в сообщении WM_CTLCOLOREDIT закрашиваем фон для edit FOV
		    g_hWndEdit_FOV=CreateWindowEx(WS_EX_CLIENTEDGE,"Edit", "undef",
		        WS_CHILD|WS_TABSTOP|WS_VISIBLE | WS_BORDER|ES_LEFT,
				75, 195, 65,32,hDlg,(HMENU)IDC_MYEDIT_FOV,g_hInst,NULL);
 
		   if(!g_hWndEdit_FOV)
			{
				MessageBox(NULL,"Edit создать не удалось", "Info",MB_OK);
				return -1;
			}
 
			// Установить функцию обработки сообщений для edit FOV
			SetWindowLong(g_hWndEdit_FOV,GWL_USERDATA,GetWindowLong(g_hWndEdit_FOV,GWL_WNDPROC));
			SetWindowLong(g_hWndEdit_FOV,GWL_WNDPROC,(LONG)(LONG_PTR)Edit_WndProc);
		
			//установить фонт для edit FOV
			SendMessage(g_hWndEdit_FOV, WM_SETFONT, WPARAM (hFont), TRUE);

			//создаем edit для aspect W
			//в сообщении WM_CTLCOLOREDIT закрашиваем фон для edit aspect W
		    g_hWndEdit_AspectW=CreateWindowEx(WS_EX_CLIENTEDGE,"Edit", "16",
				WS_CHILD|WS_TABSTOP|WS_VISIBLE | WS_BORDER|ES_LEFT,
				495, 83, 45,32,hDlg,(HMENU)IDC_MYEDIT_ASPECTW,g_hInst,NULL);
 
			if(!g_hWndEdit_AspectW)
			{
				MessageBox(NULL,"Edit создать не удалось", "Info",MB_OK);
				return -1;
			}
 
			// Установить функцию обработки сообщений
			SetWindowLong(g_hWndEdit_AspectW,GWL_USERDATA,GetWindowLong(g_hWndEdit_AspectW,GWL_WNDPROC));
			SetWindowLong(g_hWndEdit_AspectW,GWL_WNDPROC,(LONG)(LONG_PTR)Edit_WndProc);
		
			//устанавливаем фонт
			SendMessage(g_hWndEdit_AspectW, WM_SETFONT, WPARAM (hFont), TRUE);

			//создаем edit для aspect H
			//в сообщении WM_CTLCOLOREDIT закрашиваем фон для edit aspect H
		    g_hWndEdit_AspectH=CreateWindowEx(WS_EX_CLIENTEDGE,"Edit", "9",
				WS_CHILD|WS_TABSTOP|WS_VISIBLE | WS_BORDER|ES_LEFT,
				495, 122, 45,32,hDlg,(HMENU)IDC_MYEDIT_ASPECTH,g_hInst,NULL);
 
			if(!g_hWndEdit_AspectH)
			{
				MessageBox(NULL,"Edit создать не удалось", "Info",MB_OK);
				return -1;
			}
 
			// Установить функцию обработки сообщений
			SetWindowLong(g_hWndEdit_AspectH,GWL_USERDATA,GetWindowLong(g_hWndEdit_AspectH,GWL_WNDPROC));
			SetWindowLong(g_hWndEdit_AspectH,GWL_WNDPROC,(LONG)(LONG_PTR)Edit_WndProc);

			//устанавливаем фонт
			SendMessage(g_hWndEdit_AspectH, WM_SETFONT, WPARAM (hFont), TRUE);

			//создаем в гл.окне кнопку apply
			//сама кнопка рисуется в сообщении WM_DRAWITEM
			//обрабатывается в WM_COMMAND
			g_hWndButton_Apply=CreateWindowEx(0,"Button", "",
				WS_CHILD|WS_TABSTOP|WS_VISIBLE|BS_OWNERDRAW|BS_PUSHBUTTON,
				436, 233, 107, 42,hDlg,
				(HMENU)IDC_MYBUTTON_APPLY,g_hInst,NULL);
 
			if(!g_hWndButton_Apply)
			{
				MessageBox(NULL,"Button создать не удалось", "Info",MB_OK);
				return -1;
			}
			//устанавливаем фонт для кнопки
			SendMessage(g_hWndButton_Apply, WM_SETFONT, WPARAM (hFont), TRUE);

			//создаем фонт для текста в заголовке окна
			hFont = CreateFont(-13,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,FALSE,FALSE,
                //FALSE,0, FALSE,"Exo 2 Light");
				FALSE,0, FALSE,"Arial");
				//FALSE,0, FALSE,"Times New Roman");
				//FALSE,0, FALSE,"Consolas");

			//создаем staic text для заголовка окна
		    g_hWndTextCaption=CreateWindowEx(0,"Static", "",
		        WS_CHILD|WS_TABSTOP|WS_VISIBLE | SS_LEFT | SS_NOTIFY,
				13, 4, 250, 17,hDlg,(HMENU)(UINT_PTR)IDC_MYSTATICTEXT_CAPTION,g_hInst,NULL);
 
			if(!g_hWndTextCaption)
			{
				MessageBox(NULL,"Static создать не удалось", "Info",MB_OK);
				return -1;
			}
 
			// Установить оконную процедуру нажимать будем
			// левую кнопку мыши на static для перемещения окна (заголовок)
			SetWindowLong(g_hWndTextCaption,GWL_USERDATA,GetWindowLong(g_hWndTextCaption,GWL_WNDPROC));
			SetWindowLong(g_hWndTextCaption,GWL_WNDPROC,(LONG)(LONG_PTR)TextCaption_WndProc);
			
			//установить фонт для текста
			SendMessage(g_hWndTextCaption, WM_SETFONT, WPARAM (hFont), TRUE);
	
			//установить текст
			Buff = "Widescreen Tomb Raider 2345 v1.7.8";
			SendMessage(g_hWndTextCaption, WM_SETTEXT, 0, LPARAM(Buff));

			//создаем кнопку закрытия справа в заголовке окна
			//рисуем саму кнопку в сообщении WM_DRAWITEM
			//обрабатывается в WM_COMMAND
			g_hWndButtonExit_Title=CreateWindowEx(0,"Button", "",
				WS_CHILD|WS_TABSTOP|WS_VISIBLE|BS_OWNERDRAW|BS_PUSHBUTTON,
				546, 3, 16, 16,hDlg,
				(HMENU)IDC_MYBUTTON_EXIT_TITLE,g_hInst,NULL);
 
			if(!g_hWndButtonExit_Title)
			{
				MessageBox(hDlg,"Button создать не удалось", "Info",MB_OK);
				return -1;
			}

			return (INT_PTR)TRUE;

		case WM_CTLCOLORSTATIC:
			//текст у нас рисуется на static
			//закрашиваем фон текста для текста Dist, Fov, Aspect hw
			if( ((HWND)lParam == g_hWndStaticText_Dist) || ((HWND)lParam == g_hWndStaticText_FOV)
			|| ((HWND)lParam == g_hWndStaticText_Aspect) || ((HWND)lParam == g_hWndStaticText_AspectWH)
			|| ((HWND)lParam == g_hWndStaticText_Result) || ((HWND)lParam == g_hWndStaticText_60fps))
			{
				hDC = (HDC) wParam;
				::SetTextColor(hDC, RGB(0, 0, 0));
				::SetBkMode(hDC, TRANSPARENT);

				hBrush = ::CreateSolidBrush(RGB(175, 207, 206));

				return (LRESULT) hBrush;				
			}

		//закрашиваем фон для edit dist, fov, aspect wh
		case WM_CTLCOLOREDIT:
			hDC = (HDC) wParam;
			::SetTextColor(hDC, RGB(0, 0, 0));
			::SetBkMode(hDC, TRANSPARENT);

			hBrush = ::CreateSolidBrush(RGB( 184, 190, 202));

			return (LRESULT) hBrush;

		
		case WM_DRAWITEM:

			//рисуем кнопку выхода справа в заголовке окна
			if (LOWORD(wParam) == IDC_MYBUTTON_EXIT_TITLE )
			{
				DrawItemStruct = (LPDRAWITEMSTRUCT )lParam;
				hDC = DrawItemStruct->hDC;
				Rect=DrawItemStruct->rcItem;
				State=DrawItemStruct->itemState;

				if(State&ODS_SELECTED)
				{
					hBrush = CreateSolidBrush(RGB(184, 190, 202));
					::FillRect(hDC, &Rect, hBrush);
					hPen = ::CreatePen(PS_SOLID, 2, RGB(87, 114, 144));
					hOldPen = (HPEN)::SelectObject(hDC, hPen);
					POINT Pt;
					::MoveToEx(hDC, 5,5, &Pt);
					::LineTo(hDC, 11,11);

					::MoveToEx(hDC, 11,5, &Pt);
					::LineTo(hDC, 5,11);
				}
				else 
				{
					hBrush = CreateSolidBrush(RGB(184, 190, 202));
					::FillRect(hDC, &Rect, hBrush);
					hPen = ::CreatePen(PS_SOLID, 2, RGB(0,0,0));
					hOldPen = (HPEN)::SelectObject(hDC, hPen);
					POINT Pt;
					::MoveToEx(hDC, 5,5, &Pt);
					::LineTo(hDC, 11,11);

					::MoveToEx(hDC, 5,5, &Pt);
					::LineTo(hDC, 11,11);

					::MoveToEx(hDC, 11,5, &Pt);
					::LineTo(hDC, 5,11);
				}

				return (INT_PTR)TRUE;
			}

			//рисуем кнопку гл.окна apply и результ.окна кнопка exit
			if( (LOWORD(wParam) == IDC_MYBUTTON_APPLY ) ||
				(LOWORD(wParam) == IDC_MYBUTTON_EXIT ) )
			{
				DrawItemStruct = (LPDRAWITEMSTRUCT )lParam;
				hDC = DrawItemStruct->hDC;
				Rect=DrawItemStruct->rcItem;
				State=DrawItemStruct->itemState;

				if(State&ODS_SELECTED)
				{	
					::DrawFrameControl(hDC, &Rect,DFC_BUTTON, 	DFCS_BUTTONPUSH);
			
					hBrush = ::CreateSolidBrush(RGB(87, 114, 144));
					::FillRect(hDC, &Rect, hBrush);
				
					Rect.left += 4;
					Rect.top += 4;
					Rect.right -= 4;
					Rect.bottom -= 4;

					hBrush = ::CreateSolidBrush(RGB(184, 190, 202));
					::FillRect(hDC, &Rect, hBrush);
				}
				else
				{
					::DrawFrameControl(hDC, &Rect,DFC_BUTTON, 	DFCS_BUTTONPUSH);

					hBrush = ::CreateSolidBrush(RGB(87, 114, 144));
					::FillRect(hDC, &Rect, hBrush);
			
					Rect.left += 2;
					Rect.top += 2;
					Rect.right -= 6;
					Rect.bottom -= 6;

					hBrush = ::CreateSolidBrush(RGB(184, 190, 202));
					::FillRect(hDC, &Rect, hBrush);
				}

				Mode = ::SetBkMode(hDC, TRANSPARENT);
				Color = ::SetTextColor(hDC,RGB(0,0,0));
			
				if (LOWORD(wParam) == IDC_MYBUTTON_APPLY )
					Buff= "Apply";

				if (LOWORD(wParam) == IDC_MYBUTTON_EXIT )
					Buff= "Exit";

				GetTextExtentPoint32(hDC, (LPCSTR)Buff, (int) strlen(Buff), &Size);
				POINT Pt;
				Pt.x = Rect.right / 2 - Size.cx / 2;
				Pt.y = Rect.bottom / 2 - Size.cy / 2;

				TextOut(hDC, Pt.x,Pt.y, Buff, (int) strlen(Buff));
		
				SetBkMode(hDC, Mode);
				SetTextColor(hDC,Color);

				return (INT_PTR)TRUE;
			}

        case WM_CLOSE:
            DestroyWindow(hDlg);
            return (INT_PTR)TRUE;
 
        case WM_DESTROY:
            PostQuitMessage(0);
			return (INT_PTR)TRUE;

		case WM_PAINT:
			hDC = BeginPaint(hDlg, &Ps);
			
			hBrush = ::CreateSolidBrush( RGB( 184, 190, 202) );
			if (!hBrush) return 0;

			GetClientRect(hDlg, &Rect);
			::FillRect(hDC, &Rect, hBrush);

		    ::DeleteObject( hBrush );

			hBrush = ::CreateSolidBrush( RGB(87, 114, 144) );
			if (!hBrush) return 0;

			GetClientRect(hDlg, &Rect);
			Rect.top = 22;
			Rect.left = 5;
			Rect.right = Rect.right - 5;
			Rect.bottom = Rect.bottom - 5;

			::FillRect(hDC, &Rect, hBrush);

			::DeleteObject( hBrush );

			//рисуем в окне приложения первый экран
			if(Status == FIRST_SCREEN)
			{
				//cam dist
				Pt[0].x = 11;
				Pt[0].y = 28;

				Pt[1].x = 298;
				Pt[1].y = 28;

				Pt[2].x = 298;
				Pt[2].y = 120;

				Pt[3].x = 51;
				Pt[3].y = 120;

				Pt[4].x = 51;
				Pt[4].y = 68;

				Pt[5].x = 11;
				Pt[5].y = 68;

				hRgn = ::CreatePolygonRgn((const POINT *)&Pt, 6, ALTERNATE);
				
				hBrush = ::CreateSolidBrush( RGB(175, 207, 206) );
				if (!hBrush) return 0;

				::FillRgn(hDC, hRgn, hBrush);

				::DeleteObject( hBrush );
				::DeleteObject( hRgn );

				//field of view
				Pt[0].x = 11;
				Pt[0].y = 125;

				Pt[1].x = 298;
				Pt[1].y = 125;

				Pt[2].x = 298;
				Pt[2].y = 238;

				Pt[3].x = 51;
				Pt[3].y = 238;

				Pt[4].x = 51;
				Pt[4].y = 165;

				Pt[5].x = 11;
				Pt[5].y = 165;

				hRgn = ::CreatePolygonRgn((const POINT *)&Pt, 6, ALTERNATE);

				hBrush = ::CreateSolidBrush( RGB(175, 207, 206) );
				if (!hBrush) return 0;

				::FillRgn(hDC, hRgn, hBrush);

				::DeleteObject( hBrush );
				::DeleteObject( hRgn );

				//60 fps
				Pt[0].x = 11;
				Pt[0].y = 243;

				Pt[1].x = 225;
				Pt[1].y = 243;

				Pt[2].x = 225;
				//Pt[2].y = 288;
				Pt[2].y = 295;

				Pt[3].x = 51;
				//Pt[3].y = 288;
				Pt[3].y = 295;

				Pt[4].x = 51;
				Pt[4].y = 283;

				Pt[5].x = 11;
				Pt[5].y = 283;

				hRgn = ::CreatePolygonRgn((const POINT*)& Pt, 6, ALTERNATE);

				hBrush = ::CreateSolidBrush(RGB(175, 207, 206));
				if (!hBrush) return 0;

				::FillRgn(hDC, hRgn, hBrush);

				::DeleteObject(hBrush);
				::DeleteObject(hRgn);


				//aspect ratio
				Pt[0].x = 303;
				Pt[0].y = 28;

				Pt[1].x = 558;
				Pt[1].y = 28;

				Pt[2].x = 558;
				Pt[2].y = 165;

				Pt[3].x = 345;
				Pt[3].y = 165;

				Pt[4].x = 345;
				Pt[4].y = 68;

				Pt[5].x = 303;
				Pt[5].y = 68;

				hRgn = ::CreatePolygonRgn((const POINT *)&Pt, 6, ALTERNATE);

				hBrush = ::CreateSolidBrush( RGB(175, 207, 206) );
				if (!hBrush) return 0;

				::FillRgn(hDC, hRgn, hBrush);

				::DeleteObject( hBrush );
				::DeleteObject( hRgn );			
					
				//button apply

				Pt[0].x = 250;
				Pt[0].y = 250;

				Pt[1].x = 380;
				Pt[1].y = 250;

				Pt[2].x = 380;
				Pt[2].y = 190;

				Pt[3].x = 558;
				Pt[3].y = 190;

				Pt[4].x = 558;
				Pt[4].y = 295;

				Pt[5].x = 250;
				Pt[5].y = 295;

				hRgn = ::CreatePolygonRgn((const POINT *)&Pt, 6, ALTERNATE);

				hBrush = ::CreateSolidBrush( RGB(175, 207, 206) );
				if (!hBrush) return 0;

				::FillRgn(hDC, hRgn, hBrush);

				::DeleteObject( hBrush );
				::DeleteObject( hRgn );

			}
			//рисуем в окне приложения result screen
			else if(Status == AFTER_APPLY_RESULT)
			{
				//тут выводим текст
				Pt[0].x = 53;
				Pt[0].y = 50;

				Pt[1].x = 358;
				Pt[1].y = 50;

				Pt[2].x = 358;
				Pt[2].y = 192;

				Pt[3].x = 53;
				Pt[3].y = 192;

				hRgn = ::CreatePolygonRgn((const POINT *)&Pt, 4, ALTERNATE);
	    
				hBrush = ::CreateSolidBrush( RGB(175, 207, 206) );
				if (!hBrush) return 0;

				::FillRgn(hDC, hRgn, hBrush);

				DeleteObject(hBrush);
				
				//тут расположена кнопка Exit
				Pt[0].x = 403;
				Pt[0].y = 221;

				Pt[1].x = 558;
				Pt[1].y = 221;

				Pt[2].x = 558;
				Pt[2].y = 288;

				Pt[3].x = 403;
				Pt[3].y = 288;

				hRgn = ::CreatePolygonRgn((const POINT *)&Pt, 4, ALTERNATE);
				
				hBrush = ::CreateSolidBrush( RGB(175, 207, 206) );
				if (!hBrush) return 0;

				::FillRgn(hDC, hRgn, hBrush);

				DeleteObject(hBrush);
			}

			EndPaint(hDlg, &Ps);
			
			return (INT_PTR)TRUE;

			//перемещаем окно держа мышей за заголовок
			case WM_LBUTTONDOWN:

				Pt1.x = LOWORD(lParam);
				Pt1.y = HIWORD(lParam);

				Rect.left = 0;
				Rect.top = 0;
				Rect.right = 570;
				Rect.bottom = 22;

				if(PtInRect(&Rect, Pt1))
				{
					SendMessage(hDlg, WM_SYSCOMMAND, SC_MOVE|0x0002, 0);
				}

				return (INT_PTR)TRUE;

	}

	return (INT_PTR)FALSE;
}
 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_hInst = hInstance;

	g_hDlg = CreateDialog(hInstance, 
				MAKEINTRESOURCE(DLG_MAIN), 
				NULL, (DLGPROC) WndProc); 

	if(!g_hDlg) return 0;

	DWORD Style = GetWindowLong(g_hDlg,GWL_STYLE);
	Style &= ~(WS_CAPTION|WS_SYSMENU|WS_BORDER);
	SetWindowLong(g_hDlg,GWL_STYLE,Style);
	
	int Width  = GetSystemMetrics(SM_CXSCREEN);
	int Height = GetSystemMetrics(SM_CYSCREEN);

	int RWidth = (Width - WINDOW_WIDTH) / 2;
	int RHeight = (Height - WINDOW_HEIGHT) / 2;

	::MoveWindow(g_hDlg,RWidth, RHeight + 1, WINDOW_WIDTH, WINDOW_HEIGHT, true);

	ShowWindow(g_hDlg, nCmdShow);

	InvalidateRect(g_hDlg, NULL, true);

	UpdateWindow(g_hDlg);

	InitCommonControls();

	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	delete pThreadParms;

	return (int) msg.wParam;
}
 
