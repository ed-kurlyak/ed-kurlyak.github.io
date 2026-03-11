#include <iostream>
#include <vector>
#include <queue>
#include <cmath>

const int N = 5;
const int INF = (int)1e9;
const int dx[] = { -1, 1, 0, 0 };
const int dy[] = { 0, 0, -1, 1 };

struct Node
{
    int x, y, cost;

    bool operator>(const Node& other) const
    {
        return cost > other.cost;
    }
};

// Функция для вычисления эвристики (манхэттенское расстояние)
int heuristic(int x1, int y1, int x2, int y2)
{
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

// A* поиск кратчайшего пути
std::vector<std::pair<int, int>> aStar(int grid[N][N], std::pair<int, int> start, std::pair<int, int> goal)
{
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    std::vector<std::vector<int>> dist(N, std::vector<int>(N, INF));
    std::vector<std::vector<std::pair<int, int>>> parent(N, std::vector<std::pair<int, int>>(N, { -1, -1 }));

    pq.push({ start.first, start.second, 0 });
    dist[start.first][start.second] = 0;

    while (!pq.empty())
    {
        Node current = pq.top();
        pq.pop();

        if (current.x == goal.first && current.y == goal.second)
        {
            break;
        }

        for (int i = 0; i < 4; i++)
        {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (nx >= 0 && nx < N && ny >= 0 && ny < N && grid[nx][ny] == 0)
            {
                int newCost = dist[current.x][current.y] + 1;

                if (newCost < dist[nx][ny])
                {
                    dist[nx][ny] = newCost;
                    parent[nx][ny] = { current.x, current.y };
                    pq.push({ nx, ny, newCost + heuristic(nx, ny, goal.first, goal.second) });
                }
            }
        }
    }

    std::vector<std::pair<int, int>> path;
    std::pair<int, int> p = goal;

    while (p != start)
    {
        path.push_back(p);
        p = parent[p.first][p.second];
    }

    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

void printGridWithPath(int grid[N][N], const std::vector<std::pair<int, int>>& path)
{
    char displayGrid[N][N];

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (grid[i][j] == 1)
            {
                displayGrid[i][j] = '1';
            }
            else
            {
                displayGrid[i][j] = '0';
            }
        }
    }

    for (const auto& [x, y] : path)
    {
        displayGrid[x][y] = '*';
    }

    std::cout << "Map of route:" << std::endl;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            std::cout << displayGrid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int main()
{
    int grid[N][N] = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0}
    };

    std::pair<int, int> start = { 0, 0 };
    std::pair<int, int> goal = { 4, 4 };

    std::vector<std::pair<int, int>> path = aStar(grid, start, goal);

    printGridWithPath(grid, path);

    return 0;
}
