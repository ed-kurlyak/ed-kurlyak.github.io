//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include "MeshManager.h"

CMeshManager::CMeshManager()
{
}

CMeshManager::~CMeshManager()
{ 
	delete [] m_VertBuff;
	m_VertBuff = NULL;

	delete [] m_VertBuffTransformed;
	m_VertBuffTransformed = NULL;

	delete [] m_IndexBuff;
	m_IndexBuff = NULL;
}

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;

	int Slices = 20;
	int Stacks = 10;

	float ThetaFac = PI2 / Slices;
	float PhiFac = PI / Stacks;

	float Radius = 8.0f;

	m_VertCount = (Slices + 1) * (Stacks + 1);
	m_TriangleCount = Slices * Stacks * 2;

	m_VertBuff = new vector3[m_VertCount];
	m_VertBuffTransformed = new vector3[m_VertCount];

	int Index = 0;

	for ( int l = 0; l <= Slices; l++ )
	{
		for ( int b = 0; b <= Stacks; b++ )
		{
			m_VertBuff[Index].x = Radius * sinf(PhiFac * b) * sinf(ThetaFac * l);
			m_VertBuff[Index].y = Radius * cosf(PhiFac * b);
			m_VertBuff[Index].z = Radius * sinf(PhiFac * b) * cosf(ThetaFac * l);

			Index++;
		}
	}

	m_IndexBuff = new unsigned int[m_TriangleCount * 3];

	Index = 0;

	for ( int l = 0; l < Slices; l++ )
	{
		for ( int b = 0; b < Stacks; b++ )
		{
			int Next = l * (Stacks + 1) + b;
			int NextSection = (l + 1) * (Stacks + 1) + b;

			m_IndexBuff[Index + 0] = Next;
			m_IndexBuff[Index + 1] = Next + 1;
			m_IndexBuff[Index + 2] = NextSection + 1;

			m_IndexBuff[Index + 3] = Next;
			m_IndexBuff[Index + 4] = NextSection + 1;
			m_IndexBuff[Index + 5] = NextSection;

			Index += 6;
		}
	}
}

void CMeshManager::Update_MeshManager()
{
}

void CMeshManager::Draw_MeshManager()
{
	RECT Rc;
	GetClientRect(m_hWnd, &Rc);

	for ( int i = 0; i < m_VertCount; i++ )
	{
		vector3 v = m_VertBuff[i];
		
		v.z = v.z + 15.0f;
		v.x = v.x / v.z;
		v.y = v.y / v.z;
		v.x = v.x / ((float) Rc.right / Rc.bottom);

		v.x = v.x * Rc.right / 2.0f + Rc.right / 2.0f;
		v.y =-v.y * Rc.bottom / 2.0f + Rc.bottom / 2.0f;

		m_VertBuffTransformed[i] = v;
	}

	HDC hDC = GetDC(m_hWnd);

	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0) );
	HPEN hOldPen = (HPEN) SelectObject(hDC, hPen);

	for ( int i = 0; i < m_TriangleCount; i++ )
	{
		vector3 v1 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 0]];
		vector3 v2 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 1]];
		vector3 v3 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 2]];

		MoveToEx(hDC, (int) v1.x, (int) v1.y, NULL);
		LineTo(hDC, (int) v2.x, (int) v2.y);
		LineTo(hDC, (int) v3.x, (int) v3.y);
		LineTo(hDC, (int) v1.x, (int) v1.y);
	}

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
	ReleaseDC(m_hWnd, hDC);
}

