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
	};//��Ϸ״̬����
	GamePanel();
	~GamePanel();
	void startup();//��ʼ����Ϸ
	void updateWithInput();//��������صĸ���
	void updateWithoutInput();//�������޹صĸ���
	void actWithScene();  //���������ж�
	void paint();//������Ϸͼ�η���
	void drawView();  //����������Ұ
	void endGame();//��Ϸ����
private:
	Timer timer;//��ʱ��
	IMAGE *img = new IMAGE;
	Character hero = Character("character");//�����������
	Character shadow = Character("shadow");//����Ӱ��
	BackGround bg = BackGround(WIDTH, HEIGHT,1);//��������
	thread threads[1000];//�̻߳�����
	int tds_cnt = 0;//�̼߳�����
	bool success = false;//��Ϸ�Ƿ�ʤ��
	int HP = 80;//����Ѫ��ֵ

	inline bool touchShadow() {
		//������Ӱ����ײ�ļ�ⷽ��
		bool touch = false;//��ʼ��Ϊ����ײ
		Point points[4];
		//��ʼ��������ĸ��ǵ�
		points[0].pos_x = hero.getX();
		points[0].pos_y = hero.getY();

		points[1].pos_x = hero.getX() + 32;
		points[1].pos_y = hero.getY();

		points[2].pos_x = hero.getX();
		points[2].pos_y = hero.getY() + 32;

		points[3].pos_x = hero.getX() + 32;
		points[3].pos_y = hero.getY() + 32;
		Rec shadow_rec;
		//��ʼ��Ӱ�ӵľ��η���
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

	HRGN viewArr;  // ������Ұ
	int graph_array; // ��Ұ�뾶
	int graph_num; // ��ͼ����

	//���˺�ƽ̨
	vector<Platform> platforms;

	//���(��Դ)���ڵ�����Ұ
	Torch fireWork_pre;
	Torch fireWork_later;


	//����
	Door door;
	vector<Door> door_open;//���Ŷ�����������
	int door_openstep;//���Ŷ������м�����

	//�ϰ�
	vector <Barrier> houl;

	//���磨Կ�ף����ڴ򿪴��������Ϸ
	Key key;
	//��Ϸͨ�ػ���
	IMAGE group;
};

GamePanel::GamePanel()
{
	int z[100][100] = { 0 };//��ͼ��id����
	FILE* fp = NULL;
	int i, j = 0, dat = 0, is;//iΪ�к�,jΪ�к�,datΪ��ʱ�ַ�����תint�����洢,isΪ������ŵ��ж�
	char* buff;
	char tmp;
	buff = (char*)malloc(sizeof(char) * 200);
	if ((fp = fopen("map1_base.csv", "r")) == NULL) {
		//���ļ���ʧ��
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
	fclose(fp);//�ر��ļ�ָ��
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 40; j++) {
			if (z[i][j] != -1) {
				Platform tmp = Platform(32, 32, z[i][j]);
				tmp.setX(j * 32);
				tmp.setY(i * 32);
				platforms.push_back(tmp);//����ͼ���������
			}
		}
	}

	//���������û�Դλ��
	fireWork_pre = Torch(64, 64, 1);
	fireWork_pre.setX(32 * 32);
	fireWork_pre.setY(12 * 32 + 7);


	fireWork_later = Torch(64, 64, 2);
	fireWork_later.setX(32 * 32);
	fireWork_later.setY(12 * 32 + 7);
	//���������ô���λ��
	door = Door(96, 96, 1);
	door.setX(35 * 32);
	door.setY(26 * 32);
	

	for (i = 1; i <= 5; i++) {
		Door tmp1 = Door(96, 96, i);
		tmp1.setX(35 * 32);
		tmp1.setY(26 * 32);
		door_open.push_back(tmp1);//����������ѹ������
	}

	//���������û���(Կ��)λ��
	key = Key(64, 64, 1);
	key.setX(17 * 32);
	key.setY(4 * 32);

	//��ʼ����Ұ�뾶Ϊ���뾶
	graph_array = MAXVIEW;
	//��ʼ������Ѫ��Ϊ���Ѫ��
	HP = 80;
	//��ʼ���뾶������
	graph_num = 0;
	//��ʼ�����Ŷ�������id
	door_openstep = 0;
	//���ؽ�������
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
		if (GetAsyncKeyState('A')) // ����
		{
			//��������״̬
			hero.setStatusX(Character::move_L);
			//������Ӱ��״̬�����̻߳�����
			threads[tds_cnt++] = thread(&Character::setStatusSX, &shadow, Character::move_L);
			threads[tds_cnt - 1].detach();
		}
		else if (GetAsyncKeyState('D')) // ����
		{
			//��������״̬
			hero.setStatusX(Character::move_R);
			//������Ӱ��״̬�����̻߳�����
			threads[tds_cnt++] = thread(&Character::setStatusSX, &shadow, Character::move_R);
			threads[tds_cnt - 1].detach();
		}
		else {
			if (hero.getStatusX() == Character::move_L) {
				//�������ƶ�תΪ������վ��
				hero.setStatusX(Character::stand_L);
			}
			if (hero.getStatusX() == Character::move_R) {
				//�������ƶ�תΪ����վ��
				hero.setStatusX(Character::stand_R);
			}
			if (shadow.getStatusX() == Character::move_L) {
				//�������ƶ�תΪ������վ��
				threads[tds_cnt++] = thread(&Character::setStatusSX, &shadow, Character::stand_L);
				threads[tds_cnt - 1].detach();
			}
			if (shadow.getStatusX() == Character::move_R) {
				//�������ƶ�תΪ����վ��
				threads[tds_cnt++] = thread(&Character::setStatusSX, &shadow, Character::stand_R);
				threads[tds_cnt - 1].detach();
			}

		}
		if (GetAsyncKeyState('W')) // ����
		{
			//��������״̬
			hero.setJump(true);
			//������Ӱ��״̬�����̻߳�����
			threads[tds_cnt++] = thread(&Character::setSJump, &shadow, true);
			threads[tds_cnt - 1].detach();
		}
		if (GetAsyncKeyState('H'))
		{
			hero.setReadyToAct(TRUE);  //�����Χ�Ƿ���Ի���
		}
	}
	if (tds_cnt > 900) {
		//���̻߳������߳�������900ʱ,���ƻ�����������,��ջ�����
		tds_cnt = 0;
	}
	
	
	if (graph_array < MINVIEW) graph_array = MINVIEW;//����Ұ�½�Ϊ��Сʱ,������Ұ״̬
	
}

inline void GamePanel::updateWithoutInput()
{
	//��������״̬���������λ��
	hero.updateX();
	hero.updateY();
	shadow.updateX();
	shadow.updateY();
	

	bool hits_hero = false;//�Ƿ���ĳ������ײ����״ֵ̬
	bool hits_shadow = false;
	for (auto it = platforms.begin(); it != platforms.end(); ++it) {
		if (hero.isHit(*it)) {
			hits_hero = true;//��������֮һײ����Ϊ��
		};
		if (shadow.isHit(*it)) {
			hits_shadow = true;//��������֮һײ����Ϊ��
		};
	}
	if (!hits_hero) {
		hero.setStatusY(Character::air);//��ȫ����ײ,��ô��Ȼ��֧��,��air״̬
	}
	if (!hits_shadow) {
		shadow.setStatusY(Character::air);//��ȫ����ײ,��ô��Ȼ��֧��,��air״̬
	}
	hero.die();
	if (hero.getReadyToAct()) // ��������˻�����������Ƿ���Ի���
	{
		actWithScene();
		hero.setReadyToAct(FALSE); // �ָ�ΪĬ��ֵ
	}

	if (door_openstep == 4)  // ���Ŷ����������������Ϊ��Ϸͨ��
	{
		success = TRUE;
	}
	if (door.opened && door_openstep <= 3)//���Ŷ���֡������
	{
		door.setid(door_openstep++);
	}
	

	if (hero.getFlame())   //ȡ�û���֮���ö���ʵ����Χ��Ұ
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

		drawView(); //��Ұ����
		bg.drawImage();//��������
		//hero.outputData();�������ݳ���

		if (!fireWork_pre.lighted)fireWork_pre.drawImage();//����δ�����Ļ�Դ
		else fireWork_later.drawImage();//���ƻ�����Ļ�Դ
		door_open[door_openstep].drawImage();//���ƴ���ͼƬ
		key.drawImage();//���ƻ���(Կ��)


		for (auto it = platforms.begin(); it != platforms.end(); ++it) {
			it->drawImage();//��������,����ƽ̨ͼ��
		}
		hero.updateCount();//�������ﶯ�����м�����
		hero.drawImage();//��������ͼ��
		shadow.updateCount();//����Ӱ�Ӷ������м�����
		shadow.drawImage();//����Ӱ��ͼ��

		// ɾ����������
		DeleteObject(viewArr);
		// ������������
		setcliprgn(NULL);
	}
	else if(!hero.getAlive() && success == FALSE)
	{
		//����Ϸ��������״̬
		bg.drawImage();
		settextstyle(200, 0, _T("����")); // �������ִ�С������
		settextcolor(RGB(100, 0, 0));  // ����������ɫ
		outtextxy(200, 360, "GAME OVER"); // �����Ϸ�������� 
		
	}
	else if (success == TRUE)
	{
		//����Ϸʤ��,����һ��ͼƬ
		bg.drawImage();
		putimagePng(60, 80, &group);

	}
	Sleep(10);

}


inline void GamePanel::actWithScene()  //������������
{
	if (hero.getReadyToAct() == TRUE)
	{
		if (hero.interact(fireWork_pre.getX(), fireWork_pre.getY(), 64, 64) == 1)  //�ж��Ƿ����ѻ���
		{
			if (hero.getFlame())  //�������ӵ�л���
			{
				fireWork_pre.isLighted();  //��ȼ���
			}
		}
		else if (hero.interact(key.getX(), key.getY(), 64, 64) == 1) //�ж��Ƿ�����绥��
		{
			hero.setFlame(TRUE); //ȡ�û���
			graph_num = 420;
		}
		else if (hero.interact(door.getX(), door.getY(), 96, 96) == 1)
		{
			if (fireWork_pre.lighted)  //�����ѵ�ȼ����ô��������Ŵ�
			{
				door.isOpened();
				
			}
		}
	}
}


inline void GamePanel::endGame()
{
	getchar();//�������ֻ���
	closegraph();//�رջ���
}
inline void GamePanel::drawView()
{
	// ������Ұ
	int r = 70  + HP + graph_num;	// ������Ұ�뾶
	
	//int r = 100;
	POINT orgin;
	orgin.x = hero.getX()+16;						// ��������Ļ�ϵ�λ��
	orgin.y = hero.getY()+16;						// ��������Ļ�ϵ�λ��
	viewArr = CreateEllipticRgn(orgin.x - r, orgin.y - r, orgin.x + r, orgin.y + r);	// ����һ��Բ�ε�����
	setcliprgn(viewArr);							// ��������
}