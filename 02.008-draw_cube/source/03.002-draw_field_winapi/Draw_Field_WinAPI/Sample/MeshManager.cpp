//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#include "MeshManager.h"

CMeshManager::CMeshManager()
{
}

CMeshManager::~CMeshManager()
{
	Delete_BackBuffer();

}

vector3 CMeshManager::Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 &MatIn)
{
	    vector3 VecOut;

		VecOut.x =	VecIn.x * MatIn.Mat[0][0] +
					VecIn.y * MatIn.Mat[1][0] + 
					VecIn.z * MatIn.Mat[2][0] + 
							  MatIn.Mat[3][0];
        
		VecOut.y =	VecIn.x * MatIn.Mat[0][1] + 
					VecIn.y * MatIn.Mat[1][1] + 
					VecIn.z * MatIn.Mat[2][1] + 
							  MatIn.Mat[3][1];
        
		VecOut.z =	VecIn.x * MatIn.Mat[0][2] + 
					VecIn.y * MatIn.Mat[1][2] + 
					VecIn.z * MatIn.Mat[2][2] + 
							  MatIn.Mat[3][2];

		return VecOut;
}

float CMeshManager::Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2)
{
	return VecIn1.x*VecIn2.x + VecIn1.y*VecIn2.y + VecIn1.z* VecIn2.z;
}

vector3 CMeshManager::Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut;

	VecOut.x = VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y;
	VecOut.y = VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z;
	VecOut.z = VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x;

	return VecOut;
}

vector3 CMeshManager::Vec3_Normalize(vector3 &VecIn)
{
	vector3 VecOut;

	float Len = sqrtf(VecIn.x * VecIn.x + VecIn.y * VecIn.y + VecIn.z * VecIn.z);

	VecOut.x = VecIn.x / Len;
	VecOut.y = VecIn.y / Len;
	VecOut.z = VecIn.z / Len;
	
	return VecOut;

}

vector3 CMeshManager::Vec3_Scale(vector3 &VecIn, float ValIn)
{
	vector3 VecOut;

	VecOut.x = VecIn.x * ValIn;
	VecOut.y = VecIn.y * ValIn;
	VecOut.z = VecIn.z * ValIn;

	return VecOut;
}

vector3 CMeshManager::Vec3_Add(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut;

	VecOut.x = VecIn1.x + VecIn2.x;
	VecOut.y = VecIn1.y + VecIn2.y;
	VecOut.z = VecIn1.z + VecIn2.z;

	return VecOut;
}

matrix4x4 CMeshManager::Matrix_Rotation_Axis(vector3 &VecIn, float Angle)
{
		float x = VecIn.x;
		float y = VecIn.y;
		float z = VecIn.z;
	
		float s = sin(Angle);
		float c = cos(Angle);
		float omc = 1.0f - c;

		float xomc = x * omc;
		float yomc = y * omc;
		float zomc = z * omc;

		float xxomc = x * xomc;
		float xyomc = x * yomc;
		float xzomc = x * zomc;
		
		float yyomc = y * yomc;
		float yzomc = y * zomc;
		float zzomc = z * zomc;

		float xs = x * s;
		float ys = y * s;
		float zs = z * s;

		matrix4x4 MatRotate= {xxomc + c,  xyomc + zs, xzomc - ys, 0.0f,
			xyomc - zs, yyomc + c,  yzomc + xs, 0.0f,
			xzomc + ys, yzomc - xs, zzomc + c, 0.0f,
			0.0f ,0.0f, 0.0f, 1.0f};

		return MatRotate;
}

matrix4x4 CMeshManager::Get_View_Matrix()
{
	float Time = Get_Elapsed_Time();

	UINT ScreenWidth = m_ViewWidth;
	UINT ScreenHeight = m_ViewHeight;

	POINT MousePos;
	GetCursorPos(&MousePos);

	SetCursorPos(ScreenWidth/2, ScreenHeight/2);
    
	int DeltaX=ScreenWidth/2-MousePos.x;
	int DeltaY=ScreenHeight/2-MousePos.y;

	//что бы перемещать камеру вертикально
	//поставте значение m_RotationScalerY
	//больше нуля
	//float m_RotationScalerY = 0.1f;
	float m_RotationScalerY = 0.0f;
	float m_RotationScalerX = 0.075f;

	//реакция на движение мышью

	//вертикальное смещение камеры (вокруг оси X)
	if(DeltaY<0) m_RotationScalerY = m_RotationScalerY;
	else if(DeltaY>0) m_RotationScalerY = -m_RotationScalerY;
	else if(DeltaY==0) m_RotationScalerY = 0;

	matrix4x4 MatRotRight;
	MatRotRight = Matrix_Rotation_Axis(m_VecCamRight , m_RotationScalerY);
	
	m_VecCamRight = Vec3_Mat4x4_Mul(m_VecCamRight, MatRotRight);
	m_VecCamUp = Vec3_Mat4x4_Mul(m_VecCamUp, MatRotRight);
	m_VecCamLook = Vec3_Mat4x4_Mul(m_VecCamLook, MatRotRight);
	
	//горизонтальное смещение камеры (вокруг оси Y)
	if(DeltaX<0) m_RotationScalerX = m_RotationScalerX;
	else if(DeltaX>0) m_RotationScalerX = -m_RotationScalerX;
	else if(DeltaX==0) m_RotationScalerX = 0;

	vector3 VecUpTemp = { 0.0f, 1.0f, 0.0f };
	matrix4x4 MatRotUp;
	MatRotUp = Matrix_Rotation_Axis(VecUpTemp , m_RotationScalerX);
	
	m_VecCamRight = Vec3_Mat4x4_Mul(m_VecCamRight, MatRotUp);
	m_VecCamUp = Vec3_Mat4x4_Mul(m_VecCamUp, MatRotUp);
	m_VecCamLook = Vec3_Mat4x4_Mul(m_VecCamLook, MatRotUp);
	
	//реакция на клавиши W,S,A,D
	float RatioMove = 50;
	vector3 VecAccel = {0,0,0};
	
	if(GetAsyncKeyState('W')& 0xFF00) 
	{
		vector3 VecTemp = {m_VecCamLook.x,0,m_VecCamLook.z};

		VecAccel +=Vec3_Scale(VecTemp, Time * RatioMove);
	}

	if(GetAsyncKeyState('S')& 0xFF00) 
	{
		vector3 VecTemp = {m_VecCamLook.x,0,m_VecCamLook.z};

		VecAccel += Vec3_Scale(VecTemp, Time * -RatioMove);
	}

	if(GetAsyncKeyState('D')& 0xFF00) 
	{
		vector3 VecTemp = {m_VecCamRight.x,0,m_VecCamRight.z};

		VecAccel += Vec3_Scale(VecTemp, Time * RatioMove);
	}

	if(GetAsyncKeyState('A')& 0xFF00) 
	{
		vector3 VecTemp = {m_VecCamRight.x,0,m_VecCamRight.z};

		VecAccel += Vec3_Scale(VecTemp, Time * -RatioMove);
	}

	m_VecCamPos = Vec3_Add(m_VecCamPos,VecAccel);

	m_VecCamLook = Vec3_Normalize(m_VecCamLook);
	m_VecCamUp = Vec3_Cross(m_VecCamLook,m_VecCamRight);
	m_VecCamUp = Vec3_Normalize(m_VecCamUp);
	m_VecCamRight = Vec3_Cross(m_VecCamUp,m_VecCamLook);
	m_VecCamRight = Vec3_Normalize(m_VecCamRight);

	float xp = -Vec3_Dot(m_VecCamPos, m_VecCamRight);
	float yp = -Vec3_Dot(m_VecCamPos, m_VecCamUp);
	float zp = -Vec3_Dot(m_VecCamPos, m_VecCamLook);

	matrix4x4 MatView = {
        m_VecCamRight.x, m_VecCamUp.x, m_VecCamLook.x, 0,
		m_VecCamRight.y, m_VecCamUp.y, m_VecCamLook.y, 0,
		m_VecCamRight.z, m_VecCamUp.z, m_VecCamLook.z, 0,
		xp,		  yp,	 zp,	  1 };

	return MatView;
}

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;

	RECT Rc;
    ::GetClientRect( m_hWnd, &Rc );

    m_ViewWidth  = Rc.right - Rc.left;
    m_ViewHeight = Rc.bottom - Rc.top;

	Create_BackBuffer();

	ShowCursor(FALSE);

	Timer_Start();

	UINT j=0;
	float k=0.0f;
	UINT i=0;
	//линии паралельно оси X
	while(i< ( 25 + 1) )
	{
	
		m_VertBuff[j].x = 0;
		m_VertBuff[j].y = 0.0f;
		m_VertBuff[j].z = k;

		m_VertBuff[j + 1].x = FILED_SIZE*CELL_SIZE;
		m_VertBuff[j + 1].y = 0.0f;
		m_VertBuff[j + 1].z = k;

		i++;
		j+=2;
		k+=CELL_SIZE;
	}

	//j=25;
	k=0.0f;
	i=0;
	//линии паралельно оси Z
	while(i<( 25 + 1))

	{
	
		m_VertBuff[j].x = k;
		m_VertBuff[j].y = 0.0f;
		m_VertBuff[j].z = 0;
		
		m_VertBuff[j + 1].x = k;
		m_VertBuff[j + 1].y = 0.0f;
		m_VertBuff[j + 1].z = FILED_SIZE*CELL_SIZE;

		i++;
		j+=2;
		k+=CELL_SIZE;
	}

	m_VecCamUp.x = 0.0f; 
	m_VecCamUp.y = 1.0f; 
	m_VecCamUp.z = 0.0f; 
	
	m_VecCamLook.x = 0.0f;
	m_VecCamLook.y = 0.0f;
	//m_VecCamLook.z = -1.0f;
	m_VecCamLook.z = 1.0f;

	//камера может смотреть в сторону +Z или -Z
	if(m_VecCamLook.z > 0)
	{
		m_VecCamRight.x = 1.0f;
		m_VecCamRight.y = 0.0f;
		m_VecCamRight.z = 0.0f;
	}
		
	if(m_VecCamLook.z < 0)
	{
		m_VecCamRight.x = -1.0f;
		m_VecCamRight.y = 0.0f;
		m_VecCamRight.z = 0.0f;
	}

	m_VecCamPos.x = 50.0f;
	m_VecCamPos.y = 5.0f;
	m_VecCamPos.z = -25.0f;
}

vector3 CMeshManager::Calc_Edge(vector3 &VecIn1, vector3 &VecIn2)
{
	//вершина перед передней плоскостью v1
	//вершина за передней проскостью v2

	vector3 VecOut;

	float d = VecIn1.z / (VecIn1.z - VecIn2.z);

	VecOut.x = (VecIn2.x - VecIn1.x) * d + VecIn1.x;
	VecOut.y = (VecIn2.y - VecIn1.y) * d + VecIn1.y;
	//VecOut.z = m_ZNear + 0.1f;
	VecOut.z = 0.1f;
	
	//VecOut.tu = (VecIn2.tu - VecIn1.tu) * d + VecIn1.tu;
	//VecOut.tv = (VecIn2.tv - VecIn1.tv) * d + VecIn1.tv;

	return VecOut;
}

void CMeshManager::Update_MeshManager()
{

}

void CMeshManager::Draw_MeshManager ()
{
	matrix4x4 MatView = Get_View_Matrix();

	//у нас 100 вершин это 50 линий
	//плюс 2 замыкающие линии это 4 вершины
	for ( int i = 0; i < ( 100 + 4 ); i++ )
	{
		vector3 v = Vec3_Mat4x4_Mul(m_VertBuff[i], MatView);

		m_VertBuffTransformed[i] = v;
	}

	Clear_BackBuffer();

	HPEN hPen = CreatePen(PS_SOLID, 4, RGB(255, 255, 127));
	HPEN hOldPen = (HPEN) SelectObject(m_hBackBuffer, hPen);

	//50 линий сетка + еще 2 линий в конце
	//одна по горизонтали и одна по вертикали (замыкающие линии)
	for ( int i = 0; i < ( 50 + 2 ); i++ )
	{
		vector3 v1 = m_VertBuffTransformed[ i * 2 + 0 ];
		vector3 v2 = m_VertBuffTransformed[ i * 2 + 1 ];

		//выполняем отсечение по передней плоскости отсечения
		//если это необходимо, это необходимо когда
		//Z координата вершины v1 или v2 меньше нуля

		//линия за передней плоскостью отсечения
		//обе вершины за передней плоскостью отсечения
		if( (v1.z < 0) && (v2.z < 0) )
			continue;

		//первая вершина v1 за передней плоскостью отсечения
		if(v1.z < 0)
		{
			//v2 перед передней плоскостью
			//v1 за передней плоскостью
			v1 = Calc_Edge(v2, v1);
			
			/*
			float d = v2.z / (v2.z - v1.z);
			v1.x = (v1.x - v2.x) * d + v2.x;
			v1.y = (v1.y - v2.y) * d + v2.y;
			v1.z = 0.1f;
			*/
		}

		//вторая вершина v2 за передней плоскостью отсечения
		if(v2.z < 0)
		{
			//v1 перед передней плоскостью
			//v2 за передней плоскостью
			v2 = Calc_Edge(v1, v2);
			
			/*
			float d = v1.z / (v1.z - v2.z);
			v2.x = (v2.x - v1.x) * d + v1.x;
			v2.y = (v2.y - v1.y) * d + v1.y;
			v2.z = 0.1f;
			*/
		}

		//далее перспективное деление, вычисление аспекта
		//экранные координаты для каждой вершины v1 и v2

		v1.x = v1.x / v1.z;
		v1.y = v1.y / v1.z;
		v1.x = v1.x / ((float) m_ViewWidth / (float) m_ViewHeight);

		v1.x = v1.x * m_ViewWidth / 2.0f + m_ViewWidth / 2.0f;
		v1.y =-v1.y * m_ViewHeight / 2.0f + m_ViewHeight / 2.0f;

		v2.x = v2.x / v2.z;
		v2.y = v2.y / v2.z;
		v2.x = v2.x / ((float) m_ViewWidth / (float) m_ViewHeight);

		v2.x = v2.x * m_ViewWidth / 2.0f + m_ViewWidth / 2.0f;
		v2.y =-v2.y * m_ViewHeight / 2.0f + m_ViewHeight / 2.0f;

		MoveToEx(m_hBackBuffer, (int)v1.x, (int)v1.y, NULL);
		LineTo(m_hBackBuffer, (int)v2.x, (int)v2.y);
	}

	SelectObject(m_hBackBuffer, hOldPen);
	DeleteObject(hPen);

	Present_BackBuffer();

}

void CMeshManager::Timer_Start()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)& m_PerfFreq);
	QueryPerformanceCounter((LARGE_INTEGER*)& m_LastTime);
	m_StartTime = m_LastTime;
	m_TimeScale = 1.0f / m_PerfFreq;
}

float CMeshManager::Get_Elapsed_Time()
{
	__int64 nowTime;
	
	QueryPerformanceCounter((LARGE_INTEGER*)& nowTime);
	m_ElapsedTime = (nowTime - m_StartTime) * m_TimeScale;
	m_StartTime = nowTime;

	return m_ElapsedTime;
}

void CMeshManager::Create_BackBuffer()
{
	//размеры окна приложения (заднего буфера)
	GetClientRect(m_hWnd, &m_Rc);

	m_ViewWidth = m_Rc.right;
	m_ViewHeight = m_Rc.bottom;

	m_hDC = GetDC(m_hWnd);

	//кисть для очистки окна приложения (заднего буфера)
	m_hBrush = CreateSolidBrush(RGB(0,0,0));
	//m_hBrush = CreateSolidBrush(RGB(255,255,255));

	//наш задний буфер
	m_hBackBuffer = CreateCompatibleDC(m_hDC);
	m_hBitmap = CreateCompatibleBitmap(m_hDC, m_ViewWidth, m_ViewHeight);
	m_hOldBitmap = (HBITMAP) SelectObject(m_hBackBuffer, m_hBitmap);
}

void CMeshManager::Clear_BackBuffer()
{
	//очистка окна приложения
	FillRect(m_hBackBuffer, &m_Rc, m_hBrush);
}

void CMeshManager::Present_BackBuffer()
{
	//выводим задний буфер на экран приложения
	BitBlt(m_hDC, 0, 0, m_ViewWidth, m_ViewHeight, m_hBackBuffer, 0, 0, SRCCOPY);
}

void CMeshManager::Delete_BackBuffer()
{
	//уничтожение заднего буфера
	SelectObject(m_hBackBuffer, m_hOldBitmap);
	DeleteObject(m_hBitmap);

	DeleteObject(m_hBrush);

	ReleaseDC(m_hWnd, m_hDC);
	DeleteDC(m_hBackBuffer);
}
