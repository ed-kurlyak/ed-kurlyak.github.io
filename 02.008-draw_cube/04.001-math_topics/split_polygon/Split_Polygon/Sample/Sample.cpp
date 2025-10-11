#include <windows.h>

class vertex
{
    public:
        vertex(float ix = 0, float iy = 0, float iz = 0);
        ~vertex();

        float x;
        float y;
        float z;

	vertex &operator = (vertex &v);
	vertex operator - (vertex v);
	vertex operator * (float v);
	vertex operator + (vertex v);
	float operator | (vertex v);

};

vertex::vertex(float ix, float iy, float iz)
:
    x(ix),
    y(iy),
    z(iz)
{
}

vertex::~vertex()
{
}


float vertex::operator | (vertex v)
{
	float ret = x * v.x + y * v.y + z * v.z;
	return ret;
};


vertex &vertex::operator = (vertex &v)
{
	x = v.x;
	y = v.y;
	z = v.z;

	//tv = v.tv;
	//tu = v.tu;

	return *this;
}

vertex vertex::operator - (vertex v)
{
	vertex t;

	t.x = x - v.x;
	t.y = y - v.y;
	t.z = z - v.z;

	//t.tu = tu - v.tu;
	//t.tv = tv - v.tv;

	return t;
};


vertex vertex::operator + (vertex v)
{
	vertex t;

	t.x = x + v.x;
	t.y = y + v.y;
	t.z = z + v.z;

	//t.tu = tu + v.tu;
	//t.tv = tv + v.tv;

	return t;
};

vertex vertex::operator * (float v)
{
	vertex t;

	t.x = x * v;
	t.y = y * v;
	t.z = z * v;

	//t.tu = tu * v;
	//t.tv = tv * v;

	return t;
};



struct plane
{
	float a, b, c, d;

	float Classify_Point (vertex eye);
	vertex Normal();
};

float plane::Classify_Point (vertex eye)
{
	float p=a*eye.x + b*eye.y + c*eye.z + d;
	return p;
}

vertex plane::Normal()
{
	vertex t;

	t.x = a;
	t.y = b;
	t.z = c;

	return t;

}




class polygon
{
    public:
        polygon();
        ~polygon();

        vertex Vertex[3];
};

polygon::polygon()
{
}

polygon::~polygon()
{
}

void Split_Polygon (polygon *poly, plane *part, polygon *&front, polygon *&back, int &cf, int &cb)
{
   //int   count = poly->NumVertices (),
   int   count = 3;
   int front_c = 0, back_c = 0;
   vertex ptA, ptB, front_set[4], back_set[4];
   float	sideA, sideB;

   ptA = poly->Vertex[count - 1];

   sideA = part->Classify_Point (ptA);

   float epsilon = 0.001f;

   if(sideA < epsilon && sideA > -epsilon)
	   sideA = 0.0f;
   
   for (short i = -1; ++i < count;)
   {
      ptB = poly->Vertex[i];

      sideB = part->Classify_Point (ptB);

	  if(sideB < epsilon && sideB > -epsilon)
	   sideB = 0.0f;

      if (sideB > 0)
      {
         if (sideA < 0)
         {
            // compute the intersection point of the line
            // from point A to point B with the partition
            // plane. This is a simple ray-plane intersection.
            vertex v = ptB - ptA;

			float   sect = - part->Classify_Point (ptA) / (part->Normal() | v);

			front_set[front_c++] = back_set[back_c++] = ptA + (v * sect);
         }
		 
		 front_set[front_c++] = ptB;
		 
      }
      else if (sideB < 0)
      {
         if (sideA > 0)
         {
            // compute the intersection point of the line
            // from point A to point B with the partition
            // plane. This is a simple ray-plane intersection.
            vertex v = ptB - ptA;
		
			float   sect = - part->Classify_Point (ptA) / (part->Normal() | v);
		
			front_set[front_c++] = back_set[back_c++] = ptA + (v * sect);
		
         }

		 back_set[back_c++] = ptB;
		 
      }
      else
	  {
		  front_set[front_c++] = back_set[back_c++] = ptB;
			
	  }
  
	  ptA = ptB;
      sideA = sideB;
   }

   if(front_c == 4) //in return we got 2 triangles
   {
		front = new polygon[2];
		ZeroMemory(front, 2 * sizeof(polygon));
   
		//first tri
		front[0].Vertex[0] = front_set[0];
		front[0].Vertex[1] = front_set[1];
		front[0].Vertex[2] = front_set[2];

		//second tri
		front[1].Vertex[0] = front_set[0];
		front[1].Vertex[1] = front_set[2];
		front[1].Vertex[2] = front_set[3];
  
		//output tri count
		cf = 2;

   }
   else if(front_c == 3) //in return we got 1 triangle
   {
		front = new polygon[1];
		ZeroMemory(front, 1 * sizeof(polygon));

		front[0].Vertex[0] = front_set[0];
		front[0].Vertex[1] = front_set[1];
		front[0].Vertex[2] = front_set[2];
		
		//output tri count
		cf = 1;

   }
   else
   {
	   //output tri count
	   cf = 0;
   }

   if(back_c == 4) //in return we got 2 triangles
   {
		back = new polygon[2];
		ZeroMemory(back, 2 * sizeof(polygon));

		//first tri
		back[0].Vertex[0] = back_set[0];
		back[0].Vertex[1] = back_set[1];
		back[0].Vertex[2] = back_set[2];
		
		//second tri
		back[1].Vertex[0] = back_set[0];
		back[1].Vertex[1] = back_set[2];
		back[1].Vertex[2] = back_set[3];

		//output tri count
		cb = 2;
   }
   else if(back_c == 3 ) //in return we got 1 triangle
   {
		back = new polygon[1];
		ZeroMemory(back, 1 * sizeof(polygon));

		back[0].Vertex[0] = back_set[0];
		back[0].Vertex[1] = back_set[1];
		back[0].Vertex[2] = back_set[2];
		
		//output tri count
		cb = 1;
		
   }
   else
   {
	   //output tri count
		cb = 0;
   }
}




int main (void)
{
	polygon *poly = new polygon;
	
	poly->Vertex[0].x = -8.0;
	poly->Vertex[0].y = 0.0;
	poly->Vertex[0].z = 0.0;
	
	poly->Vertex[1].x = 8.0;
	poly->Vertex[1].y = 8.0;
	poly->Vertex[1].z = 0.0;
	
	poly->Vertex[2].x = 8.0;
	poly->Vertex[2].y = 0.0;
	poly->Vertex[2].z = 0.0;

	plane *part = new plane;

	part->a = 1.0;
	part->b = 0.0;
	part->c = 0.0;
	part->d = 0.0;

	polygon *front = NULL;
	polygon *back = NULL;
	int cf, cb;
	
	Split_Polygon (poly, part, front, back, cf, cb);

	//на выходе 
	//cf = 1 или 2 треугольника спереди плоскости
	//cb = 1 или 2 треугольника сзади плоскости

	//front[0] - первый треугольник спереди плоскоти
	//front[1] - второй треугольник спереди плоскости
	//back[0] - первый треуольник сзади плоскости
	//back[1] - второй треугольник сзади плоскости

	return 0;
}