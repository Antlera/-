#pragma once
#include<easyx.h>
#include<EasyXPng.h>
class Scene {
protected:
	IMAGE scene_img;//������ͼƬ����
	int img_width;
	int img_height;//ͼƬ���
	int pos_x;
	int pos_y; // ͼƬ����
	int icon_id; //���ؿ�id
		//������ɾ������
	

public:
	Scene();
	//Scene(IMAGE img,int x,int y);
	~Scene();
	//���з���
	//������and�޸���
	int getX();
	int getY();
	int getid();
	int getImg_width();
	int getImg_height();
	void setX(const int x);
	void setY(const int y);
	void setImage(IMAGE img);
	void drawImage();
};

Scene::Scene()
{

}



inline Scene::~Scene()
{
}

inline int Scene::getX()
{
	return pos_x;
}

inline int Scene::getY()
{
	return pos_y;
}

inline int Scene::getid() 
{
	return icon_id;
}
inline int Scene::getImg_width()
{
	return img_width;
}

inline int Scene::getImg_height()
{
	return img_height;
}
inline void Scene::setX(const int x)
{
	pos_x = x;
}

inline void Scene::setY(const int y)
{
	pos_y = y;
}

inline void Scene::setImage(IMAGE img)
{//���ڸ���ͼƬ,����ƽ̨�ж���ͼƬ,��Ӧ��Ÿ��ļ���
	scene_img = img;
}

inline void Scene::drawImage()
{
	putimagePng(pos_x, pos_y, &scene_img);
}


class BackGround : public Scene
{
	//������
public:
	BackGround(int width, int height, int icon_id);
	~BackGround();
	void drawImage();
private:

};

BackGround::BackGround(int width,int height, int icon_id)
{
	char filename[256];
	_stprintf(filename, "res\\background\\background%d.png", icon_id);
	//��������ͼ��(ԭʼͼ�񻻳�����վ����ͼ��)
	loadimage(&scene_img, filename, width, height);
	img_width = scene_img.getwidth();
	img_height = scene_img.getheight();
	pos_x = 0;
	pos_y = 0;
}
inline void BackGround::drawImage() {
	putimage(pos_x, pos_y, &scene_img);
};
inline BackGround::~BackGround()
{

}

class Platform :public Scene//���˺���ƽ̨����
{
public:
	Platform(int width, int height, int icon_id);
	~Platform();
	void drawImage();
};

Platform::Platform(int width, int height, int icon_id)
{
	char filename[256];
	_stprintf(filename, "res\\platform\\platform%d.png", icon_id);
	//����ƽ̨ͼ��
	loadimage(&scene_img, filename, 32, 32);
	img_width = scene_img.getwidth();
	img_height = scene_img.getheight();
}
inline void Platform::drawImage() {
	putimage(pos_x, pos_y, &scene_img);
};
inline Platform::~Platform()
{

}

class Barrier :public Scene {//�����˺����ϰ��ﳡ��
public:
	Barrier(int width, int height, int icon_id);
	~Barrier();
};

Barrier::Barrier(int width, int height, int icon_id)
{
	char filename[256];
	_stprintf(filename, "res\\barrier\\barrier%d.png", icon_id);
	//�����ϰ�ͼƬ
	loadimage(&scene_img, filename, 32, 32);
	img_width = scene_img.getwidth();
	img_height = scene_img.getheight();
}

inline Barrier::~Barrier()
{
}

class Torch :public Scene {//��泡��
public:
	bool lighted = FALSE;  //����Ƿ񱻵�ȼ
	Torch();
	Torch(int width, int height, int icon_id);
	~Torch();
	void isLighted();//����ȼʱ���ã����û��Ϊ��ȼ״̬
};

inline Torch::Torch()
{

}
Torch::Torch(int width, int height, int icon_id)
{
	char filename[256];
	_stprintf(filename, "res\\torch\\lamp%d.png", icon_id);
	//���ػ��ͼƬ
	loadimage(&scene_img, filename, 96, 96);
	img_width = scene_img.getwidth();
	img_height = scene_img.getheight();
}

inline Torch::~Torch()
{
	
}
inline void Torch::isLighted()
{
	lighted = TRUE;
}


class Key :public Scene {//���糡��
public:

	Key(int width, int height, int icon_id);
	Key();
	~Key();;
};

Key::Key() {

}
Key::Key(int width, int height, int icon_id)
{
	char filename[256];
	_stprintf(filename, "res\\key\\key.png", icon_id);
	loadimage(&scene_img, filename, 64, 64);
	img_width = scene_img.getwidth();
	img_height = scene_img.getheight();
}
inline Key::~Key()
{
}

class Door :public Scene {//ͨ�ش��ų���
public:
	bool opened = FALSE;  //�����Ƿ���
	Door();
	Door(int width, int height, int icon_id);
	~Door();
	void setid(int i);
	void isOpened();//��ѱ���ȼ�����ǻ���ʱ���ã�����Ϊ����״̬
};

inline Door::Door()
{
}
inline void Door::setid(int i) {
	icon_id = i;
}
Door::Door(int width, int height, int icon_id)
{
	char filename[256];
	_stprintf(filename, "res\\door\\door%d.png", icon_id);
	//������
	loadimage(&scene_img, filename, 96, 96);
	img_width = scene_img.getwidth();
	img_height = scene_img.getheight();
}

inline Door::~Door()
{
}
inline void Door::isOpened() //������������
{
	opened = TRUE;
}
