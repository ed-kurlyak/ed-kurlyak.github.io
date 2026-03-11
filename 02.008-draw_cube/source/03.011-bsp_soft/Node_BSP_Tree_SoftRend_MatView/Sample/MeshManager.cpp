//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#include "MeshManager.h"

//**********************************
//Конструктор класса CMeshManager
//**********************************
CMeshManager::CMeshManager()
{
}

//**********************************
//Деструктор класса CMeshManager
//**********************************
CMeshManager::~CMeshManager()
{
	Delete_Backbuffer();

	if (m_pLevelTile[0] != NULL)
	{
		delete [] m_pLevelTile[0];
		m_pLevelTile[0] = NULL;
	}

	if (m_pLevelTile[1] != NULL)
	{
		delete[] m_pLevelTile[1];
		m_pLevelTile[1] = NULL;
	}

	if (m_pLevelTile[2] != NULL)
	{
		delete[] m_pLevelTile[2];
		m_pLevelTile[2] = NULL;
	}

	if(m_pLevelTile != NULL)
	{
		delete [] m_pLevelTile;
		m_pLevelTile = NULL;
	}
	
	Delete_BSP(m_Root);

	if(m_Root != NULL)
	{
		delete m_Root;
		m_Root = NULL;
	}
}

//*************************************************
//Функция нормализует вектор - приводит вектор к
//единичной длинне
//*************************************************
vector3 CMeshManager::Vec4_Normalize(vector3 &VecIn)
{
	vector3 VecOut;

	float Len = sqrtf(VecIn.x * VecIn.x + VecIn.y * VecIn.y + VecIn.z * VecIn.z);

	VecOut.x = VecIn.x / Len;
	VecOut.y = VecIn.y / Len;
	VecOut.z = VecIn.z / Len;
	
	return VecOut;

}

//**************************************************************
//Функция вычисляет cross product двух векторов
//**************************************************************
vector3 CMeshManager::Vec4_Cross(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut;

	VecOut.x = VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y;
	VecOut.y = VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z;
	VecOut.z = VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x;

	return VecOut;
}

//**************************************************************
//Функция вычисляет скалярное произведение двух векторов
//**************************************************************
float CMeshManager::Vec4_Dot(vector3 &VecIn1, vector3 &VecIn2)
{
	return VecIn1.x*VecIn2.x + VecIn1.y*VecIn2.y + VecIn1.z* VecIn2.z;
}

//**************************************************************
//Функция вычисляет сумму двух векторов
//**************************************************************
vector3 CMeshManager::Vec4_Add(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 VecOut;

	VecOut.x = VecIn1.x + VecIn2.x;
	VecOut.y = VecIn1.y + VecIn2.y;
	VecOut.z = VecIn1.z + VecIn2.z;

	return VecOut;
}

//**************************************************************
//Функция масштабирует вектор
//**************************************************************
vector3 CMeshManager::Vec4_Scale(vector3 &VecIn, float ValIn)
{
	vector3 VecOut;

	VecOut.x = VecIn.x * ValIn;
	VecOut.y = VecIn.y * ValIn;
	VecOut.z = VecIn.z * ValIn;

	return VecOut;
}

//**************************************************************
//Функция возвращает матрицу поворота вокруг заданого вектора
//**************************************************************
void CMeshManager::Matrix_Rotation_Axis(vector3 &VecIn, float Angle, matrix4x4 &MatOut)
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

	MatOut = matrix4x4 (xxomc + c,  xyomc + zs, xzomc - ys, 0.0f,
		xyomc - zs, yyomc + c,  yzomc + xs, 0.0f,
		xzomc + ys, yzomc - xs, zzomc + c, 0.0f,
		0.0f ,0.0f, 0.0f, 1.0f);
}

//*****************************************************
//Функция умножение вектора на матрицу
//*****************************************************
vector3 CMeshManager::Vec4_Mat4x4_View_Mul(vector3 &VecIn, matrix4x4 & MatIn)
{
	vector3 VecOut;

	//позиция камеры в мировых координатах
	VecIn.x -=  MatIn.Mat[3][0];
	VecIn.y -=  MatIn.Mat[3][1];
	VecIn.z -=  MatIn.Mat[3][2];

	VecOut.x = VecIn.x * MatIn.Mat[0][0] + VecIn.y * MatIn.Mat[1][0] + VecIn.z * MatIn.Mat[2][0];
	VecOut.y = VecIn.x * MatIn.Mat[0][1] + VecIn.y * MatIn.Mat[1][1] + VecIn.z * MatIn.Mat[2][1];
	VecOut.z = VecIn.x * MatIn.Mat[0][2] + VecIn.y * MatIn.Mat[1][2] + VecIn.z * MatIn.Mat[2][2];

	VecOut.tu = VecIn.tu;
	VecOut.tv = VecIn.tv;

	return VecOut;

}


//*****************************************************
//Функция умножение вектора на матрицу
//*****************************************************
vector3 CMeshManager::Vec4_Mat4x4_Mul(vector3 &VecIn, matrix4x4 & MatIn)
{
	vector3 VecOut;

	VecOut.x = VecIn.x * MatIn.Mat[0][0] + VecIn.y * MatIn.Mat[1][0] + VecIn.z * MatIn.Mat[2][0] + MatIn.Mat[3][0];
	VecOut.y = VecIn.x * MatIn.Mat[0][1] + VecIn.y * MatIn.Mat[1][1] + VecIn.z * MatIn.Mat[2][1] + MatIn.Mat[3][1];
	VecOut.z = VecIn.x * MatIn.Mat[0][2] + VecIn.y * MatIn.Mat[1][2] + VecIn.z * MatIn.Mat[2][2] + MatIn.Mat[3][2];

	VecOut.tu = VecIn.tu;
	VecOut.tv = VecIn.tv;

	return VecOut;

}

//********************************************
//Функция возвращает следующий треугольник
//из списка треугольников
//********************************************
polygon* list::Get_From_List ()
{
	polygon *plg;
	plg = &PolyList[PolygonCurr];
	PolygonCurr++;
	if(PolygonCurr > PolygonCount)
		return NULL;

	return plg;
}

//****************************************************
//Функция проверяет пустой ли список стреугольников
//или нет
//****************************************************
bool list::Is_Empty_List ()
{
		if(PolygonCount == 0)
			return true;

		return false;
}

//*****************************************
//Функция добавляет треугольник в список
//*****************************************
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

//***************************************************
//Функция возвращает цифровой индекс в зависимости
//от имени файла текстуры
//***************************************************
int CMeshManager::Get_TextureID(char * szFilename)
{
	if(!_strcmpi(szFilename, "texture1.bmp\n")) //текстура пола
	{
		return 0;
	}
	else if(!_strcmpi(szFilename, "texture2.bmp\n")) //текстура стен
	{
		return 1;
	}
	else if(!_strcmpi(szFilename, "texture3.bmp\n")) //текстура потолка
	{
		return 2;
	}
	
	return -1;
}

//*******************************
//Функция инициализирует сцену
//*******************************
void CMeshManager::Init_MeshManager(HWND hWnd)
{
	ShowCursor(FALSE);

	m_hWnd = hWnd;

	Timer_Start();

    RECT Rc;
    ::GetClientRect( m_hWnd, &Rc );

	m_BackLpitch = Rc.right * 4;
    
	m_ViewWidth  = Rc.right - Rc.left;
    m_ViewHeight = Rc.bottom - Rc.top;

	Create_Backbuffer();

	//меньше угол обзора- больше размер куба на экране
	float Fov = PI / 2.0f; //угол обзора 90 градусов
	//cos() делить на sin() будет котангенс, т.е. 1/tang()
	Persp = (Rc.bottom / 2.0f) * (cosf(Fov / 2.0f) / sinf(Fov / 2.0f));

	CenterX = m_ViewWidth / 2.0f;
	CenterY = m_ViewHeight / 2.0f;

	//ширина текстуры 256, высота текстуры 256
	//256 * 256 = 65536, три цвета r,g,b
	//то есть массив каждой текстуры
	//содержит 256 * 256 * 3 элементов
	//всего на сцене у нас 3 текстуры
	//текстура пола, стен, потолка
	m_pLevelTile = new unsigned char *[3];

	Load_BMP((char*)"texture1.bmp", 0);
	Load_BMP((char*)"texture2.bmp", 1);
	Load_BMP((char*)"texture3.bmp", 2);

	FILE *f;
	fopen_s(&f,"level.txt", "rt");

	char Buffer[1024];
	fgets(Buffer, 1024, f);

	int Size;
	sscanf_s(Buffer,"%d",&Size);

	polygon Poly;
	
	while(!feof(f))
	{
		vector3 *VecPos;
		VecPos = new vector3[4];
		
		fgets(Buffer, 1024, f);
		sscanf_s(Buffer,"%f %f %f %f %f",&VecPos[0].x, &VecPos[0].y, &VecPos[0].z, &VecPos[0].tu, &VecPos[0].tv);
	
		fgets(Buffer, 1024, f);
		sscanf_s(Buffer,"%f %f %f %f %f",&VecPos[1].x, &VecPos[1].y, &VecPos[1].z, &VecPos[1].tu, &VecPos[1].tv);
	
		fgets(Buffer, 1024, f);
		sscanf_s(Buffer,"%f %f %f %f %f",&VecPos[2].x, &VecPos[2].y, &VecPos[2].z, &VecPos[2].tu, &VecPos[2].tv);
	
		fgets(Buffer, 1024, f);
		sscanf_s(Buffer,"%f %f %f %f %f",&VecPos[3].x, &VecPos[3].y, &VecPos[3].z, &VecPos[3].tu, &VecPos[3].tv);
						
		fgets(Buffer, 1024, f);
						
		char TexFileName[256];
		strcpy_s(TexFileName,256,Buffer);
		int TexId = Get_TextureID(TexFileName);

		//ширина и высота всех текстур 256х256 пикселей
		//почему -1 размер текстуры участвует в формировании
		//индекса массива по которому извлекаются пиксели из текстуры
		//индекс массива должен быть больше 0 и меньше равно 255
		//т.е. размер самой текстуры 256х256
		VecPos[0].tu *= m_TextureWidth - 1;
		VecPos[0].tv *= m_TextureHeight - 1;
		
		VecPos[1].tu *= m_TextureWidth - 1;
		VecPos[1].tv *= m_TextureHeight - 1;

		VecPos[2].tu *= m_TextureWidth - 1;
		VecPos[2].tv *= m_TextureHeight - 1;

		VecPos[3].tu *= m_TextureWidth - 1;
		VecPos[3].tv *= m_TextureHeight - 1;

		Poly.Vertex[0] = VecPos[0];
		Poly.Vertex[1] = VecPos[1];
		Poly.Vertex[2] = VecPos[2];

		Poly.TexID = TexId;

		//из файла прочитали 4 вершины
		//добавляем в список первый треугольник
		m_Polygons.Add_To_List(&Poly);

		Poly.Vertex[0] = VecPos[0];
		Poly.Vertex[1] = VecPos[2];
		Poly.Vertex[2] = VecPos[3];

		Poly.TexID = TexId;
		
		//из файла прочитали 4 вершины
		//добавляем в список второй треугольник
		m_Polygons.Add_To_List(&Poly);

		delete[] VecPos;
	}

	fclose(f);

	m_VecCamUp = vector3( 0.0, 1.0, 0.0);
	m_VecCamLook = vector3( 0.0, 0.0, 1.0);

	//камера может смотреть в направлении +Z и -Z
	if(m_VecCamLook.z > 0)
	{
		m_VecCamRight = vector3( 1.0, 0.0, 0.0 );
	}

	if(m_VecCamLook.z < 0)
	{
		m_VecCamRight = vector3( -1.0, 0.0, 0.0 );
	}

	m_VecCamPos = vector3( 25.0f, 5.0f, 25.0f);
	
    //Строим BSP дерево
	m_Root = NULL;
	m_Root = new BSP_tree;
    Build_BSP_Tree(m_Root, m_Polygons);
}

//**********************************************
//Функция камеры, функция следит за нажатиями
//на клавиши для перемещения по сцене и 
//возвращает матрицу вида
//**********************************************
void CMeshManager::Get_View_Matrix(matrix4x4 &MatOut)
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
	
	m_VecCamRight = Vec4_Mat4x4_Mul(m_VecCamRight, MatRotRight);
	m_VecCamUp = Vec4_Mat4x4_Mul(m_VecCamUp, MatRotRight);
	m_VecCamLook = Vec4_Mat4x4_Mul(m_VecCamLook, MatRotRight);
	
	//горизонтальное смещение камеры (вокруг оси Y)
	if(DeltaX<0) m_RotationScalerX = m_RotationScalerX;
	else if(DeltaX>0) m_RotationScalerX = -m_RotationScalerX;
	else if(DeltaX==0) m_RotationScalerX = 0;

	vector3 VecUpTemp = vector3( 0.0f, 1.0f, 0.0f );
	matrix4x4 MatRotUp;
	Matrix_Rotation_Axis(VecUpTemp , m_RotationScalerX, MatRotUp);
	
	m_VecCamRight = Vec4_Mat4x4_Mul(m_VecCamRight, MatRotUp);
	m_VecCamUp = Vec4_Mat4x4_Mul(m_VecCamUp, MatRotUp);
	m_VecCamLook = Vec4_Mat4x4_Mul(m_VecCamLook, MatRotUp);
	
	//реакция на клавиши W,S,A,D
	float RatioMove = 50;
	vector3 VecAccel = vector3(0,0,0);
	
	if(GetAsyncKeyState('W')& 0xFF00) 
	{
		vector3 VecTemp = vector3(m_VecCamLook.x,0,m_VecCamLook.z);

		VecTemp = Vec4_Scale(VecTemp, RatioMove);
		VecAccel += Vec4_Scale(VecTemp, Time);
	}

	if(GetAsyncKeyState('S')& 0xFF00) 
	{
		vector3 VecTemp = vector3(m_VecCamLook.x,0,m_VecCamLook.z);

		VecTemp = Vec4_Scale(VecTemp, -RatioMove);
		VecAccel += Vec4_Scale(VecTemp, Time);
	}

	if(GetAsyncKeyState('D')& 0xFF00) 
	{
		vector3 VecTemp = vector3(m_VecCamRight.x,0,m_VecCamRight.z);

		VecTemp = Vec4_Scale(VecTemp, RatioMove);
		VecAccel += Vec4_Scale(VecTemp, Time);
	}

	if(GetAsyncKeyState('A')& 0xFF00) 
	{
		vector3 VecTemp = vector3(m_VecCamRight.x,0,m_VecCamRight.z);
		
		VecTemp = Vec4_Scale(VecTemp, -RatioMove);
		VecAccel += Vec4_Scale(VecTemp, Time);
	}

	m_VecCamPos = Vec4_Add(m_VecCamPos,VecAccel);

	m_VecCamLook = Vec4_Normalize(m_VecCamLook);
	m_VecCamUp = Vec4_Cross(m_VecCamLook,m_VecCamRight);
	m_VecCamUp = Vec4_Normalize(m_VecCamUp);
	m_VecCamRight = Vec4_Cross(m_VecCamUp,m_VecCamLook);
	m_VecCamRight = Vec4_Normalize(m_VecCamRight);

	//позиция камеры в мировых координатах
	MatOut = matrix4x4(
        m_VecCamRight.x,	m_VecCamUp.x,	m_VecCamLook.x, 0.0f,
		m_VecCamRight.y,	m_VecCamUp.y,	m_VecCamLook.y, 0.0f,
		m_VecCamRight.z,	m_VecCamUp.z,	m_VecCamLook.z, 0.0f,
		m_VecCamPos.x,		m_VecCamPos.y,	m_VecCamPos.z,	1.0f );
}

//***************************************************
//Функция используется для рассечения треугольника
//ближней плоскостью отсечения
//***************************************************
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

//******************************************************
//Функция обрезает полигон в экранных координатах
//******************************************************
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

//******************************************************
//Функция трансформирует сцену
//******************************************************
void CMeshManager::Update_MeshManager()
{
	Get_View_Matrix(m_MatView);
	
	float Fov =  PI/2.0f; // FOV 90 degree
	float Aspect = (float)m_ViewWidth / m_ViewHeight;
	float ZFar = 250.0f;
	m_ZNear = 0.01f;

	float h, w, Q;
    w = (1.0f/tanf(Fov*0.5f))/Aspect;  
    h = 1.0f/tanf(Fov*0.5f);   
    Q = ZFar/(ZFar - m_ZNear);
	
	//используем полный расчет матрицы проекции
	m_MatProj = matrix4x4(
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, Q, 1,
		0, 0, -Q*m_ZNear, 0);

	Transform_BSP_Tree (m_Root);

}

//******************************************************
//Функция отображает сцену на экране
//******************************************************
void CMeshManager::Draw_MeshManager()
{
	Clear_Backbuffer();

	Draw_BSP_Tree (m_Root, m_VecCamPos);

	Present_Backbuffer();
}

//************************************************
//Функция выводит список треугольников на экран
//************************************************
void CMeshManager::Draw_Polygon_List(list Polygons)
{
	for (int loop = 0; loop < Polygons.PolygonCount; loop++)
    {
		vector3 Vec1, Vec2, Vec3;
		Vec1 = Polygons.PolyList[loop].Vertex[0];
		Vec2 = Polygons.PolyList[loop].Vertex[1];
		Vec3 = Polygons.PolyList[loop].Vertex[2];

		UINT tex = Polygons.PolyList[loop].TexID;
		//устанавливаем нужную текстуру
		m_Res  = (UCHAR *) m_pLevelTile[tex];

		Draw_Textured_Triangle(Vec1, Vec2, Vec3);
	}
}

//**********************************************************
//Функция разбивает входящий треугольник на два
//верхний и нижний и вызывает функцию
//растеризации одного треугольника
//или может быть один треугольник с плоским низом, верхом
//**********************************************************
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

//*******************************************
//Функция растеризации одного треугольника
//*******************************************
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
			float z = 1.0f/zi;
			float u = ui * z;
			float v = vi * z;

			UINT t = (int)u  + (((int)v) * m_TextureWidth);

			if( t < 0 || t > (m_TextureWidth * m_TextureHeight - 1) )
				continue;

			t= t*3;

			int Index = y * m_BackLpitch + x * 4;
			
			m_Data[Index + 0] = m_Res[t + 0];
			m_Data[Index + 1] = m_Res[t + 1];
			m_Data[Index + 2] = m_Res[t + 2];
			m_Data[Index + 3] = 0;
				
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

//***********************************************
//Функция загрузает BMP файл с текстурой сцены
//***********************************************
bool CMeshManager::Load_BMP(char *szFilename, int Tile)
{
	FILE *fp;

	fopen_s(&fp, szFilename,"rb");
	if(fp==NULL) printf("Error Open File");

	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(bfh), 1, fp);

	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(bih), 1, fp);

	m_pLevelTile[Tile] = new unsigned char [bih.biWidth*bih.biHeight*3];

	fread(m_pLevelTile[Tile],bih.biWidth*bih.biHeight*3,1,fp);

	//все текстуры одного размера 256х256 пикселей
	m_TextureWidth = bih.biWidth;
	m_TextureHeight = bih.biHeight;

	return true;
}

//****************************
//Функция строит BSP дерево
//****************************
void CMeshManager::Build_BSP_Tree(BSP_tree *Tree, list Polygons)
{
	memset(Tree,0, sizeof(BSP_tree));

	polygon   *root = Polygons.Get_From_List();
	
	Tree->Partition= root->Get_Plane();
	Tree->Polygons.Add_To_List(root);

	list FrontList, BackList;
    polygon   *Poly;

	while ((Poly = Polygons.Get_From_List()) != 0)
	{
		int   Result = Tree->Partition.Classify_Polygon (Poly);

		switch (Result)
		{
			case COINCIDENT:
				Tree->Polygons.Add_To_List (Poly);
				break;
			
			case IN_BACK_OF:
				BackList.Add_To_List (Poly);
			    break;
			
			case IN_FRONT_OF:
	            FrontList.Add_To_List (Poly);
		        break;
			
			case SPANNING:
				int cf = 0, cb = 0;
				polygon   *FrontPiece=NULL, *BackPiece=NULL;
				Split_Polygon (Poly, &Tree->Partition, FrontPiece, BackPiece, cf, cb);

				//cf - количество треугольников спереди секущей плоскости
				//cb - количество треугольников сздали секущей плоскости
				if(cf == 1 && cb == 1)
				{
					FrontList.Add_To_List (&FrontPiece[0]);
					BackList.Add_To_List (&BackPiece[0]);
				}
				else if(cf == 1 && cb == 0)
				{
					FrontList.Add_To_List (&FrontPiece[0]);
				}
				else if(cf == 1 && cb == 2)
				{
					FrontList.Add_To_List (&FrontPiece[0]);
					
					BackList.Add_To_List (&BackPiece[0]);	
					BackList.Add_To_List (&BackPiece[1]);
				}
				else if(cb == 1 && cf == 0)
				{
					BackList.Add_To_List (&BackPiece[0]);	
				}
				else if(cb == 1 && cf == 2)
				{
					BackList.Add_To_List (&BackPiece[0]);	
						
					FrontList.Add_To_List (&FrontPiece[0]);	
					FrontList.Add_To_List (&FrontPiece[1]);
				}
			break;
		  }
	}

	if (!FrontList.Is_Empty_List())
	{
		Tree->Front = new BSP_tree;
		Build_BSP_Tree (Tree->Front, FrontList);
	}
	if ( ! BackList.Is_Empty_List ())
	{
		Tree->Back = new BSP_tree;
		Build_BSP_Tree (Tree->Back, BackList);
	}
}

//*******************************************
//Функция на основе полигона (треугольника)
//создает плоскость
//*******************************************
plane polygon::Get_Plane()
{
	plane p;

	vector3 VecEdge1 = Vertex[1] - Vertex[0];
	vector3 VecEdge2 = Vertex[2] - Vertex[0];
	
	p.a = (VecEdge1.y*VecEdge2.z)-(VecEdge2.y*VecEdge1.z);
    p.b = (VecEdge1.z*VecEdge2.x)-(VecEdge2.z*VecEdge1.x);
    p.c = (VecEdge1.x*VecEdge2.y)-(VecEdge2.x*VecEdge1.y);
    
	float len = sqrtf(p.a * p.a + p.b * p.b + p.c * p.c);

	p.a = p.a / len;
	p.b = p.b / len;
	p.c = p.c / len;

	p.d=-(p.a*Vertex[0].x+p.b*Vertex[0].y+p.c*Vertex[0].z);

	return p;
}

//*******************************************
//Функция возвращает нормаль к треугольнику
//*******************************************
vector3 polygon::GetNormal()
{
	vector3 VecTemp; 
        
	vector3 VecEdge1 = Vertex[1] - Vertex[0];
	vector3 VecEdge2 = Vertex[2] - Vertex[0];
	
	VecTemp.x = (VecEdge1.y*VecEdge2.z)-(VecEdge2.y*VecEdge1.z);
    VecTemp.y = (VecEdge1.z*VecEdge2.x)-(VecEdge2.z*VecEdge1.x);
    VecTemp.z = (VecEdge1.x*VecEdge2.y)-(VecEdge2.x*VecEdge1.y);
    
	return VecTemp;
}


//****************************************************
//Функция определяет расстояние точки до пслоскости
//****************************************************
float plane::Classify_Point (vector3 Eye)
{
	return a*Eye.x + b*Eye.y + c*Eye.z + d;
}

//****************************************************
//Функция определяет треугольник лежит на плоскости,
//перед, за плоскостью, или разбивается плоскостью
//****************************************************
int plane::Classify_Polygon (polygon *Poly)
{
	float p[3];
	
	for(int i=0;i<3;i++)
		p[i]=a*Poly->Vertex[i].x + b*Poly->Vertex[i].y + c*Poly->Vertex[i].z + d;

	if(p[0]==0 && p[1]==0 && p[2]==0)
		return COINCIDENT;

	else if(p[0]<0 && p[1]<0 && p[2]<0)
		return IN_BACK_OF;

	else if(p[0]>0  && p[1]>0 && p[2]>0)
		return IN_FRONT_OF;
	
	else
		return SPANNING;
}

//*****************************************
//Функция возвращает нормаль к плоскости
//*****************************************
vector3 plane::Normal()
{
	vector3 t;

	t.x = a;
	t.y = b;
	t.z = c;

	return t;
}

//*******************************************
//Функция разбивает треугольник плоскостью
//на части
//*******************************************
void CMeshManager::Split_Polygon (polygon *Poly, plane *Part, polygon *&Front, polygon *&Back, int &cf, int &cb)
{
   int   Count = 3;
   int FrontC = 0, BackC = 0;
   vector3 PtA, PtB, FrontSet[4], BackSet[4];
   float	SideA, SideB;

   PtA = Poly->Vertex[Count - 1];

   SideA = Part->Classify_Point ((vector3)PtA);

   float epsilon = 0.001f;

   if(SideA < epsilon && SideA > -epsilon)
	   SideA = 0.0f;
   
   for (short i = -1; ++i < Count;)
   {
      PtB = Poly->Vertex[i];

      SideB = Part->Classify_Point ((vector3)PtB);

	  if(SideB < epsilon && SideB > -epsilon)
	   SideB = 0.0f;

      if (SideB > 0)
      {
         if (SideA < 0)
         {
            // compute the intersection point of the line
            // from point A to point B with the Partition
            // plane. This is a simple ray-plane intersection.
            vector3 v = PtB - PtA;

			float   sect = - Part->Classify_Point (PtA) / (Part->Normal() | v);

			BackSet[BackC++] = PtA + (v * sect);
			FrontSet[FrontC++] = PtA + (v * sect);
         }
		 
		 FrontSet[FrontC++] = PtB;
		 
      }
      else if (SideB < 0)
      {
         if (SideA > 0)
         {
            // compute the intersection point of the line
            // from point A to point B with the Partition
            // plane. This is a simple ray-plane intersection.
            vector3 v = PtB - PtA;
		
			float   sect = - Part->Classify_Point (PtA) / (Part->Normal() | v);
		
			BackSet[BackC++] = PtA + (v * sect);
			FrontSet[FrontC++] = PtA + (v * sect);
		
         }

		 BackSet[BackC++] = PtB;
		 
      }
      else
	  {
		  BackSet[BackC++] = PtB;
		  FrontSet[FrontC++] = PtB;
			
	  }
  
	  PtA = PtB;
      SideA = SideB;
   }

   if(FrontC == 4) //in return we got 2 triangles
   {
		Front = new polygon[2];
		ZeroMemory(Front, 2 * sizeof(polygon));
   
		//first tri
		Front[0].Vertex[0] = FrontSet[0];
		Front[0].Vertex[1] = FrontSet[1];
		Front[0].Vertex[2] = FrontSet[2];
		Front[0].TexID = Poly->TexID;

		//second tri
		Front[1].Vertex[0] = FrontSet[0];
		Front[1].Vertex[1] = FrontSet[2];
		Front[1].Vertex[2] = FrontSet[3];
		Front[1].TexID = Poly->TexID;
  
		//output tri Count
		cf = 2;

   }
   else if(FrontC == 3) //in return we got 1 triangle
   {
		Front = new polygon[1];
		ZeroMemory(Front, 1 * sizeof(polygon));

		Front[0].Vertex[0] = FrontSet[0];
		Front[0].Vertex[1] = FrontSet[1];
		Front[0].Vertex[2] = FrontSet[2];
		Front[0].TexID = Poly->TexID;
		
		//output tri Count
		cf = 1;

   }
   else
   {
	   //output tri Count
	   cf = 0;
   }

   if(BackC == 4) //in return we got 2 triangles
   {
		Back = new polygon[2];
		ZeroMemory(Back, 2 * sizeof(polygon));

		//first tri
		Back[0].Vertex[0] = BackSet[0];
		Back[0].Vertex[1] = BackSet[1];
		Back[0].Vertex[2] = BackSet[2];
		Back[0].TexID = Poly->TexID;

		
		//second tri
		Back[1].Vertex[0] = BackSet[0];
		Back[1].Vertex[1] = BackSet[2];
		Back[1].Vertex[2] = BackSet[3];
		Back[1].TexID = Poly->TexID;

		//output tri Count
		cb = 2;
   }
   else if(BackC == 3 ) //in return we got 1 triangle
   {
		Back = new polygon[1];
		ZeroMemory(Back, 1 * sizeof(polygon));

		Back[0].Vertex[0] = BackSet[0];
		Back[0].Vertex[1] = BackSet[1];
		Back[0].Vertex[2] = BackSet[2];
		Back[0].TexID = Poly->TexID;
		
		//output tri Count
		cb = 1;
		
   }
   else
   {
	   //output tri Count
		cb = 0;
   }
}

//*******************************************
//Перегруженый оператор вычитания векторов
//*******************************************
vector3 vector3::operator - (const vector3 & VecIn)
{
	vector3 t;

	t.x = x - VecIn.x;
	t.y = y - VecIn.y;
	t.z = z - VecIn.z;

	t.tu = tu - VecIn.tu;
	t.tv = tv - VecIn.tv;

	return t;
};

//******************************************
//Перегруженый оператор сложения векторов
//******************************************
vector3 vector3::operator + (const vector3 & VecIn)
{
	vector3 t;

	t.x = x + VecIn.x;
	t.y = y + VecIn.y;
	t.z = z + VecIn.z;

	t.tu = tu + VecIn.tu;
	t.tv = tv + VecIn.tv;

	return t;
};


vector3 & vector3::operator += (const vector3 & VecIn)
{
	x = x + VecIn.x;
	y = y + VecIn.y;
	z = z + VecIn.z;

	tu = tu + VecIn.tu;
	tv = tv + VecIn.tv;

	return *this;
};

//******************************************
//Перегруженый оператор умножения вектора
//на скаляр
//******************************************
vector3 vector3::operator * (const float & ValIn)
{
	vector3 t;

	t.x = x * ValIn;
	t.y = y * ValIn;
	t.z = z * ValIn;

	t.tu = tu * ValIn;
	t.tv = tv * ValIn;

	return t;
};

//*************************************************
//Перегруженый оператор присваивания для вектора
//*************************************************
vector3 & vector3::operator = (const vector3 & VecIn)
{
	x = VecIn.x;
	y = VecIn.y;
	z = VecIn.z;

	tu = VecIn.tu;
	tv = VecIn.tv;

	return *this;
}

//**************************************************
//Конструктор с параметрами для структуры вектора
//**************************************************
vector3::vector3(float ix, float iy, float iz)
{
	x = ix;
	y = iy;
	z = iz;
}

//***********************************
//Деструктор для структуры вектора
//***********************************
vector3::~vector3()
{

};

//*************************************************
//Конструктор по умолчанию для структуры вектора
//*************************************************
vector3::vector3()
{

};

//************************************************
//Перегруженый оператор скалярного произведения
//векторов
//************************************************
float vector3::operator | (const vector3 & VecIn)
{
	return x * VecIn.x + y * VecIn.y + z * VecIn.z;
};

//*****************************************************
//Функция удаляет BSP дерево
//*****************************************************
void CMeshManager::Delete_BSP(BSP_tree *Tree)
{
	if(Tree->Front != NULL)
	{
		Delete_BSP(Tree->Front);
		
		delete Tree->Front;
		Tree->Front = NULL;
	}

	if(Tree->Back != NULL)
	{
		Delete_BSP(Tree->Back);
    
		delete Tree->Back;
		Tree->Back = NULL;
	}
}

//***********************************************
//Функция трансформирует BSP дерево (умножение
//на матрицы мира, вида, проекции и т.п.)
//***********************************************
void CMeshManager::Transform_BSP_Tree (BSP_tree *Tree)
{
	if(!Tree)
		return;

	//обнуляем список транформированных полигонов
	//для следующего кадра - например в этом кадре
	//камера переместилась
	Tree->TransformedPolygons.Reset();

	//в цикле перебираем все полигоны в списке
	//и трансформируем их - умножение на матрицы
	//вида, мира, отбрасывание задних поверхностей,
	//отсечение по передней плоскости просмотра
	//матрица проекции, деление на z, экранные координаты
	//заносим полигон в список трансформированных полигонов
	for ( int j = 0; j < Tree->Polygons.PolygonCount; j++)
	 {
		 
		//если полигон прошел тест на объем просмотра
		//получаем вершины из списка полигонов (исходный массив)
		vector3 Vec1 = Tree->Polygons.PolyList[j].Vertex[0];
		vector3 Vec2 = Tree->Polygons.PolyList[j].Vertex[1];
		vector3 Vec3 = Tree->Polygons.PolyList[j].Vertex[2];
		
		//умножаем вершины на матрицу вида (перемещение камеры)
		Vec1 = Vec4_Mat4x4_View_Mul(Vec1, m_MatView);
		Vec2 = Vec4_Mat4x4_View_Mul(Vec2, m_MatView);
		Vec3 = Vec4_Mat4x4_View_Mul(Vec3, m_MatView);

		//backface culling
		//отбрасывание задних поверхностей
		//vector subtract
		vector3 VecEdge1 = Vec2 - Vec1;
		vector3 VecEdge2 = Vec3 - Vec1;

		VecEdge1 = Vec4_Normalize(VecEdge1);
		VecEdge2 = Vec4_Normalize(VecEdge2);

		//vector cross
		vector3 VecCross = Vec4_Cross(VecEdge1, VecEdge2);
		VecCross = Vec4_Normalize(VecCross);

		//vector dot
		float Dot = VecCross | Vec1;

		//если cos от 0 до 90 градусов
		//отбрасываем полигон - не рисуем
		//переходим к следующему полигону
		if (Dot >= 0.0)
			continue;
		
		//если полигон прошел тест на backface culling
		//начинаем отсечение по передней плоскости
		//отсечение производится в пространстве вида
		//в результате отсечения накапливаем новые
		//треугольники в FrontList
		//FrontList может содержать один или два треугольника
		//получившихся после отсечения передней плоскостью
		//отсечения, либо один исходный треугольник
		//если небыло отсечения
		
		list FrontList;

		//все три вершины перед плоскостью
		if( (Vec1.z > m_ZNear) && (Vec2.z > m_ZNear) && (Vec3.z > m_ZNear) )
		{
			polygon Poly;

			Poly.Vertex[0] = Vec1;
			Poly.Vertex[1] = Vec2;
			Poly.Vertex[2] = Vec3;

			Poly.TexID = Tree->Polygons.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);
		}

		//вариант 1 - Vec1 за плоскостью Vec2, Vec3 перед плоскостью
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

			Poly.TexID = Tree->Polygons.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec3;
			Poly.Vertex[2] = vt1;

			Poly.TexID = Tree->Polygons.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);
		}

		//вариант 2 - Vec2 за плоскостью, Vec1, Vec3 перед плоскостью
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

			Poly.TexID = Tree->Polygons.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec1;
			Poly.Vertex[2] = vt1;

			Poly.TexID = Tree->Polygons.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);
		}

		//вариант 3 - Vec3 за плоскостью, Vec1, Vec2 перед плоскостью
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

			Poly.TexID = Tree->Polygons.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec2;
			Poly.Vertex[2] = vt1;

			Poly.TexID = Tree->Polygons.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);
		}

		//вариант 4 - Vec1, Vec2 за плоскостью, Vec3 перед плоскостью
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

			Poly.TexID = Tree->Polygons.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

		}

		//вариант 5 - Vec1, Vec3 за плоскостью, Vec2 перед плоскостью
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

			Poly.TexID = Tree->Polygons.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

		}

		//вариант 6 - Vec2, Vec3 за плоскостью, Vec1 перед плоскостью
		if( (Vec1.z > m_ZNear) && (Vec2.z < m_ZNear) && (Vec3.z < m_ZNear ) )
		{
			// Vec3 -> Vec1
			// Vec2 -> Vec1
			vector3 vt0 = Calc_Edge(Vec1, Vec3);
			vector3 vt1 = Calc_Edge(Vec1, Vec2);

			polygon Poly;

			Poly.Vertex[0] = vt0;
			Poly.Vertex[1] = Vec1;
			Poly.Vertex[2] = vt1;

			Poly.TexID = Tree->Polygons.PolyList[j].TexID; 

			FrontList.Add_To_List(&Poly);

		}
		
		//отсечение в пространстве вида сделано
		//можно дальше трансформировать вершины
		//умножаем вершины на матрицу проекции
		//и далее деление на z, и экранные координаты
		//список полигонов FrontList может содержать
		//либо один либо два полигона получившихся после
		//отсечения передней плоскостью отсечения, или
		//если небыло отсечения один полигон
		for ( int i = 0; i < FrontList.PolygonCount; i++)
		{
			vector3 Vec1, Vec2, Vec3;

			Vec1 = FrontList.PolyList[i].Vertex[0];
			Vec2 = FrontList.PolyList[i].Vertex[1];
			Vec3 = FrontList.PolyList[i].Vertex[2];

			float z1 = Vec1.z / Persp;
			float z2 = Vec2.z / Persp;
			float z3 = Vec3.z / Persp;

			//после умножения вершин на матрицу проекции
			//деление на z
			//преобразование вершин в экранные координаты
			Vec1.x = Vec1.x / z1 + CenterX;
			Vec1.y = -Vec1.y / z1 + CenterY;

			Vec2.x = Vec2.x / z2 + CenterX;
			Vec2.y = -Vec2.y / z2 + CenterY;

			Vec3.x = Vec3.x / z3 + CenterX;
			Vec3.y = -Vec3.y / z3 + CenterY;

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
				
				Tree->TransformedPolygons.Add_To_List(&pt1);
			}
			else if(Verts == 4)
			{
				polygon pt1;
				
				pt1.Vertex[0] = Vertex[0];
				pt1.Vertex[1] = Vertex[1];
				pt1.Vertex[2] = Vertex[2];

				pt1.TexID = FrontList.PolyList[i].TexID;

				Tree->TransformedPolygons.Add_To_List(&pt1);

				polygon pt2;

				pt2.Vertex[0] = Vertex[0];
				pt2.Vertex[1] = Vertex[2];
				pt2.Vertex[2] = Vertex[3];

				pt2.TexID = FrontList.PolyList[i].TexID;
			
				Tree->TransformedPolygons.Add_To_List(&pt2);
			}
			
			else if(Verts == 5)
			{
				polygon pt1;
			
				pt1.Vertex[0] = Vertex[0];
				pt1.Vertex[1] = Vertex[1];
				pt1.Vertex[2] = Vertex[2];

				pt1.TexID = FrontList.PolyList[i].TexID;

				Tree->TransformedPolygons.Add_To_List(&pt1);

				polygon pt2;

				pt2.Vertex[0] = Vertex[0];
				pt2.Vertex[1] = Vertex[2];
				pt2.Vertex[2] = Vertex[3];

				pt2.TexID = FrontList.PolyList[i].TexID;
				
				Tree->TransformedPolygons.Add_To_List(&pt2);

				polygon pt3;

				pt3.Vertex[0] = Vertex[0];
				pt3.Vertex[1] = Vertex[3];
				pt3.Vertex[2] = Vertex[4];

				pt3.TexID = FrontList.PolyList[i].TexID;
				
				Tree->TransformedPolygons.Add_To_List(&pt3);
			}
			else if(Verts == 6)
			{
				polygon pt1;
			
				pt1.Vertex[0] = Vertex[0];
				pt1.Vertex[1] = Vertex[1];
				pt1.Vertex[2] = Vertex[2];

				pt1.TexID = FrontList.PolyList[i].TexID;

				Tree->TransformedPolygons.Add_To_List(&pt1);

				polygon pt2;

				pt2.Vertex[0] = Vertex[0];
				pt2.Vertex[1] = Vertex[2];
				pt2.Vertex[2] = Vertex[3];

				pt2.TexID = FrontList.PolyList[i].TexID;
				
				Tree->TransformedPolygons.Add_To_List(&pt2);

				polygon pt3;

				pt3.Vertex[0] = Vertex[0];
				pt3.Vertex[1] = Vertex[3];
				pt3.Vertex[2] = Vertex[4];

				pt3.TexID = FrontList.PolyList[i].TexID;
				
				Tree->TransformedPolygons.Add_To_List(&pt3);

				polygon pt4;

				pt4.Vertex[0] = Vertex[0];
				pt4.Vertex[1] = Vertex[4];
				pt4.Vertex[2] = Vertex[5];

				pt4.TexID = FrontList.PolyList[i].TexID;
				
				Tree->TransformedPolygons.Add_To_List(&pt4);
			}
			
		}

		//очищаем список полигонов
		FrontList.Reset();
	}

	//рекурсивно трансформируем все дерево
	Transform_BSP_Tree (Tree->Front);
	Transform_BSP_Tree (Tree->Back);
}

//**************************************
//Функция выводит BSP дерево на экран
//**************************************
void CMeshManager::Draw_BSP_Tree (BSP_tree *Tree, vector3 Eye)
{
	if(!Tree)
		return;

	float Result = Tree->Partition.Classify_Point (Eye);
	
	if (Result > 0)
	{
		Draw_BSP_Tree (Tree->Back, Eye);
		Draw_Polygon_List (Tree->TransformedPolygons);
		Draw_BSP_Tree (Tree->Front, Eye);
	}
	else if (Result < 0)
	{
		Draw_BSP_Tree (Tree->Front, Eye);
		Draw_Polygon_List (Tree->TransformedPolygons);
		Draw_BSP_Tree (Tree->Back, Eye);
	}
	else //результат равен 0
	{
		Draw_BSP_Tree (Tree->Front, Eye);
		Draw_BSP_Tree (Tree->Back, Eye);
	}
	
}

//***************************
//Функция запускает таймер
//***************************
void CMeshManager::Timer_Start()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)& m_PerfFreq);
	QueryPerformanceCounter((LARGE_INTEGER*)& m_LastTime);
	m_StartTime = m_LastTime;
	m_TimeScale = 1.0f / m_PerfFreq;
}

//*****************************************
//Функция возвращает время пройденное за
//один кадр
//*****************************************
float CMeshManager::Get_Elapsed_Time()
{
	__int64 nowTime;

	QueryPerformanceCounter((LARGE_INTEGER*)& nowTime);
	m_ElapsedTime = (nowTime - m_StartTime) * m_TimeScale;
	m_StartTime = nowTime;

	return m_ElapsedTime;

}

//*************************************************
//Конструктор по умолчанию для структуры матрицы
//*************************************************
matrix4x4::matrix4x4()
{
};

//***********************************
//Деструктор для структуры матрицы
//***********************************
matrix4x4::~matrix4x4()
{
};

//******************************************
//Конструктор с параметрами для структуры
//матрицы
//******************************************
matrix4x4::matrix4x4(float in_r1c1, float in_r1c2, float in_r1c3, float in_r1c4,
	float in_r2c1, float in_r2c2, float in_r2c3, float in_r2c4,
	float in_r3c1, float in_r3c2, float in_r3c3, float in_r3c4,
	float in_r4c1, float in_r4c2, float in_r4c3, float in_r4c4)
{
	Mat[0][0] = in_r1c1; Mat[0][1] = in_r1c2; Mat[0][2] = in_r1c3; Mat[0][3] = in_r1c4;
	Mat[1][0] = in_r2c1; Mat[1][1] = in_r2c2; Mat[1][2] = in_r2c3; Mat[1][3] = in_r2c4;
	Mat[2][0] = in_r3c1; Mat[2][1] = in_r3c2; Mat[2][2] = in_r3c3; Mat[2][3] = in_r3c4;
	Mat[3][0] = in_r4c1; Mat[3][1] = in_r4c2; Mat[3][2] = in_r4c3; Mat[3][3] = in_r4c4;
};

//***********************************************
//Перегруженный оператор умножения двух матриц
//***********************************************
matrix4x4 matrix4x4::operator * (const matrix4x4 MatIn)
{
	matrix4x4 MatOut;

	//row1 * col1
	MatOut.Mat[0][0] = Mat[0][0] * MatIn.Mat[0][0] + Mat[0][1] * MatIn.Mat[1][0] + Mat[0][2] * MatIn.Mat[2][0] + Mat[0][3] * MatIn.Mat[3][0];
	//row1 * col2
	MatOut.Mat[0][1] = Mat[0][0] * MatIn.Mat[0][1] + Mat[0][1] * MatIn.Mat[1][1] + Mat[0][2] * MatIn.Mat[2][1] + Mat[0][3] * MatIn.Mat[3][1];
	//row1 * col3
	MatOut.Mat[0][2] = Mat[0][0] * MatIn.Mat[0][2] + Mat[0][1] * MatIn.Mat[1][2] + Mat[0][2] * MatIn.Mat[2][2] + Mat[0][3] * MatIn.Mat[3][2];
	//row1 * col4
	MatOut.Mat[0][3] = Mat[0][0] * MatIn.Mat[0][3] + Mat[0][1] * MatIn.Mat[1][3] + Mat[0][2] * MatIn.Mat[2][3] + Mat[0][3] * MatIn.Mat[3][3];

	//row2 * col1
	MatOut.Mat[1][0] = Mat[1][0] * MatIn.Mat[0][0] + Mat[1][1] * MatIn.Mat[1][0] + Mat[1][2] * MatIn.Mat[2][0] + Mat[1][3] * MatIn.Mat[3][0];
	//row2 * col2
	MatOut.Mat[1][1] = Mat[1][0] * MatIn.Mat[0][1] + Mat[1][1] * MatIn.Mat[1][1] + Mat[1][2] * MatIn.Mat[2][1] + Mat[1][3] * MatIn.Mat[3][1];
	//row2 * col3
	MatOut.Mat[1][2] = Mat[1][0] * MatIn.Mat[0][2] + Mat[1][1] * MatIn.Mat[1][2] + Mat[1][2] * MatIn.Mat[2][2] + Mat[1][3] * MatIn.Mat[3][2];
	//row2 * col4
	MatOut.Mat[1][3] = Mat[1][0] * MatIn.Mat[0][3] + Mat[1][1] * MatIn.Mat[1][3] + Mat[1][2] * MatIn.Mat[2][3] + Mat[1][3] * MatIn.Mat[3][3];

	//row3 * col1
	MatOut.Mat[2][0] = Mat[2][0] * MatIn.Mat[0][0] + Mat[2][1] * MatIn.Mat[1][0] + Mat[2][2] * MatIn.Mat[2][0] + Mat[2][3] * MatIn.Mat[3][0];
	//row3 * col2
	MatOut.Mat[2][1] = Mat[2][0] * MatIn.Mat[0][1] + Mat[2][1] * MatIn.Mat[1][1] + Mat[2][2] * MatIn.Mat[2][1] + Mat[2][3] * MatIn.Mat[3][1];
	//row3 * col3
	MatOut.Mat[2][2] = Mat[2][0] * MatIn.Mat[0][2] + Mat[2][1] * MatIn.Mat[1][2] + Mat[2][2] * MatIn.Mat[2][2] + Mat[2][3] * MatIn.Mat[3][2];
	//row3 * col4
	MatOut.Mat[2][3] = Mat[2][0] * MatIn.Mat[0][3] + Mat[2][1] * MatIn.Mat[1][3] + Mat[2][2] * MatIn.Mat[2][3] + Mat[2][3] * MatIn.Mat[3][3];

	//row4 * col1
	MatOut.Mat[3][0] = Mat[3][0] * MatIn.Mat[0][0] + Mat[3][1] * MatIn.Mat[1][0] + Mat[3][2] * MatIn.Mat[2][0] + Mat[3][3] * MatIn.Mat[3][0];
	//row4 * col2
	MatOut.Mat[3][1] = Mat[3][0] * MatIn.Mat[0][1] + Mat[3][1] * MatIn.Mat[1][1] + Mat[3][2] * MatIn.Mat[2][1] + Mat[3][3] * MatIn.Mat[3][1];
	//row4 * col3
	MatOut.Mat[3][2] = Mat[3][0] * MatIn.Mat[0][2] + Mat[3][1] * MatIn.Mat[1][2] + Mat[3][2] * MatIn.Mat[2][2] + Mat[3][3] * MatIn.Mat[3][2];
	//row4 * col4
	MatOut.Mat[3][3] = Mat[3][0] * MatIn.Mat[0][3] + Mat[3][1] * MatIn.Mat[1][3] + Mat[3][2] * MatIn.Mat[2][3] + Mat[3][3] * MatIn.Mat[3][3];

	return MatOut;

}

//********************************
//Функция создает задний буфер
//********************************
void CMeshManager::Create_Backbuffer()
{
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

//********************************
//Функция очищает задний буфер
//********************************
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

//****************************************
//Функция выводит задний буфер на экран
//****************************************
void CMeshManager::Present_Backbuffer()
{
	//переворачиваем задний буфер
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

//********************************
//Функция удаляет задний буфер
//********************************
void CMeshManager::Delete_Backbuffer()
{
	DrawDibClose(m_hDD);

	free(m_Data);
	m_Data = NULL;

	free(m_DataTemp);
	m_DataTemp = NULL;
}
