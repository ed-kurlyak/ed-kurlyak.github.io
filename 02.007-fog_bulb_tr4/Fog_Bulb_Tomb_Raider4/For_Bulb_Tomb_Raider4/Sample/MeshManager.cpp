//======================================================================================
//	Ed Kurlyak 2024 Volume Fog Tomb Raider 4 (Fog Bulb)
//======================================================================================

#include "MeshManager.h"

CMeshManager::CMeshManager()
{
};

CMeshManager::~CMeshManager()
{
	
}

void CMeshManager::Init_MeshManager(HWND hWnd, LPDIRECT3DDEVICE9 pD3DDevice)
{
	m_hWnd = hWnd;
	m_pD3DDevice=pD3DDevice;

	m_NumTriangles = 2076;

	FILE* f;
	fopen_s(&f, "Room.txt", "rt");

	char Buffer[1024];
	fgets(Buffer, 1024, f);

	//всего количество треугольников 2076
	int Size;
	sscanf_s(Buffer, "%d", &Size);

	for (unsigned int i = 0; i < m_NumTriangles * 3; i++)
	{
		fgets(Buffer, 1024, f);
		sscanf_s(Buffer, "%f %f %f %f %f", &m_Vertices[i].p.x,
			&m_Vertices[i].p.y,
			&m_Vertices[i].p.z,
			&m_Vertices[i].tu,
			&m_Vertices[i].tv);
	}

	fclose(f);

	D3DXCreateTextureFromFile( m_pD3DDevice, "Room.bmp", &m_pMeshTextures);
}

void CMeshManager::Update_MeshManager(D3DXVECTOR3 CamPos)
{

	for( DWORD i=0; i < m_NumTriangles * 3; i++ )
    {
	
		D3DXVECTOR3 vPos(m_Vertices[i].p.x,m_Vertices[i].p.y,m_Vertices[i].p.z);
	
		D3DXVECTOR3 cameraPos = D3DXVECTOR3(CamPos.x, CamPos.y ,CamPos.z);	

		float fog = CheckSphere(cameraPos,vPos);

		float foga1;
	
		//radius sphere 4000, diameter 8000
		foga1= fog/8000.0f;
		if(foga1>1.0f) foga1 = 1.0f;

		int a = 255 - (int) (foga1 * 255.0f);

		m_Vertices[i].specular = D3DCOLOR_ARGB(a, 0, 0, 0);

	}
}

void CMeshManager::Draw_MeshManager()
{

	HRESULT hr;

	hr = m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	hr = m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

	DWORD fog_colr = D3DCOLOR_ARGB(0xFF, 0, 223, 191);

	m_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, fog_colr);

	float m_fFogStart = 1024.0f * 12.0f;
	float m_fFogEnd = 1024.0f * 20.0f;

	m_pD3DDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&m_fFogStart));
	m_pD3DDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&m_fFogEnd));
	
	//m_pD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	
	m_pD3DDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
	
	m_pD3DDevice->SetFVF( FVF );
	m_pD3DDevice->SetTexture( 0, m_pMeshTextures );
	m_pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_NumTriangles, m_Vertices,sizeof(D3DVERTEX));

	m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	
}

D3DXVECTOR3 CMeshManager::ComponentProd(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2)
{
	return D3DXVECTOR3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

float CMeshManager::CheckSphere(D3DXVECTOR3 &	cameraPos,D3DXVECTOR3 &	vertexPos)
{
	float fSphereRadius = 4000.0f;

	D3DXVECTOR3 vSphereCenter = D3DXVECTOR3(0,0,0);
	
	D3DXVECTOR3 m_Pos = vSphereCenter;
	
	D3DXVECTOR3 m_Scale(1.F / fSphereRadius, 1.F / fSphereRadius, 1.F / fSphereRadius);

	D3DXVECTOR3 adjCameraPos = ComponentProd((cameraPos - m_Pos), m_Scale);
	D3DXVECTOR3 adjVertexPos = ComponentProd((vertexPos - m_Pos), m_Scale);
	
	D3DXVECTOR3 adjDistance = adjVertexPos - adjCameraPos;

	float OD = D3DXVec3Dot(&adjDistance, &adjCameraPos);
	float D2 = D3DXVec3Dot(&adjDistance, &adjDistance);
	float O2 = D3DXVec3Dot(&adjCameraPos, &adjCameraPos);

	float radix = OD*OD - D2*(O2 - 1);
	if (radix <= 0) {
		
		return 0.0f;
	}

	float sradix = float(sqrt(radix));

	if (radix <= 0)
		return 0.0f;

	float t1 = (-OD - sradix) / D2;
	float t2 = (-OD + sradix) / D2;

	D3DXVECTOR3 v1; 
	D3DXVECTOR3 v2; 

	//камера вне сферы - обе точки пересечения на сфере
	if(t1 >= 0.0f && t1 < 1.0f && t2 > 0.0f && t2 <= 1.0f)
	{
		v1 = cameraPos + t1*(vertexPos - cameraPos);
		v2 = cameraPos + t2*(vertexPos - cameraPos);
	
		D3DXVECTOR3 vDist = v2 - v1;

		float val = D3DXVec3Length(&vDist);

		return val;
	}

	//вершина треугольника внутри сферы - камера вне сферы
	if(t1 >= 0.0f && t1 < 1.0f && t2 > 1.0f)
	{

		t2 = 1.0f;

		v1 = cameraPos + t1*(vertexPos - cameraPos);
		v2 = cameraPos + t2*(vertexPos - cameraPos);
	
		D3DXVECTOR3 vDist = v2 - v1;

		float val = D3DXVec3Length(&vDist);

		return val;
	}


	//камера внутри сферы - вершина треугльника вне сферы
	if(t1 < 0.0f && t2 > 0.0f && t2 <= 1.0f)
	{
		t1 = 0.0f;
		v1 = cameraPos + t1*(vertexPos - cameraPos);
		v2 = cameraPos + t2*(vertexPos - cameraPos);
	
		D3DXVECTOR3 vDist = v2 - v1;

		float val = D3DXVec3Length(&vDist);

		return val;
	}

	//камера внутри сферы - вершина внутри сферы		
	if(t1 < 0.0f && t2 > 1.0f)
	{
		D3DXVECTOR3 vDist = cameraPos - vertexPos;
		float val = D3DXVec3Length(&vDist);
		return val;
	}

	 return 0.0f;

}
