#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<easyx.h>
#include<graphics.h>
#include<mmsystem.h>
#pragma comment (lib,"winmm.lib")


#define ROW 10
#define COL 10
#define minemun 10
#define imgsize 40
IMAGE imags[12];
void loadResource() {
	for (int i = 0; i < 12; i++) {
		char imgpath[50] = { 0 };
		sprintf_s(imgpath, "i\\%d.png",i);
		loadimage(&imags[i], imgpath, imgsize,imgsize);
		
	}

}


void show(int map[][COL]);
void init(int map[][COL]);
void draw(int map[][COL]);
void mouseMsg(ExMessage* msg, int map[][COL]);
void boom(int map[][COL], int row,int col);
int judge(int map[][COL], int row, int col);
void flash(int map[][COL]);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	//creat w
	initgraph(400,400);
	//bgm
	mciSendString("open i\\bgm.mp3 alias bgm", NULL, 0, NULL);
	mciSendString("play bgm", NULL, 0, NULL);
	//mine map
	int map[ROW][COL] = { 0 };
	init(map);
	//show(map);
	//游戏主循环
	while (true) {
		ExMessage msg;
		while (peekmessage(&msg, EM_MOUSE)) {
			switch (msg.message) {
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				mouseMsg(&msg, map);
				int ret = judge(map, msg.y/ imgsize, msg.x / imgsize);
				//flash(map);
				//printf("z,%d\n",ret);
				//printf("%d  %d", msg.y / imgsize, msg.x / imgsize);//测试数据
				if (ret == -1) {
					int select = MessageBox(GetHWnd(), "loser never be winner while gives up", "败者食尘", MB_OKCANCEL);
					if (select == IDOK) {
						init(map);
						//flash(map);
					}
					else {
						exit(0);
					}

					}
					else if (ret == 1) {
					mciSendString("stop bgm", NULL, 0, NULL);
					mciSendString("play i\\win.mp3", NULL, 0, NULL);
					int select = MessageBox(GetHWnd(), "winner should be better and better", "胜者不骄", MB_OKCANCEL);
					if (select == IDOK) {
						init(map);
						//flash(map);
					}
					else {
						exit(0);
					}
					}


					break;
				}
			}
		}

		getchar();
		return 0;
	}


//show没问题
void show(int map[][COL])
{
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			printf("%3d", map[i][k]);
		}
		printf("\n");
	}
}
//init没问题
void init(int map[][COL]) {
	loadResource();
	//10 mine
	srand((unsigned)time(NULL));//random seed
	memset(map, 0, sizeof(int)*ROW*COL);
	for (int i = 0; i < minemun;) {
		int r = rand() % ROW;
		int c = rand() % COL;
		if (map[r][c] == 0) {
			map[r][c] = -1;
			i++;
		}
	}
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			if (map[i][k] == -1) {
				for (int r = i - 1; r <= i + 1; r++) {
					for (int c = k - 1; c <= k + 1; c++) {
						if ((r >= 0 && r < ROW && c >= 0 && c < COL) && map[r][c] != -1) {
							++map[r][c];
						}
					}
				}
			}
		}
	}

	//加密
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			map[i][k] +=20;//19~28
		}
	}
	draw(map);


}
//draw没问题
void draw(int map[][COL]) {
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			if (map[i][k] >= 0 && map[i][k] <= 8) {
				putimage(k * imgsize, i * imgsize, &imags[map[i][k]]);
			}//数字
			else if (map[i][k] == -1) {
				putimage(k * imgsize, i * imgsize, &imags[9]);
			}//雷
			else if (map[i][k] >= 19 && map[i][k] <= 28) {
				putimage(k * imgsize, i * imgsize, &imags[10]);
			}//普通未打开格子
			else if (map[i][k] >= 39) {
				putimage(k * imgsize, i * imgsize, &imags[11]);
			}//标记格子

		}
	}

}
//鼠标操作
void mouseMsg(ExMessage* msg, int map[][COL]) {
	int c = msg->x / imgsize;
	int r = msg->y / imgsize;
	if (r >= 0 && r <= 9 && c >= 0 && c <= 9) {
		if (msg->message == WM_LBUTTONDOWN) {
			if (map[r][c] >= 19 && map[r][c] <= 28) {
				map[r][c] -= 20;
				boom(map, r, c);
			}
			
		}


		else if (msg->message == WM_RBUTTONDOWN) {
			if (map[r][c] >= 19 && map[r][c] <= 28) {
				map[r][c] += 20;

			}
			else if (map[r][c] >= 39) {
				map[r][c] -= 20;


			}
		}draw(map);
	}
	}

void boom(int map[][COL], int row, int col) {
	if (map[row][col] == 0) {
		for (int r = row - 1; r <= row + 1; r++) {
			for (int c = col - 1; c <= col + 1; c++) {
				if ((r >= 0 && r < ROW && c >= 0 && c < COL)
					&& map[r][c] >= 19 && map[r][c] <= 28) {
					map[r][c] -= 20;
					boom(map, r, c);
				}
			}

		}

	}
}
//输赢判断，没问题
int judge(int map[][COL],int row,int col) {
	if (map[row][col] == -1) {
		return -1;
	}
	int cnt = 0;
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			if (map[i][k]>=0 && map[i][k] <= 8) {
				++cnt;
			}
		}
	}
	if ((ROW * COL - minemun) == cnt) {
		return 1;
	}
	return 0;
}
//刷新
void flash(int map[][COL]) {
	system("cls");
	show(map);
}



