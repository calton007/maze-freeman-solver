#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define DEFAULT_FILENAME "maze5.txt"
#define INFINITY 50000

typedef enum { PATH, WALL, RESULT } ElemTag;

typedef struct MazeNode
{
    ElemTag tag;
    int location;
} MazeNode;

typedef struct Freeman
{
    int code;
    MazeNode* p;
} Freeman;

typedef struct
{
    MazeNode* mp;
    int m;
    int n;
    int x1, y1;
    int x2, y2;
    Freeman* fp;
} Maze;

static const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
static const int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

static int InMaze(const Maze& M, int x, int y)
{
    return x >= 0 && x < M.m && y >= 0 && y < M.n;
}

static MazeNode* NodeAt(const Maze& M, int x, int y)
{
    return M.mp + x * M.n + y;
}

static void DestroyMaze(Maze& M)
{
    free(M.mp);
    free(M.fp);
    M.mp = NULL;
    M.fp = NULL;
    M.m = M.n = 0;
    M.x1 = M.x2 = M.y1 = M.y2 = -1;
}

static int InitMaze(Maze& M, const char* filename)
{
    FILE* file;
    int i, j, t;

    M.mp = NULL;
    M.fp = NULL;
    M.m = M.n = 0;
    M.x1 = M.y1 = M.x2 = M.y2 = -1;

    file = fopen(filename, "r");
    if (!file)
    {
        printf("迷宫文件打开失败: %s\n", filename);
        return 0;
    }

    if (fscanf(file, "%d %d", &M.m, &M.n) != 2 || M.m <= 2 || M.n <= 2)
    {
        printf("迷宫文件格式错误: 行列非法\n");
        fclose(file);
        return 0;
    }

    if (fscanf(file, "%d %d", &M.x1, &M.y1) != 2)
    {
        printf("迷宫文件格式错误: 入口缺失\n");
        fclose(file);
        return 0;
    }

    if (fscanf(file, "%d %d", &M.x2, &M.y2) != 2)
    {
        printf("迷宫文件格式错误: 出口缺失\n");
        fclose(file);
        return 0;
    }

    if (!InMaze(M, M.x1, M.y1) || M.x1 == 0 || M.x1 == M.m - 1 || M.y1 == 0 || M.y1 == M.n - 1)
    {
        printf("入口坐标非法\n");
        fclose(file);
        return 0;
    }

    if (!InMaze(M, M.x2, M.y2) || M.x2 == 0 || M.x2 == M.m - 1 || M.y2 == 0 || M.y2 == M.n - 1)
    {
        printf("出口坐标非法\n");
        fclose(file);
        return 0;
    }

    M.mp = (MazeNode*)malloc(sizeof(MazeNode) * M.m * M.n);
    M.fp = (Freeman*)malloc(sizeof(Freeman) * (M.m * M.n + 1));
    if (!M.mp || !M.fp)
    {
        printf("内存分配失败\n");
        fclose(file);
        DestroyMaze(M);
        return 0;
    }

    for (i = 0; i < M.m * M.n + 1; i++)
    {
        M.fp[i].code = INFINITY;
        M.fp[i].p = NULL;
    }

    for (i = 0; i < M.m; i++)
    {
        for (j = 0; j < M.n; j++)
        {
            if (fscanf(file, "%d", &t) != 1)
            {
                printf("迷宫文件格式错误: 格子数量不足\n");
                fclose(file);
                DestroyMaze(M);
                return 0;
            }

            if (t != 0 && t != 1)
            {
                printf("迷宫文件格式错误: 格子只能是0或1\n");
                fclose(file);
                DestroyMaze(M);
                return 0;
            }

            NodeAt(M, i, j)->tag = (t == 0) ? PATH : WALL;
            NodeAt(M, i, j)->location = INFINITY;
        }
    }

    fclose(file);

    if (NodeAt(M, M.x1, M.y1)->tag != PATH)
    {
        printf("入口不是通路\n");
        DestroyMaze(M);
        return 0;
    }

    if (NodeAt(M, M.x2, M.y2)->tag != PATH)
    {
        printf("出口不是通路\n");
        DestroyMaze(M);
        return 0;
    }

    return 1;
}

static int SolveMaze(Maze& M)
{
    int* visited;
    int* nextDir;
    int x, y, nx, ny, d, top, found;

    visited = (int*)calloc(M.m * M.n, sizeof(int));
    nextDir = (int*)calloc(M.m * M.n, sizeof(int));
    if (!visited || !nextDir)
    {
        printf("内存分配失败\n");
        free(visited);
        free(nextDir);
        return 0;
    }

    x = M.x1;
    y = M.y1;
    top = 0;
    found = 0;
    visited[x * M.n + y] = 1;
    NodeAt(M, x, y)->location = 0;

    while (top >= 0)
    {
        if (x == M.x2 && y == M.y2)
        {
            found = 1;
            break;
        }

        d = nextDir[x * M.n + y];
        if (d >= 8)
        {
            if (top == 0)
                break;

            d = M.fp[top].code;
            NodeAt(M, x, y)->location = INFINITY;
            visited[x * M.n + y] = 0;
            top--;
            x -= dx[d];
            y -= dy[d];
            continue;
        }

        nextDir[x * M.n + y]++;
        nx = x + dx[d];
        ny = y + dy[d];
        if (!InMaze(M, nx, ny))
            continue;
        if (NodeAt(M, nx, ny)->tag != PATH)
            continue;
        if (visited[nx * M.n + ny])
            continue;

        top++;
        M.fp[top].code = d;
        M.fp[top].p = NodeAt(M, nx, ny);
        NodeAt(M, nx, ny)->location = top;
        visited[nx * M.n + ny] = 1;
        x = nx;
        y = ny;
    }

    free(visited);
    free(nextDir);

    if (!found)
    {
        M.fp[0].code = 0;
        return 0;
    }

    M.fp[0].code = top;
    return 1;
}

static int OptimizeFreeman(Maze& M)
{
    int t, d, min, dmin, flag;
    MazeNode* p;
    MazeNode* pt;
    MazeNode* pmin;

    if (M.fp[0].code < 0)
        return 0;

    p = NodeAt(M, M.x2, M.y2);
    t = M.fp[0].code;

    while (t > 0)
    {
        flag = 0;
        pmin = NULL;
        dmin = INFINITY;

        if (M.fp[t].code != INFINITY)
        {
            min = INFINITY;
            for (d = 0; d < 8; d++)
            {
                pt = p + dx[d] * M.n + dy[d];
                if (pt < M.mp || pt >= M.mp + M.m * M.n)
                    continue;

                if (pt->location < min)
                {
                    min = pt->location;
                    pmin = pt;
                    dmin = d;
                }
            }

            if (!pmin)
                return 0;

            if (min != t - 1)
            {
                for (d = t; d > min + 1; d--)
                    M.fp[d].code = INFINITY;

                M.fp[min + 1].code = (dmin + 4) % 8;
                t = min;
                p = pmin;
                flag = 1;
            }
        }

        if (!flag)
        {
            p = pmin;
            t--;
        }
    }

    return 1;
}

static void DisplayResult_T(Maze& M)
{
    int x, y, t, d, flag, count;

    x = M.x1;
    y = M.y1;
    flag = 0;
    count = 1;

    printf("迷宫的解:\n");
    printf("(%d,%d)", x, y);
    NodeAt(M, x, y)->tag = RESULT;

    for (t = 1; t <= M.fp[0].code; t++)
    {
        d = M.fp[t].code;
        if (d == INFINITY)
            continue;

        if (d < 0 || d >= 8)
        {
            flag = 1;
            break;
        }

        x += dx[d];
        y += dy[d];
        if (!InMaze(M, x, y))
        {
            flag = 1;
            break;
        }

        printf("(%d,%d)", x, y);
        NodeAt(M, x, y)->tag = RESULT;
        count++;
        if (count % 8 == 0)
            printf("\n");
    }

    if (flag)
        printf("\n路径数据错误");
}

static void DisplayResult_G(const Maze& M)
{
    int m, n;

    printf("\n");
    for (m = 0; m < M.m; m++)
    {
        for (n = 0; n < M.n; n++)
        {
            switch (NodeAt(M, m, n)->tag)
            {
            case PATH:
                printf("  ");
                break;
            case WALL:
                printf("[]");
                break;
            case RESULT:
                printf(" o");
                break;
            }
        }
        printf("\n");
    }
}

static void WaitBeforeExitIfOwnConsole()
{
#ifdef _WIN32
    DWORD processList[2];
    DWORD processCount = GetConsoleProcessList(processList, 2);
    if (processCount == 1)
    {
        printf("\n按 Enter 键退出...");
        getchar();
    }
#endif
}

static void InitConsoleEncoding()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

int main(int argc, char* argv[])
{
    Maze M;
    const char* filename;

    InitConsoleEncoding();

    filename = (argc > 1) ? argv[1] : DEFAULT_FILENAME;

    if (!InitMaze(M, filename))
    {
        WaitBeforeExitIfOwnConsole();
        return 1;
    }

    if (!SolveMaze(M))
    {
        printf("迷宫无解\n");
        DestroyMaze(M);
        WaitBeforeExitIfOwnConsole();
        return 1;
    }

    if (!OptimizeFreeman(M))
    {
        printf("路径优化失败\n");
        DestroyMaze(M);
        WaitBeforeExitIfOwnConsole();
        return 1;
    }

    DisplayResult_T(M);
    DisplayResult_G(M);
    DestroyMaze(M);
    WaitBeforeExitIfOwnConsole();
    return 0;
}
