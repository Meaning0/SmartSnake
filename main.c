#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 60

/*
作者：The0ne
QQ：1393822176
Github：https://github.com/Meaning0
此程序为利用广度优先搜索算法实现的自动寻路贪吃蛇，
该程序没有任何卵用，仅供学习 :P
*/

typedef struct Body {
	// 蛇的身子
	int x, y;
	char stat;
}Body;

typedef struct Snake {
	// 蛇
	int length;
	Body* bodys[MAX];
}Snake;

typedef struct Flag {
	// 标志，蛇该转弯的位置
	int x, y;
	char stat;
}Flag;

typedef struct Node {
	// 用于广度优先
	int visited;
	int x, y;
	struct Node* parent;
}Node;

typedef struct Queue {
	// 整广度优先算法的队列
	Node* q[MAX*MAX];
	int tail;
	int head;
}Queue;

void initSpace(char (*space)[MAX]){
	// 初始化整个空间
	int i, j;
	for(i=0;i<MAX;i++){
		for(j=0;j<MAX;j++){
			if(i==0 || i==MAX-1){
				*(*(space+i)+j) = '#';
			}else if(j==0){
				*(*(space+i)+j) = '#';
			}else if(j==MAX-1){
				*(*(space+i)+j) = '#';
			}else{
				*(*(space+i)+j) = ' ';
			}
			//*(*(space+i)+j) = '#';
		}
	}
}

void initSnake(Snake* snake){
	// 初始化整条蛇
	int i;
	snake->length = 5;
	// 初始化蛇身
	for(i=0;i<=snake->length;i++){
		Body* body = (Body*)malloc(sizeof(Body));
		snake->bodys[i] = body;
	}
	// 初始化蛇的状态
	for(i=0;i<=snake->length;i++){
		snake->bodys[i]->x = 6;
		snake->bodys[i]->y = 6+i;
		snake->bodys[i]->stat = 'U';
	}
}

void initFlag(Flag* flags[MAX]){
	// 初始化标志
	int i;
	for(i=0;i<MAX*3;i++){
		*(flags+i) = (Flag*)malloc(sizeof(Flag));
	}
}

void moveUP(char (*space)[MAX], Snake* snake, int i){
	// 向上移动
	*(*(space+snake->bodys[i]->y)+snake->bodys[i]->x) = ' ';
	snake->bodys[i]->y--;
}

void moveRight(char (*space)[MAX], Snake* snake, int i){
	*(*(space+snake->bodys[i]->y)+snake->bodys[i]->x) = ' ';
	snake->bodys[i]->x++;
}

void moveLeft(char (*space)[MAX], Snake* snake, int i){
	*(*(space+snake->bodys[i]->y)+snake->bodys[i]->x) = ' ';
	snake->bodys[i]->x--;
}

void moveDown(char (*space)[MAX], Snake* snake, int i){
	*(*(space+snake->bodys[i]->y)+snake->bodys[i]->x) = ' ';
	snake->bodys[i]->y++;
}

void moveSnake(char (*space)[MAX], Snake* snake){
	// 移动蛇
	int i;
	for(i=0;i<=snake->length;i++){
		if(snake->bodys[i]->stat=='U'){
			moveUP(space, snake, i);
		}else if(snake->bodys[i]->stat=='D'){
			moveDown(space, snake, i);
		}else if(snake->bodys[i]->stat=='R'){
			moveRight(space, snake, i);
		}else if(snake->bodys[i]->stat=='L'){
			moveLeft(space, snake, i);
		}
	}
}

void lainBlessMe(Flag* flags[MAX*3], Snake* snake, char (*space)[MAX]){
	// 核心代码，检测身体是否倒转弯点，并赋予这个点的状态（lain保佑我）
	int i, j;
	for(i=0;i<=snake->length;i++){
		for(j=0;j<MAX;j++){
			if(flags[j]->x==snake->bodys[i]->x && flags[j]->y==snake->bodys[i]->y){
				snake->bodys[i]->stat = flags[j]->stat;
			}
		}
	}
	moveSnake(space, snake);
}

void renderSnake(char (*space)[MAX], Snake* snake){
	// 渲染整条蛇
	int i;
	for(i=0;i<=snake->length;i++){
		*(*(space+snake->bodys[i]->y)+snake->bodys[i]->x) = '#';
	}
}

void show(char (*space)[MAX]){
	// 显示整个空间
	int i, j;
	for(i=0;i<MAX;i++){
		for(j=0;j<MAX;j++){
			printf("%c", *(*(space+i)+j));
		}
		printf("\n");
	}
}

void thinkover(Node* nodes[MAX*MAX], Snake* snake, Flag* flags[MAX*MAX], int flag){
	int i;
	for(i=0;i<flag-1;i++){
		if(((nodes[i]->x)<(nodes[i+1]->x)) && snake->bodys[0]->stat!='R'){
			flags[i]->x = nodes[i]->x;
			flags[i]->y = nodes[i]->y;
			flags[i]->stat = 'R';
		}else if(((nodes[i]->x)>(nodes[i+1]->x)) && snake->bodys[0]->stat!='L'){
			flags[i]->x = nodes[i]->x;
			flags[i]->y = nodes[i]->y;
			flags[i]->stat = 'L';
		}else if(((nodes[i]->y)<(nodes[i+1]->y)) && snake->bodys[0]->stat!='D'){
			flags[i]->x = nodes[i]->x;
			flags[i]->y = nodes[i]->y;
			flags[i]->stat = 'D';
		}else if(((nodes[i]->y)>(nodes[i+1]->y)) && snake->bodys[0]->stat!='U'){
			flags[i]->x = nodes[i]->x;
			flags[i]->y = nodes[i]->y;
			flags[i]->stat = 'U';
		}
	}
}

Node* lainBlessMeAgain(int goal_x, int goal_y, int x, int y, Flag* flags[MAX*MAX], Snake* snake){
	// 广度优先实现自动寻路
	int used[MAX][MAX];
	int i,j;
	// 初始化队列
	Queue* queue = (Queue*)malloc(sizeof(Queue));
	queue->head = 0,
	queue->tail = 0;
	// 初始化起点节点
	Node* node = (Node*)malloc(sizeof(Node));
	node->x = x;
	node->y = y;
	node->parent = NULL;
	queue->q[queue->tail++] = node;
	// 记录步书
	int flag = 0;
	
	// 依照上右下左遍历
	int px[4] = {0, 1, 0, -1};
	int py[4] = {-1, 0, 1, 0};

	// 初始化访问访问过了的节点
	for(i=0;i<MAX;i++){
		for(j=0;j<MAX;j++){
			used[i][j] = 1;
		}
	}

	used[y][x] = 0;

	while(queue->head != queue->tail){
		// 广度优先， 这里node变量的命名顺序有问题，调试的时候整得，由于太多所以不改了
		Node* node_2 = queue->q[queue->head++];
		if(node_2->x == goal_x && node_2->y == goal_y){
			// 如果到了终点
			Node* end = node_2;
			while(node_2->parent != NULL){
				node_2 = node_2->parent;
				flag++;
			}
			Node* nodes[++flag];
			for(i=0;i<flag;i++){
				nodes[flag-i-1] = end;
				end = end->parent;
			}
			for(i=0;i<flag;i++){
				printf("(%d %d)\n", nodes[i]->x, nodes[i]->y);
			}
			// 演算结束,开始走路 XD
			thinkover(nodes, snake, flags, flag);
		}
		//printf("%d %d\n", node_2->x, node_2->y);
		for(i=0;i<4;i++){
			Node* node_1 = (Node*)malloc(sizeof(Node));
			node_1->x = node_2->x+px[i];
			node_1->y = node_2->y+py[i];
			node_1->parent = node_2;
			//printf("%d %d\n", node_1->x, node_1->y);
			if((node_1->x)>0 && (node_1->x)<MAX && (node_1->y)>0 && (node_1->y)<MAX && used[node_1->y][node_1->x]!=0){
			//	printf("%d %d\n", node_1->x, node_1->y);
				used[node_1->y][node_1->x] = 0;
				queue->q[queue->tail++] = node_1;
			}
		}
	}
}

int main(){
	// 初始化工作
	//printf("debug");
	char space[MAX][MAX];
	char fakespace[MAX][MAX];
	Snake * snake = (Snake*)malloc(sizeof(Snake));
	Flag* flags[MAX*3];
	int flag_flag = 0;
	//printf("debug");

	initSpace(space);
	initSpace(fakespace);
	initSnake(snake);
	initFlag(flags);
	//renderSnake(space, snake);
	//show(space);
	//printf("\n");
	//flags[0]->x = 6;
	//flags[0]->y = 3;
	//flags[0]->stat = 'D';

	//flags[1]->x = 9;
	//flags[1]->y = 15;
	//flags[1]->stat = 'D';
	int flag = 1;
	int x,y;

	while(flag){
		initSnake(snake);
		initSpace(space);
		printf("Please enter X: ");
		scanf("%d", &x);
		printf("Please enter Y: ");
		scanf("%d", &y);
		lainBlessMeAgain(x, y, 6, 6, flags, snake);
		//printf("HHHH");
		//renderSnake(space, snake);
		//show(space);
		//printf("%d %d\n", snake->bodys[0]->x, snake->bodys[0]->y);
		space[y][x] = 'O';
	
		while(1){
			if (snake->bodys[0]->x==x && snake->bodys[0]->y==y){
				break;
			}
			lainBlessMe(flags, snake, space);
			//lainBlessMeAgain(flags, snake, space, fakespace, flag_flag, 30, 40);
			renderSnake(space, snake);
			show(space);
			system("clear");
			sleep(1);
		}
		printf("Do you want to play again?(1/0):");
		scanf("%d", &flag);
	}
	printf("\n");
	return 0;
}
