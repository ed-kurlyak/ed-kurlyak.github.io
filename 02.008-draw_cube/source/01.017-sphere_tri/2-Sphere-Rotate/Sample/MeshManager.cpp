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

vector3 CMeshManager::Vec3_Mat4x4_Mul(vector3 VecIn, matrix4x4 MatIn)
{
	vector3 VecOut;

	VecOut.x =	VecIn.x * MatIn[0][0] +
				VecIn.y * MatIn[1][0] +
				VecIn.z * MatIn[2][0] +
						  MatIn[3][0];

	VecOut.y =	VecIn.x * MatIn[0][1] +
				VecIn.y * MatIn[1][1] +
				VecIn.z * MatIn[2][1] +
						  MatIn[3][1];

	VecOut.z =	VecIn.x * MatIn[0][2] +
				VecIn.y * MatIn[1][2] +
				VecIn.z * MatIn[2][2] +
						  MatIn[3][2];

	return VecOut;
}

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;

	GetClientRect(m_hWnd, &m_Rc);

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
	static float Angle = 0.0f;

	matrix4x4 MatRotateY = {
	cosf(Angle),	0.0,	-sinf(Angle),	0.0,
	0.0,			1.0,	0.0,			0.0,
	sinf(Angle),	0.0,	cosf(Angle),	0.0,
	0.0,			0.0,	0.0,			1.0f };

	matrix4x4 MatRotateX = {
	1.0,			0.0,			0.0,			0.0,
	0.0,			cosf(Angle),	sinf(Angle),	0.0,
	0.0,			-sinf(Angle),	cosf(Angle),	0.0,
	0.0,			0.0,			0.0,			1.0f };

	Angle += PI / 100.0f;
	if(Angle > PI2 )
		Angle = 0.0f;

	for ( int i = 0; i < m_VertCount; i++ )
	{
		vector3 Vec = Vec3_Mat4x4_Mul(m_VertBuff[i], MatRotateY);
		Vec = Vec3_Mat4x4_Mul(Vec, MatRotateX);

		Vec.z = Vec.z + 15.0f;
		Vec.x = Vec.x / Vec.z;
		Vec.y = Vec.y / Vec.z;
		Vec.x = Vec.x / ((float)m_Rc.right / m_Rc.bottom);

		Vec.x = Vec.x * m_Rc.right / 2.0f + m_Rc.right / 2.0f;
		Vec.y =-Vec.y * m_Rc.bottom / 2.0f + m_Rc.bottom / 2.0f;

		m_VertBuffTransformed[i] = Vec;
	}

}

void CMeshManager::Draw_MeshManager()
{

	HDC hDC = GetDC(m_hWnd);

	HBRUSH hBrush = CreateSolidBrush(RGB(255,255,255));

	FillRect(hDC, &m_Rc, hBrush);

	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0) );
	HPEN hOldPen = (HPEN) SelectObject(hDC, hPen);

	for ( int i = 0; i < m_TriangleCount; i++ )
	{
		vector3 Vec1 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 0]];
		vector3 Vec2 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 1]];
		vector3 Vec3 = m_VertBuffTransformed[m_IndexBuff[i * 3 + 2]];

		MoveToEx(hDC, (int) Vec1.x, (int) Vec1.y, NULL);
		LineTo(hDC, (int) Vec2.x, (int) Vec2.y);
		LineTo(hDC, (int) Vec3.x, (int) Vec3.y);
		LineTo(hDC, (int) Vec1.x, (int) Vec1.y);
	}

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);

	DeleteObject(hBrush);

	ReleaseDC(m_hWnd, hDC);

	Sleep(50);
}

