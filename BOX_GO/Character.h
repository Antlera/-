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
	//��ṹ��
	int pos_x;
	int pos_y;
};
struct Rec {
	//���νṹ��
	int pos_x_begin;
	int pos_y_begin;
	int pos_x_end;
	int pos_y_end;
};
inline bool recContainPoint(Rec rec, Point p) {
	//���ΰ�������ж�����
	if (p.pos_x >= rec.pos_x_begin&&p.pos_x <= rec.pos_x_end) {
		if (p.pos_y >= rec.pos_y_begin&&p.pos_y <= rec.pos_y_end) {
			return true;
		}
	}
	return false;
}
inline bool recContainPointIn(Rec rec, Point p) {
	//���ΰ�������ж�����
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
	//�ɷ������ݳ�Ա
	enum Status {
		stand_L = 1, stand_R, move_L, move_R, air,ground, act
	};
	
private:
	//���ݳ�Ա
	int pos_x;
	int pos_y;//��������
	int v_x = 0;//�����ٶ�
	int v_y;//����������ٶ�
	int gravity = 3;//��������
	Status status_x = stand_R;//��ʼ������ˮƽ״̬Ϊ����վ��
	Status status_y = ground;//��ʼ�����ﴹֱ״̬Ϊ�ڵ�����
	Status pre_status_x = stand_R;//��ʼ��������һˮƽ״̬
	Status pre_status_y = ground;//��ʼ��������һ��ֱ״̬

	bool jump_now = false;//��ʼ������״̬Ϊ����Ծ
	bool alive;//�����Ƿ���
	bool ReadyToAct = FALSE;  //�Ƿ��»�����
	bool flame = FALSE;       //�ж������Ƿ���л���

	IMAGE character_img;//�����ͼƬ����
	int img_width;//����ͼ����
	int img_height;//����ͼ��߶�
	Timer timer;//��ʱ��
	
	int count_fall = 0;//�������䶯�����б��
	int count_move_left = 0;//���������ƶ��������б��
	int count_move_right = 0;//���������ƶ������б��
	int count_stand_left = 0;//��������վ���������б��
	int count_stand_right = 0;//��������վ���������б��
	
	
public:
	
	//ͼ������
	vector<IMAGE> img_stand_right;//��������վ��������������
	vector<IMAGE> img_stand_left;//��������վ��������������
	vector<IMAGE> img_fall_left;//�����������䶯����������
	vector<IMAGE> img_fall_right;//�����������䶯����������
	IMAGE img_jump_left;//����������Ծͼ��
	IMAGE img_jump_right;//����������Ծͼ��
	vector<IMAGE> img_move_left;//���������ƶ�������������
	vector<IMAGE> img_move_right;//���������ƶ�������������
	//���з���
	//������ɾ������
	Character();
	Character(const char name[]);
	~Character();
	//������and�޸���
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
	//����������ط���
	void setReadyToAct(bool value);
	bool getReadyToAct();
	void setFlame(bool value);
	bool getFlame();
	//������ط���
	bool getAlive();
	void setAlive(bool value);
	void die();


	//�����볡����ײ���������
	bool isHit(Scene block);
	//���ﶯ��
	void standLeft();
	void standRight();
	void moveLeft();
	void moveRight();
	void jump();
	void fall();
	int interact(int barrier_x, int barrier_y, int barrier_width, int barrier_height);
	//����ͼ�����
	void drawImage();
	//�������������
	void updateY();
	void updateX();
	//���¶�������id
	void updateCount();
	//�������ݲ��Ժ���
	void outputData();


};
Character::Character()
{
	//��������ͼ��(ԭʼͼ�񻻳�����վ����ͼ��)

	char filename[256];

	//��������ͼ��(ԭʼͼ�񻻳�����վ����ͼ��)
	loadimage(&character_img, "res\\character\\standright\\1.png", 32, 32);
	img_width = character_img.getwidth();
	img_height = character_img.getheight();

	//����Ծͼ��
	loadimage(&img_jump_left, "res\\character\\jumpleft\\1.png", 32, 32);
	img_width = character_img.getwidth();
	img_height = character_img.getheight();

	//����Ծͼ��
	loadimage(&img_jump_right, "res\\character\\jumpright\\1.png", 32, 32);
	img_width = character_img.getwidth();
	img_height = character_img.getheight();

	//��վ������
	for (int i = 1; i < 6; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\standright\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_stand_right.push_back(tmp);
	}

	//��վ������
	for (int i = 1; i < 6; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\standleft\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_stand_left.push_back(tmp);
	}

	//�ҵ�������
	for (int i = 1; i < 4; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\fallright\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_fall_right.push_back(tmp);
	}
	//���������
	for (int i = 1; i < 4; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\fallleft\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_fall_left.push_back(tmp);
	}

	//���ƶ�����
	for (int i = 1; i < 8; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\moveright\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_move_left.push_back(tmp);
	}

	//���ƶ�����
	for (int i = 0; i < 3; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\character\\moveleft\\%d.png", i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_move_left.push_back(tmp);
	}
	//����״̬��ʼ��
	alive = true;
	status_x = stand_R;

}


inline Character::Character(const char name[])
{

	char filename[256];

	//����Ծͼ��
	IMAGE tmp;
	_stprintf(filename, "res\\%s\\jumpleft\\%d.png", name, 1);
	loadimage(&img_jump_left, filename, 32, 32);
	img_width = character_img.getwidth();
	img_height = character_img.getheight();

	//����Ծͼ��
	_stprintf(filename, "res\\%s\\jumpright\\%d.png", name, 1);
	loadimage(&img_jump_right, filename, 32, 32);
	img_width = character_img.getwidth();
	img_height = character_img.getheight();

	//��վ������
	for (int i = 1; i < 6; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\standright\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_stand_right.push_back(tmp);
	}

	//��վ������
	for (int i = 1; i < 6; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\standleft\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_stand_left.push_back(tmp);
	}

	//�ҵ�������
	for (int i = 1; i < 2; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\fallright\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_fall_right.push_back(tmp);
	}
	//���������
	for (int i = 1; i < 2; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\fallleft\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_fall_left.push_back(tmp);
	}

	//���ƶ�����
	for (int i = 1; i < 8; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\moveright\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_move_right.push_back(tmp);
	}

	//���ƶ�����
	for (int i = 1; i < 8; i++) {
		IMAGE tmp;
		_stprintf(filename, "res\\%s\\moveleft\\%d.png", name,i);
		loadimage(&tmp, filename, 32, 32);
		img_width = tmp.getwidth();
		img_height = tmp.getheight();
		img_move_left.push_back(tmp);
	}


	//����״̬��ʼ��
	alive = true;
	status_x = stand_R;

	//����������ʼ��
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

	this_thread::sleep_for(chrono::seconds(1));//���߳���ʱ1sִ��,���Ӱ�ӵĶ����ӳ�
	status_x = s;
	updateX();


}
inline void Character::setStatusY(const Status s) {
	status_y = s;
}
inline void Character::setStatusSY(const Status s) {
	this_thread::sleep_for(chrono::seconds(1));//���߳���ʱ1sִ��,���Ӱ�ӵĶ����ӳ�
	status_y = s;
}

inline void Character::setReadyToAct(bool value)
{
	//���û���״̬
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
	//���������Ƿ���
	alive = value;
}

inline void Character::die()
{
	//�����������
	if ((pos_x + img_width <= -32) || (pos_x >= 1280) || (pos_y >= 1024) || (pos_y + img_height <= 0))
	{
		alive = FALSE;
	}
}


inline void Character::setJump(bool jump)
{
	//���������Ƿ���Ծ
	jump_now = jump;
}

inline void Character::setSJump(bool jump)
{
	this_thread::sleep_for(chrono::seconds(1));//���߳���ʱ1sִ��,���Ӱ�ӵĶ����ӳ�
	jump_now = jump;
}

inline bool Character::isHit(Scene block)
{
	//�����ײ������
	bool hit = false;//�洢�Ƿ���ײ�ķ���ֵ
	int v_x_back = -v_x, v_y_back = -v_y;//�����õ��ٶ�ֵ
	Point points[4];//���������ĸ��ǵ�
	Rec block_rec;//�����ľ���
	double vec_cos[4];//�ĸ����εĵ��뱻����������ߵ�cosֵ

	double vec_cos_v = 5;//��ʼ���ٶȵ�cosֵ,ȡ5�ǲ�����ֵ

	if (v_x != 0 || v_y != 0) {
		vec_cos_v = pow(-v_x, 2) / (pow(-v_x, 2) + pow(-v_y, 2));//�����ٶȵ�cosֵ
		//�趨cosֵ�ķ���
		if (v_x_back < 0) {
			vec_cos_v = -vec_cos_v;//
		}
	}
	//��ʼ��������ĸ��ǵ�
	points[0].pos_x = pos_x;
	points[0].pos_y = pos_y;

	points[1].pos_x = pos_x + img_width;
	points[1].pos_y = pos_y;

	points[2].pos_x = pos_x;
	points[2].pos_y = pos_y + img_height;

	points[3].pos_x = pos_x + img_width;
	points[3].pos_y = pos_y + img_height;
	//��ʼ����������
	block_rec.pos_x_begin = block.getX();
	block_rec.pos_y_begin = block.getY();

	block_rec.pos_x_end = block.getX() + block.getImg_width();
	block_rec.pos_y_end = block.getY() + block.getImg_height();




	for (int i = 0; i < 4; i++)
	{
		if (recContainPoint(block_rec, points[i])) {
			//����������ײ
			hit = true;
			//�����ʱ���ε��ĸ�cosֵ
			vec_cos[0] = -pow((points[i].pos_x - block_rec.pos_x_begin), 2) / ((pow((points[i].pos_x - block_rec.pos_x_begin), 2)) + (pow((points[i].pos_y - block_rec.pos_y_begin), 2)));
			vec_cos[1] = pow((points[i].pos_x - (block_rec.pos_x_end)), 2) / ((pow((points[i].pos_x - (block_rec.pos_x_end)), 2)) + (pow((points[i].pos_y - block_rec.pos_y_begin), 2)));
			vec_cos[2] = pow((points[i].pos_x - (block_rec.pos_x_end)), 2) / ((pow((points[i].pos_x - (block_rec.pos_x_end)), 2)) + (pow((points[i].pos_y - (block_rec.pos_y_end)), 2)));
			vec_cos[3] = -pow((points[i].pos_x - block_rec.pos_x_begin), 2) / ((pow((points[i].pos_x - block_rec.pos_x_begin), 2)) + (pow((points[i].pos_y - (block_rec.pos_y_end)), 2)));
			//���±�ע�͵���һ�������ٶ���ʱ������,������Ϊģ��Ħ����������ֹͣ
			if (v_y_back < 0) {
				//����������µ���
				if (vec_cos_v >= vec_cos[0] && vec_cos_v <= vec_cos[1]) {
					//������ƽ̨������
					v_y_back = -((points[i].pos_y - block_rec.pos_y_begin));//������pos_y���������ش�
					//v_x_back *= (points[i].pos_y - block_rec.pos_y_begin) / (double)(v_y);
					//���ٶȹ���
					status_y = ground;
					v_y = 0;

				}
				else {
					//δ������ƽ̨������
					if (v_x_back < 0) {
						//������ײǽ��
						v_x_back = -(abs(points[i].pos_x - block_rec.pos_x_begin) + 5);
						//v_y_back *= fabs(points[i].pos_x - block_rec.pos_x_begin) / (double)(v_x);
					}
					else if (v_x_back > 0) {
						//������ײǽ��
						v_x_back = (abs(points[i].pos_x - (block_rec.pos_x_end)) + 5);
						//v_y_back *= fabs(points[i].pos_x - (block_rec.pos_x_begin + 32)) / (double)(v_x);
					}
					//���ٶȹ���
					v_x = 0;
					v_y = 0;
					status_y = air;
				}

			}
			else if (v_y_back > 0) {
				//�������������Ծ
				if (vec_cos_v >= vec_cos[4] && vec_cos_v <= vec_cos[3]) {
					//����ײ��ƽ̨�±���
					v_y_back = (abs(points[i].pos_y - (block_rec.pos_y_end)) + 1);//������������ƽ̨����
					//v_x_back = v_x * (abs(points[i].pos_y - (block_rec.pos_y_begin + 32))) / (double)(v_y);
					//v_x = 0;
					v_y = 0;
					status_y = air;
				}
				else {
					if (v_x_back < 0) {
						//������ײǽ��
						v_x_back = -(abs(points[i].pos_x - block_rec.pos_x_begin) + 5);
						//v_y_back *= (points[i].pos_x - block_rec.pos_x_begin) / (double)(v_x);
					}
					else if (v_x_back > 0) {
						//������ײǽ��
						v_x_back = (abs(points[i].pos_x - (block_rec.pos_x_end)) + 5);
						//v_y_back *= (points[i].pos_x - (block_rec.pos_x_begin + 32)) / (double)(v_x);
					}
					v_x = 0;
					v_y = 0;
					status_y = air;
				}

			}
			else {
				//�����޵����ٶ�,����ƽ̨���ܶ�
				if (v_x_back < 0) {
					//������ײǽ��
					v_x_back = -(abs(points[i].pos_x - block_rec.pos_x_begin));
					//v_y_back *= (points[i].pos_x - block_rec.pos_x_begin) / (double)(v_x);
					if (!(i == 2 || i == 3)) {
						v_x = 0;
					}
				}
				else if (v_x_back > 0) {
					//������ײǽ��
					v_x_back = (abs(points[i].pos_x - (block_rec.pos_x_end)));
					//v_y_back *= (points[i].pos_x - (block_rec.pos_x_begin + 32)) / (double)(v_x);
					if (!(i == 2 || i == 3)) {
						v_x = 0;
					}
					
				}
				if (!recContainPointIn(block_rec, points[i]) && recContainPoint(block_rec, points[i])) {
					//��ֻ����������ܶ�,����ײǽ��,���з�������,��֤����������ƽ̨���ƶ�
					if (i == 2 || i == 3) {
						pos_x -= v_x_back;
					}
					
				}
			}
			//��������������λ��
			
			pos_x += v_x_back;
			pos_y += v_y_back;

			
			//�����ĸ����λ��
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

	return hit;//����ײ��״̬
}

inline void Character::standLeft()
{
	v_x = 0;//վ������ˮƽ�ٶ�
}
inline void Character::standRight()
{
	v_x = 0;//վ������ˮƽ�ٶ�

}
inline void Character::moveLeft()
{
	//���ٶ�δ�����ֵ,����м���
	if (v_x >= -4) {
		v_x -= 1;
	}
	//������������
	pos_x += v_x;
}

inline void Character::moveRight()
{
	//���ٶ�δ�����ֵ,����м���
	if (v_x <= 4) {
		v_x += 1;
	}
	//������������
	pos_x += v_x;
}

inline void Character::jump()
{
	if (status_y == ground) {
		//���ڿ��з�������
		v_y = -30;//�����������ϵĳ��ٶ�
		status_y = air;//���������ڿ���
		jump_now = false;//�ر���Ծ
	}
	

}

inline void Character::fall()
{
	if (status_y == air) {
		//�ڿ��вŻ�����
		//��δ�ﵽ��������ٶ�,���������´�ֱ�ٶ�
		if (v_y <= 24)
		{
			v_y += gravity;
		}
		//�������ﴹֱ����
		pos_y += v_y;
	}

	
}


inline int Character::interact(int barrier_x, int barrier_y, int barrier_width, int barrier_height)
{
	//��ɫͼƬ���Ƿ�Χ
	int x_left = pos_x, x_right = pos_x + img_width, xBarrier_left = barrier_x, xBarrier_right = barrier_x + barrier_width;
	//�ϰ�ͼƬ���Ƿ�Χ
	int y_up = pos_y, y_down = pos_y + img_height, yBarrier_up = barrier_y, yBarrier_down = barrier_y + barrier_height;

	int state = 0; //0Ϊ���ɻ�����1Ϊ�ɻ���״̬
	if ((x_left >= xBarrier_left && x_left <= barrier_x + barrier_width) || (x_right >= barrier_x && x_right <= xBarrier_right))
	{
		if ((y_up >= yBarrier_up && y_up <= yBarrier_down))
		{
			state = 1;//��ʶΪ�ɻ���״̬
		}
	}
	return state;
}

inline void Character::drawImage()
{

	//����״̬��������ͼƬ,��������֡                                                                              
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
	//����ͼƬ��������
	putimagePng(pos_x, pos_y, &character_img);

	

}

inline void Character::updateY()
{
	//���ݴ�ֱ�Լ���Ծ״̬��������Ĵ�ֱ����
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
	//����ˮƽ״̬���������ˮƽ����
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
	//���������ǰһ״̬���¶���������֡,������״̬ת����ǰһ����֡��count����
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
	//�����Ҫ����������,���ڵ��Գ���
	TCHAR a[20]; // �����ַ�������
	_stprintf(a, _T("%d"), pos_x); // ��score1ת��Ϊ�ַ���
	settextstyle(70, 0, _T("����")); // �������ִ�С������
	settextcolor(RGB(100, 0, 0));  // ����������ɫ
	outtextxy(50+50, 100, a); // ����÷�����
	_stprintf(a, _T("%d"), pos_y); // ��score1ת��Ϊ�ַ���
	settextstyle(70, 0, _T("����")); // �������ִ�С������
	settextcolor(RGB(100, 0, 0));  // ����������ɫ
	outtextxy(200 + 50, 100, a); // ����÷�����
	_stprintf(a, _T("%d"), v_x); // ��score1ת��Ϊ�ַ���
	settextstyle(70, 0, _T("����")); // �������ִ�С������
	settextcolor(RGB(100, 0, 0));  // ����������ɫ
	outtextxy(50 + 50, 200, a); // ����÷�����
	_stprintf(a, _T("%d"), v_y); // ��score1ת��Ϊ�ַ���
	settextstyle(70, 0, _T("����")); // �������ִ�С������
	settextcolor(RGB(100, 0, 0));  // ����������ɫ
	outtextxy(200 + 50, 200, a); // ����÷�����
	_stprintf(a, _T("%d"), status_x); // ��score1ת��Ϊ�ַ���
	settextstyle(70, 0, _T("����")); // �������ִ�С������
	settextcolor(RGB(100, 0, 0));  // ����������ɫ
	outtextxy(50 + 50, 300, a); // ����÷�����
	_stprintf(a, _T("%d"), status_y); // ��score1ת��Ϊ�ַ���
	settextstyle(70, 0, _T("����")); // �������ִ�С������
	settextcolor(RGB(100, 0, 0));  // ����������ɫ
	outtextxy(200 + 50, 300, a); // ����÷�����
}
