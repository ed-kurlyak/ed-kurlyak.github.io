//======================================================================================
//	Ed Kurlyak 2024 Volume Fog Tomb Raider 4 (Fog Bulb)
//======================================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>

struct D3DVERTEX
{
	D3DXVECTOR3 p;
	D3DCOLOR	specular;
	float tu, tv;
};

#define FVF D3DFVF_XYZ | D3DFVF_SPECULAR | D3DFVF_TEX1

class CMeshManager
{
public:

	CMeshManager();
	~CMeshManager();

	void Init_MeshManager(HWND hWnd, LPDIRECT3DDEVICE9 pD3DDevice);
	void Update_MeshManager(D3DXVECTOR3 CamPos);
	void Draw_MeshManager();

private:
	
	HWND m_hWnd;

	LPDIRECT3DDEVICE9 m_pD3DDevice;

	D3DVERTEX m_Vertices[2076 * 3];
	DWORD m_NumTriangles;
	LPDIRECT3DTEXTURE9 m_pMeshTextures;

	D3DXVECTOR3 ComponentProd(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
	float CheckSphere(D3DXVECTOR3 &	cameraPos,D3DXVECTOR3 &	vertexPos);

};

#endif