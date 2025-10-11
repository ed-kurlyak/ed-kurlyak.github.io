#include <iostream>
#include <queue>
#include <vector>

using namespace std;

const int N = 5;  // Размерность карты (5x5)
const int M = 5;

int grid[N][M] =
{
    {0, 0, 0, 1, 0},
    {0, 1, 0, 1, 0},
    {0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0}
};

struct Point
{
    int x, y;
};

bool is_valid(int x, int y)
{
    return x >= 0 && x < N&& y >= 0 && y < M && grid[x][y] == 0;
}

vector<Point> get_neighbors(Point p)
{
    vector<Point> neighbors;
    int dx[] = { -1, 1, 0, 0 };
    int dy[] = { 0, 0, -1, 1 };

    for (int i = 0; i < 4; i++)
    {
        int nx = p.x + dx[i];
        int ny = p.y + dy[i];
        if (is_valid(nx, ny))
        {
            neighbors.push_back({ nx, ny });
        }
    }

    return neighbors;
}

vector<Point> wave_algorithm(Point start, Point end)
{
    vector<vector<int>> distance(N, vector<int>(M, -1));
    queue<Point> q;
    vector<Point> path;

    distance[start.x][start.y] = 0;
    q.push(start);

    while (!q.empty())
    {
        Point p = q.front();
        q.pop();

        if (p.x == end.x && p.y == end.y)
        {
            break;
        }

        for (const Point& neighbor : get_neighbors(p))
        {
            if (distance[neighbor.x][neighbor.y] == -1)
            {
                distance[neighbor.x][neighbor.y] = distance[p.x][p.y] + 1;
                q.push(neighbor);
            }
        }
    }

    if (distance[end.x][end.y] == -1)
    {
        return path;  // Путь не найден
    }

    Point p = end;
    while (p.x != start.x || p.y != start.y)
    {
        path.push_back(p);

        for (const Point& neighbor : get_neighbors(p))
        {
            if (distance[neighbor.x][neighbor.y] == distance[p.x][p.y] - 1)
            {
                p = neighbor;
                break;
            }
        }
    }

    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

void print_map(const vector<Point>& path)
{
    vector<vector<char>> map(N, vector<char>(M, '1'));

    // Заполняем карту проходимыми клетками
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (grid[i][j] == 0)
            {
                map[i][j] = '0';
            }
        }
    }

    // Отмечаем путь на карте
    for (const Point& p : path)
    {
        map[p.x][p.y] = '*';
    }

    // Выводим карту
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            cout << map[i][j] << " ";
        }
        cout << endl;
    }
}

int main()
{
    //Point start = { 0, 0 };
    //Point end = { 4, 4 };

    Point start = { 0, 4 };
    Point end = { 4, 0 };

    vector<Point> path = wave_algorithm(start, end);
    
    if (!path.empty())
    {
        cout << "Way is found, map with route:" << endl;
        print_map(path);
    }
    else
    {
        cout << "Way is not found" << endl;
        // Выводим карту без пути
        print_map({});
    }

    return 0;
}
