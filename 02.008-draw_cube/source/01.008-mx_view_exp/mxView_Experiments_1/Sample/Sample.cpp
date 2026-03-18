//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <math.h>

struct vector3
{
	float x,y,z;
};

typedef float matrix4x4[4][4];

float Vec3_Dot(vector3 &VecIn1, vector3 &VecIn2)
{
	return VecIn1.x * VecIn2.x + VecIn1.y * VecIn2.y + VecIn1.z * VecIn2.z;
}

//нормализация вектора- приведение к единичной длинне
vector3 Vec3_Normalize(vector3 &VecIn)
{
	float len = sqrtf((VecIn.x * VecIn.x) + (VecIn.y * VecIn.y) + (VecIn.z * VecIn.z));

	vector3 VecOut = { VecIn.x / len, VecIn.y / len, VecIn.z / len };

	return VecOut;
}

//векторное произведение векторов
vector3 Vec3_Cross(vector3 &VecIn1, vector3 &VecIn2)
{
	vector3 t = { VecIn1.y * VecIn2.z - VecIn1.z * VecIn2.y,
		     VecIn1.z * VecIn2.x - VecIn1.x * VecIn2.z,
		     VecIn1.x * VecIn2.y - VecIn1.y * VecIn2.x };

	return t;
}

void Get_View_Matrix(matrix4x4 * MatView)
{
	vector3 VecCamPos = { 0.0f, 5.0f, -15.0f };
	vector3 VecCamUp = { 0.0f, 1.0f, 0.0f };
	vector3 VecCamLook = { 0, 0, 1.0f};
	
	vector3 VecCamRight = {0};
	
	if(VecCamLook.z > 0)
	{
		VecCamRight.x = 1.0f;
		VecCamRight.y = 0.0f;
		VecCamRight.z = 0.0f;
	}

	if(VecCamLook.z < 0)
	{
		VecCamRight.x = -1.0f;
		VecCamRight.y = 0.0f;
		VecCamRight.z = 0.0f;
	}

	VecCamLook = Vec3_Normalize(VecCamLook);

	VecCamUp = Vec3_Cross(VecCamLook, VecCamRight);
	VecCamUp = Vec3_Normalize(VecCamUp);
	VecCamRight = Vec3_Cross(VecCamUp, VecCamLook);
	VecCamRight = Vec3_Normalize(VecCamRight);

	float xp = - Vec3_Dot(VecCamPos, VecCamRight);
	float yp = - Vec3_Dot(VecCamPos, VecCamUp);
	float zp = - Vec3_Dot(VecCamPos, VecCamLook);

	matrix4x4 View = {
		VecCamRight.x,	VecCamUp.x,	VecCamLook.x,	0.0,
		VecCamRight.y,   VecCamUp.y,  VecCamLook.y,    0.0,
		VecCamRight.z,   VecCamUp.z,  VecCamLook.z,    0.0,
		xp,			yp,		zp,			1.0 };	

	memcpy(MatView, &View, sizeof(matrix4x4));
}

vector3 Vec3_Mat4x4_Mul(vector3 &VecIn, matrix4x4 MatIn)
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

int main(void)
{
	vector3 VecIn = { 0.0f, 0.0f, 2.0f };

	matrix4x4 MatView;
	Get_View_Matrix(&MatView);

	vector3 VecOut = Vec3_Mat4x4_Mul(VecIn, MatView);

	return 0;
}
	

