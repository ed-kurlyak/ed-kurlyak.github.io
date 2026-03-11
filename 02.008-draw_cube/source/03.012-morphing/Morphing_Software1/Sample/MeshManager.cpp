//======================================================================================
//	Ed Kurlyak 2023 Software Rendering
//======================================================================================

#include "MeshManager.h"

CMeshManager::CMeshManager()
{
};

CMeshManager::~CMeshManager()
{
	Delete_Backbuffer();

	delete [] m_CubeVertsArray;
	delete [] m_SphereVertsArray;
	delete [] m_PyramidVertsArray;
	delete [] m_ResultVertsArray;

	delete [] m_VertBuffTransformed;
}

matrix4x4 CMeshManager::Mat4x4_Mat4x4_Mul(matrix4x4 &MatIn1, matrix4x4 &MatIn2)
{
	matrix4x4 MatOut;

	//row1 * col1
	MatOut.Mat[0][0] = MatIn1.Mat[0][0]*MatIn2.Mat[0][0] + MatIn1.Mat[0][1]*MatIn2.Mat[1][0] + MatIn1.Mat[0][2]*MatIn2.Mat[2][0] + MatIn1.Mat[0][3]*MatIn2.Mat[3][0];
	//row1 * col2
	MatOut.Mat[0][1] = MatIn1.Mat[0][0]*MatIn2.Mat[0][1] + MatIn1.Mat[0][1]*MatIn2.Mat[1][1] + MatIn1.Mat[0][2]*MatIn2.Mat[2][1] + MatIn1.Mat[0][3]*MatIn2.Mat[3][1];
	//row1 * col3
	MatOut.Mat[0][2] = MatIn1.Mat[0][0]*MatIn2.Mat[0][2] + MatIn1.Mat[0][1]*MatIn2.Mat[1][2] + MatIn1.Mat[0][2]*MatIn2.Mat[2][2] + MatIn1.Mat[0][3]*MatIn2.Mat[3][2];
	//row1 * col4
	MatOut.Mat[0][3] = MatIn1.Mat[0][0]*MatIn2.Mat[0][3] + MatIn1.Mat[0][1]*MatIn2.Mat[1][3] + MatIn1.Mat[0][2]*MatIn2.Mat[2][3] + MatIn1.Mat[0][3]*MatIn2.Mat[3][3];

	//row2 * col1
	MatOut.Mat[1][0] = MatIn1.Mat[1][0]*MatIn2.Mat[0][0] + MatIn1.Mat[1][1]*MatIn2.Mat[1][0] + MatIn1.Mat[1][2]*MatIn2.Mat[2][0] + MatIn1.Mat[1][3]*MatIn2.Mat[3][0];
	//row2 * col2
	MatOut.Mat[1][1] = MatIn1.Mat[1][0]*MatIn2.Mat[0][1] + MatIn1.Mat[1][1]*MatIn2.Mat[1][1] + MatIn1.Mat[1][2]*MatIn2.Mat[2][1] + MatIn1.Mat[1][3]*MatIn2.Mat[3][1];
	//row2 * col3
	MatOut.Mat[1][2] = MatIn1.Mat[1][0]*MatIn2.Mat[0][2] + MatIn1.Mat[1][1]*MatIn2.Mat[1][2] + MatIn1.Mat[1][2]*MatIn2.Mat[2][2] + MatIn1.Mat[1][3]*MatIn2.Mat[3][2];
	//row2 * col4
	MatOut.Mat[1][3] = MatIn1.Mat[1][0]*MatIn2.Mat[0][3] + MatIn1.Mat[1][1]*MatIn2.Mat[1][3] + MatIn1.Mat[1][2]*MatIn2.Mat[2][3] + MatIn1.Mat[1][3]*MatIn2.Mat[3][3];

	//row3 * col1
	MatOut.Mat[2][0] = MatIn1.Mat[2][0]*MatIn2.Mat[0][0] + MatIn1.Mat[2][1]*MatIn2.Mat[1][0] + MatIn1.Mat[2][2]*MatIn2.Mat[2][0] + MatIn1.Mat[2][3]*MatIn2.Mat[3][0];
	//row3 * col2
	MatOut.Mat[2][1] = MatIn1.Mat[2][0]*MatIn2.Mat[0][1] + MatIn1.Mat[2][1]*MatIn2.Mat[1][1] + MatIn1.Mat[2][2]*MatIn2.Mat[2][1] + MatIn1.Mat[2][3]*MatIn2.Mat[3][1];
	//row3 * col3
	MatOut.Mat[2][2] = MatIn1.Mat[2][0]*MatIn2.Mat[0][2] + MatIn1.Mat[2][1]*MatIn2.Mat[1][2] + MatIn1.Mat[2][2]*MatIn2.Mat[2][2] + MatIn1.Mat[2][3]*MatIn2.Mat[3][2];
	//row3 * col4
	MatOut.Mat[2][3] = MatIn1.Mat[2][0]*MatIn2.Mat[0][3] + MatIn1.Mat[2][1]*MatIn2.Mat[1][3] + MatIn1.Mat[2][2]*MatIn2.Mat[2][3] + MatIn1.Mat[2][3]*MatIn2.Mat[3][3];

	//row4 * col1
	MatOut.Mat[3][0] = MatIn1.Mat[3][0]*MatIn2.Mat[0][0] + MatIn1.Mat[3][1]*MatIn2.Mat[1][0] + MatIn1.Mat[3][2]*MatIn2.Mat[2][0] + MatIn1.Mat[3][3]*MatIn2.Mat[3][0];
	//row4 * col2
	MatOut.Mat[3][1] = MatIn1.Mat[3][0]*MatIn2.Mat[0][1] + MatIn1.Mat[3][1]*MatIn2.Mat[1][1] + MatIn1.Mat[3][2]*MatIn2.Mat[2][1] + MatIn1.Mat[3][3]*MatIn2.Mat[3][1];
	//row4 * col3
	MatOut.Mat[3][2] = MatIn1.Mat[3][0]*MatIn2.Mat[0][2] + MatIn1.Mat[3][1]*MatIn2.Mat[1][2] + MatIn1.Mat[3][2]*MatIn2.Mat[2][2] + MatIn1.Mat[3][3]*MatIn2.Mat[3][2];
	//row4 * col4
	MatOut.Mat[3][3] = MatIn1.Mat[3][0]*MatIn2.Mat[0][3] + MatIn1.Mat[3][1]*MatIn2.Mat[1][3] + MatIn1.Mat[3][2]*MatIn2.Mat[2][3] + MatIn1.Mat[3][3]*MatIn2.Mat[3][3];

	return MatOut;
}

vector3 CMeshManager::Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn)
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

void CMeshManager::GetStringFromDAT(char *Str)
{
	do
	{
		fgets(Str, 1024, Fp);
	} while(Str[0] == '\n');
}


void CMeshManager::Init_MeshManager(HWND hWnd)
{
	m_hWnd = hWnd;

	Create_Backbuffer();
	
	fopen_s(&Fp,".\\cube.txt", "rt");

	char Buff[256];
	GetStringFromDAT(Buff);

	m_VertCount = atoi(Buff);

	//************************************
	//MAKE CUBE
	//************************************

	m_CubeVertsArray = new vector3[m_VertCount];

	//загружаем вершины из файла
	for( UINT i = 0; i < m_VertCount; i++)
	{
		
		GetStringFromDAT(Buff);
		float x,y,z;
		sscanf_s(Buff, "%f;%f;%f;", &x, &y, &z);

		//одна сторона куба равна 50 единиц
		//от центра до стороны 25 единиц
		m_CubeVertsArray[i].x = x;
		m_CubeVertsArray[i].y = y;
		m_CubeVertsArray[i].z = z;

	}

	GetStringFromDAT(Buff);

	m_ColorArray = new color_rgb[m_VertCount];
	//загружаем цвета из файла
	for( UINT i = 0; i < m_VertCount; i++)
	{
		
		GetStringFromDAT(Buff);
		float r,g,b;
		sscanf_s(Buff, "%f;%f;%f;", &r, &g, &b);

		m_ColorArray[i].r = (int) r;
		m_ColorArray[i].g = (int) g;
		m_ColorArray[i].b = (int) b;

	}

	GetStringFromDAT(Buff);
	
	m_TriangleCount = atoi(Buff);

	m_IndicesArray = new unsigned int[m_TriangleCount * 3];
	
	//загружаем индексы из файла
	for( UINT i = 0; i < m_TriangleCount; i++)
	{
		GetStringFromDAT(Buff);

		UINT v1, v2, v3;
		sscanf_s(Buff, "%d;%d;%d;", &v1, &v2, &v3);

		m_IndicesArray[i * 3 + 0] = v1;
		m_IndicesArray[i * 3 + 1] = v2;
		m_IndicesArray[i * 3 + 2] = v3;
	}

	fclose(Fp);

	//************************************
	//MAKE SPHERE
	//************************************

	m_SphereVertsArray = new vector3[m_VertCount];
	
	//как делаем сферу
	//каждую вершину нормализуем и умножаем на радиус
	//радису сферы это от центра координат до стороны
	//(не до угла куба- до стороны)
	for( UINT i = 0; i < m_VertCount; i++)
	{
		//вычисляем длинну вектора до вершины
		float len = sqrtf(m_CubeVertsArray[i].x * m_CubeVertsArray[i].x +
			m_CubeVertsArray[i].y * m_CubeVertsArray[i].y +
			m_CubeVertsArray[i].z * m_CubeVertsArray[i].z);
		
		//нормализуем и умножаем на радиус 25 
		//25 это от цента до одной стороны куба
		m_SphereVertsArray[i].x = m_CubeVertsArray[i].x / len * 25.0f;
		m_SphereVertsArray[i].y = m_CubeVertsArray[i].y / len * 25.0f;
		m_SphereVertsArray[i].z = m_CubeVertsArray[i].z / len * 25.0f;
	}

	//************************************
	//MAKE PYRAMID
	//************************************

	m_PyramidVertsArray = new vector3[m_VertCount];

	//как делаем пирамиду
	//общая высота куба 50 (от центра вниз 25 и вверх 25)
	//мы y каждой вершины переводим от 0 до 50
	//затем вычисляем коофициент y / 50
	//коофициент будет от 0 до 1
	//затем x,z каждой вершины умножаем на этот коэфициент
	for( UINT i = 0; i < m_VertCount; i++)
	{
		float y = 0;

		//если y вершины меньше 0
		//т.е. веришна ниже центра координат
		//а центр куба в центре координат
		//переводим y вершины от 0 до 50
		if( m_CubeVertsArray[i].y < 0)
			y = 25.0f + fabsf(m_CubeVertsArray[i].y);

		//переводим y вершины от 0 до 50
		if( m_CubeVertsArray[i].y > 0)
			y = 25.0f - m_CubeVertsArray[i].y;

		//если вершина на уровне центра коорданат
		//то y = 25
		if( m_CubeVertsArray[i].y == 0)
			y = 25;

		//вычисляем коофициент
		float fac = y / 50.0f;

		//умножаем x и z каждой вершины на этот коэфициент
		//что бы уменьшить значение x и z
		m_PyramidVertsArray[i].x = m_CubeVertsArray[i].x * fac ;
		m_PyramidVertsArray[i].y = m_CubeVertsArray[i].y;
		m_PyramidVertsArray[i].z = m_CubeVertsArray[i].z * fac;
	}

	//***************************
	//CREATE RESULT VERT BUFF
	//***************************

	m_ResultVertsArray = new vector3[m_VertCount];

	m_VertBuffTransformed = new vector3[m_VertCount];

}

void CMeshManager::Update(vector3 *VecIn1, vector3 *VecIn2, float Scalar)
{
	for( DWORD i=0; i < m_VertCount; i++ )
    {

		vector3 VecSrc = VecIn1[i];
		vector3 VecDst = VecIn2[i];

		VecSrc= VecSrc * (float)(1.0f - Scalar);
		VecDst= VecDst * Scalar;

		m_ResultVertsArray[i] = VecSrc + VecDst;

	}
}

void CMeshManager::Update_MeshManager()
{
	float BlendWeight;

	static int NumBlend = 0;

	static FLOAT BlendVal = 0.0f;
	BlendVal += 0.0075f;

	//считаем до 0 до 1 BlendWeight
	if(BlendVal < 1.0f)
	{
		BlendWeight = BlendVal;
	}
	else
	{
		//делаем паузу после перехода
		//анимации оставляем мешь в позиции интерполяции 1.0
		BlendWeight = 1.0;

		//если пауза закончилась т.е. больше 3.0
		//обнуляем BlendWeight
		//переключаем на следующую анимацию NumBlend++
		if(BlendVal > 3.0f)
		{
			BlendWeight = 0.0;
			BlendVal = 0.0f;
			NumBlend++;
		}

		//если закончились все анимации
		//начинаем сначала
		if(NumBlend > 3)
			NumBlend = 0;
	}

	//первая анимация куб-сфера
	if(NumBlend == 0)
	{
		Update(m_CubeVertsArray, m_SphereVertsArray, BlendWeight);
	}
	//вторая анимация сфера-пирамида
	else if(NumBlend == 1)
	{
		Update(m_SphereVertsArray, m_PyramidVertsArray, BlendWeight);
	}
	//третья анимация пирамида-сфера
	else if(NumBlend == 2)
	{
		Update(m_PyramidVertsArray, m_SphereVertsArray, BlendWeight);
	}
	//четвертая анимация сфера-куб
	else if(NumBlend == 3)
	{
		Update(m_SphereVertsArray, m_CubeVertsArray, BlendWeight);
	}

	//рисуем результирующий мешь после интерполяции
	static float Angle = 0.0f;;

	matrix4x4 MatRotateX={
		1, 0, 0, 0,
        0, cosf(Angle), sinf(Angle), 0,
        0, -sinf(Angle), cosf(Angle), 0,
        0, 0, 0, 1};
	
	matrix4x4 MatRotateY={
		cosf(Angle), 0, -sinf(Angle), 0,
		0, 1, 0, 0,
		sinf(Angle), 0, cosf(Angle), 0,
		0, 0, 0, 1};

	matrix4x4 MatRotateZ={
		cosf(Angle), sinf(Angle), 0, 0,
		-sinf(Angle), cosf(Angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};
	
	Angle += 0.005f;
    if(Angle> PI2)
		Angle = 0;
	
	matrix4x4 MatWorld={
		1, 0, 0, 0,
		0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 80, 1};

	matrix4x4 MatView={
		1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

	float Fov =(float) PI / 2.0f; // FOV 90 degree
    float Aspect = (float) m_ViewWidth/m_ViewHeight;
	float ZFar = 100.0f;
	float ZNear = 0.1f;

	float h, w, Q;

    w = (1.0f/tanf(Fov*0.5f))/Aspect;  
    h = 1.0f/tanf(Fov*0.5f);   
    Q = ZFar/(ZFar - ZNear);

	/*
    matrix4x4 MatProj={
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, Q, 1,
        0, 0, -Q*fZNear, 0 };
	*/

    matrix4x4 MatProj={
		w, 0, 0, 0,
        0, h, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 };

	float Alpha = (0.5f * m_ViewWidth);
	float Beta  = (0.5f * m_ViewHeight);
	
	matrix4x4 MatScreen = {
		Alpha,  0,	    0,    0, 
		0,      -Beta,  0,    0, 
		0,		0,		1,    0,
		Alpha,  Beta,	0,    1};

	for (UINT i = 0; i < m_VertCount; i++)
	{
		matrix4x4 MatTemp;

		MatTemp = Mat4x4_Mat4x4_Mul(MatRotateX, MatRotateY);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatRotateZ);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatWorld);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatView);
		MatTemp = Mat4x4_Mat4x4_Mul(MatTemp, MatProj);

		vector3 VecTemp;

		VecTemp = Vec3_Mat4x4_Mul(m_ResultVertsArray[i], MatTemp);

		VecTemp.x = VecTemp.x / VecTemp.z;
		VecTemp.y = VecTemp.y / VecTemp.z;

		VecTemp = Vec3_Mat4x4_Mul(VecTemp, MatScreen);

		m_VertBuffTransformed[i] = VecTemp;
	}
}

void CMeshManager::Draw_Color_Poly(int y1, int y2, color_rgb Color)
{
	for ( int y = y1; y < y2; y++ )
	{
		for (int x= (int)m_xl; x < (int)m_xr; x++)
		{
			int Index =  y * 4 * m_ViewWidth + x * 4;
			
			m_Data[Index + 0] = Color.b; // blue
			m_Data[Index + 1] = Color.g; // green
			m_Data[Index + 2] = Color.r; // red
			m_Data[Index + 3] = 0; 
		}

		m_xl+=m_dxl;
		m_xr+=m_dxr;
	}
}

void CMeshManager::Draw_Color_Triangle(float x1,float y1,
					   float x2,float y2,
					   float x3,float y3,
					   color_rgb Color)
{
	float Temp;
	int Side;

	if (y2 < y1)
	{
		SWAP(x2,x1,Temp);
		SWAP(y2,y1,Temp);
	}

	if (y3 < y1)
	{
		SWAP(x3,x1,Temp);
		SWAP(y3,y1,Temp);
		
	}

	if (y3 < y2)
	{
		
		SWAP(x3,x2,Temp);
		SWAP(y3,y2,Temp);
		
	}

	int y1i = (int) floor (y1);
	int y2i = (int) floor (y2);
	int y3i = (int) floor (y3);

	if (y2i > y1i && y3i > y2i)
	{
		float dxdy1 = (x2 - x1) / (y2 - y1);
		float dxdy2 = (x3 - x1) / (y3 - y1);
		Side = dxdy2 > dxdy1;
	}

	if (y1i == y2i)
		Side = x1 > x2;

	if (y2i == y3i)
		Side = x3 > x2;

	if (!Side)//если Side = 0 левая сторона длиннее
	{
		m_dxl = (x3 - x1) / (y3 - y1);
		
		m_xl = x1;
		
		if ( y1 < y2) //треугольник с плоским низом x3 < x2
		{
			m_dxr = (x2 - x1) / (y2 - y1);

			m_xr = x1;

			Draw_Color_Poly(y1i, y2i, Color);	
		}

		if(y2 < y3) //треугольник с плоским верхом x1 < x2
		{
			m_dxr = (x3 - x2) / (y3 - y2);

			m_xr = x2;

			Draw_Color_Poly(y2i, y3i, Color);
		}
	}
	else
	{
		m_dxr = (x3 - x1) / (y3 - y1);
		
		m_xr = x1;
		
		if (y1 < y2) //треугольинк с плоским низом x3 > x2
		{
			m_dxl = (x2 - x1) / (y2 - y1);

			m_xl = x1;

			Draw_Color_Poly(y1i, y2i, Color);
	
		}
		if (y2 < y3) //треугольник с плоским верхом x1 > x2
		{
	
			m_dxl = (x3 - x2) / (y3 - y2);
			
			m_xl = x2;

			Draw_Color_Poly(y2i, y3i, Color);
		}
	}
}

void CMeshManager::Draw_MeshManager()
{
	 Clear_BackBuffer();

    for (UINT i = 0; i < m_TriangleCount; i++)
	 {
		vector3 Vec1 = m_VertBuffTransformed[m_IndicesArray[i * 3 + 0]];
		vector3 Vec2 = m_VertBuffTransformed[m_IndicesArray[i * 3 + 1]];
		vector3 Vec3 = m_VertBuffTransformed[m_IndicesArray[i * 3 + 2]];

		//для отбрасывания задних поверхностей
		float s = (Vec2.x - Vec1.x) * (Vec3.y - Vec1.y) - (Vec2.y - Vec1.y) * (Vec3.x - Vec1.x);

		if (s <= 0)
			continue;

		//цвета для всех 3х вершин одинаковые
		//поэтому берем только цвет 1й вершины
		color_rgb Color = m_ColorArray[m_IndicesArray[i * 3]];

		Draw_Color_Triangle(Vec1.x, Vec1.y, Vec2.x, Vec2.y, Vec3.x, Vec3.y, Color);
     }  

	Present_BackBuffer();
	
}

void CMeshManager::Create_Backbuffer()
{
	RECT Rc;
	GetClientRect(m_hWnd, &Rc);

	m_ViewWidth = Rc.right;
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

void CMeshManager::Delete_Backbuffer()
{
	DrawDibClose(m_hDD);

	free(m_Data);
	m_Data = NULL;
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
	HDC hDC = GetDC(m_hWnd);
	DrawDibDraw(m_hDD, hDC, 0, 0, m_ViewWidth, m_ViewHeight, &m_Bih, m_Data, 0, 0, m_ViewWidth, m_ViewHeight, 0);
	ReleaseDC(m_hWnd, hDC);
}

