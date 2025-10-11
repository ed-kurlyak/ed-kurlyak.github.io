//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#ifndef _MYAPP_
#define _MYAPP_

#include <windows.h>
#include <windowsx.h>
#include "MeshManager.h"

#include <d3dx9.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

#define APPNAME "Software Rendering"
#define CLASSNAME "Sample"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

class CMyApp
{
public:
	int Program_Begin(HINSTANCE	hInstance, int nCmdShow);

private:
	static LRESULT CALLBACK Static_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void RenderScene();

	HWND m_hWnd;
	CMeshManager m_MeshManager;

	LPDIRECT3DDEVICE9 p_d3d_Device;
	LPDIRECT3D9 p_d3d;
};

#endif