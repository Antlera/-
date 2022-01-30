#pragma once
#include<conio.h>
#include<graphics.h>
#include<iostream>
#include<thread>

#include"Timer.h"
#include "Character.h"
#include "Scene.h"
using namespace std;

#define WIDTH 1280
#define HEIGHT 1024

#define MAXVIEW 40
#define MINVIEW 3



class GamePanel
{
public:
	enum GameStatus {
		initial = 1, gaming, gameover, pause,save
	};//游戏状态变量
	GamePanel();
	~GamePanel();
	void startup();//初始化游戏
	void updateWithInput();//与输入相关的更新
	void updateWithoutInput();//与输入无关的更新
	void actWithScene();  //场景互动判断
	void paint();//绘制游戏图形方法
	void drawView();  //锁定绘制视野
	void endGame();//游戏结束
private:
	Timer timer;//计时器
	IMAGE *img = new IMAGE;
	Character hero = Character("character");//创建玩家人物
	Character shadow = Character("shadow");//创建影子
	BackGround bg = BackGround(WIDTH, HEIGHT,1);//创建背景
	thread threads[1000];//线程缓冲区
	int tds_cnt = 0;//线程计数器
	bool success = false;//游戏是否胜利
	int HP = 80;//人物血量值

	inline bool touchShadow() {
		//人物与影子碰撞的检测方法
		bool touch = false;//初始化为不碰撞
		Point points[4];
		//初始化人物的四个角点
		points[0].pos_x = hero.getX();
		points[0].pos_y = hero.getY();

		points[1].pos_x = hero.getX() + 32;
		points[1].pos_y = hero.getY();

		points[2].pos_x = hero.getX();
		points[2].pos_y = hero.getY() + 32;

		points[3].pos_x = hero.getX() + 32;
		points[3].pos_y = hero.getY() + 32;
		Rec shadow_rec;
		//初始化影子的矩形方块
		shadow_rec.pos_x_begin = shadow.getX();
		shadow_rec.pos_y_begin = shadow.getY();

		shadow_rec.pos_x_end = shadow.getX() + 32;
		shadow_rec.pos_y_end = shadow.getY() + 32;
		for (int i = 0; i < 4; i++)
		{
			if (recContainPointIn(shadow_rec, points[i])) {
				touch = true;
			}
		}
		return touch;
	}

	HRGN viewArr;  // 定义视野
	int graph_array; // 视野半径
	int graph_num; // 绘图次数

	//无伤害平台
	vector<Platform> platforms;

	//火炬(火源)用于点亮视野
	Torch fireWork_pre;
	Torch fireWork_later;


	//大门
	Door door;
	vector<Door> door_open;//大门动画序列容器
	int door_openstep;//大门动画序列计数器

	//障碍
	vector <Barrier> houl;

	//火苗（钥匙）用于打开大门完成游戏
	Key key;
	//游戏通关画面
	IMAGE group;
};

GamePanel::GamePanel()
{
	int z[100][100] = { 0 };//地图块id数组
	FILE* fp = NULL;
	int i, j = 0, dat = 0, is;//i为列号,j为行号,dat为临时字符变量转int变量存储,is为读入符号的判断
	char* buff;
	char tmp;
	buff = (char*)malloc(sizeof(char) * 200);
	if ((fp = fopen("map1_base.csv", "r")) == NULL) {
		//若文件打开失败
	}
	while (j < 32) {
		for (i = 0; i < 40; i++) {
			dat = 0, is = 1;
			tmp = (char)(fgetc(fp));
			while (tmp != ',' && tmp != '\n') {
				if (tmp == '-') {
					is = -1;
				}
				else dat = dat * 10 + tmp - '0';

				tmp = (char)(fgetc(fp));
			}
			z[j][i] = dat * is;
		}
		j++;
	}
	fclose(fp);//关闭文件指针
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 40; j++) {
			if (z[i][j] != -1) {
				Platform tmp = Platform(32, 32, z[i][j]);
				tmp.setX(j * 32);
				tmp.setY(i * 32);
				platforms.push_back(tmp);//将地图块放入容器
			}
		}
	}

	//创建与设置火源位置
	fireWork_pre = Torch(64, 64, 1);
	fireWork_pre.setX(32 * 32);
	fireWork_pre.setY(12 * 32 + 7);


	fireWork_later = Torch(64, 64, 2);
	fireWork_later.setX(32 * 32);
	fireWork_later.setY(12 * 32 + 7);
	//创建与设置大门位置
	door = Door(96, 96, 1);
	door.setX(35 * 32);
	door.setY(26 * 32);
	

	for (i = 1; i <= 5; i++) {
		Door tmp1 = Door(96, 96, i);
		tmp1.setX(35 * 32);
		tmp1.setY(26 * 32);
		door_open.push_back(tmp1);//将大门序列压入容器
	}

	//创建与设置火苗(钥匙)位置
	key = Key(64, 64, 1);
	key.setX(17 * 32);
	key.setY(4 * 32);

	//初始化视野半径为最大半径
	graph_array = MAXVIEW;
	//初始化人物血量为最大血量
	HP = 80;
	//初始化半径计数器
	graph_num = 0;
	//初始化大门动画序列id
	door_openstep = 0;
	//加载结束界面
	loadimage(&group, "res\\group.png");
	
}

GamePanel::~GamePanel()
{

}

inline void GamePanel::startup()
{
	initgraph(WIDTH, HEIGHT);
	hero.setX(WIDTH / 2 - 30);
	hero.setY(HEIGHT / 2 + 140);
	shadow.setX(WIDTH / 2 - 30);
	shadow.setY(HEIGHT / 2 + 140);
}

inline void GamePanel::updateWithInput()
{
	if (kbhit()) {
		if (GetAsyncKeyState('A')) // 向左
		{
			//设置人物状态
			hero.setStatusX(Character::move_L);
			//将设置影子状态加入线程缓冲区
			threads[tds_cnt++] = thread(&Character::setStatusSX, &shadow, Character::move_L);
			threads[tds_cnt - 1].detach();
		}
		else if (GetAsyncKeyState('D')) // 向右
		{
			//设置人物状态
			hero.setStatusX(Character::move_R);
			//将设置影子状态加入线程缓冲区
			threads[tds_cnt++] = thread(&Character::setStatusSX, &shadow, Character::move_R);
			threads[tds_cnt - 1].detach();
		}
		else {
			if (hero.getStatusX() == Character::move_L) {
				//从向左移动转为向左面站立
				hero.setStatusX(Character::stand_L);
			}
			if (hero.getStatusX() == Character::move_R) {
				//从向右移动转为向右站立
				hero.setStatusX(Character::stand_R);
			}
			if (shadow.getStatusX() == Character::move_L) {
				//从向左移动转为向左面站立
				threads[tds_cnt++] = thread(&Character::setStatusSX, &shadow, Character::stand_L);
				threads[tds_cnt - 1].detach();
			}
			if (shadow.getStatusX() == Character::move_R) {
				//从向右移动转为向右站立
				threads[tds_cnt++] = thread(&Character::setStatusSX, &shadow, Character::stand_R);
				threads[tds_cnt - 1].detach();
			}

		}
		if (GetAsyncKeyState('W')) // 向上
		{
			//设置人物状态
			hero.setJump(true);
			//将设置影子状态加入线程缓冲区
			threads[tds_cnt++] = thread(&Character::setSJump, &shadow, true);
			threads[tds_cnt - 1].detach();
		}
		if (GetAsyncKeyState('H'))
		{
			hero.setReadyToAct(TRUE);  //检测周围是否可以互动
		}
	}
	if (tds_cnt > 900) {
		//当线程缓冲区线程数大于900时,下移缓冲器计数器,清空缓冲区
		tds_cnt = 0;
	}
	
	
	if (graph_array < MINVIEW) graph_array = MINVIEW;//当视野下降为最小时,保持视野状态
	
}

inline void GamePanel::updateWithoutInput()
{
	//持续根据状态更新人物的位置
	hero.updateX();
	hero.updateY();
	shadow.updateX();
	shadow.updateY();
	

	bool hits_hero = false;//是否与某个方块撞击的状态值
	bool hits_shadow = false;
	for (auto it = platforms.begin(); it != platforms.end(); ++it) {
		if (hero.isHit(*it)) {
			hits_hero = true;//若与其中之一撞击则为真
		};
		if (shadow.isHit(*it)) {
			hits_shadow = true;//若与其中之一撞击则为真
		};
	}
	if (!hits_hero) {
		hero.setStatusY(Character::air);//如全不碰撞,那么显然无支撑,是air状态
	}
	if (!hits_shadow) {
		shadow.setStatusY(Character::air);//如全不碰撞,那么显然无支撑,是air状态
	}
	hero.die();
	if (hero.getReadyToAct()) // 如果按下了互动键，检测是否可以互动
	{
		actWithScene();
		hero.setReadyToAct(FALSE); // 恢复为默认值
	}

	if (door_openstep == 4)  // 开门动画播放完毕则设置为游戏通关
	{
		success = TRUE;
	}
	if (door.opened && door_openstep <= 3)//开门动画帧数序列
	{
		door.setid(door_openstep++);
	}
	

	if (hero.getFlame())   //取得火苗之后获得短暂实践大范围视野
	{
		if (graph_num > 0)
		{
			graph_num-=10;
		}
	}
	if (touchShadow()) {
		if(HP >= 0)
		{
			HP -= 10;
		}
		if (HP < 0)
		{
			hero.setAlive(FALSE);
		}
	}

	
}

inline void GamePanel::paint()
{
	cleardevice();
	
	if (hero.getAlive() && success == FALSE)
	{

		drawView(); //视野锁定
		bg.drawImage();//背景绘制
		//hero.outputData();测试数据程序

		if (!fireWork_pre.lighted)fireWork_pre.drawImage();//绘制未互动的火源
		else fireWork_later.drawImage();//绘制互动完的火源
		door_open[door_openstep].drawImage();//绘制大门图片
		key.drawImage();//绘制火苗(钥匙)


		for (auto it = platforms.begin(); it != platforms.end(); ++it) {
			it->drawImage();//遍历容器,绘制平台图像
		}
		hero.updateCount();//更新人物动画序列计数器
		hero.drawImage();//绘制人物图像
		shadow.updateCount();//更新影子动画序列计数器
		shadow.drawImage();//绘制影子图像

		// 删除遮罩区域
		DeleteObject(viewArr);
		// 消除遮罩区域
		setcliprgn(NULL);
	}
	else if(!hero.getAlive() && success == FALSE)
	{
		//若游戏正常进行状态
		bg.drawImage();
		settextstyle(200, 0, _T("黑体")); // 设置文字大小、字体
		settextcolor(RGB(100, 0, 0));  // 设置字体颜色
		outtextxy(200, 360, "GAME OVER"); // 输出游戏结束字样 
		
	}
	else if (success == TRUE)
	{
		//若游戏胜利,绘制一下图片
		bg.drawImage();
		putimagePng(60, 80, &group);

	}
	Sleep(10);

}


inline void GamePanel::actWithScene()  //场景互动函数
{
	if (hero.getReadyToAct() == TRUE)
	{
		if (hero.interact(fireWork_pre.getX(), fireWork_pre.getY(), 64, 64) == 1)  //判断是否与火把互动
		{
			if (hero.getFlame())  //如果主角拥有火苗
			{
				fireWork_pre.isLighted();  //点燃火把
			}
		}
		else if (hero.interact(key.getX(), key.getY(), 64, 64) == 1) //判断是否与火苗互动
		{
			hero.setFlame(TRUE); //取得火苗
			graph_num = 420;
		}
		else if (hero.interact(door.getX(), door.getY(), 96, 96) == 1)
		{
			if (fireWork_pre.lighted)  //如果火把点燃，那么互动后大门打开
			{
				door.isOpened();
				
			}
		}
	}
}


inline void GamePanel::endGame()
{
	getchar();//堵塞保持画布
	closegraph();//关闭画布
}
inline void GamePanel::drawView()
{
	// 锁定视野
	int r = 70  + HP + graph_num;	// 计算视野半径
	
	//int r = 100;
	POINT orgin;
	orgin.x = hero.getX()+16;						// 计算在屏幕上的位置
	orgin.y = hero.getY()+16;						// 计算在屏幕上的位置
	viewArr = CreateEllipticRgn(orgin.x - r, orgin.y - r, orgin.x + r, orgin.y + r);	// 创建一个圆形的区域
	setcliprgn(viewArr);							// 锁定区域
}