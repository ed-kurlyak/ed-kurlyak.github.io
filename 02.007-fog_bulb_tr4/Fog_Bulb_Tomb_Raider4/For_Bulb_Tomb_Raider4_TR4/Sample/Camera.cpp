//======================================================================================
//	Ed Kurlyak 2024 Volume Fog Tomb Raider 4 (Fog Bulb)
//======================================================================================

#include "camera.h"

void CFirstPersonCamera::InitCamera(HWND hWnd)
{
	m_hWnd = hWnd;

	D3DXMatrixIdentity(&matView);
	
	vRight = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
    vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    vLook = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    vPos = D3DXVECTOR3( 0.0f, 150.0f, -6000.0f );

	D3DXMatrixLookAtLH( &matView,  &vPos,
		                           &D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f ),
			                       &vUp );
	

    m_vMouseDelta = D3DXVECTOR2(0,0);
	m_vVelocity     = D3DXVECTOR3(0,0,0);
	m_fDragTimer    = 0.0f;
	m_fTotalDragTimeToZero = 0.25;
	m_vVelocityDrag = D3DXVECTOR3(0,0,0);
}

D3DXMATRIX CFirstPersonCamera::FrameMove(float fTime)
{
	ShowCursor(FALSE);
	
	POINT mousePos;
	GetCursorPos(&mousePos);

	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	SetCursorPos(rc.right/2,rc.bottom/2);

	float m_fFramesToSmoothMouseData = 2.0f;
	float fPercentOfNew =  1.0f / m_fFramesToSmoothMouseData;
    float fPercentOfOld =  1.0f - fPercentOfNew;
    
	m_vMouseDelta.x = m_vMouseDelta.x*fPercentOfOld + mousePos.x*fPercentOfNew;
    m_vMouseDelta.y = m_vMouseDelta.y*fPercentOfOld + mousePos.y*fPercentOfNew;

	float m_fRotationScaler = (float) 0.005f;

	D3DXVECTOR2 m_vRotVelocity = m_vMouseDelta * m_fRotationScaler;

	float m_RotVelocityY = 0.0;
	float m_RotVelocityX = 0.0;

	int nDeltaX=rc.right/2-mousePos.x;
	if(nDeltaX<0) m_RotVelocityX = m_vRotVelocity.x;
	if(nDeltaX>0) m_RotVelocityX = -m_vRotVelocity.x;
	if(nDeltaX==0) m_RotVelocityX = 0.0;

	D3DXMATRIX matRotUp;	
	D3DXVECTOR3 vUpTemp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixRotationAxis(&matRotUp,&vUpTemp,D3DXToRadian(m_RotVelocityX ));
		
	D3DXVec3TransformNormal(&vRight,&vRight,&matRotUp);
	D3DXVec3TransformNormal(&vUp,&vUp,&matRotUp);
	D3DXVec3TransformNormal(&vLook,&vLook,&matRotUp);
		

	int nDeltaY=rc.bottom/2-mousePos.y;
	if(nDeltaY<0) m_RotVelocityY = m_vRotVelocity.y;
	if(nDeltaY>0) m_RotVelocityY = -m_vRotVelocity.y;
	if(nDeltaY==0) m_RotVelocityY = 0.0;

	D3DXMATRIX matRotRight;	
	D3DXMatrixRotationAxis(&matRotRight,&vRight,D3DXToRadian(m_RotVelocityY));
	D3DXVec3TransformNormal(&vRight,&vRight,&matRotRight);
	D3DXVec3TransformNormal(&vUp,&vUp,&matRotRight);
	D3DXVec3TransformNormal(&vLook,&vLook,&matRotRight);

	float ratioMove = 7500;
	D3DXVECTOR3 vAccel = D3DXVECTOR3(0,0,0);

	if(GetAsyncKeyState('W')& 0xFF00) 
	{
		vAccel = D3DXVECTOR3(vLook.x, 0.0f, vLook.z) * ratioMove * fTime;
	}
	else if(GetAsyncKeyState('S')& 0xFF00) 
	{
		vAccel = D3DXVECTOR3(vLook.x, 0.0f, vLook.z) * -ratioMove * fTime;
	}
	else if(GetAsyncKeyState('D')& 0xFF00) 
	{
		vAccel = D3DXVECTOR3(vRight.x, 0.0f, vRight.z) * ratioMove * fTime;
	}
	else if(GetAsyncKeyState('A')& 0xFF00) 
	{
		vAccel = D3DXVECTOR3(vRight.x, 0.0f, vRight.z) * -ratioMove * fTime;
	}
	
	if( D3DXVec3LengthSq( &vAccel ) > 0 )
        {
            m_vVelocity = vAccel;
            m_fDragTimer = m_fTotalDragTimeToZero;
            m_vVelocityDrag = vAccel / m_fDragTimer;
        }
        else 
        {
            if( m_fDragTimer > 0 )
            {
                
                m_vVelocity -= m_vVelocityDrag * fTime;
                m_fDragTimer -= fTime;
				
            }
            else
            {
                
                m_vVelocity = D3DXVECTOR3(0,0,0);
            }
        }
    
	vPos += m_vVelocity;

	//vPos += vAccel;

	D3DXVec3Normalize(&vLook,&vLook);
	D3DXVec3Cross(&vUp,&vLook,&vRight);
	D3DXVec3Normalize(&vUp,&vUp);
	D3DXVec3Cross(&vRight,&vUp,&vLook);
	D3DXVec3Normalize(&vRight,&vRight);

	matView._11 = vRight.x;
	matView._21 = vRight.y;
	matView._31 = vRight.z;

	matView._12 = vUp.x;
	matView._22 = vUp.y;
	matView._32 = vUp.z;

	matView._13 = vLook.x;
	matView._23 = vLook.y;
	matView._33 = vLook.z;

	matView._41 =- D3DXVec3Dot( &vPos, &vRight );
    matView._42 =- D3DXVec3Dot( &vPos, &vUp    );
	matView._43 =- D3DXVec3Dot( &vPos, &vLook  );

	return matView;
}

