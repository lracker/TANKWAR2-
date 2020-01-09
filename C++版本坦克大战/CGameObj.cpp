#include "pch.h"
#include "CGameObj.h"
using std::cout;
using std::endl;

//*****************************************************************************************
// 函数名称: StartGame
// 函数说明: 开始游戏
// 作    者: lracker
// 时    间: 2019/08/19
// 返 回 值: void
//*****************************************************************************************
void CGameObj::StartGame()
{
	system("cls");
	InitMap();
	CStaticObjList StaticObjList;										//初始化链表
	CGrassPosList g_GrassPosList;										//建立草地链表来记录草地的坐标
	CDynamicObjList DynamicObjList;										//初始化动态链表
	int ret = ChoiceMap(StaticObjList, DynamicObjList, g_GrassPosList);
	if (ret == 3)  //选择退出了
	{
		return;
	}
	ScoreBoard();														//打印出计分板
	DynamicObjList.CreatePlayer(g_nFRIENDNUM);							//插入一个玩家
	DynamicObjList.CreateEnery(g_nENEMYNUM);						    //链表插入，插入n个敌人
	DynamicObjList.DrawObj(DRAW);									    //画出动态物品
	Gaming(DynamicObjList, StaticObjList, g_GrassPosList);				//开始游戏
	return;
}

//*****************************************************************************************
// 函数名称: InitMap
// 函数说明: 初始化地图内容全为空
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: void
//*****************************************************************************************
void CGameObj::InitMap()
{
	for (int i = 0; i < MaxX; i++)
	{
		for (int j = 0; j < MaxY; j++)
		{
			g_Map[i][j] = BLANK;
		}
	}
}

//*****************************************************************************************
// 函数名称: ContinueGame
// 函数说明: 继续游戏
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: const char * szFileName
// 返 回 值: void
//*****************************************************************************************
void CGameObj::ContinueGame(const char* szFileName)
{
	system("cls");
	InitMap();															//初始化地图
	CStaticObjList StaticObjList;										//初始化链表
	CGrassPosList g_GrassPosList;										//建立草地链表来记录草地的坐标
	CDynamicObjList DynamicObjList;
	ReadMap(szFileName, StaticObjList, g_GrassPosList, DynamicObjList);  //读取暂停的存档
	CDynamicObjList::CDynamicObjNode* p = DynamicObjList.m_pHead->m_next;
	for (int i = 0; i < g_nFRIENDNUM; i++)
	{
		DynamicObjList.m_pPlayer[i] = p;  //获取玩家个数。并保存到指针里
		p++;
	}
	Gaming(DynamicObjList, StaticObjList, g_GrassPosList);
}


//*****************************************************************************************
// 函数名称: Gaming
// 函数说明:
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: CDynamicObjList & DynamicObjList
// 参    数: CStaticObjList & StaticObjList
// 参    数: CGrassPosList g_GrassPosList
// 返 回 值: void
//*****************************************************************************************
void CGameObj::Gaming(CDynamicObjList& DynamicObjList, CStaticObjList& StaticObjList, CGrassPosList g_GrassPosList)
{
	CTankObj* pPlayer1 = (CTankObj*)DynamicObjList.m_pPlayer[0]->m_pDynamicObj;
	nGameOver = FALSE;													//设定一开始还活着
	while (DynamicObjList.IsExist() && !nGameOver)
	{
		DynamicObjList.DrawHealthAndScore();							//打印出成绩和生命值
		if (_kbhit())													//玩家按下键盘
		{
			int ESC = DynamicObjList.WhichOrder();
			if (ESC == 1)
			{
				int RetSuspend = SuspendGame(StaticObjList, DynamicObjList);
				if (RetSuspend == 3)  //回到菜单
				{
					nGameOver = TRUE;
				}
				else
					StaticObjList.DrawObj();  //把东西再画一遍
			}
		}
		if (nGameOver)  //如果暂停的时候选择了回到菜单，就不用重新打印一遍动态物体再退出了。
		{
			break;
		}
		DynamicObjList.AllMove(StaticObjList, g_GrassPosList);
		if (pPlayer1->GetScore() == 4)//如果到达四分了，那就进入下一关
		{
			static int num = 0;
			num++;
			char szNum[2] = { 0 };
			sprintf_s(szNum, 2, "%d", num);
			char Buffer[10] = "Mission";
			strcat_s(Buffer, 10, szNum);
			system("cls");
			WriteChar({ Text_LOCX,Text_LOCY }, "下一关", 0xf);
			Sleep(1000);
			system("cls");
			ContinueGame(Buffer);
		}
	}
	system("cls");
	WriteChar({ Text_LOCX, Text_LOCY }, "YOU FAIL");
	Sleep(1500);
	return;
}

//*****************************************************************************************
// 函数名称: SizeOfWindow
// 函数说明: 全屏要用到的函数
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: HANDLE hConsoleOutput
// 返 回 值: SMALL_RECT
//*****************************************************************************************
SMALL_RECT SizeOfWindow(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hConsoleOutput, &info);
	return info.srWindow;
}

//*****************************************************************************************
// 函数名称: FullScreen
// 函数说明: 全屏函数并且负责隐藏鼠标等信息
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: void
//*****************************************************************************************
void FullScreen()
{
	HANDLE hOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
	/************设置全屏************/
	HWND hWnd = GetForegroundWindow();  //获取前景窗口句柄  
	int nCx = GetSystemMetrics(SM_CXSCREEN);  //检索屏幕宽度  
	int nCy = GetSystemMetrics(SM_CYSCREEN);  //检索屏幕高度  
	LONG l_WinStyle = GetWindowLong(hWnd, GWL_STYLE);  //窗口样式  
	SetWindowLong(hWnd, GWL_STYLE, (l_WinStyle | WS_POPUP | WS_MAXIMIZE) & ~WS_CAPTION & ~WS_THICKFRAME & ~WS_BORDER);  // 设置窗口信息 最大化 取消标题栏及边框
	SetWindowPos(hWnd, HWND_TOP, 0, 0, nCx, nCy, 0);
	/************设置无光标************/
	CONSOLE_CURSOR_INFO cci;  //设置光标信息的结构体
	cci.dwSize = 1;  //一定要赋值才嫩隐藏
	cci.bVisible = FALSE;  //设置为不可见
	SetConsoleCursorInfo(hOutPut, &cci);  //设置光标信息
	/************设置无滚动条************/
	CONSOLE_SCREEN_BUFFER_INFO info;  //设置缓冲区信息的结构体
	GetConsoleScreenBufferInfo(hOutPut, &info);
	SMALL_RECT rect = SizeOfWindow(hOutPut);  //、/获取窗口size
	COORD size = { rect.Right + 1,rect.Bottom + 1 };	//定义缓冲区大小 
	SetConsoleScreenBufferSize(hOutPut, size);
	/**************一开始设置输入法为英文***************/
	keybd_event(VK_SHIFT, 0, 0, 0);
	Sleep(100);
	keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);  //切换英文
}

//*****************************************************************************************
// 函数名称: DrawWelcome
// 函数说明: 打印出欢迎界面
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: void
//*****************************************************************************************
void DrawWelcome()
{
	system("cls");
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = {};
	SetConsoleCursorPosition(hOutput, position);
	SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "■■■■■■     ■         ■       ■ ■    ■      " << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "    ■■       ■  ■       ■ ■    ■ ■   ■       " << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "    ■■      ■    ■      ■  ■   ■ ■  ■     " << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "    ■■     ■■■■■     ■   ■  ■ ■■" << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "    ■■    ■        ■    ■    ■ ■ ■  ■              " << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "    ■■   ■          ■   ■     ■■ ■    ■            " << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "    ■■ ■              ■ ■       ■ ■      ■          " << endl;

	Sleep(1000);
}

//*****************************************************************************************
// 函数名称: PrintMenu
// 函数说明: 打印出菜单
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: void
//*****************************************************************************************
void PrintMenu()
{
	while (true)
	{
		system("cls");
		CGameObj Game;
		system("cls");
		WriteChar({ Text_LOCX, Text_LOCY - 4 }, "1.开始游戏");
		WriteChar({ Text_LOCX, Text_LOCY - 2 }, "2.继续游戏");
		WriteChar({ Text_LOCX, Text_LOCY + 0 }, "3.自定义地图");
		WriteChar({ Text_LOCX, Text_LOCY + 2 }, "4.双人作战");
		WriteChar({ Text_LOCX, Text_LOCY + 4 }, "5.删除地图");
		WriteChar({ Text_LOCX, Text_LOCY + 6 }, "6.退出游戏");
		char MenuChoice = _getch() - 0x30;
		switch (MenuChoice)
		{
		case 1:
			g_nFRIENDNUM = 1;		//设置玩家数量
			Game.StartGame();
			break;
		case 2:
			Game.ContinueGame("Suspend");
			break;
		case 3:
			CustomMap();
			break;
		case 4:
			g_nFRIENDNUM = 2;
			Game.StartGame();
			break;
		case 5:
		{
			CStaticObjList test1;  //没用的，用来填充参数
			CGrassPosList test3;
			CDynamicObjList test2;
			LoadDeleteMap(Delete, test1, test2, test3);
			break;
		}
		case 6:
			exit(0);
			break;
		default:
			break;
		}
	}
}

//*****************************************************************************************
// 函数名称: ScoreBoard
// 函数说明: 打印出计分板
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: void
//*****************************************************************************************
void ScoreBoard()
{
	for (short i = g_ScoreX; i < g_ScoreX + 15; i++)
	{
		for (short j = 0; j < g_ScoreY; j++)
		{
			if (i == g_ScoreX || i == 74 || j == 0 || j == g_ScoreY - 1)
			{
				WriteChar({i, j}, "※", FOREGROUND_GREEN | FOREGROUND_RED);
			}
		}
	}
}

//*****************************************************************************************
// 函数名称: CustomMap
// 函数说明: 自定义地图
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: void
//*****************************************************************************************
void CustomMap()
{
	system("cls");
	CreateFolder();  //创建文件夹来存放地图
	CDynamicObjList DynamicObjList;
	CStaticObjList StaticObjList;
	/********画出墙*********/
	StaticObjList.CreateWall();	//画出围墙
	StaticObjList.CreateHome(); //画出家
	StaticObjList.DrawObj();	//画出来
	Tips();					    //画出提示框
	/**********获取鼠标事件和键盘事件****************/
	HANDLE hInPut = GetStdHandle(STD_INPUT_HANDLE);  //获取句柄
	INPUT_RECORD ir = {};  //设置结构体
	DWORD dwCount = 0;
	int nChoice = 0;
	bool keyDown = false;
	SetConsoleMode(hInPut, ENABLE_MOUSE_INPUT);
	while (true)
	{
		ReadConsoleInput(hInPut, &ir, 1, &dwCount);
		if (ir.EventType == MOUSE_EVENT)  //获取鼠标事件
		{
			if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)  //画出来
			{
				COORD pos = ir.Event.MouseEvent.dwMousePosition;
				CStaticObjList::CStaticObjNode* pNode = new CStaticObjList::CStaticObjNode;
				if (pos.X / 2 >= g_ScoreX + 3 && pos.X / 2 <= g_ScoreX + 7 && pos.Y == 7)	   //判断是否按了木头
				{
					nChoice = EARTH;
					keyDown = true;
					continue;
				}
				else if (pos.X / 2 >= g_ScoreX + 3 && pos.X / 2 <= g_ScoreX + 7 && pos.Y == 9)		//判断是否按了铁墙
				{
					nChoice = IRON;
					keyDown = true;
					continue;
				}
				else if (pos.X / 2 >= g_ScoreX + 3 && pos.X / 2 <= g_ScoreX + 7 && pos.Y == 11)		//判断是否按了河流
				{
					nChoice = WATER;
					keyDown = true;
					continue;
				}
				else if (pos.X / 2 >= g_ScoreX + 3 && pos.X / 2 <= g_ScoreX + 7 && pos.Y == 13)		//判断是否按了草地
				{
					nChoice = GRASS;
					keyDown = true;
					continue;
				}
				if (!keyDown)  //如果没有进行选择的话。那就不能画了
				{
					continue;
				}
				if (pos.X / 2 > MaxX - 2 || pos.X / 2 < 1 || pos.Y > MaxY - 2 || pos.Y < 1)		  //判断是否超过围墙了
				{
					continue;
				}
				if ((pos.X/2 == 27&&pos.Y==42)|| (pos.X / 2 == 27 && pos.Y == 43) || (pos.X / 2 == 28 && pos.Y == 42) || (pos.X / 2 == 28 && pos.Y == 43))  //是否碰到将军了
				{
					continue;
				}
				switch (nChoice)
				{
				case EARTH:
				{
					CEarth* obj = new CEarth;
					obj->Birth({ pos.X / 2,pos.Y });
					pNode->m_Staticobj = obj;
					break;
				}
				case IRON:
				{
					CIron* obj = new CIron;
					obj->Birth({ pos.X / 2,pos.Y });
					pNode->m_Staticobj = obj;
					break;
				}
				case WATER:
				{
					CWater* obj = new CWater;
					obj->Birth({ pos.X / 2,pos.Y });
					pNode->m_Staticobj = obj;
					break;
				}
				case GRASS:
				{
					CGrass* obj = new CGrass;
					obj->Birth({ pos.X / 2,pos.Y });
					pNode->m_Staticobj = obj;
					break;
				}
				default:
					break;
				}
				pNode->m_Staticobj->Draw(DRAW);
				StaticObjList.InsertCStaticObjList(pNode);
				g_Map[pos.X / 2][pos.Y] = STATIC;
			}
			if (ir.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)  //设置空地，撤销障碍物
			{
				COORD pos = ir.Event.MouseEvent.dwMousePosition;
				CStaticObjList::CStaticObjNode* pNode = new CStaticObjList::CStaticObjNode;
				pNode = StaticObjList.m_pHead->m_next;
				if (pos.X / 2 > MaxX - 2 || pos.X / 2 < 1 || pos.Y > MaxY - 2 || pos.Y < 1)				//判断是否超过围墙了
				{
					continue;
				}
				if ((pos.X / 2 == 27 && pos.Y == 42) || (pos.X / 2 == 27 && pos.Y == 43) || (pos.X / 2 == 28 && pos.Y == 42) || (pos.X / 2 == 28 && pos.Y == 43))  //是否碰到将军了
				{
					continue;
				}
				while (pNode)
				{
					if (pNode->m_Staticobj->IsBoundary({ pos.X / 2,pos.Y }))  //如果找到该物体了，从链表中删除它
					{
						pNode->m_Staticobj->Draw(CLEAN);
						if (!pNode->m_next)		//如果删到最后一个了						
						{
							pNode->m_prior->m_next = pNode->m_next;
							pNode->m_prior = NULL;
						}
						else
						{
							pNode->m_prior->m_next = pNode->m_next;
							pNode->m_next->m_prior = pNode->m_prior;
						}
						delete pNode;
						pNode = NULL;
						break;
					}
					pNode = pNode->m_next;
				}
				g_Map[pos.X / 2][pos.Y] = BLANK;
			}
		}
		if (ir.EventType == KEY_EVENT)
		{
			if (ir.Event.KeyEvent.wVirtualKeyCode == VK_RETURN)  //如果按下回车了
			{
				struct tm t;   //tm结构指针
				time_t now;  //声明time_t类型指针
				time(&now);
				localtime_s(&t, &now);
				char tmp[64];
				char tmp2[64];
				strftime(tmp, sizeof(tmp), "Map\\%Y年%m月%d日%H点%M分%S秒", &t);  //用时间来做文件名
				strftime(tmp2, sizeof(tmp2), "Map\\%Y年%m月%d日%H点%M分%S秒", &t);
				SaveMap(&StaticObjList, &DynamicObjList, tmp);  //保存地图
				SaveMapName(tmp2);  //保存地图名
				system("cls");
				break;
			}
			else if (ir.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)  //不保存
			{
				system("cls");
				break;
			}
		}
	}
}

//*****************************************************************************************
// 函数名称: CreateFolder
// 函数说明: 创建文件夹
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: bool
//*****************************************************************************************
bool CreateFolder()
{
	//文件夹名称
	char folderName[] = "Map";
	// 文件夹不存在则创建文件夹
	if (_access(folderName, 0) == -1)
	{
		_mkdir(folderName);
	}
	return false;
}

//*****************************************************************************************
// 函数名称: Tips
// 函数说明: 画出提示板
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: void
//*****************************************************************************************
void Tips()
{
	for (short i = g_ScoreX; i < 75; i++)
	{
		for (short j = 0; j < g_ScoreY; j++)
		{
			if (i == g_ScoreX || i == 74 || j == 0 || j == g_ScoreY - 1)
			{
				WriteChar({ i, j }, "※", FOREGROUND_RED);
			}
			if (i == (g_ScoreX + 3) && j == 3)
			{
				WriteChar({ i, j }, "回车：保存");
				WriteChar({ i, j + 2 }, "ESC：撤销绘图");
				WriteChar({ i, j + 4 }, "▓ 土墙", FOREGROUND_GREEN | FOREGROUND_RED);
				WriteChar({ i, j + 6 }, "■ 铁墙", FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
				WriteChar({ i, j + 8 }, "∷ 河流", FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				WriteChar({ i, j + 10 }, "※ 草地", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			}
		}
	}
}

//*****************************************************************************************
// 函数名称: SaveMap
// 函数说明: 保存地图
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: CStaticObjList * StaticObjList
// 参    数: CDynamicObjList * DynamicObjList
// 参    数: const char * c_szFileName
// 返 回 值: void
//*****************************************************************************************
void SaveMap(CStaticObjList* StaticObjList, CDynamicObjList* DynamicObjList, const char* c_szFileName)
{
	CStaticObjList::CStaticObjNode* pStaticNode = StaticObjList->m_pHead->m_next;  //指向首节点
	FILE *pFile = NULL;
	fopen_s(&pFile, c_szFileName, "wb+");  //打开文件
	char buffer[10] = { 0 };
	sprintf_s(buffer, "%d", StaticObjList->GetLen());
	fwrite(buffer, sizeof(buffer), 1, pFile); //写入静态链表的长度
	fclose(pFile);
	while (pStaticNode)
	{
		pStaticNode->m_Staticobj->Save(c_szFileName);
		pStaticNode = pStaticNode->m_next;
	}
	if (DynamicObjList)  //如果这个指针不是指空的话，那就保存动态的
	{
		FILE *pFile = NULL;
		fopen_s(&pFile, c_szFileName, "ab+");  //打开文件
		CDynamicObjList::CDynamicObjNode* pDynamicNode = DynamicObjList->m_pHead->m_next;
		char buffer2[10] = { 0 };
		sprintf_s(buffer2, "%d", DynamicObjList->GetLen());
		fwrite(buffer2, sizeof(buffer2), 1, pFile); //写入动态链表的长度
		fclose(pFile);
		while (pDynamicNode)
		{
			pDynamicNode->m_pDynamicObj->Save(c_szFileName);
			pDynamicNode = pDynamicNode->m_next;
		}
	}

}

//*****************************************************************************************
// 函数名称: SaveMapName
// 函数说明: 保存地图名
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: const char * c_szFileName
// 返 回 值: void
//*****************************************************************************************
void SaveMapName(const char* c_szFileName)
{
	FILE *pFile = NULL;
	fopen_s(&pFile, "Map\\MapName.txt", "ab+");
	fprintf(pFile, "%s ", c_szFileName);
	fclose(pFile);
	pFile = NULL;
}

//*****************************************************************************************
// 函数名称: ReadMap
// 函数说明: 读取地图
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: const char * c_szFileName
// 参    数: CStaticObjList & StaticObjList
// 参    数: CGrassPosList & GrassPosList
// 参    数: CDynamicObjList & DynamicObjList
// 返 回 值: void
//*****************************************************************************************
void ReadMap(const char* c_szFileName, CStaticObjList& StaticObjList, CGrassPosList& GrassPosList, CDynamicObjList& DynamicObjList)
{
	system("cls");
	FILE *pFile = NULL;
	fopen_s(&pFile, c_szFileName, "rb+");  //打开文件
	char buffer[10] = { 0 };
	fread(buffer, sizeof(buffer), 1, pFile); //写入静态链表的长度
	int nStaticLen = atoi(buffer);			 //获取链表长度		
	for (int i = 0; i < nStaticLen; i++)
	{
		CStaticObjList::CStaticObjNode* pNode = new CStaticObjList::CStaticObjNode;
		char szBuffer[10] = { 0 };
		int nName = 0;
		fread(szBuffer, sizeof(szBuffer), 1, pFile);
		nName = atoi(szBuffer);
		switch (nName)
		{
		case WALL:
		{
			CWALL* pobj = new CWALL;
			fread((int*)pobj+1, sizeof(CWALL)-4, 1, pFile); //写入静态链表的长度
			pNode->m_Staticobj = pobj;
			break;
		}
		case EARTH:
		{
			CEarth* pobj = new CEarth;
			fread((int*)pobj + 1, sizeof(CEarth)-4, 1, pFile); //写入静态链表的长度
			pNode->m_Staticobj = pobj;
			break;
		}
		case IRON:
		{
			CIron* pobj = new CIron;
			fread((int*)pobj + 1, sizeof(CIron)-4, 1, pFile); //写入静态链表的长度
			pNode->m_Staticobj = pobj;
			break;
		}
		case WATER:
		{
			CWater* pobj = new CWater;
			fread((int*)pobj + 1, sizeof(CWater)-4, 1, pFile); //写入静态链表的长度
			pNode->m_Staticobj = pobj;
			break;
		}
		case GRASS:
		{
			CGrass* pobj = new CGrass;
			fread((int*)pobj + 1, sizeof(CGrass)-4, 1, pFile); //写入静态链表的长度
			pNode->m_Staticobj = pobj;
			CGrassPosList::GrassPos* pPos = new CGrassPosList::GrassPos;
			pPos->pos = pobj->GetPos();
			GrassPosList.GrassPosInsert(pPos);
			CGrass::nGRASSNUMBER++;
			break;
		}
		case GEN:
		{
			CGen* pobj = new CGen;
			fread((int*)pobj + 1, sizeof(CGen) - 4, 1, pFile); //写入静态链表的长度
			pNode->m_Staticobj = pobj;
			break;
		}
		default:
			break;
		}
		StaticObjList.InsertCStaticObjList(pNode);
		g_Map[pNode->m_Staticobj->GetPos().X][pNode->m_Staticobj->GetPos().Y] = STATIC;
	}
	StaticObjList.DrawObj();  //画出所有静态物体

	char buffer2[10] = { 0 };
	fread(buffer2, sizeof(buffer), 1, pFile); //写入静态链表的长度
	int nStaticLen2 = atoi(buffer2);			 //获取链表长度		
	if (nStaticLen2)
	{
		for (int i = 0; i < nStaticLen2; i++)
		{
			CDynamicObjList::CDynamicObjNode* pNode = new CDynamicObjList::CDynamicObjNode;
			char szBuffer2[10] = { 0 };
			int nName2 = 0;
			fread(szBuffer2, sizeof(szBuffer2), 1, pFile);
			nName2 = atoi(szBuffer2);
			switch (nName2)
			{
			case BULLET:
			{
				CBulletObj* pobj = new CBulletObj;
				fread((int*)pobj + 1, sizeof(CBulletObj) - 4, 1, pFile); //写入静态链表的长度
				pobj->UpDate();
				pNode->m_pDynamicObj = pobj;
				break;
			}
			case TANK:
			{
				CTankObj* pobj = new CTankObj;
				fread((int*)pobj + 1, sizeof(CTankObj) - 4, 1, pFile); //写入静态链表的长度
				pobj->UpDate();
				pNode->m_pDynamicObj = pobj;
				break;
			}
			default:
				break;
			}
			DynamicObjList.InsertNode(pNode);
			g_Map[pNode->m_pDynamicObj->RetPos().X][pNode->m_pDynamicObj->RetPos().Y] = DYNAMIC;
		}
	}
	fclose(pFile);
	pFile = NULL;
}

//*****************************************************************************************
// 函数名称: ChoiceMap
// 函数说明: 选择地图
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: CStaticObjList & StaticObjList
// 参    数: CDynamicObjList & DynamicObjList
// 参    数: CGrassPosList & g_GrassPosList
// 返 回 值: int
//*****************************************************************************************
int ChoiceMap(CStaticObjList& StaticObjList, CDynamicObjList& DynamicObjList, CGrassPosList& g_GrassPosList)
{
	int nChoice = 0;
	while (TRUE)
	{
		system("cls");
		WriteChar({ Text_LOCX, Text_LOCY }, "1.使用随机地图");
		WriteChar({ Text_LOCX, Text_LOCY + 2 }, "2.使用自定义地图");
		WriteChar({ Text_LOCX, Text_LOCY + 4 }, "3.回到菜单");

		nChoice = _getch() - 0x30;
		switch (nChoice)
		{
		case 1:
			system("cls");
			StaticObjList.CreateWall();										    //建立围墙
			StaticObjList.CreateHome();										    //建立家
			StaticObjList.CreateEarth(10);										//建立树木
			StaticObjList.CreateIron(20);								        //建立铁块
			StaticObjList.CreateWater(10);										//建立水
			StaticObjList.CreateGrass(GrassNumber, &g_GrassPosList);			//建立草地
			StaticObjList.DrawObj();											//画出静态物品
			return 1;
		case 2:
			if (LoadDeleteMap(Load, StaticObjList, DynamicObjList, g_GrassPosList)) //使用自定义地图
			{
				return 2;
			}
			break;
		case 3:
			return 3;
		default:
			break;
		}
	}
}

//*****************************************************************************************
// 函数名称: LoadDeleteMap
// 函数说明: 读取或者删除地图
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: int LoadOrDelete
// 参    数: CStaticObjList & StaticObjList
// 参    数: CDynamicObjList DynamicObjList
// 参    数: CGrassPosList & GrassPosList
// 返 回 值: bool
//*****************************************************************************************
bool LoadDeleteMap(int LoadOrDelete, CStaticObjList& StaticObjList, CDynamicObjList DynamicObjList, CGrassPosList& GrassPosList)
{
	int nExit = 0;		 //判断地图是否存在
	system("cls");
	FILE *pFile = NULL;
	fopen_s(&pFile, "Map\\MapName.txt", "rb");  //打开地图名索引文件
	if (!pFile)  //如果没有这个文件会提示
	{
		WriteChar({ Text_LOCX, Text_LOCY }, "请先自定义地图");
		Sleep(1000);
		return false;
	}
	if (fgetc(pFile) == EOF)	//如果地图删完了的话就会提示
	{
		WriteChar({ Text_LOCX, Text_LOCY }, "请先自定义地图");
		Sleep(1000);
		fclose(pFile);
		pFile = NULL;
		return false;
	}
	else
	{
		rewind(pFile);		  //由于fgetc的原因，使用重置pFile
		if (LoadOrDelete == Load)
		{
			WriteChar({ Text_LOCX - 5, Text_LOCY }, "请输入使用的地图序号");
		}
		else if (LoadOrDelete == Delete)
		{
			WriteChar({ Text_LOCX - 5, Text_LOCY }, "请输入删除地图序号");
		}
		PMapName MapnameList = (PMapName)malloc(sizeof(MapName) * 1);
		MapnameList->next = NULL;
		int ret = 0;		   //检测是否读到文件尾
		int i = 1;				//记录地图序号
		char MapNum[10];
		while (true)  //从文件中读取地图名
		{
			PMapName temp = (PMapName)malloc(sizeof(MapName) * 1);
			ret = fscanf_s(pFile, "%s ", temp->m_szMapName, 64);
			temp->m_nChoice = i++;
			if (ret == EOF)
			{
				break;
			}
			temp->next = MapnameList->next;
			MapnameList->next = temp;
		}
		fclose(pFile);
		pFile = NULL;
		PMapName P = MapnameList;	   //定义指针指向链表头
		while (P->next)  //打印地图名列表
		{
			sprintf_s(MapNum, "%d", P->next->m_nChoice);
			WriteChar({(short) (Text_LOCX - 5), (short)(Text_LOCY + 2 * P->next->m_nChoice) }, MapNum);
			WriteChar({ (short)(Text_LOCX - 3), (short)(Text_LOCY + 2 * P->next->m_nChoice) }, P->next->m_szMapName);
			P = P->next;
			memset(MapNum, 0, 10);
		}
		P = MapnameList;				//重置指针
		int MapChoice = _getch() - 0x30;  //获取地图序号
		char szSeletedMapName[64] = { 0 };			   //保存文件名
		while (P->next)
		{
			if (MapChoice == P->next->m_nChoice)
			{
				strcpy_s(szSeletedMapName, 64, P->next->m_szMapName);  //根据输入来获取地图名字
				nExit = 1;
				break;
			}
			P = P->next;
		}
		if (!nExit)
		{
			system("cls");
			WriteChar({ Text_LOCX, Text_LOCY }, "请输入正确的序号");
			Sleep(1000);
			return false;
		}
		switch (LoadOrDelete)
		{
		case 1:  //1是加载地图
		{
			ReadMap(szSeletedMapName, StaticObjList, GrassPosList, DynamicObjList);
			break;
		}
		case 2:  //2是删除地图
		{
			FILE *pFile = NULL;
			fopen_s(&pFile, "Map\\MapName.txt", "wb");
			P = MapnameList;
			while (P->next)
			{
				if (MapChoice == P->next->m_nChoice)
				{
					PMapName q = P->next;
					P->next = P->next->next;
					free(q);
					q->next = NULL;
					break;
				}
				P = P->next;
			}
			P = MapnameList;
			while (P->next)  //重新写进去
			{
				fprintf(pFile, "%s ", P->next->m_szMapName);
				P = P->next;
			}
			fclose(pFile);
			pFile = NULL;
			break;
		}
		}
		free(P);
		P = NULL;
		return true;
	}
}

//*****************************************************************************************
// 函数名称: SuspendGame
// 函数说明: 暂停游戏
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: CStaticObjList & StaticObjList
// 参    数: CDynamicObjList & DynamicObjList
// 返 回 值: int
//*****************************************************************************************
int SuspendGame(CStaticObjList& StaticObjList, CDynamicObjList& DynamicObjList)
{
	system("cls");
	while (true)
	{
		WriteChar({ Text_LOCX, Text_LOCY }, "1.继续游戏");
		WriteChar({ Text_LOCX, Text_LOCY + 2 }, "2.保存游戏");
		WriteChar({ Text_LOCX, Text_LOCY + 4 }, "3.回到菜单");
		char cSuspendChoice = _getch() - 0x30;
		system("cls");
		switch (cSuspendChoice)
		{
		case 1:
			return 1;
			break;
		case 2:
			SaveMap(&StaticObjList, &DynamicObjList, "Suspend");
			break;
		case 3:
			return 3;
			break;
		default:
			break;
		}
	}
}
