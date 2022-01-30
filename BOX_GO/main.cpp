
#include "GamePanel.h"
#pragma comment(lib,"Winmm.lib")
using namespace std;

IMAGE startBackGround, title, startGuide;
void startInterface();
void startInterfaceInit();

//测试程序样例
int main() {
	GamePanel game ;
	int mode = 0;//游戏目前所处的阶段，0为开始界面，1为游戏进行
	game.startup();//游戏初始化
	startInterfaceInit();//开始界面初始化

	
	BeginBatchDraw();//开启批量绘制
	while (true) {
		if (mode == 0)//若为开始界面
		{
			startInterface();//开始界面绘制
			FlushBatchDraw();//更新批量绘制
			if (kbhit()) {
				if (GetAsyncKeyState('C'))
				{
					mode = 1;//按"C"进入游戏进行状态
				}
			}
		}
		if (mode == 1)
		{
			game.updateWithInput();//游戏输入有关更新
			game.updateWithoutInput();//游戏输入无关更新
			game.paint();//游戏元素绘制
			FlushBatchDraw();//更新批量绘制
		}
	}
	game.endGame(); //游戏结束
	EndBatchDraw();//结束批量绘制
}

void startInterfaceInit()
{
	//开始界面初始化
	loadimage(&startBackGround, "res\\start\\startBackGround.jpg", 1280, 1024);
	loadimage(&title, "res\\start\\title.png");
	loadimage(&startGuide, "res\\start\\guideToStart.png");
}

void startInterface()
{
	//开始界面绘制
	putimage(0, 0, &startBackGround);
	putimagePng(700, 180, &title);
	putimagePng(805, 847, &startGuide);

}
