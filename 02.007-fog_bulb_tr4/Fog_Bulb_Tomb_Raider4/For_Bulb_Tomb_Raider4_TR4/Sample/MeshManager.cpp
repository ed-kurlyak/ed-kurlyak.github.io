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


//оригинальный код из Tomb Raider 4
void CMeshManager::Update_MeshManager(CFirstPersonCamera *pCamera)
{
	DWORD m_dwNumVertices = 2076 * 3;

	//инициализация туманной лампы
	FogBulb bulb;
	bulb.inRange = true;
	bulb.WorldPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//bulb.vec = D3DXVECTOR3(0.0f, 0.0f, 9000.0f);
	bulb.rad = 4128.0f;
	bulb.sqrad = bulb.rad * bulb.rad;
	bulb.inv_sqrad = 1.0f / bulb.sqrad;
	//bulb.dist = bulb.rad;
	bulb.density = 128;

	bulb.vec = bulb.WorldPos - pCamera->vPos;

	D3DXVECTOR3 temp = bulb.vec;
	D3DXVec3TransformNormal(&bulb.vec, &temp, &pCamera->matView);


	bulb.pos = bulb.vec;

	D3DXVec3Normalize(&bulb.vec, &bulb.vec);

	bulb.vec.x = bulb.rad * bulb.vec.x + bulb.pos.x;
	bulb.vec.y = bulb.rad * bulb.vec.y + bulb.pos.y;
	bulb.vec.z = bulb.rad * bulb.vec.z + bulb.pos.z;

	bulb.dist = sqrtf(SQUARE(bulb.pos.x) + SQUARE(bulb.pos.y) + SQUARE(bulb.pos.z));

	if (!bulb.inRange)
		return;

	D3DXVECTOR3 pos, dP, dV;
	float val, val2;
	long lVal;

	for (DWORD i = 0; i < m_dwNumVertices; i++)
	{
		D3DVERTEX v = m_Vertices[i];

		//позиция вершины в мировых координатах
		D3DXVECTOR3 worldPos(v.p.x, v.p.y, v.p.z);

		//преобразование вершины в видовое пространство (камера → мир)
		D3DXVECTOR3 viewPos;
		D3DXVec3TransformCoord(&viewPos, &worldPos, &pCamera->matView);

		v.p.x = viewPos.x;
		v.p.y = viewPos.y;
		v.p.z = viewPos.z;

		//сброс альфа перед вычислением
		m_Vertices[i].specular = 0xFF000000 | (m_Vertices[i].specular & 0xFFFFFF);

		if (bulb.inRange)
		{
			pos.x = v.p.x;
			pos.y = v.p.y;
			pos.z = v.p.z;

			if (bulb.pos.z < pos.z)
			{
				pos.x *= bulb.dist * (1.0f / v.p.z);
				pos.y *= bulb.dist * (1.0f / v.p.z);
				pos.z *= bulb.dist * (1.0f / v.p.z);
			}

			dP.x = pos.x - bulb.pos.x;
			dP.y = pos.y - bulb.pos.y;
			dP.z = pos.z - bulb.pos.z;

			dV.x = bulb.vec.x - bulb.pos.x;
			dV.y = bulb.vec.y - bulb.pos.y;
			dV.z = bulb.vec.z - bulb.pos.z;

			val = SQUARE(dV.x) + SQUARE(dV.y) + SQUARE(dV.z);

			if (val)
			{
				val2 = (dP.x * dV.x + dP.y * dV.y + dP.z * dV.z) / val;

				if (val2 >= -1.0f)
				{
					if (val2 > 0.0f)
					{
						dP.x -= val2 * dV.x;
						dP.y -= val2 * dV.y;
						dP.z -= val2 * dV.z;
					}

					val = SQUARE(dP.x) + SQUARE(dP.y) + SQUARE(dP.z);

					if (val && val < bulb.sqrad)
					{
						val *= bulb.inv_sqrad * bulb.density;
						lVal = (long)val + ((m_Vertices[i].specular >> 24) & 0xFF) - bulb.density;

						if (lVal < 0)
							lVal = 0;

						m_Vertices[i].specular = (lVal << 24) | (m_Vertices[i].specular & 0xFFFFFF);
					}
				}
			}
		}
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
