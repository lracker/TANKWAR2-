#include "pch.h"
#include <iostream>
#include "CGameObj.h"
#pragma comment(lib, "Winmm.lib")
int main()
{
	FullScreen();		//设置全屏
	PlaySoundA("Sound\\begin.wav", NULL, SND_ASYNC | SND_NODEFAULT); // 播放音乐
	DrawWelcome();		//设置欢迎界面
	PrintMenu();		//打印菜单
	return 0;
}

