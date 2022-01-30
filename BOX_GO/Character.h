#include<easyx.h>
#include<string>
#include<thread>
#include<mutex>
#include<EasyXPng.h>
#include"Timer.h"
#include<vector>
#include"Scene.h"
#include<math.h>
using namespace std;
mutex mut;
struct Point
{
	//点结构体
	int pos_x;
	int pos_y;
};
struct Rec {
	//矩形结构体
	int pos_x_begin;
	int pos_y_begin;
	int pos_x_end;
	int pos_y_end;
};
inline bool recContainPoint(Rec rec, Point p) {
	//矩形包含点的判定方法
	if (p.pos_x >= rec.pos_x_begin&&p.pos_x <= rec.pos_x_end) {
		if (p.pos_y >= rec.pos_y_begin&&p.pos_y <= rec.pos_y_end) {
			return true;
		}
	}
	return false;
}
inline bool recContainPointIn(Rec rec, Point p) {
	//矩形包含点的判定方法
	if (p.pos_x > rec.pos_x_begin&&p.pos_x < rec.pos_x_end) {
		if (p.pos_y > rec.pos_y_begin&&p.pos_y < rec.pos_y_end) {
			return true;
		}
	}
	return false;
}
class Character
{
public:
	//可访问数据成员
	enum Status {
		stand_L = 1, stand_R, move_L, move_R, air,ground, act
	};
	
private:
	//数据成员
	int pos_x;
	int pos_y;//人物坐标
	int v_x = 0;//人物速度
	int v_y;//跳起和下落速度
	int gravity = 3;//人物重力
	Status status_x = stand_R;//初始化人物水平状态为向右站立
	Status status_y = ground;//初始化人物垂直状态为在地面上
	Status pre_status_x = stand_R;//初始化人物上一水平状态
	Status pre_status_y = ground;//初始化人物上一垂直状态

	bool jump_now = false;//初始化人物状态为不跳跃
	bool alive;//人物是否存活
	bool ReadyToAct = FALSE;  //是否按下互动键
	bool flame = FALSE;       //判断人物是否持有火苗

	IMAGE character_img;//人物的图片对象
	int img_width;//人物图像宽度
	int img_height;//人物图像高度
	Timer timer;//计时器
	
	int count_fall = 0;//人物下落动画序列编号
	int count_move_left = 0;//人物向左移动动画序列编号
	int count_move_right = 0;//人物向右移动画序列编号
	int count_stand_left = 0;//人物向左站立动画序列编号
	int count_stand_right = 0;//人物向右站立动画序列编号
	
	
public:
	
	//图像容器
	vector<IMAGE> img_stand_right;//人物向右站立动画序列容器
	vector<IMAGE> img_stand_left;//人物向左站立动画序列容器
	vector<IMAGE> img_fall_left;//人物向左下落动画序列容器
	vector<IMAGE> img_fall_right;//人物向右下落动画序列容器
	IMAGE img_jump_left;//人物向左跳跃图像
	IMAGE img_jump_right;//人物向右跳跃图像
	vector<IMAGE> img_move_left;//人物向左移动动画序列容器
	vector<IMAGE> img_move_right;//人物向左移动动画序列容器
	//公有方法
	//创建与删除人物
	Character();
	Character(const char name[]);
	~Character();
	//访问器and修改器
	int getX();
	int getY();
	void setX(const int x);
	void setY(const int y);
	int getStatusX();
	int getStatusY();
	void setStatusX(const enum Status);
	void setStatusY(const enum Status);
	void setStatusSX(const enum Status);
	void setStatusSY(const enum Status);
	void setJump(bool jump);
	void setSJump(bool jump);
	//场景互动相关方法
	void setReadyToAct(bool value);
	bool getReadyToAct();
	void setFlame(bool value);
	bool getFlame();
	//死亡相关方法
	bool getAlive();
	void setAlive(bool value);
	void die();


	//人物与场景碰撞检测与修正
	bool isHit(Scene block);
	//人物动作
	void standLeft();
	void standRight();
	void moveLeft();
	void moveRight();
	void jump();
	void fall();
	int interact(int barrier_x, int barrier_y, int barrier_width, int barrier_height);
	//人物图像绘制
	void drawImage();
	//更新人物的坐标
	void updateY();
	void updateX();
	//更新动画序列id
	void updateCount();
	//人物数据测试函数
	void outputData();


};
Character::Character()
{
	//加载人物图像(原始图像换成向右站立的图像)

	char filename[256];

	//加载人物图像(原始图像换成向右站立的图像)
	loadimage(&character_img, "res\\character\\standright\\1.png", 32, 32);
	img_width = character_img.getwidth();
	img_height = character_img.getheight();

	//左跳跃图像
	loadimage(&img_jump_left, "res\\character\\jumpleft\\1.png", 32, 32);
	img_width = character_img.getwidth();
	img_height = character_img.getheight();

	//右跳跃图像
	loadimage(&img_jump_right, "res\\character\\jumpright\\1.png", 32, 32);
	img_width = character_img.getwidth();
	img_height = character_img.getheight();

	//右站立序列
	for (int i = 1; i < 6; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\standright\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_stand_right.push_back(tmp);
	}

	//左站立序列
	for (int i = 1; i < 6; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\standleft\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_stand_left.push_back(tmp);
	}

	//右掉落序列
	for (int i = 1; i < 4; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\fallright\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_fall_right.push_back(tmp);
	}
	//左掉落序列
	for (int i = 1; i < 4; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\fallleft\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_fall_left.push_back(tmp);
	}

	//右移动序列
	for (int i = 1; i < 8; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\moveright\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_move_left.push_back(tmp);
	}

	//左移动序列
	for (int i = 0; i < 3; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\moveleft\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_move_left.push_back(tmp);
	}
	//人物状态初始化
	alive = true;
	status_x = stand_R;

}


inline Character::Character(const char name[])
{

	char filename[256];

	//左跳跃图像
	IMAGE tmp;
	_stprintf(filename, "res\\%s\\jumpleft\\%d.png", name, 1);
	loadimage(&img_jump_left, filename, 32, 32);
	img_width = character_img.getwidth();
	img_height = character_img.getheight();

	//右跳跃图像
	_stprintf(filename, "res\\%s\\jumpright\\%d.png", name, 1);
	loadimage(&img_jump_right, filename, 32, 32);
	img_width = character_img.getwidth();
	img_height = character_img.getheight();

	//右站立序列
	for (int i = 1; i < 6; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\standright\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_stand_right.push_back(tmp);
	}

	//左站立序列
	for (int i = 1; i < 6; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\standleft\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_stand_left.push_back(tmp);
	}

	//右掉落序列
	for (int i = 1; i < 2; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\fallright\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_fall_right.push_back(tmp);
	}
	//左掉落序列
	for (int i = 1; i < 2; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\fallleft\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_fall_left.push_back(tmp);
	}

	//右移动序列
	for (int i = 1; i < 8; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\moveright\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_move_right.push_back(tmp);
	}

	//左移动序列
	for (int i = 1; i < 8; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\moveleft\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_move_left.push_back(tmp);
	}


	//人物状态初始化
	alive = true;
	status_x = stand_R;

	//动作计数初始化
	count_fall = 0;
	count_move_left = 0;
	count_move_right = 0;
	count_stand_left = 0;
	count_stand_right = 0;


}

Character::~Character()
{

}

int Character::getX()
{
	return pos_x;
	
}
int Character::getY()
{
	return pos_y;
}

inline void Character::setX(const int x)
{
	pos_x = x;
}



inline void Character::setY(const int y)
{
	pos_y = y;
}


int Character::getStatusY()
{
	return status_y;
}
int Character::getStatusX()
{
	return status_x;
}


inline void Character::setStatusX(const Status s) {
	
	status_x = s;
	updateX();
}
inline void Character::setStatusSX(const Status s) {

	this_thread::sleep_for(chrono::seconds(1));//本线程延时1s执行,完成影子的动作延迟
	status_x = s;
	updateX();


}
inline void Character::setStatusY(const Status s) {
	status_y = s;
}
inline void Character::setStatusSY(const Status s) {
	this_thread::sleep_for(chrono::seconds(1));//本线程延时1s执行,完成影子的动作延迟
	status_y = s;
}

inline void Character::setReadyToAct(bool value)
{
	//设置互动状态
	ReadyToAct = value;
}
inline bool Character::getReadyToAct()
{
	return ReadyToAct;
}
inline void Character::setFlame(bool value)
{
	flame = value;
}
inline bool Character::getFlame()
{
	return flame;
}


inline bool Character::getAlive()
{
	return alive;
}
inline void Character::setAlive(bool value)
{
	//设置人物是否存活
	alive = value;
}

inline void Character::die()
{
	//人物出界死亡
	if ((pos_x + img_width <= -32) || (pos_x >= 1280) || (pos_y >= 1024) || (pos_y + img_height <= 0))
	{
		alive = FALSE;
	}
}


inline void Character::setJump(bool jump)
{
	//设置人物是否跳跃
	jump_now = jump;
}

inline void Character::setSJump(bool jump)
{
	this_thread::sleep_for(chrono::seconds(1));//本线程延时1s执行,完成影子的动作延迟
	jump_now = jump;
}

inline bool Character::isHit(Scene block)
{
	//检测碰撞并修正
	bool hit = false;//存储是否碰撞的返回值
	int v_x_back = -v_x, v_y_back = -v_y;//修正用的速度值
	Point points[4];//人物对象的四个角点
	Rec block_rec;//场景的矩形
	double vec_cos[4];//四个矩形的点与被包含点的连线的cos值

	double vec_cos_v = 5;//初始化速度的cos值,取5是不可能值

	if (v_x != 0 || v_y != 0) {
		vec_cos_v = pow(-v_x, 2) / (pow(-v_x, 2) + pow(-v_y, 2));//计算速度的cos值
		//设定cos值的符号
		if (v_x_back < 0) {
			vec_cos_v = -vec_cos_v;//
		}
	}
	//初始化人物的四个角点
	points[0].pos_x = pos_x;
	points[0].pos_y = pos_y;

	points[1].pos_x = pos_x + img_width;
	points[1].pos_y = pos_y;

	points[2].pos_x = pos_x;
	points[2].pos_y = pos_y + img_height;

	points[3].pos_x = pos_x + img_width;
	points[3].pos_y = pos_y + img_height;
	//初始化场景矩形
	block_rec.pos_x_begin = block.getX();
	block_rec.pos_y_begin = block.getY();

	block_rec.pos_x_end = block.getX() + block.getImg_width();
	block_rec.pos_y_end = block.getY() + block.getImg_height();




	for (int i = 0; i < 4; i++)
	{
		if (recContainPoint(block_rec, points[i])) {
			//若包含则碰撞
			hit = true;
			//计算此时矩形的四个cos值
			vec_cos[0] = -pow((points[i].pos_x - block_rec.pos_x_begin), 2) / ((pow((points[i].pos_x - block_rec.pos_x_begin), 2)) + (pow((points[i].pos_y - block_rec.pos_y_begin), 2)));
			vec_cos[1] = pow((points[i].pos_x - (block_rec.pos_x_end)), 2) / ((pow((points[i].pos_x - (block_rec.pos_x_end)), 2)) + (pow((points[i].pos_y - block_rec.pos_y_begin), 2)));
			vec_cos[2] = pow((points[i].pos_x - (block_rec.pos_x_end)), 2) / ((pow((points[i].pos_x - (block_rec.pos_x_end)), 2)) + (pow((points[i].pos_y - (block_rec.pos_y_end)), 2)));
			vec_cos[3] = -pow((points[i].pos_x - block_rec.pos_x_begin), 2) / ((pow((points[i].pos_x - block_rec.pos_x_begin), 2)) + (pow((points[i].pos_y - (block_rec.pos_y_end)), 2)));
			//以下被注释的另一个修正速度暂时不启用,其作用为模拟摩擦力将物体停止
			if (v_y_back < 0) {
				//人物从上向下掉落
				if (vec_cos_v >= vec_cos[0] && vec_cos_v <= vec_cos[1]) {
					//掉落在平台表面上
					v_y_back = -((points[i].pos_y - block_rec.pos_y_begin));//将人物pos_y修正到边沿处
					//v_x_back *= (points[i].pos_y - block_rec.pos_y_begin) / (double)(v_y);
					//将速度归零
					status_y = ground;
					v_y = 0;

				}
				else {
					//未掉落在平台表面上
					if (v_x_back < 0) {
						//向右碰撞墙体
						v_x_back = -(abs(points[i].pos_x - block_rec.pos_x_begin) + 5);
						//v_y_back *= fabs(points[i].pos_x - block_rec.pos_x_begin) / (double)(v_x);
					}
					else if (v_x_back > 0) {
						//向左碰撞墙体
						v_x_back = (abs(points[i].pos_x - (block_rec.pos_x_end)) + 5);
						//v_y_back *= fabs(points[i].pos_x - (block_rec.pos_x_begin + 32)) / (double)(v_x);
					}
					//将速度归零
					v_x = 0;
					v_y = 0;
					status_y = air;
				}

			}
			else if (v_y_back > 0) {
				//人物从下向上跳跃
				if (vec_cos_v >= vec_cos[4] && vec_cos_v <= vec_cos[3]) {
					//人物撞击平台下表面
					v_y_back = (abs(points[i].pos_y - (block_rec.pos_y_end)) + 1);//将人物修正到平台以外
					//v_x_back = v_x * (abs(points[i].pos_y - (block_rec.pos_y_begin + 32))) / (double)(v_y);
					//v_x = 0;
					v_y = 0;
					status_y = air;
				}
				else {
					if (v_x_back < 0) {
						//向右碰撞墙体
						v_x_back = -(abs(points[i].pos_x - block_rec.pos_x_begin) + 5);
						//v_y_back *= (points[i].pos_x - block_rec.pos_x_begin) / (double)(v_x);
					}
					else if (v_x_back > 0) {
						//向左碰撞墙体
						v_x_back = (abs(points[i].pos_x - (block_rec.pos_x_end)) + 5);
						//v_y_back *= (points[i].pos_x - (block_rec.pos_x_begin + 32)) / (double)(v_x);
					}
					v_x = 0;
					v_y = 0;
					status_y = air;
				}

			}
			else {
				//人物无掉落速度,即在平台上跑动
				if (v_x_back < 0) {
					//向右碰撞墙体
					v_x_back = -(abs(points[i].pos_x - block_rec.pos_x_begin));
					//v_y_back *= (points[i].pos_x - block_rec.pos_x_begin) / (double)(v_x);
					if (!(i == 2 || i == 3)) {
						v_x = 0;
					}
				}
				else if (v_x_back > 0) {
					//向左碰撞墙体
					v_x_back = (abs(points[i].pos_x - (block_rec.pos_x_end)));
					//v_y_back *= (points[i].pos_x - (block_rec.pos_x_begin + 32)) / (double)(v_x);
					if (!(i == 2 || i == 3)) {
						v_x = 0;
					}
					
				}
				if (!recContainPointIn(block_rec, points[i]) && recContainPoint(block_rec, points[i])) {
					//若只在物体表面跑动,无碰撞墙体,进行反向修正,保证人物正常在平台上移动
					if (i == 2 || i == 3) {
						pos_x -= v_x_back;
					}
					
				}
			}
			//修正并更新人物位置
			
			pos_x += v_x_back;
			pos_y += v_y_back;

			
			//更新四个点的位置
			points[0].pos_x = pos_x;
			points[0].pos_y = pos_y;

			points[1].pos_x = pos_x + img_width;
			points[1].pos_y = pos_y;

			points[2].pos_x = pos_x;
			points[2].pos_y = pos_y + img_height;

			points[3].pos_x = pos_x + img_width;
			points[3].pos_y = pos_y + img_height;

			block_rec.pos_x_begin = block.getX();
			block_rec.pos_y_begin = block.getY();
		}
	}

	return hit;//返回撞击状态
}

inline void Character::standLeft()
{
	v_x = 0;//站立置零水平速度
}
inline void Character::standRight()
{
	v_x = 0;//站立置零水平速度

}
inline void Character::moveLeft()
{
	//若速度未达最大值,则进行加速
	if (v_x >= -4) {
		v_x -= 1;
	}
	//更新人物坐标
	pos_x += v_x;
}

inline void Character::moveRight()
{
	//若速度未达最大值,则进行加速
	if (v_x <= 4) {
		v_x += 1;
	}
	//更新人物坐标
	pos_x += v_x;
}

inline void Character::jump()
{
	if (status_y == ground) {
		//不在空中方能跳起
		v_y = -30;//给予人物向上的初速度
		status_y = air;//设置人物在空中
		jump_now = false;//关闭跳跃
	}
	

}

inline void Character::fall()
{
	if (status_y == air) {
		//在空中才会下落
		//若未达到最大下落速度,用重力更新垂直速度
		if (v_y <= 24)
		{
			v_y += gravity;
		}
		//更新人物垂直坐标
		pos_y += v_y;
	}

	
}


inline int Character::interact(int barrier_x, int barrier_y, int barrier_width, int barrier_height)
{
	//角色图片覆盖范围
	int x_left = pos_x, x_right = pos_x + img_width, xBarrier_left = barrier_x, xBarrier_right = barrier_x + barrier_width;
	//障碍图片覆盖范围
	int y_up = pos_y, y_down = pos_y + img_height, yBarrier_up = barrier_y, yBarrier_down = barrier_y + barrier_height;

	int state = 0; //0为不可互动，1为可互动状态
	if ((x_left >= xBarrier_left && x_left <= barrier_x + barrier_width) || (x_right >= barrier_x && x_right <= xBarrier_right))
	{
		if ((y_up >= yBarrier_up && y_up <= yBarrier_down))
		{
			state = 1;//标识为可互动状态
		}
	}
	return state;
}

inline void Character::drawImage()
{

	//根据状态设置人物图片,更新序列帧                                                                              
	if (jump_now) {
		if (status_x == stand_L || status_x == move_L)character_img = img_jump_left;
		else character_img = img_jump_right;
	}
	else {
		switch (status_y)
		{
		case Character::air:

			if (status_x == stand_L || status_x == move_L) {
				character_img = img_fall_left[(count_fall++) % img_fall_left.size()];
			}
			else character_img = img_fall_right[(count_fall++) % img_fall_right.size()];
			break;
		case Character::ground:
			switch (Character::status_x)
			{
			case Character::stand_L:
				character_img = img_stand_left[(count_stand_left++) % img_stand_left.size()];
				break;
			case Character::stand_R:
				character_img = img_stand_right[(count_stand_right++) % img_stand_right.size()];
				break;
			case Character::move_L:
				character_img = img_move_left[(count_move_left++) % img_move_left.size()];
				break;
			case Character::move_R:
				character_img = img_move_right[(count_move_right++) % img_move_right.size()];
				break;
			default:
				break;
			}
		default:
			break;
		}
	}
	//绘制图片到画布上
	putimagePng(pos_x, pos_y, &character_img);

	

}

inline void Character::updateY()
{
	//根据垂直以及跳跃状态更新人物的垂直坐标
	if (jump_now) {
		jump();
	}
	if (status_y == air) {
		fall();
	}
	if (status_y == ground) {
		v_y = 0;
	}

}

inline void Character::updateX()
{
	//根据水平状态更新人物的水平坐标
	switch (status_x)
	{
	case stand_L:
		standLeft();
		break;
	case Character::stand_R:
		standRight();
		break;
	case Character::move_L:
		moveLeft();
		break;
	case Character::move_R:
		moveRight();
		break;
	default:
		break;
	}
}

inline void Character::updateCount()
{
	//根据人物的前一状态更新动画的序列帧,若发生状态转换则将前一序列帧的count置零
	switch (pre_status_x)
	{
	case Character::stand_L:
		if (status_x != stand_L)
		{
			count_stand_left = 0;
		}
		break;
	case Character::stand_R:
		if (status_x != stand_R)
		{
			count_stand_right = 0;
		}
		break;
	case Character::move_L:
		if (status_x != move_L)
		{
			count_move_left = 0;
		}
		break;
	case Character::move_R:
		if (status_x != move_R)
		{
			count_move_right = 0;
		}
		break;
	default:
		switch (pre_status_y)
		{
		case Character::air:
			if (status_y != air)
			{
				count_fall = 0;
			}
			break;
		default:
			break;
		}
		pre_status_x = status_x;

	}
}

inline void Character::outputData()
{
	//输出必要的人物数据,便于调试程序
	TCHAR a[20]; // 定义字符串数组
	_stprintf(a, _T("%d"), pos_x); // 将score1转换为字符串
	settextstyle(70, 0, _T("黑体")); // 设置文字大小、字体
	settextcolor(RGB(100, 0, 0));  // 设置字体颜色
	outtextxy(50+50, 100, a); // 输出得分文字
	_stprintf(a, _T("%d"), pos_y); // 将score1转换为字符串
	settextstyle(70, 0, _T("黑体")); // 设置文字大小、字体
	settextcolor(RGB(100, 0, 0));  // 设置字体颜色
	outtextxy(200 + 50, 100, a); // 输出得分文字
	_stprintf(a, _T("%d"), v_x); // 将score1转换为字符串
	settextstyle(70, 0, _T("黑体")); // 设置文字大小、字体
	settextcolor(RGB(100, 0, 0));  // 设置字体颜色
	outtextxy(50 + 50, 200, a); // 输出得分文字
	_stprintf(a, _T("%d"), v_y); // 将score1转换为字符串
	settextstyle(70, 0, _T("黑体")); // 设置文字大小、字体
	settextcolor(RGB(100, 0, 0));  // 设置字体颜色
	outtextxy(200 + 50, 200, a); // 输出得分文字
	_stprintf(a, _T("%d"), status_x); // 将score1转换为字符串
	settextstyle(70, 0, _T("黑体")); // 设置文字大小、字体
	settextcolor(RGB(100, 0, 0));  // 设置字体颜色
	outtextxy(50 + 50, 300, a); // 输出得分文字
	_stprintf(a, _T("%d"), status_y); // 将score1转换为字符串
	settextstyle(70, 0, _T("黑体")); // 设置文字大小、字体
	settextcolor(RGB(100, 0, 0));  // 设置字体颜色
	outtextxy(200 + 50, 300, a); // 输出得分文字
}
