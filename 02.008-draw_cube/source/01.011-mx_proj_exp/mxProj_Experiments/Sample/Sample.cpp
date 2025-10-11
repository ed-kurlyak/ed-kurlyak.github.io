//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#include <windows.h>
#include <math.h>

#define PI 3.14159265358979f

struct vector3
{
	float x,y,z;
};

typedef float matrix4x4[4][4];

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
	vector3 VecIn = { 0.0f, 4.0f, 0.0f };

	float Fov = PI/2.0f; // FOV 90 degree
	//float Fov = PI/4.0f; // FOV 45 degree
	float Aspect = 640.0f/480.0f;
	float ZFar = 100.0f;
	float ZNear = 1.0f;

	float    h, w, Q;
	w = (float)(1/tan(Fov*0.5f))/Aspect;
	h = (float)1/tan(Fov*0.5f);
	Q = ZFar/(ZFar - ZNear);

   	matrix4x4 MatProj={
		w, 0, 0, 0,
		0, h, 0, 0,
		0, 0, Q, 1,
		0, 0, -Q*ZNear, 0 };
	
	vector3 VecOut = Vec3_Mat4x4_Mul(VecIn, MatProj);

	return 0;
}
	

