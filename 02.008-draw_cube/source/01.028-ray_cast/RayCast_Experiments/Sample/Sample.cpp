#include <stdio.h>
#include <math.h>

double FOV = 10.0 * 3.14159265 / 180.0;
double screenWidth = 10.0;
double playerAngle = 3.14159265 / 4.0f; //45 degree

int playerX = 4; // центр карты
int playerY = 2;

#define MAP_X 5
#define MAP_Y 5

int map[5][5] = {
    {0,0,0,0,1},
    {0,0,0,0,1},
    {0,0,0,0,1},
    {0,0,0,0,1},
    {1,1,1,1,1}
};


int main(void)
{
	for(int x = 0; x < screenWidth; x++)
	{
		double rayAngle = playerAngle - FOV/2.0 + (FOV * x) / screenWidth;

		//направление луча в координатах карты
        double rayDirX = cos(rayAngle);
        double rayDirY = sin(rayAngle);

        double rayX = playerX;
        double rayY = playerY;

        double step = 0.02;
        int hit = 0;

        while(!hit)
        {
			//добавляем к текущей позиции луча малый шаг в направлении луча
			//цель — определить, где луч столкнётся со стеной.
            rayX += rayDirX * step;
            rayY += rayDirY * step;

            int mapX = (int)rayX;
            int mapY = (int)rayY;

            if(mapX < 0 || mapX >= MAP_X || mapY < 0 || mapY >= MAP_Y)
                break;

            if(map[mapY][mapX] == 1)
                hit = 1;
        }

        double dx = rayX - playerX;
		double dy = rayY - playerY;
		double rawDistance = sqrt(dx*dx + dy*dy);
		double distance = rawDistance * cos(rayAngle - playerAngle);

		printf("%d - %f\n", x, distance);

	}

	return 0;
}