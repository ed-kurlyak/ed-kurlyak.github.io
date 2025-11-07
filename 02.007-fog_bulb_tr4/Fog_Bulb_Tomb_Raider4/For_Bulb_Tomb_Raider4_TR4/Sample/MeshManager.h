//======================================================================================
//	Ed Kurlyak 2024 Volume Fog Tomb Raider 4 (Fog Bulb)
//======================================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include "camera.h"

struct D3DVERTEX
{
	D3DXVECTOR3 p;
	D3DCOLOR	specular;
	float tu, tv;
};

#define FVF D3DFVF_XYZ | D3DFVF_SPECULAR | D3DFVF_TEX1

#define SQUARE(x) ((x) * (x))

struct FogBulb
{
	D3DXVECTOR3 WorldPos;    //мирова€ позици€ лампы (дл€ справки, может не использоватьс€)
	D3DXVECTOR3 pos;         //позици€ центра в видовом пространстве
	D3DXVECTOR3 vec;         //вектор направлени€ туманного объЄма
	float rad;               //радиус
	float sqrad;             //радиус в квадрате
	float inv_sqrad;         //обратный квадрат радиуса
	float dist;              //длина туманного объЄма
	long density;            //плотность тумана (0Ц255)
	long inRange;            //флаг активности (1 Ч включЄн)
	long timer;              //врем€ жизни эффекта (если нужно)
	long active;             //1 Ч активен, 0 Ч выключен
	long FXRad;              //радиус визуального эффекта (дл€ вспышек/огн€ и т. п.)
	long room_number;        //номер комнаты (если есть разбиение уровней)
	long r;                  //красна€ компонента цвета тумана (0Ц255)
	long g;                  //зелЄна€ компонента
	long b;                  //син€€ компонента
};

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();

	void Init_MeshManager(HWND hWnd, LPDIRECT3DDEVICE9 pD3DDevice);
	void Update_MeshManager(CFirstPersonCamera *pCamera);
	void Draw_MeshManager();

private:
	
	HWND m_hWnd;

	LPDIRECT3DDEVICE9 m_pD3DDevice;

	D3DVERTEX m_Vertices[2076 * 3];
	DWORD m_NumTriangles;
	LPDIRECT3DTEXTURE9 m_pMeshTextures;

};

#endif