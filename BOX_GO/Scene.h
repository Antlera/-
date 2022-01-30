#pragma once
#include<easyx.h>
#include<EasyXPng.h>
class Scene {
protected:
	IMAGE scene_img;//场景的图片对象
	int img_width;
	int img_height;//图片宽高
	int pos_x;
	int pos_y; // 图片坐标
	int icon_id; //像素块id
		//创建与删除场景
	

public:
	Scene();
	//Scene(IMAGE img,int x,int y);
	~Scene();
	//公有方法
	//访问器and修改器
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
{//用于更改图片,例如平台有多种图片,对应序号更改即可
	scene_img = img;
}

inline void Scene::drawImage()
{
	putimagePng(pos_x, pos_y, &scene_img);
}


class BackGround : public Scene
{
	//背景类
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
	//加载人物图像(原始图像换成向右站立的图像)
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

class Platform :public Scene//无伤害的平台场景
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
	//加载平台图像
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

class Barrier :public Scene {//带有伤害的障碍物场景
public:
	Barrier(int width, int height, int icon_id);
	~Barrier();
};

Barrier::Barrier(int width, int height, int icon_id)
{
	char filename[256];
	_stprintf(filename, "res\\barrier\\barrier%d.png", icon_id);
	//加载障碍图片
	loadimage(&scene_img, filename, 32, 32);
	img_width = scene_img.getwidth();
	img_height = scene_img.getheight();
}

inline Barrier::~Barrier()
{
}

class Torch :public Scene {//火炬场景
public:
	bool lighted = FALSE;  //火炬是否被点燃
	Torch();
	Torch(int width, int height, int icon_id);
	~Torch();
	void isLighted();//被点燃时调用，设置火把为点燃状态
};

inline Torch::Torch()
{

}
Torch::Torch(int width, int height, int icon_id)
{
	char filename[256];
	_stprintf(filename, "res\\torch\\lamp%d.png", icon_id);
	//加载火炬图片
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


class Key :public Scene {//火苗场景
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

class Door :public Scene {//通关大门场景
public:
	bool opened = FALSE;  //大门是否开启
	Door();
	Door(int width, int height, int icon_id);
	~Door();
	void setid(int i);
	void isOpened();//火把被点燃且主角互动时调用，设置为开启状态
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
	//加载门
	loadimage(&scene_img, filename, 96, 96);
	img_width = scene_img.getwidth();
	img_height = scene_img.getheight();
}

inline Door::~Door()
{
}
inline void Door::isOpened() //满足打开条件则打开
{
	opened = TRUE;
}
