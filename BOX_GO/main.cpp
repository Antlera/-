
#include "GamePanel.h"
#pragma comment(lib,"Winmm.lib")
using namespace std;

IMAGE startBackGround, title, startGuide;
void startInterface();
void startInterfaceInit();

//���Գ�������
int main() {
	GamePanel game ;
	int mode = 0;//��ϷĿǰ�����Ľ׶Σ�0Ϊ��ʼ���棬1Ϊ��Ϸ����
	game.startup();//��Ϸ��ʼ��
	startInterfaceInit();//��ʼ�����ʼ��

	
	BeginBatchDraw();//������������
	while (true) {
		if (mode == 0)//��Ϊ��ʼ����
		{
			startInterface();//��ʼ�������
			FlushBatchDraw();//������������
			if (kbhit()) {
				if (GetAsyncKeyState('C'))
				{
					mode = 1;//��"C"������Ϸ����״̬
				}
			}
		}
		if (mode == 1)
		{
			game.updateWithInput();//��Ϸ�����йظ���
			game.updateWithoutInput();//��Ϸ�����޹ظ���
			game.paint();//��ϷԪ�ػ���
			FlushBatchDraw();//������������
		}
	}
	game.endGame(); //��Ϸ����
	EndBatchDraw();//������������
}

void startInterfaceInit()
{
	//��ʼ�����ʼ��
	loadimage(&startBackGround, "res\\start\\startBackGround.jpg", 1280, 1024);
	loadimage(&title, "res\\start\\title.png");
	loadimage(&startGuide, "res\\start\\guideToStart.png");
}

void startInterface()
{
	//��ʼ�������
	putimage(0, 0, &startBackGround);
	putimagePng(700, 180, &title);
	putimagePng(805, 847, &startGuide);

}
