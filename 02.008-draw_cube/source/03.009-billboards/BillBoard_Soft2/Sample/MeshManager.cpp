//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#include "MeshManager.h"

CMeshManager::CMeshManager()
{
}

CMeshManager::~CMeshManager()
{
	Delete_Backbuffer();

	delete [] m_pLevelTile24[0];
	m_pLevelTile24[0] = NULL;

	delete [] m_pLevelTile24;
	m_pLevelTile24 = NULL;

	delete [] m_pLevelTile32[0];
	m_pLevelTile32[0] = NULL;

	delete [] m_pLevelTile32;
	m_pLevelTile32 = NULL;

	delete [] Tex;
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

	VecOut.tu = VecIn.tu;
	VecOut.tv = VecIn.tv;

	return VecOut;
}

void CMeshManager::Matrix_Rotation_Axis(vector3 &VecIn, float Angle, matrix4x4 &MatRotate)
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

	MatRotate = matrix4x4 (xxomc + c,  xyomc + zs, xzomc - ys, 0.0f,
		xyomc - zs, yyomc + c,  yzomc + xs, 0.0f,
		xzomc + ys, yzomc - xs, zzomc + c, 0.0f,
		0.0f ,0.0f, 0.0f, 1.0f);
}

bool list::Is_Empty_List ()
{
		if(PolygonCount == 0)
			return true;

		return false;
}

void list::Add_To_List(polygon *p)
{
	if ( PolygonCount == 0)
	{	
		PolygonCount++;
		PolyList=(polygon*)malloc(PolygonCount*sizeof(polygon));
	}
	else
	{
		PolygonCount++;
		PolyList=(polygon*)realloc(PolyList, PolygonCount*sizeof(polygon));
	}

	for(int i=0;i<3;i++)
		PolyList[PolygonCount-1].Vertex[i] = p->Vertex[i];

	PolyList[PolygonCount-1].TexID = p->TexID;
}

polygon* list::Get_From_List ()
{
	polygon *plg;
	plg = &PolyList[PolygonCurr];
	PolygonCurr++;
	if(PolygonCurr > PolygonCount)
		return NULL;

	return plg;
}

void CMeshManager::Draw_Polygon_List_Floor()
{

	for (int i = 0; i < m_TransformedPolyFloor.PolygonCount; i++)
    {
		vector3 Vec1 = m_TransformedPolyFloor.PolyList[i].Vertex[0];
		vector3 Vec2 = m_TransformedPolyFloor.PolyList[i].Vertex[1];
		vector3 Vec3 = m_TransformedPolyFloor.PolyList[i].Vertex[2];
		
		UINT TexNo = m_TransformedPolyFloor.PolyList[i].TexID;
		m_Res  = (UCHAR *) m_pLevelTile24[TexNo];

		m_TextureWidth = Tex[TexNo].TextureWidth;
		m_TextureHeight = Tex[TexNo].TextureHeight;

		CurrTexId = 0;

		Draw_Textured_Triangle(Vec1, Vec2, Vec3);
	}
}

void CMeshManager::Draw_Polygon_List_Billboard()
{

	for (int loop = 0; loop < m_TransformedPolyBillboard.PolygonCount; loop++)
    {
		vector3 Vec1 = m_TransformedPolyBillboard.PolyList[loop].Vertex[0];
		vector3 Vec2 = m_TransformedPolyBillboard.PolyList[loop].Vertex[1];
		vector3 Vec3 = m_TransformedPolyBillboard.PolyList[loop].Vertex[2];
		
		UINT TexNo = m_TransformedPolyBillboard.PolyList[loop].TexID;
		m_Res  = (UCHAR *) m_pLevelTile32[TexNo];

		m_TextureWidth = Tex[TexNo + 1].TextureWidth;
		m_TextureHeight = Tex[TexNo + 1].TextureHeight;

		CurrTexId = 1;

		Draw_Textured_Triangle(Vec1, Vec2, Vec3);
	}
}

void CMeshManager::Init_MeshManager(HWND hwnd)
{
	ShowCursor(FALSE);

	m_hWnd = hwnd;

	Timer_Start();
    
	Create_Backbuffer();

	//массивы для хранения двух текстур
	//1 текстура кипричного пола
	m_pLevelTile24 = new unsigned char *[1];
	//1 текстура дерева 
	m_pLevelTile32 = new unsigned char *[1];

	//структура для хранения высоты и ширины
	//каждой текстуры
	Tex = new tex_info[2];

	//загружаем текстуру 24 бита BMP
	Load_BMP24((char*)"texture1.bmp", 0);
	//загружаем текстуру 32 бита BMP с альфа каналом
	//текстура дерева
	Load_BMP32((char*)"tree.bmp", 0);

	polygon Poly;

	//-----------
	//создаем пол для сцены

	//первый треугольник пола
	//левый нижний угол
	Poly.Vertex[0].x = -50.0f;
	Poly.Vertex[0].y = 0.0f;
	Poly.Vertex[0].z = -50.0f;
	Poly.Vertex[0].tu = 0.0f;
	Poly.Vertex[0].tv = (float)Tex[0].TextureHeight * 5 - 1;

	//левый верхний угол
	Poly.Vertex[1].x = -50.0f;
	Poly.Vertex[1].y = 0.0f;
	Poly.Vertex[1].z = 50.0f;
	Poly.Vertex[1].tu = 0.0f;
	Poly.Vertex[1].tv = 0.0f;

	//правый верхний угол
	Poly.Vertex[2].x = 50.0f;
	Poly.Vertex[2].y = 0.0f;
	Poly.Vertex[2].z = 50.0f;
	Poly.Vertex[2].tu = (float)Tex[0].TextureWidth * 5 - 1;
	Poly.Vertex[2].tv = 0.0f;
	
	Poly.TexID = 0;

	m_PolygonsFloor.Add_To_List(&Poly);

	//второй треугольник пола
	//левый нижний угол
	Poly.Vertex[0].x = -50.0f;
	Poly.Vertex[0].y = 0.0f;
	Poly.Vertex[0].z = -50.0f;
	Poly.Vertex[0].tu = 0.0f;
	Poly.Vertex[0].tv = (float)Tex[0].TextureHeight * 5 - 1;

	//правый верхний угол
	Poly.Vertex[1].x = 50.0f;
	Poly.Vertex[1].y = 0.0f;
	Poly.Vertex[1].z = 50.0f;
	Poly.Vertex[1].tu = (float)Tex[0].TextureWidth * 5 - 1;
	Poly.Vertex[1].tv = 0.0f;

	//правый нижний угол
	Poly.Vertex[2].x = 50.0f;
	Poly.Vertex[2].y = 0.0f;
	Poly.Vertex[2].z = -50.0f;
	Poly.Vertex[2].tu = (float)Tex[0].TextureWidth * 5 - 1;
	Poly.Vertex[2].tv = (float)Tex[0].TextureHeight * 5 - 1;
	
	Poly.TexID = 0;

	m_PolygonsFloor.Add_To_List(&Poly);

	//-------------
	//создаем billboard в функции Update_MeshManager()
	
	//--------------

	//инициализируем векторы камеры
	m_VecCamUp = vector3(0.0, 1.0, 0.0);
	m_VecCamLook = vector3(0.0, 0.0, 1.0);

	//камера может смотреть в направлении +Z и -Z
	if(m_VecCamLook.z > 0)
	{
		m_VecCamRight = vector3(1.0, 0.0, 0.0);
	}

	if(m_VecCamLook.z < 0)
	{
		m_VecCamRight = vector3(-1.0, 0.0, 0.0);
	}

	m_VecCamPos = vector3(0.0f, 5.0f, -60.0f);

	//создаем Z буфер - массив размером окна приложения
	//будет содержать глубину каждого пикселя на экране
	m_ZBuff = new float*[m_ViewHeight];

	for (UINT i = 0; i < m_ViewHeight; i++)
	{
		m_ZBuff[i] = new float[m_ViewWidth];
	}
}

void CMeshManager::Get_View_Matrix(matrix4x4 &MatView)
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
	Matrix_Rotation_Axis(m_VecCamRight , m_RotationScalerY, MatRotRight);
	
	m_VecCamRight = Vec3_Mat4x4_Mul(m_VecCamRight, MatRotRight);
	m_VecCamUp = Vec3_Mat4x4_Mul(m_VecCamUp, MatRotRight);
	m_VecCamLook = Vec3_Mat4x4_Mul(m_VecCamLook, MatRotRight);
	
	//горизонтальное смещение камеры (вокруг оси Y)
	if(DeltaX<0) m_RotationScalerX = m_RotationScalerX;
	else if(DeltaX>0) m_RotationScalerX = -m_RotationScalerX;
	else if(DeltaX==0) m_RotationScalerX = 0;

	vector3 VecUpTemp = vector3( 0.0f, 1.0f, 0.0f );
	matrix4x4 MatRotUp;
	Matrix_Rotation_Axis(VecUpTemp , m_RotationScalerX, MatRotUp);
	
	m_VecCamRight = Vec3_Mat4x4_Mul(m_VecCamRight, MatRotUp);
	m_VecCamUp = Vec3_Mat4x4_Mul(m_VecCamUp, MatRotUp);
	m_VecCamLook = Vec3_Mat4x4_Mul(m_VecCamLook, MatRotUp);
	
	//реакция на клавиши W,S,A,D
	float RatioMove = 50;
	vector3 VecAccel = vector3(0,0,0);
	
	if(GetAsyncKeyState('W')& 0xFF00) 
	{
		vector3 VecTemp = vector3(m_VecCamLook.x,0,m_VecCamLook.z);

		VecAccel += Vec3_Scale(VecTemp, Time * RatioMove);
	}

	if(GetAsyncKeyState('S')& 0xFF00) 
	{
		vector3 VecTemp = vector3(m_VecCamLook.x,0,m_VecCamLook.z);

		VecAccel += Vec3_Scale(VecTemp, Time * -RatioMove);
	}

	if(GetAsyncKeyState('D')& 0xFF00) 
	{
		vector3 VecTemp = vector3(m_VecCamRight.x,0,m_VecCamRight.z);

		VecAccel += Vec3_Scale(VecTemp, Time * RatioMove);
	}

	if(GetAsyncKeyState('A')& 0xFF00) 
	{
		vector3 VecTemp = vector3(m_VecCamRight.x,0,m_VecCamRight.z);
		
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

	MatView = matrix4x4(
        m_VecCamRight.x, m_VecCamUp.x, m_VecCamLook.x, 0,
		m_VecCamRight.y, m_VecCamUp.y, m_VecCamLook.y, 0,
		m_VecCamRight.z, m_VecCamUp.z, m_VecCamLook.z, 0,
		xp,		  yp,	 zp,	  1 );
}

vector3 CMeshManager::Calc_Edge(vector3 &VecIn1, vector3 &VecIn2)
{
	//вершина перед передней плоскостью v1
	//вершина за передней проскостью v2

	vector3 VecOut;

	float d = (m_ZNear - VecIn1.z) / (VecIn2.z - VecIn1.z);

    VecOut.x = VecIn1.x + d * (VecIn2.x - VecIn1.x);
    VecOut.y = VecIn1.y + d * (VecIn2.y - VecIn1.y);
    VecOut.z = VecIn1.z + d * (VecIn2.z - VecIn1.z);

	//VecOut.z = m_ZNear;

	VecOut.tu = VecIn1.tu + d * (VecIn2.tu - VecIn1.tu);
    VecOut.tv = VecIn1.tv + d * (VecIn2.tv - VecIn1.tv);

	return VecOut;
}

int CMeshManager::Clip_Vertices_Screen(int Num, vector3 *Source)
{
	float MinClipX = 0;
	float MaxClipX = (float)m_ViewWidth - 1;

	float MinClipY = 0;
	float MaxClipY = (float)m_ViewHeight - 1;

    float Scale;
    vector3 Vertices[8];

	//последняя вершина
	vector3 *l = &Source[Num - 1];
    int j = 0;

    for (int i = 0; i < Num; i++)
	{
		//сюда записываем результат
        vector3 *v1 = &Vertices[j];
        //последняя вершина v2
		vector3 *v2 = l;
        //нулевая (первая) вершина l
		l = &Source[i];

        if (v2->x < MinClipX)
		{
            if (l->x < MinClipX)
			{
                continue;
            }
            Scale = (MinClipX - l->x) / (v2->x - l->x);
            v1->x = MinClipX;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }
		else if (v2->x > MaxClipX)
		{
            if (l->x > MaxClipX)
			{
                continue;
            }
            Scale = (MaxClipX - l->x) / (v2->x - l->x);
            v1->x = MaxClipX;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }

        if (l->x < MinClipX)
		{
            Scale = (MinClipX - l->x) / (v2->x - l->x);
            v1->x = MinClipX;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }
		else if (l->x > MaxClipX)
		{
            Scale = (MaxClipX - l->x) / (v2->x - l->x);
            v1->x = MaxClipX;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }
		else
		{
            v1->x = l->x;
            v1->y = l->y;
            v1->z = l->z;
            v1->tu = l->tu;
            v1->tv = l->tv;
			v1 = &Vertices[++j];
        }
    }

    if (j < 3)
	{
        return 0;
    }

    Num = j;
    l = &Vertices[j - 1];
    j = 0;

    for (int i = 0; i < Num; i++)
	{
        vector3 *v1 = &Source[j];
        vector3 *v2 = l;
        l = &Vertices[i];

        if (v2->y < MinClipY)
		{
            if (l->y < MinClipY)
			{
                continue;
            }
            Scale = (MinClipY - l->y) / (v2->y - l->y);
            v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = MinClipY;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Source[++j];
        }
		else if (v2->y > MaxClipY)
		{
            if (l->y > MaxClipY)
			{
                continue;
            }
            Scale = (MaxClipY - l->y) / (v2->y - l->y);
            v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = MaxClipY;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Source[++j];
        }

        if (l->y < MinClipY)
		{
            Scale = (MinClipY - l->y) / (v2->y - l->y);
            v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = MinClipY;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Source[++j];
        }
		else if (l->y > MaxClipY)
		{
            Scale = (MaxClipY - l->y) / (v2->y - l->y);
            v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = MaxClipY;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
            v1 = &Source[++j];
        }
		else
		{
            v1->x = l->x;
            v1->y = l->y;
            v1->z = l->z;
            v1->tu = l->tu;
            v1->tv = l->tv;
			v1 = &Source[++j];
        }
    }
	
    if (j < 3)
	{
        return 0;
    }

	return j;

}

void CMeshManager::Update_MeshManager()
{
	matrix4x4 MatWorld = matrix4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

	matrix4x4 MatView;
    Get_View_Matrix(MatView);

	float Fov = PI / 2.0f; // FOV 90 degree
    float Aspect = (float) m_ViewWidth/m_ViewHeight;
	float ZFar = 100.0f;
	m_ZNear = 0.01f;

	float h, w, Q;

    w = (float)(1.0f/tanf(Fov*0.5f))/Aspect;  
    h = (float)1.0f/tanf(Fov*0.5f);   
    Q = ZFar/(ZFar - m_ZNear);

	/*
    matrix4x4 MatProj(
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, Q, 1,
        0, 0, -Q*m_ZNear, 0);

	*/

	//так как нам Z от 0 до 1 не надо
	//мы не будем заполнять этими значениями
	//буфер глубины, на W делить не будем,
	//то возьмем упрощенный
	//вариант матрицы проекции
    matrix4x4 MatProj(
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

	float Alpha = 0.5f * m_ViewWidth;
	float Beta  = 0.5f * m_ViewHeight;
	
	matrix4x4 MatScreen(
		Alpha,  0,	    0,    0, 
		0,      -Beta,  0,    0, 
		0,		0,		1,    0,
		Alpha,  Beta,	0,    1);
    
	//очищаем массив трансформированных вершин
	//для следующего кадра
	m_TransformedPolyFloor.Reset();
		
	for ( int j =0; j < m_PolygonsFloor.PolygonCount; j++)
	{
		vector3 Vec1 = m_PolygonsFloor.PolyList[j].Vertex[0];
		vector3 Vec2 = m_PolygonsFloor.PolyList[j].Vertex[1];
		vector3 Vec3 = m_PolygonsFloor.PolyList[j].Vertex[2];

		Vec1 = Vec3_Mat4x4_Mul(Vec1, MatWorld);
		Vec2 = Vec3_Mat4x4_Mul(Vec2, MatWorld);
		Vec3 = Vec3_Mat4x4_Mul(Vec3, MatWorld);
		
		Vec1 = Vec3_Mat4x4_Mul(Vec1, MatView);
		Vec2 = Vec3_Mat4x4_Mul(Vec2, MatView);
		Vec3 = Vec3_Mat4x4_Mul(Vec3, MatView);
		//---------------------------------
		//backface culling

		vector3 VecEdge1,VecEdge2,VecCross;

		VecEdge1 = Vec2 - Vec1;
		VecEdge2 = Vec3 - Vec1;

		VecEdge1 = Vec3_Normalize(VecEdge1);
		VecEdge2 = Vec3_Normalize(VecEdge2);

		VecCross = Vec3_Cross(VecEdge1, VecEdge2);

		VecCross = Vec3_Normalize(VecCross);

		float Dot = Vec3_Dot(VecCross, Vec1);
		
		if(Dot >= 0.0) //backface culling
			continue;

		//---------------------------------

		//начинаем отсечение по передней плоскости
		//отсечение производится в пространстве вида
		//то есть после умножения вершин на матрицу вида
		//в результате отсечения накапливаем новые
		//треугольники в FrontList
		
		list FrontList;

		//все три вершины перед плоскостью
		if( (Vec1.z > m_ZNear) && (Vec2.z > m_ZNear) && (Vec3.z > m_ZNear) )
		{
			polygon Poly;

			Poly.Vertex[0] = Vec1;
			Poly.Vertex[1] = Vec2;
			Poly.Vertex[2] = Vec3;

			Poly.TexID = m_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

		}

		//1 - Vec1 за плоскостью Vec2, Vec3 перед плоскостью
		if( (Vec1.z < m_ZNear) && (Vec2.z > m_ZNear) && (Vec3.z > m_ZNear) )
		{
			// Vec1 -> Vec2
			// Vec1 -> Vec3

			vector3 vt0 = Calc_Edge(Vec2, Vec1);
			vector3 vt1 = Calc_Edge(Vec3, Vec1);

			
			polygon Poly;

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec2;
			Poly.Vertex[2] = Vec3;

			Poly.TexID = m_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec3;
			Poly.Vertex[2] = vt1;

			Poly.TexID = m_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);
		}

		//2 - Vec2 за плоскостью, Vec1, Vec3 перед плоскостью
		if( (Vec1.z > m_ZNear) && (Vec2.z < m_ZNear) && (Vec3.z > m_ZNear ) )
		{
			// Vec2 -> Vec3
			// Vec2 -> Vec1
			vector3 vt0 = Calc_Edge(Vec3, Vec2);
			vector3 vt1 = Calc_Edge(Vec1, Vec2);

			polygon Poly;

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec3;
			Poly.Vertex[2] = Vec1;

			Poly.TexID = m_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec1;
			Poly.Vertex[2] = vt1;

			Poly.TexID = m_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);
		}

		//3 - Vec3 за плоскостью, Vec1, Vec2 перед плоскостью
		if( (Vec1.z > m_ZNear) && (Vec2.z > m_ZNear) && (Vec3.z < m_ZNear ) )
		{
			// Vec3 -> Vec1
			// Vec3 -> Vec2
			vector3 vt0 = Calc_Edge(Vec1, Vec3);
		
			vector3 vt1 = Calc_Edge(Vec2, Vec3);

			polygon Poly;

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec1;
			Poly.Vertex[2] = Vec2;

			Poly.TexID = m_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec2;
			Poly.Vertex[2] = vt1;

			Poly.TexID = m_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);
		}

		//4 - Vec1, Vec2 за плоскостью, Vec3 перед плоскостью
		if( (Vec1.z < m_ZNear) && (Vec2.z < m_ZNear) && (Vec3.z > m_ZNear ) )
		{
			// Vec2 -> Vec3
			// Vec1 -> Vec3
			vector3 vt0 = Calc_Edge(Vec3, Vec2);
		
			vector3 vt1 = Calc_Edge(Vec3, Vec1);

			polygon Poly;

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec3;
			Poly.Vertex[2] = vt1;

			Poly.TexID = m_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

		}

		//5 - Vec1, Vec3 за плоскостью, Vec2 перед плоскостью
		if( (Vec1.z < m_ZNear) && (Vec2.z > m_ZNear) && (Vec3.z < m_ZNear ) )
		{
			// Vec1 -> Vec2
			// Vec3 -> Vec2
			vector3 vt0 = Calc_Edge(Vec2, Vec1);
		
			vector3 vt1 = Calc_Edge(Vec2, Vec3);

			polygon Poly;

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec2;
			Poly.Vertex[2] = vt1;

			Poly.TexID = m_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

		}

		//6 - Vec2, Vec3 за плоскостью, Vec1 перед плоскостью
		if( (Vec1.z > m_ZNear) && (Vec2.z < m_ZNear) && (Vec3.z < m_ZNear ) )
		{
			// Vec2 -> Vec1
			// Vec3 -> Vec1
			vector3 vt0 = Calc_Edge(Vec1, Vec2);
		
			vector3 vt1 = Calc_Edge(Vec1, Vec3);

			polygon Poly;

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec1;
			Poly.Vertex[2] = vt1;

			Poly.TexID = m_PolygonsFloor.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

		}

		//отсечение в пространстве вида сделано
		//можно дальше трансформировать вершины
		//умножаем вершины на матрицу проекции
		//и далее деление на z

		for ( int i = 0; i < FrontList.PolygonCount; i++)
		{
			vector3 Vec1, Vec2, Vec3;

			Vec1 = FrontList.PolyList[i].Vertex[0];
			Vec2 = FrontList.PolyList[i].Vertex[1];
			Vec3 = FrontList.PolyList[i].Vertex[2];
			
			Vec1 = Vec3_Mat4x4_Mul(Vec1, MatProj);
			Vec1.x = Vec1.x / Vec1.z;
			Vec1.y = Vec1.y / Vec1.z;

			Vec2 = Vec3_Mat4x4_Mul(Vec2, MatProj);
			Vec2.x = Vec2.x / Vec2.z;
			Vec2.y = Vec2.y / Vec2.z;

			Vec3 = Vec3_Mat4x4_Mul(Vec3, MatProj);
			Vec3.x = Vec3.x / Vec3.z;
			Vec3.y = Vec3.y / Vec3.z;

			Vec1 = Vec3_Mat4x4_Mul(Vec1, MatScreen);
			Vec2 = Vec3_Mat4x4_Mul(Vec2, MatScreen);
			Vec3 = Vec3_Mat4x4_Mul(Vec3, MatScreen);

			//отбрасываем треугольники которые вне экрана
			if(Vec1.y < 0 && Vec2.y < 0 && Vec3.y < 0)
				continue;

			//отбрасываем треугольники которые вне экрана
			if(Vec1.y > m_ViewHeight && Vec2.y > m_ViewHeight && Vec3.y > m_ViewHeight)
				continue;

			//отбрасываем треугольники которые вне экрана
			if(Vec1.x < 0 && Vec2.x < 0 && Vec3.x < 0)
				continue;

			//отбрасываем треугольники которые вне экрана
			if(Vec1.x > m_ViewWidth && Vec2.x > m_ViewWidth && Vec3.x > m_ViewWidth)
				continue;

			//если хоть одна вершина выходит за границы экрана делаем
			//клиппинг в экранных координатах
			if(Vec1.x < 0 || Vec1.y < 0 || Vec1.x > m_ViewWidth || Vec1.y > m_ViewHeight ||
				Vec2.x < 0 || Vec2.y < 0 || Vec2.x > m_ViewWidth || Vec2.y > m_ViewHeight ||
				Vec3.x < 0 || Vec3.y < 0 || Vec3.x > m_ViewWidth || Vec3.y > m_ViewHeight )
			{

				vector3 Vertex[8];

				memset(Vertex, 0, sizeof(vector3) * 8);

				Vertex[0].x = Vec1.x;
				Vertex[0].y = Vec1.y;
				Vertex[0].z = 1.0f / Vec1.z;
				Vertex[0].tu = Vec1.tu * Vertex[0].z;
				Vertex[0].tv = Vec1.tv * Vertex[0].z;

				Vertex[1].x = Vec2.x;
				Vertex[1].y = Vec2.y;
				Vertex[1].z = 1.0f / Vec2.z;
				Vertex[1].tu = Vec2.tu * Vertex[1].z;
				Vertex[1].tv = Vec2.tv * Vertex[1].z;

				Vertex[2].x = Vec3.x;
				Vertex[2].y = Vec3.y;
				Vertex[2].z = 1.0f / Vec3.z;
				Vertex[2].tu = Vec3.tu * Vertex[2].z;
				Vertex[2].tv = Vec3.tv * Vertex[2].z;

				int Verts = Clip_Vertices_Screen(3, &Vertex[0]);

				if(Verts == 3)
				{
					polygon pt1;
					
					pt1.Vertex[0] = Vertex[0];
					pt1.Vertex[1] = Vertex[1];
					pt1.Vertex[2] = Vertex[2];

					pt1.TexID = FrontList.PolyList[i].TexID;
					
					m_TransformedPolyFloor.Add_To_List(&pt1);
				}
				else if(Verts == 4)
				{
					polygon pt1;
					
					pt1.Vertex[0] = Vertex[0];
					pt1.Vertex[1] = Vertex[1];
					pt1.Vertex[2] = Vertex[2];

					pt1.TexID = FrontList.PolyList[i].TexID;

					m_TransformedPolyFloor.Add_To_List(&pt1);

					polygon pt2;

					pt2.Vertex[0] = Vertex[0];
					pt2.Vertex[1] = Vertex[2];
					pt2.Vertex[2] = Vertex[3];

					pt2.TexID = FrontList.PolyList[i].TexID;
				
					m_TransformedPolyFloor.Add_To_List(&pt2);
				}
				
				else if(Verts == 5)
				{
					polygon pt1;
				
					pt1.Vertex[0] = Vertex[0];
					pt1.Vertex[1] = Vertex[1];
					pt1.Vertex[2] = Vertex[2];

					pt1.TexID = FrontList.PolyList[i].TexID;

					m_TransformedPolyFloor.Add_To_List(&pt1);

					polygon pt2;

					pt2.Vertex[0] = Vertex[0];
					pt2.Vertex[1] = Vertex[2];
					pt2.Vertex[2] = Vertex[3];

					pt2.TexID = FrontList.PolyList[i].TexID;
					
					m_TransformedPolyFloor.Add_To_List(&pt2);

					polygon pt3;

					pt3.Vertex[0] = Vertex[0];
					pt3.Vertex[1] = Vertex[3];
					pt3.Vertex[2] = Vertex[4];

					pt3.TexID = FrontList.PolyList[i].TexID;
					
					m_TransformedPolyFloor.Add_To_List(&pt3);
				}
				else if(Verts == 6)
				{
					polygon pt1;
				
					pt1.Vertex[0] = Vertex[0];
					pt1.Vertex[1] = Vertex[1];
					pt1.Vertex[2] = Vertex[2];

					pt1.TexID = FrontList.PolyList[i].TexID;

					m_TransformedPolyFloor.Add_To_List(&pt1);

					polygon pt2;

					pt2.Vertex[0] = Vertex[0];
					pt2.Vertex[1] = Vertex[2];
					pt2.Vertex[2] = Vertex[3];

					pt2.TexID = FrontList.PolyList[i].TexID;
					
					m_TransformedPolyFloor.Add_To_List(&pt2);

					polygon pt3;

					pt3.Vertex[0] = Vertex[0];
					pt3.Vertex[1] = Vertex[3];
					pt3.Vertex[2] = Vertex[4];

					pt3.TexID = FrontList.PolyList[i].TexID;
					
					m_TransformedPolyFloor.Add_To_List(&pt3);

					polygon pt4;

					pt4.Vertex[0] = Vertex[0];
					pt4.Vertex[1] = Vertex[4];
					pt4.Vertex[2] = Vertex[5];

					pt4.TexID = FrontList.PolyList[i].TexID;
					
					m_TransformedPolyFloor.Add_To_List(&pt4);
				}
			}
			else
			{
				polygon pt;
				
				pt.Vertex[0].x = Vec1.x;
				pt.Vertex[0].y = Vec1.y;
				pt.Vertex[0].z = 1.0f / Vec1.z;
				pt.Vertex[0].tu = Vec1.tu * pt.Vertex[0].z;
				pt.Vertex[0].tv = Vec1.tv * pt.Vertex[0].z;

				pt.Vertex[1].x = Vec2.x;
				pt.Vertex[1].y = Vec2.y;
				pt.Vertex[1].z = 1.0f / Vec2.z;
				pt.Vertex[1].tu = Vec2.tu * pt.Vertex[1].z;
				pt.Vertex[1].tv = Vec2.tv * pt.Vertex[1].z;

				pt.Vertex[2].x = Vec3.x;
				pt.Vertex[2].y = Vec3.y;
				pt.Vertex[2].z = 1.0f / Vec3.z;
				pt.Vertex[2].tu = Vec3.tu * pt.Vertex[2].z;
				pt.Vertex[2].tv = Vec3.tv * pt.Vertex[2].z;

				pt.TexID = FrontList.PolyList[i].TexID;
				
				m_TransformedPolyFloor.Add_To_List(&pt);

			} //end for clipping
		}

		FrontList.Reset();
	}

	//-----------
	//billboard

	//очищаем массив трансформированных вершин
	//для следующего кадра
	
	m_TransformedPolyBillboard.Reset();

	//у нас 1 билбоард (цикл)
	for ( int i = 0; i < 1; i++ )
	{

		//билбоард расположен в центре поля (нулевые координаты)
		//размер билбоарда 50 на 50 пикслей (высота, ширина)
		vector3 Vec = vector3(0.0f, 0.0f, 0.0f);

		Vec = Vec3_Mat4x4_Mul(Vec, MatView);

		vector3 Vec1;
		vector3 Vec2;
		vector3 Vec3;
		vector3 Vec4;

		Vec1.x = Vec.x - 25.0f;
		Vec1.y = Vec.y;
		Vec1.z = Vec.z;
		Vec1.tu = 0.0f;
		Vec1.tv = (float)Tex[1].TextureHeight - 1;;

		Vec2.x = Vec.x - 25.0f;
		Vec2.y = Vec.y + 50.0f;
		Vec2.z = Vec.z;
		Vec2.tu = 0.0f;
		Vec2.tv = 0.0f;

		Vec3.x = Vec.x + 25.0f;
		Vec3.y = Vec.y + 50.0f;
		Vec3.z = Vec.z;
		Vec3.tu = (float)Tex[1].TextureWidth - 1;
		Vec3.tv = 0.0f;

		Vec4.x = Vec.x + 25.0f;
		Vec4.y = Vec.y;
		Vec4.z = Vec.z;
		Vec4.tu = (float)Tex[1].TextureWidth - 1;;
		Vec4.tv = (float)Tex[1].TextureHeight - 1;;

		if( Vec.z < 0 )
			continue;

		Vec1 = Vec3_Mat4x4_Mul(Vec1, MatProj);
		Vec2 = Vec3_Mat4x4_Mul(Vec2, MatProj);
		Vec3 = Vec3_Mat4x4_Mul(Vec3, MatProj);
		Vec4 = Vec3_Mat4x4_Mul(Vec4, MatProj);
		
		//у всех вершин билбоарда одинаковая глубина 
		//за счет этого билбоард всегда повернут
		//лицевой стороной к наблюдателю

		Vec1.x = Vec1.x / Vec.z;
		Vec1.y = Vec1.y / Vec.z;
	
		Vec2.x = Vec2.x / Vec.z;
		Vec2.y = Vec2.y / Vec.z;

		Vec3.x = Vec3.x / Vec.z;
		Vec3.y = Vec3.y / Vec.z;

		Vec4.x = Vec4.x / Vec.z;
		Vec4.y = Vec4.y / Vec.z;

		Vec1 = Vec3_Mat4x4_Mul(Vec1, MatScreen);
		Vec2 = Vec3_Mat4x4_Mul(Vec2, MatScreen);
		Vec3 = Vec3_Mat4x4_Mul(Vec3, MatScreen);
		Vec4 = Vec3_Mat4x4_Mul(Vec4, MatScreen);
			
		//отбрасываем треугольники которые вне экрана
		if(Vec1.y < 0 && Vec2.y < 0 && Vec3.y < 0 && Vec4.y < 0)
			continue;

		//отбрасываем треугольники которые вне экрана
		if(Vec1.y > m_ViewHeight && Vec2.y > m_ViewHeight && Vec3.y > m_ViewHeight && Vec4.y > m_ViewHeight)
			continue;

		//отбрасываем треугольники которые вне экрана
		if(Vec1.x < 0 && Vec2.x < 0 && Vec3.x < 0 && Vec4.x < 0)
			continue;

		//отбрасываем треугольники которые вне экрана
		if(Vec1.x > m_ViewWidth && Vec2.x > m_ViewWidth && Vec3.x > m_ViewWidth && Vec4.x > m_ViewWidth)
			continue;

		//если хоть одна вершина выходит за границы экрана делаем
		//клиппинг в экранных координатах

		vector3 Vertex[8];

		memset(Vertex, 0, sizeof(vector3) * 8);

		Vertex[0].x = Vec1.x;
		Vertex[0].y = Vec1.y;
		Vertex[0].z = 1.0f / Vec.z;
		Vertex[0].tu = Vec1.tu * Vertex[0].z;
		Vertex[0].tv = Vec1.tv * Vertex[0].z;

		Vertex[1].x = Vec2.x;
		Vertex[1].y = Vec2.y;
		Vertex[1].z = 1.0f / Vec.z;
		Vertex[1].tu = Vec2.tu * Vertex[1].z;
		Vertex[1].tv = Vec2.tv * Vertex[1].z;

		Vertex[2].x = Vec3.x;
		Vertex[2].y = Vec3.y;
		Vertex[2].z = 1.0f / Vec.z;
		Vertex[2].tu = Vec3.tu * Vertex[2].z;
		Vertex[2].tv = Vec3.tv * Vertex[2].z;

		Vertex[3].x = Vec4.x;
		Vertex[3].y = Vec4.y;
		Vertex[3].z = 1.0f / Vec.z;
		Vertex[3].tu = Vec4.tu * Vertex[3].z;
		Vertex[3].tv = Vec4.tv * Vertex[3].z;

		int Verts = Clip_Vertices_Screen(4, &Vertex[0]);

		if(Verts == 3)
		{
			polygon pt1;
					
			pt1.Vertex[0] = Vertex[0];
			pt1.Vertex[1] = Vertex[1];
			pt1.Vertex[2] = Vertex[2];

			pt1.TexID = 0;
					
			m_TransformedPolyBillboard.Add_To_List(&pt1);
		}
		else if(Verts == 4)
		{
			polygon pt1;
				
			pt1.Vertex[0] = Vertex[0];
			pt1.Vertex[1] = Vertex[1];
			pt1.Vertex[2] = Vertex[2];

			pt1.TexID = 0;

			m_TransformedPolyBillboard.Add_To_List(&pt1);

			polygon pt2;

			pt2.Vertex[0] = Vertex[0];
			pt2.Vertex[1] = Vertex[2];
			pt2.Vertex[2] = Vertex[3];

			pt2.TexID = 0;
				
			m_TransformedPolyBillboard.Add_To_List(&pt2);
		}
		else if(Verts == 5)
		{
			polygon pt1;
				
			pt1.Vertex[0] = Vertex[0];
			pt1.Vertex[1] = Vertex[1];
			pt1.Vertex[2] = Vertex[2];

			pt1.TexID = 0;

			m_TransformedPolyBillboard.Add_To_List(&pt1);

			polygon pt2;

			pt2.Vertex[0] = Vertex[0];
			pt2.Vertex[1] = Vertex[2];
			pt2.Vertex[2] = Vertex[3];

			pt2.TexID = 0;
			
			m_TransformedPolyBillboard.Add_To_List(&pt2);

			polygon pt3;

			pt3.Vertex[0] = Vertex[0];
			pt3.Vertex[1] = Vertex[3];
			pt3.Vertex[2] = Vertex[4];

			pt3.TexID = 0;
				
			m_TransformedPolyBillboard.Add_To_List(&pt3);
		}
		else if(Verts == 6)
		{
			polygon pt1;
				
			pt1.Vertex[0] = Vertex[0];
			pt1.Vertex[1] = Vertex[1];
			pt1.Vertex[2] = Vertex[2];

			pt1.TexID = 0;

			m_TransformedPolyBillboard.Add_To_List(&pt1);

			polygon pt2;

			pt2.Vertex[0] = Vertex[0];
			pt2.Vertex[1] = Vertex[2];
			pt2.Vertex[2] = Vertex[3];

			pt2.TexID = 0;
					
			m_TransformedPolyBillboard.Add_To_List(&pt2);

			polygon pt3;

			pt3.Vertex[0] = Vertex[0];
			pt3.Vertex[1] = Vertex[3];
			pt3.Vertex[2] = Vertex[4];

			pt3.TexID = 0;
					
			m_TransformedPolyBillboard.Add_To_List(&pt3);

			polygon pt4;

			pt4.Vertex[0] = Vertex[0];
			pt4.Vertex[1] = Vertex[4];
			pt4.Vertex[2] = Vertex[5];

			pt4.TexID = 0;
				
			m_TransformedPolyBillboard.Add_To_List(&pt4);
		}
	
	}
	
	//очищаем Z-Buffer
	for (UINT i = 0; i < m_ViewHeight; i++)
	{
		for(UINT j = 0; j < m_ViewWidth; j++)
		{
			//дальняя плоскость отсечения 25000.0f
			//это максимальная глубина Z
			//мы этой глубиной очищаем Z-Buffer
			m_ZBuff[i][j] = 25000.0f;
		}
	}
}

void CMeshManager::Draw_MeshManager()
{
	Clear_Backbuffer();

	//рисуем пол
	Draw_Polygon_List_Floor();
	//рисуем дерево
	//для альфа смешивания важно последовательность
	//отрисовки на сцене
	//меши которые будут смешиваться должны
	//рисоваться последними, что бы был уже
	//готовый фон для смешивания
	Draw_Polygon_List_Billboard();

	Present_Backbuffer();
}

void CMeshManager::Draw_Textured_Triangle(vector3 VecIn1, vector3 VecIn2, vector3 VecIn3)
{

	int Side;
	float x1, x2, x3;
	float y1, y2, y3;
	float iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;
	float FloatTemp;
	int IntTemp;

	x1 = VecIn1.x;
	y1 = VecIn1.y;
	x2 = VecIn2.x;
	y2 = VecIn2.y;
	x3 = VecIn3.x;
	y3 = VecIn3.y;
	
	iz1 = VecIn1.z;
	uiz1 = VecIn1.tu;
	viz1 = VecIn1.tv;

	iz2 = VecIn2.z;
	uiz2 = VecIn2.tu;
	viz2 = VecIn2.tv;
	
	iz3 = VecIn3.z;
	uiz3 = VecIn3.tu;
	viz3 = VecIn3.tv;

	//координаты на экране у нас целые числа X и Y
	//поэтому некоторые операции необходимо
	//делать целочисленные (float труднее для
	//сравнения, например)

	int y1i = (int) floor (y1);
	int y2i = (int) floor (y2);
	int y3i = (int) floor (y3);

	if (y1i > y2i)
	{
		SWAP(x1, x2, FloatTemp);
		SWAP(y1, y2, FloatTemp);
		SWAP(iz1, iz2, FloatTemp);
		SWAP(uiz1, uiz2, FloatTemp);
		SWAP(viz1, viz2, FloatTemp);
		SWAP(y1i, y2i, IntTemp);
	}
	if (y1i > y3i)
	{
		SWAP(x1, x3, FloatTemp);
		SWAP(y1, y3, FloatTemp);
		SWAP(iz1, iz3, FloatTemp);
		SWAP(uiz1, uiz3, FloatTemp);
		SWAP(viz1, viz3, FloatTemp);
		SWAP(y1i, y3i, IntTemp);
	}
	if (y2i > y3i)
	{
		SWAP(x2, x3, FloatTemp);
		SWAP(y2, y3, FloatTemp);
		SWAP(iz2, iz3, FloatTemp);
		SWAP(uiz2, uiz3, FloatTemp);
		SWAP(viz2, viz3, FloatTemp);
		SWAP(y2i, y3i, IntTemp);
	}
	
	if(y2i > y1i && y3i > y2i)
	{
		//вычисляем наклон левой и правой стороны
		//треугольника, по знаку величины наклона
		//(+ или -) определяем какая сторона
		//(левая или правая) имеет большее значение по Y
		float dxdy1 = (x2 - x1) / (y2 - y1);
		float dxdy2 = (x3 - x1) / (y3 - y1);
		Side= dxdy2 > dxdy1;
	}

	if (y1i == y2i)
		Side = x1 > x2;
	if (y2i == y3i)
		Side = x3 > x2;

	if (!Side)
	{
		m_xl = x1;
		m_ul = uiz1;
		m_vl = viz1;
		m_zl = iz1;

		m_dxdyl = (x3 - x1) / (y3 - y1);
		m_dudyl = (uiz3 - uiz1) / (y3 - y1);
		m_dvdyl = (viz3 - viz1) / (y3 - y1);
		m_dzdyl = (iz3 - iz1) / (y3 - y1);
	
		if (y1i < y2i)
		{
			m_xr = x1;
			m_ur = uiz1;
			m_vr = viz1;
			m_zr = iz1;

			m_dxdyr = (x2 - x1) / (y2 - y1);
			m_dudyr = (uiz2 - uiz1) / (y2 - y1);
			m_dvdyr = (viz2 - viz1) / (y2 - y1);
			m_dzdyr = (iz2 - iz1) / (y2 - y1);
			
			Draw_Textured_Poly(y1i, y2i);
		}
		if (y2i < y3i)
		{
			m_xr = x2;
			m_ur = uiz2;
			m_vr = viz2;
			m_zr = iz2;

			m_dxdyr = (x3 - x2) / (y3 - y2);
			m_dudyr = (uiz3 - uiz2) / (y3 - y2);
			m_dvdyr = (viz3 - viz2) / (y3 - y2);
			m_dzdyr = (iz3 - iz2) / (y3 - y2);
			
			Draw_Textured_Poly(y2i, y3i);
		}
	}
	else
	{
		m_xr = x1;
		m_ur = uiz1;
		m_vr = viz1;
		m_zr = iz1;
		
		m_dxdyr = (x3 - x1) / (y3 - y1);
		m_dudyr = (uiz3 - uiz1) / (y3 - y1);
		m_dvdyr = (viz3 - viz1) / (y3 - y1);
		m_dzdyr = (iz3 - iz1) / (y3 - y1);
		
		if (y1 < y2)
		{
			m_xl = x1;
			m_ul = uiz1;
			m_vl = viz1;
			m_zl = iz1;

			m_dxdyl = (x2 - x1) / (y2 - y1);
			m_dudyl = (uiz2 - uiz1) / (y2 - y1);
			m_dvdyl = (viz2 - viz1) / (y2 - y1);
			m_dzdyl = (iz2 - iz1) / (y2 - y1);
			
			Draw_Textured_Poly(y1i, y2i);
		}
		if (y2 < y3)
		{
			m_xl = x2;
			m_ul = uiz2;
			m_vl = viz2;
			m_zl = iz2;

			m_dxdyl = (x3 - x2) / (y3 - y2);
			m_dudyl = (uiz3 - uiz2) / (y3 - y2);
			m_dvdyl = (viz3 - viz2) / (y3 - y2);
			m_dzdyl = (iz3 - iz2) / (y3 - y2);
						
			Draw_Textured_Poly(y2i, y3i);
		}
	}
}

void CMeshManager::Draw_Textured_Poly(int y1, int y2)
{
	float ui, vi, zi;
	float du, dv, dz;
	
	for (int y = y1; y<y2; y++)
	{
		if ((m_xr - m_xl)>0)
		{
			du = (m_ur - m_ul)/(m_xr - m_xl);
			dv = (m_vr - m_vl)/(m_xr - m_xl);
			dz = (m_zr - m_zl)/(m_xr - m_xl);
		}
		else
		{
			du = 0;
			dv = 0;
			dz = 0;
		}

		/*
		int xln = (int)m_xl;

		float dxt = 1 - (m_xl - xln);

		zi = m_zl + dxt * dz;
		ui = m_ul + dxt * du;
		vi = m_vl + dxt * dv;
		*/
		
		zi = m_zl;
		ui = m_ul;
		vi = m_vl;

		for (int x=(int)m_xl; x<(int)m_xr; x++)
		{

			float fZVal = m_ZBuff[y][x];

			float z = 1.0f/zi;
			float uf = ui * z;
			float vf = vi * z;

			UINT u = (UINT)uf;
			UINT v = (UINT)vf;

			u = u % m_TextureWidth;
			v = v % m_TextureHeight;


			//если глубина fZVal меньше
			//чем глубина пикселя Z
			//не рисуем пиксель пропускаем его
			if( fZVal < z )
			{	
				ui+=du;
				vi+=dv;
				zi+=dz;
						
				continue;
			}

			m_ZBuff[y][x] = z;

			UINT t = (int)u  + (((int)v) * m_TextureWidth);

			if( t < 0 || t > (m_TextureWidth * m_TextureHeight - 1) )
				continue;

			/*
			t= t*3;

			int Index = y * m_BackLpitch + x * 4;
			
			m_Data[Index + 0] = m_Res[t + 0];
			m_Data[Index + 1] = m_Res[t + 1];
			m_Data[Index + 2] = m_Res[t + 2];
			m_Data[Index + 3] = 0;
			*/
			int Index = y * m_BackLpitch + x * 4;

			//текущаая текстура дерева
			if(CurrTexId == 1)
			{
				t= t*4;

				//выполняем альфа смешивание
				//для текстуры дерева
				//смешиваем пиксель текстуры дерева
				//с пикселем фона (того что уже есть на сцене)

				//получаем пиксель из текстуры дерева
				//и преобразуем в диапазон от 0 до 1
				float r = m_Res[t + 2] / 255.0f;
				float g = m_Res[t + 1] / 255.0f;
				float b = m_Res[t + 0] / 255.0f;
				float a = m_Res[t + 3] / 255.0f;

				//умножаем пиксель текстуры дерева
				//на его альфа канал
				float SrcR = r * a;
				float SrcG = g * a;
				float SrcB = b * a;

				//получаем пиксель бак- буфера (фона)
				//и преобразуем в диапазон от 0 до 1
				float rb = m_Data[Index + 2] / 255.0f;
				float gb = m_Data[Index + 1] / 255.0f;
				float bb = m_Data[Index + 0] / 255.0f;

				//умножаем пиксель из бак- буфера
				//на 1 - альфа канал текстуры дерева
				float DestR = rb * (1.0f - a);
				float DestG = gb * (1.0f - a);
				float DestB = bb * (1.0f - a);

				//складываем значения и умрожаем на 255
				float ResR = (SrcR + DestR) * 255;
				float ResG = (SrcG + DestG) * 255;
				float ResB = (SrcB + DestB) * 255;
			
				//заполняем бак- буфер
				
				m_Data[Index + 0] = (int)ResB;
				m_Data[Index + 1] = (int)ResG;
				m_Data[Index + 2] = (int)ResR;
				m_Data[Index + 3] = 0;
			}
			//текущая текстура пола
			else
			{
				//умножаем индекс на 3 так как 
				//изображение в массиве содержит три
				//три цветовых компоненты r,g,b
				t= t*3;
			
				//просто берем значения пикселей из текстуры
				m_Data[Index + 0] = m_Res[t + 0];
				m_Data[Index + 1] = m_Res[t + 1];
				m_Data[Index + 2] = m_Res[t + 2];
				m_Data[Index + 3] = 0;
			}

				
			ui+=du;
			vi+=dv;
			zi+=dz;
		}

		m_xl += m_dxdyl;
		m_ul += m_dudyl;
		m_vl += m_dvdyl;
		m_zl += m_dzdyl;

		m_xr += m_dxdyr;
		m_ur += m_dudyr;
		m_vr += m_dvdyr;
		m_zr += m_dzdyr;
	}
}

bool CMeshManager::Load_BMP24(char *szFilename, int Tile)
{
	FILE *fp;

	fopen_s(&fp, szFilename,"rb");
	if(fp==NULL) printf("Error Open File");

	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(bfh), 1, fp);

	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(bih), 1, fp);

	m_pLevelTile24[Tile] = new unsigned char [bih.biWidth*bih.biHeight*3];

	fread(m_pLevelTile24[Tile],bih.biWidth*bih.biHeight*3,1,fp);

	//первый элемент массива Tex хранит размер
	//текстуры пола (кирпичная текстура)
	Tex[0].TextureWidth = bih.biWidth;
	Tex[0].TextureHeight = bih.biHeight;

	return true;
}

bool CMeshManager::Load_BMP32(char *szFilename, int Tile)
{
	FILE *fp;

	fopen_s(&fp, szFilename,"rb");
	if(fp==NULL) printf("Error Open File");

	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(bfh), 1, fp);

	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(bih), 1, fp);

	m_pLevelTile32[Tile] = new unsigned char [bih.biWidth*bih.biHeight*4];

	fread(m_pLevelTile32[Tile],bih.biWidth*bih.biHeight*4,1,fp);

	//второй элемент массива Tex хранит размер
	//текстуры дерева (билбоард)
	Tex[1].TextureWidth = bih.biWidth;
	Tex[1].TextureHeight = bih.biHeight;

	return true;

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

void CMeshManager::Mat4x4_Inverse(matrix4x4 &MatIn, matrix4x4 &MatOut)
{
	//computes the inverse of a 4x4, assumes that the last
	//column is [0 0 0 1]t

	float det =  ( MatIn.Mat[0][0] * ( MatIn.Mat[1][1] * MatIn.Mat[2][2] - MatIn.Mat[1][2] * MatIn.Mat[2][1] ) -
		           MatIn.Mat[0][1] * ( MatIn.Mat[1][0] * MatIn.Mat[2][2] - MatIn.Mat[1][2] * MatIn.Mat[2][0] ) +
			       MatIn.Mat[0][2] * ( MatIn.Mat[1][0] * MatIn.Mat[2][1] - MatIn.Mat[1][1] * MatIn.Mat[2][0] ) );

	// test determinate to see if it's 0
	if (fabs(det) < EPSILON_E5)
		return;

	float det_inv  = 1.0f / det;

	MatOut.Mat[0][0] =  det_inv * ( MatIn.Mat[1][1] * MatIn.Mat[2][2] - MatIn.Mat[1][2] * MatIn.Mat[2][1] );
	MatOut.Mat[0][1] = -det_inv * ( MatIn.Mat[0][1] * MatIn.Mat[2][2] - MatIn.Mat[0][2] * MatIn.Mat[2][1] );
	MatOut.Mat[0][2] =  det_inv * ( MatIn.Mat[0][1] * MatIn.Mat[1][2] - MatIn.Mat[0][2] * MatIn.Mat[1][1] );
	MatOut.Mat[0][3] = 0.0f; // always 0

	MatOut.Mat[1][0] = -det_inv * ( MatIn.Mat[1][0] * MatIn.Mat[2][2] - MatIn.Mat[1][2] * MatIn.Mat[2][0] );
	MatOut.Mat[1][1] =  det_inv * ( MatIn.Mat[0][0] * MatIn.Mat[2][2] - MatIn.Mat[0][2] * MatIn.Mat[2][0] );
	MatOut.Mat[1][2] = -det_inv * ( MatIn.Mat[0][0] * MatIn.Mat[1][2] - MatIn.Mat[0][2] * MatIn.Mat[1][0] );
	MatOut.Mat[1][3] = 0.0f; // always 0

	MatOut.Mat[2][0] =  det_inv * ( MatIn.Mat[1][0] * MatIn.Mat[2][1] - MatIn.Mat[1][1] * MatIn.Mat[2][0] );
	MatOut.Mat[2][1] = -det_inv * ( MatIn.Mat[0][0] * MatIn.Mat[2][1] - MatIn.Mat[0][1] * MatIn.Mat[2][0] );
	MatOut.Mat[2][2] =  det_inv * ( MatIn.Mat[0][0] * MatIn.Mat[1][1] - MatIn.Mat[0][1] * MatIn.Mat[1][0] );
	MatOut.Mat[2][3] = 0.0f; // always 0

	MatOut.Mat[3][0] = -( MatIn.Mat[3][0] * MatOut.Mat[0][0] + MatIn.Mat[3][1] * MatOut.Mat[1][0] + MatIn.Mat[3][2] * MatOut.Mat[2][0] );
	MatOut.Mat[3][1] = -( MatIn.Mat[3][0] * MatOut.Mat[0][1] + MatIn.Mat[3][1] * MatOut.Mat[1][1] + MatIn.Mat[3][2] * MatOut.Mat[2][1] );
	MatOut.Mat[3][2] = -( MatIn.Mat[3][0] * MatOut.Mat[0][2] + MatIn.Mat[3][1] * MatOut.Mat[1][2] + MatIn.Mat[3][2] * MatOut.Mat[2][2] );
	MatOut.Mat[3][3] = 1.0f; // always 0
}

void CMeshManager::Create_Backbuffer()
{
	RECT Rc;
    ::GetClientRect( m_hWnd, &Rc );

	m_BackLpitch = Rc.right * 4;
    
	m_ViewWidth  = Rc.right;
    m_ViewHeight = Rc.bottom;

	DWORD m_dwSize = m_ViewWidth * (BITS_PER_PIXEL >> 3) * m_ViewHeight;

	m_Data = (LPBYTE)malloc(m_dwSize * sizeof(BYTE));
	m_DataTemp = (LPBYTE)malloc(m_dwSize * sizeof(BYTE));

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

void CMeshManager::Clear_Backbuffer()
{
	for (UINT h = 0; h < m_ViewHeight; h++)
	{
		for (UINT w = 0; w < m_ViewWidth; w++)
		{
			int Index = h * 4 * m_ViewWidth + w * 4;

			m_Data[Index + 0] = (BYTE)(255.0 * 0.3f); // blue
			m_Data[Index + 1] = (BYTE)(255.0 * 0.125f); // green
			m_Data[Index + 2] = 0; // red
			m_Data[Index + 3] = 0;
		}
	}
}

void CMeshManager::Present_Backbuffer()
{
	//переворачиваем задний буфер
	//так как DrawDibDraw рисует перевернутое изображение
	
	//DWORD m_dwSize = m_ViewWidth * (BITS_PER_PIXEL >> 3) * m_ViewHeight;
	//LPBYTE m_DataTemp = (LPBYTE)malloc(m_dwSize * sizeof(BYTE));

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

	//выводим задний буфер на экран
	HDC hDC = GetDC(m_hWnd);
	DrawDibDraw(m_hDD, hDC, 0, 0, m_ViewWidth, m_ViewHeight, &m_Bih, m_DataTemp, 0, 0, m_ViewWidth, m_ViewHeight, 0);
	ReleaseDC(m_hWnd, hDC);
}

void CMeshManager::Delete_Backbuffer()
{
	DrawDibClose(m_hDD);

	free(m_Data);
	free(m_DataTemp);

	m_Data = NULL;
	m_DataTemp = NULL;
}
