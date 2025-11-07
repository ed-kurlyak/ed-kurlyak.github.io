//======================================================================================
//	Ed Kurlyak 2024 Volume Fog Tomb Raider 4 (Fog Bulb)
//======================================================================================

#ifndef _CAMERA_
#define _CAMERA_

#include <d3dx9math.h>

class CFirstPersonCamera
{
public:
	void InitCamera(HWND hWnd);
	D3DXMATRIX FrameMove(float fTime);
	D3DXVECTOR3 vRight,vUp,vLook,vPos;
	D3DXMATRIX matView;
	
private:
	D3DXVECTOR2 m_vMouseDelta;
	D3DXVECTOR3 m_vVelocity;
	FLOAT m_fDragTimer;
	FLOAT m_fTotalDragTimeToZero;
	D3DXVECTOR3 m_vVelocityDrag;
	HWND m_hWnd;
	
};

#endif