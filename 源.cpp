#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#define FILENAME "maze5.txt"		//修改为maze.txt，程序运行时创建迷宫，修改为已存在文件名，求解已存在迷宫
#define INFINITY 50000
FILE *fp;					//全局文件指针
typedef enum{ PATH, WALL,RESULT } ElemTag;  //PATH==0:无障碍,WALL==1:有障碍,RESULT==2:迷宫的解的路径
typedef struct MazeNode
{
	ElemTag tag;		//结点类型
	int location;		//在freeman链码中的位置
}MazeNode;
typedef struct Freeman
{
	int code;		//方向码
	MazeNode* p;	//产生方向码结点的地址
}Freeman;
typedef struct
{
	MazeNode* mp;		//迷宫数组
	int m;				//迷宫的行数
	int n;				//迷宫的列数
	int x1, y1;			//迷宫入口坐标
	int x2, y2;			//迷宫出口坐标
	Freeman* fp;		//迷宫的解
}Maze;
void Create()
{
	//创建迷宫文件
	int m, n, x1, y1, x2, y2, i, j, tag;
	system("cls");
	fp = fopen(FILENAME, "w");			//创建一个文本文件
	if (!fp)
	{
		printf("新建文件失败!\n");
		exit(0);
	}
	printf("输入迷宫的行数和列数(含边界):");
	scanf("%d %d", &m, &n);	
	printf("输入迷宫的入口坐标:");
	while (1)
	{
		scanf("%d %d", &x1, &y1);
		if (x1 > 0 && x1 < m && y1 > 0 && y1 < n)
			break;
		else
			printf("入口坐标输入错误，请重新输入!\n");
	}
	printf("输入迷宫的出口坐标:");
	while (1)
	{
		scanf("%d %d", &x2, &y2);
		if (x2 > 0 && x2 <m && y2 > 0 && y2 < n )
			break;
		else
			printf("出口坐标输入错误，请重新输入!\n");
	}
	//写文件
	fprintf(fp, "%d %d\n", m, n);
	fprintf(fp, "%d %d\n", x1, y1);
	fprintf(fp, "%d %d\n", x2, y2);
	printf("输入不含边界迷宫的示意图(0:无障碍，1:有障碍)\n");
	for (i = 0; i < m; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (i == 0 || i == m - 1 || j == 0 || j == n - 1)
				fprintf(fp, "1 ");
			else
			{
				scanf("%d", &tag);
				if (tag == 0 || tag == 1)
				{
					if (i == x1 && j == y1 || i == x2&& j == y2)
						tag = 0;
					fprintf(fp, "%d ", tag);
				}
				else
				{
					printf("输入有误!\n");
					j--;
				}
			}
		}
		fprintf(fp, "\n");
	}
	//写文件
	fclose(fp);
	fp = fopen(FILENAME, "r");
}//Create
void Open()
{
	//如果迷宫文件存在，令pf指向迷宫文件，否则创建迷宫文件
	fp = fopen(FILENAME, "r");
	if (!fp)
	{
		printf("迷宫文件不存在,创建迷宫文件!\n");
		system("pause");
		Create();
	}
}//Open

void InitMaze(Maze &M)
{
//根据迷宫文件构造相应的迷宫数组M
	int i, j,t;
	Open();
	fscanf(fp, "%d %d", &M.m, &M.n);
	M.mp = (MazeNode*)malloc(sizeof(MazeNode) * M.m * M.n);
	M.fp = (Freeman*)malloc(sizeof(Freeman) * M.m * M.n * 2);
	if (!M.mp || !M.fp)
	{
		printf("内存分配失败!");
		exit(0);
	}
	fscanf(fp, "%d %d", &M.x1, &M.y1);
	fscanf(fp, "%d %d", &M.x2, &M.y2);

	for (i = 0; i<M.m; i++)
		for (j = 0; j<M.n; j++)
		{
			fscanf(fp, "%d", &t);
			switch (t)
			{
			case 0:(M.mp + i * M.n + j)->tag = PATH;break;
			case 1:(M.mp + i * M.n + j)->tag = WALL; break;

			}
			(M.mp + i * M.n + j)->location = INFINITY;	//freeman链码位置置为INFINITY
		}
	fclose(fp);
}//InitMaze

void DestroyMaze(Maze &M)
{
//销毁迷宫数组M
	free(M.mp);
	free(M.fp);
	M.m = M.n = 0;
	M.x1 = M.x2 = M.y1 = M.y2 = -1;
}//DestroyMaze

void SolveMaze(Maze &M)
{
	//求解迷宫，求解结果以freeman链码形式储存
	int x, y, d, index, t1, t2;
	x = M.x1; y = M.y1;				//入口坐标
	d = 0;					//确定探测位置
	index = 2;						//freeman链码下标
	while (x != M.x2 || y != M.y2 )
	{
		t1 = x;
		t2 = y;
		switch (d)
		{					//探测坐标变化
		case 0:t1 += 1; break;
		case 1:t1 += 1; t2 += 1; break;
		case 2:t2 += 1; break;
		case 3:t1 -= 1; t2 += 1; break;
		case 4:t1 -= 1; break;
		case 5:t1 -= 1; t2 -= 1; break;
		case 6:t2 -= 1; break;
		case 7:t1 += 1; t2 -= 1; break;
		}
		if ((M.mp + t1 * M.n + t2)->tag == PATH)
		{
			
			if ((M.mp + t1 * M.n + t2)->location == INFINITY)		//判断该位置是否访问过
				(M.mp + t1 * M.n + t2)->location = index;			//记录freeman链码数组下标
			M.fp[index].code = d;	//加入freeman链码数组
			M.fp[index].p = M.mp + t1 * M.n + t2;
			index++;
			d = (d + 6) % 8;		//探测成功下一个探测位置为向右旋转90度
			x = t1;
			y = t2;
		}
		else
			d = (d + 1) % 8;			//探测失败下一个探测位置为向左旋转45度
	}
	M.fp[0].code = index - 1;
}//SolveMaze

void OptimizeFreeman(Maze &M)
{
//优化路线
	int t, d, min,dmin,flag;
	MazeNode *p , *pt, *pmin=NULL;
	p = M.mp + M.x2 * M.n + M.y2;	//指向终点
	t = M.fp[0].code;	//指向freeman链码最后一位
	//从迷宫终点出发
	while (t > 0)
	{
		flag = 0;
		if (M.fp[t].code != INFINITY)
		{
			min = INFINITY;
			for (d = 0; d < 8; d++)		//找到与该位置邻接的最早在freeman链码出现的位置
			{
				pt = p;
				switch (d)				//八个方向探测
				{
				case 0:pt = pt + M.n; break;
				case 1:pt = pt + M.n + 1; break;
				case 2:pt = pt + 1; break;
				case 3:pt = pt - M.n + 1; break;
				case 4:pt = pt - M.n; break;
				case 5:pt = pt - M.n - 1; break;
				case 6:pt = pt - 1; break;
				case 7:pt = pt + M.n - 1; break;
				}
				if (pt->location < min)		
				{
					min = pt->location;		//记录最早在freeman链码出现的位置
					pmin = pt;				//记录该点在迷宫数组的位置
					dmin = d;				//记录方向码
				}
			}
			if (min != t - 1)			//如果最早出现位置不等于freeman链码前一个位置，说明中间有岔路
			{
				for (d = t; d > min + 1; d--)		//剔除岔路
					M.fp[d].code = INFINITY;
				if (d != t)		
				{
					M.fp[min + 1].code = (dmin + 4) % 8;	//修改该位置方向码
					t = min;		//freeman从min的位置开始继续往前
					p = pmin;		//修改下一个位置
					flag = 1;		//表示有岔路，t的值改变
				}
			}			
		}
		if (!flag)
		{
			t--;
			p = pmin;
		}
	}
}//OptimizeFreeman
void DisplayResult_T(Maze M)
{
//根据freeman链码，以坐标形式输出迷宫求解结果。
	int x, y, t, d,flag = 0,count = 1;
	x = M.x1;
	y = M.y1;
	printf("迷宫的解:\n");
	(M.mp + x * M.n + y)->tag = RESULT;
	for (t = 1; t <= M.fp[0].code; t++)
	{
		flag = 0;
		d = M.fp[t].code;
		switch (d)			//坐标变换
		{
		case 0:x += 1; break;
		case 1:x += 1; y += 1; break;
		case 2:y += 1; break;
		case 3:x -= 1; y += 1; break;
		case 4:x -= 1; break;
		case 5:x -= 1; y -= 1; break;
		case 6:y -= 1; break;
		case 7:x += 1; y -= 1; break;
		case INFINITY:flag = 1; break;
		}
		if (flag == 0)
		{
			printf("(%d,%d)", x, y);
			(M.mp + x * M.n + y)->tag = RESULT;
			count++;
			if (count % 8 == 0)
				printf("\n");
		}
		
	}
}//DisplayResult
void DisplayResult_G(Maze M)
{
	//根据迷宫数组元素的tag位，以图形形式输出迷宫求解结果。
	int m, n;
	printf("\n");
	for (m = 0; m < M.m; m++)
	{
		for (n = 0; n < M.n; n++)
		{
			switch ((M.mp + m * M.n + n)->tag)
			{
			case PATH:	printf("  "); break;
			case WALL:	printf("■"); break;
			case RESULT: printf(" o"); break;
			}
		}
		printf("\n");
	}
}//DisplayResult
int main()
{
	Maze M;
	InitMaze(M);
	SolveMaze(M);
	OptimizeFreeman(M);
	DisplayResult_T(M);
	DisplayResult_G(M);
	DestroyMaze(M);
	system("pause");
	return 0;
}
