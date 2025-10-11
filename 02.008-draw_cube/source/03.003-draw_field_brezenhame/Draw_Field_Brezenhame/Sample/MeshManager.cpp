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
	MatRotRight = Matrix_Rotation_Axis(m_VecCamRight, m_RotationScalerY);
	
	m_VecCamRight = Vec3_Mat4x4_Mul(m_VecCamRight, MatRotRight);
	m_VecCamUp = Vec3_Mat4x4_Mul(m_VecCamUp, MatRotRight);
	m_VecCamLook = Vec3_Mat4x4_Mul(m_VecCamLook, MatRotRight);
	
	//горизонтальное смещение камеры (вокруг оси Y)
	if(DeltaX<0) m_RotationScalerX = m_RotationScalerX;
	else if(DeltaX>0) m_RotationScalerX = -m_RotationScalerX;
	else if(DeltaX==0) m_RotationScalerX = 0;

	vector3 VecUpTemp = { 0.0f, 1.0f, 0.0f };
	matrix4x4 MatRotUp;
	MatRotUp = Matrix_Rotation_Axis(VecUpTemp, m_RotationScalerX);
	
	m_VecCamRight = Vec3_Mat4x4_Mul(m_VecCamRight, MatRotUp);
	m_VecCamUp = Vec3_Mat4x4_Mul(m_VecCamUp, MatRotUp);
	m_VecCamLook = Vec3_Mat4x4_Mul(m_VecCamLook, MatRotUp);
	
	//реакция на клавиши W,S,A,D
	float RatioMove = 50;
	vector3 VecAccel = {0,0,0};
	
	if(GetAsyncKeyState('W')& 0xFF00) 
	{
		vector3 VecTemp = {m_VecCamLook.x,0,m_VecCamLook.z};

		VecAccel += Vec3_Scale(VecTemp, Time * RatioMove);
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

void CMeshManager::Draw_Line_Brezenhame(int x1, int y1, int x2, int y2)
{

    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);
    const int signX = x1 < x2 ? 1 : -1;
    const int signY = y1 < y2 ? 1 : -1;
    int error = deltaX - deltaY;

	int indx =  y2 * 4 * m_ViewWidth + x2 * 4;
	m_Data[indx] = 127; // blue
	m_Data[indx + 1] = 255; // green
	m_Data[indx + 2] = 255; // red
	m_Data[indx + 3] = 0; 

    while(x1 != x2 || y1 != y2) 
    {
     	int indx =  y1 * 4 * m_ViewWidth + x1 * 4;
		m_Data[indx] = 127; // blue
		m_Data[indx + 1] = 255; // green
		m_Data[indx + 2] = 255; // red
		m_Data[indx + 3] = 0; 
        
		int error2 = error * 2;
        
		if(error2 > -deltaY) 
        {
            error -= deltaY;
            x1 += signX;
        }
        
		if(error2 < deltaX) 
        {
            error += deltaX;
            y1 += signY;
        }
    }
}

void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;
	
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

int CMeshManager::ComputeCode(int x, int y, int left, int right, int top, int bottom)
{
    int code = 0;
    if (x < left) code |= LEFT;
    if (x > right) code |= RIGHT;
    if (y < top) code |= TOP;
    if (y > bottom) code |= BOTTOM;
    return code;
}

void CMeshManager::CohenSutherlandClip(int x1, int y1, int x2, int y2,
                         int left, int right, int top, int bottom)
{
    int code1 = ComputeCode(x1, y1, left, right, top, bottom);
    int code2 = ComputeCode(x2, y2, left, right, top, bottom);
    bool accept = false;

    while (true)
    {
        if (!(code1 | code2)) //Оба конца внутри окна
        {
            accept = true;
            break;
        }
        else if (code1 & code2) //Оба конца вне окна в одном направлении
        {
            break;
        }
        else
        {
            int codeOut;
            int x, y;

            if (code1 != 0)
                codeOut = code1;
            else
                codeOut = code2;

            if (codeOut & TOP)
            {
                x = x1 + (x2 - x1) * (top - y1) / (y2 - y1);
                y = top;
            }
            else if (codeOut & BOTTOM)
            {
                x = x1 + (x2 - x1) * (bottom - y1) / (y2 - y1);
                y = bottom;
            }
            else if (codeOut & RIGHT)
            {
                y = y1 + (y2 - y1) * (right - x1) / (x2 - x1);
                x = right;
            }
            else if (codeOut & LEFT)
            {
                y = y1 + (y2 - y1) * (left - x1) / (x2 - x1);
                x = left;
            }

            if (codeOut == code1)
            {
                x1 = x;
                y1 = y;
                code1 = ComputeCode(x1, y1, left, right, top, bottom);
            }
            else
            {
                x2 = x;
                y2 = y;
                code2 = ComputeCode(x2, y2, left, right, top, bottom);
            }
        }
    }

    if (accept)
    {
		Draw_Line_Brezenhame(x1, y1, x2, y2);

    }
    else
    {
        //Line is outside the clipping area
    }
}

vector3 CMeshManager::Interpolate(const vector3 &VecIn1, const vector3 &VecIn2, float t)
{
	vector3 VecOut = {
        VecIn1.x + t * (VecIn2.x - VecIn1.x),
        VecIn1.y + t * (VecIn2.y - VecIn1.y),
        VecIn1.z + t * (VecIn2.z - VecIn1.z) };
	
    return VecOut;
	
}

bool CMeshManager::ClipLineZNear(const vector3 &VecIn1, const vector3 &VecIn2, float zNear,
								 vector3 &VecOut1, vector3 &VecOut2)
{
    //Оба конца перед ближней плоскостью
    if (VecIn1.z >= zNear && VecIn2.z >= zNear)
    {
		VecOut1 = VecIn1;
		VecOut2 = VecIn2;

        return true; //Линия полностью видима
    }

    // Оба конца за ближней плоскостью
    if (VecIn1.z < zNear && VecIn2.z < zNear)
    {
        return false;// Линия полностью невидима
    }

    //Один конец перед ближней плоскостью, другой за
    float t;
    VecOut1 = VecIn1;
    VecOut2 = VecIn2;

    if (VecIn1.z < zNear)
    {
        //VecIn1 за плоскостью, обрезаем
        t = (zNear - VecIn2.z) / (VecIn1.z - VecIn2.z);
        VecOut1 = Interpolate(VecIn2, VecIn1, t);
        VecOut1.z = zNear;
    }
    else if (VecIn2.z < zNear)
    {
        //VecIn2 за плоскостью, обрезаем
        t = (zNear - VecIn1.z) / (VecIn2.z - VecIn1.z);
        VecOut2 = Interpolate(VecIn1, VecIn2, t);
        VecOut2.z = zNear;
    }

	return true;
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

	//50 линий сетка + еще 2 линий в конце
	//одна по горизонтали и одна по вертикали (замыкающие линии)
	for ( int i = 0; i < ( 50 + 2 ); i++ )
	{
		vector3 Vec1t = m_VertBuffTransformed[ i * 2 + 0 ];
		vector3 Vec2t = m_VertBuffTransformed[ i * 2 + 1 ];

		//выполняем отсечение по передней плоскости отсечения
		//если это необходимо, это необходимо когда
		//Z координата вершины v1 или v2 меньше нуля

		vector3 Vec1, Vec2;

		float zNear = 1.0f; // Положение ближней плоскости
		if(!ClipLineZNear(Vec1t, Vec2t, zNear, Vec1, Vec2))
			continue;

		//далее перспективное деление, вычисление аспекта
		//экранные координаты для каждой вершины Vec1 и Vec2

		Vec1.x = Vec1.x / Vec1.z;
		Vec1.y = Vec1.y / Vec1.z;
		Vec1.x = Vec1.x / ((float) m_ViewWidth / m_ViewHeight);

		Vec1.x = Vec1.x * m_ViewWidth / 2.0f + m_ViewWidth / 2.0f;
		Vec1.y =-Vec1.y * m_ViewHeight / 2.0f + m_ViewHeight / 2.0f;

		Vec2.x = Vec2.x / Vec2.z;
		Vec2.y = Vec2.y / Vec2.z;
		Vec2.x = Vec2.x / ((float) m_ViewWidth / m_ViewHeight);

		Vec2.x = Vec2.x * m_ViewWidth / 2.0f + m_ViewWidth / 2.0f;
		Vec2.y =-Vec2.y * m_ViewHeight / 2.0f + m_ViewHeight / 2.0f;

		//перед этим было отсечение по Z
		//в пространсве вида
		//теперь еще нужно сделать отсечение
		//в экранных координатах
		//мы ранее использовали LineTo
		//для рисования линий - эта функция
		//сама делает отсечение в экранных координатах
		//но мы используем для рисования линий 
		//свой алгоритм (алгоритм Брезенхема)
		//поэтому нам еще нужно сделать отсечение
		//в экранных координатах

		CohenSutherlandClip((int)Vec1.x, (int)Vec1.y, (int)Vec2.x, (int)Vec2.y, 0, m_ViewWidth - 1, 0, m_ViewHeight - 1);
	}

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
	RECT Rc;
    ::GetClientRect( m_hWnd, &Rc );

    m_ViewWidth  = Rc.right;
    m_ViewHeight = Rc.bottom;

	DWORD m_dwSize = m_ViewWidth * (BITS_PER_PIXEL >> 3) * m_ViewHeight;
	

	m_Data = (LPBYTE)malloc(m_dwSize*sizeof(BYTE));

	memset(&m_Bih, 0, sizeof(BITMAPINFOHEADER));
	m_Bih.biSize = sizeof(BITMAPINFOHEADER);
	m_Bih.biWidth = m_ViewWidth;
	m_Bih.biHeight = m_ViewHeight;
	m_Bih.biPlanes = 1;
	m_Bih.biBitCount = BITS_PER_PIXEL;
	m_Bih.biCompression = BI_RGB;
	m_Bih.biSizeImage = m_dwSize;

	m_hDD = DrawDibOpen();
}

void CMeshManager::Clear_BackBuffer()
{
	for ( UINT i = 0; i <  m_ViewHeight; i++)
	{
		for ( UINT j = 0; j < m_ViewWidth; j++ )
		{
			int Index = i * 4 * m_ViewWidth + j * 4;

			m_Data[Index + 0] = (BYTE)(255.0 * 0.3f); // blue
			m_Data[Index + 1] = (BYTE)(255.0 * 0.125f); // green
			m_Data[Index + 2] = 0; // red
			m_Data[Index + 3] = 0; 
		}
	}
}

void CMeshManager::Present_BackBuffer()
{
	//переворачиваем задний буфер
	DWORD m_dwSize = m_ViewWidth * (BITS_PER_PIXEL >> 3) * m_ViewHeight;
	
	LPBYTE m_DataTemp = (LPBYTE)malloc(m_dwSize * sizeof(BYTE));

	for (UINT h = 0; h < m_ViewHeight; h++ )
	{
		for (UINT w = 0; w < m_ViewWidth; w++)
		{
			int Index = h * 4 * m_ViewWidth + w * 4;

			BYTE b = m_Data[Index + 0]; // blue
			BYTE g = m_Data[Index + 1]; // green
			BYTE r = m_Data[Index + 2]; // red
			
			int IndexTemp = (m_ViewHeight - 1 - h) * 4 * m_ViewWidth + w * 4;
			
			m_DataTemp[IndexTemp + 0] = b;
			m_DataTemp[IndexTemp + 1] = g;
			m_DataTemp[IndexTemp + 2] = r;
			m_DataTemp[IndexTemp + 3] = 0;
		}
	}

	HDC hDC = GetDC(m_hWnd);
	DrawDibDraw(m_hDD, hDC, 0, 0, m_ViewWidth, m_ViewHeight, &m_Bih, m_DataTemp, 0, 0, m_ViewWidth, m_ViewHeight, 0);
	ReleaseDC(m_hWnd, hDC);

	free(m_DataTemp);
}
	
void CMeshManager::Delete_BackBuffer()
{
	DrawDibClose(m_hDD);

	free(m_Data);
	m_Data = NULL;
}
