#include "pch.h"
#include "CGameObj.h"
using std::cout;
using std::endl;

//*****************************************************************************************
// ��������: StartGame
// ����˵��: ��ʼ��Ϸ
// ��    ��: lracker
// ʱ    ��: 2019/08/19
// �� �� ֵ: void
//*****************************************************************************************
void CGameObj::StartGame()
{
	system("cls");
	InitMap();
	CStaticObjList StaticObjList;										//��ʼ������
	CGrassPosList g_GrassPosList;										//�����ݵ���������¼�ݵص�����
	CDynamicObjList DynamicObjList;										//��ʼ����̬����
	int ret = ChoiceMap(StaticObjList, DynamicObjList, g_GrassPosList);
	if (ret == 3)  //ѡ���˳���
	{
		return;
	}
	ScoreBoard();														//��ӡ���Ʒְ�
	DynamicObjList.CreatePlayer(g_nFRIENDNUM);							//����һ�����
	DynamicObjList.CreateEnery(g_nENEMYNUM);						    //������룬����n������
	DynamicObjList.DrawObj(DRAW);									    //������̬��Ʒ
	Gaming(DynamicObjList, StaticObjList, g_GrassPosList);				//��ʼ��Ϸ
	return;
}

//*****************************************************************************************
// ��������: InitMap
// ����˵��: ��ʼ����ͼ����ȫΪ��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: void
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
// ��������: ContinueGame
// ����˵��: ������Ϸ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: const char * szFileName
// �� �� ֵ: void
//*****************************************************************************************
void CGameObj::ContinueGame(const char* szFileName)
{
	system("cls");
	InitMap();															//��ʼ����ͼ
	CStaticObjList StaticObjList;										//��ʼ������
	CGrassPosList g_GrassPosList;										//�����ݵ���������¼�ݵص�����
	CDynamicObjList DynamicObjList;
	ReadMap(szFileName, StaticObjList, g_GrassPosList, DynamicObjList);  //��ȡ��ͣ�Ĵ浵
	CDynamicObjList::CDynamicObjNode* p = DynamicObjList.m_pHead->m_next;
	for (int i = 0; i < g_nFRIENDNUM; i++)
	{
		DynamicObjList.m_pPlayer[i] = p;  //��ȡ��Ҹ����������浽ָ����
		p++;
	}
	Gaming(DynamicObjList, StaticObjList, g_GrassPosList);
}


//*****************************************************************************************
// ��������: Gaming
// ����˵��:
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: CDynamicObjList & DynamicObjList
// ��    ��: CStaticObjList & StaticObjList
// ��    ��: CGrassPosList g_GrassPosList
// �� �� ֵ: void
//*****************************************************************************************
void CGameObj::Gaming(CDynamicObjList& DynamicObjList, CStaticObjList& StaticObjList, CGrassPosList g_GrassPosList)
{
	CTankObj* pPlayer1 = (CTankObj*)DynamicObjList.m_pPlayer[0]->m_pDynamicObj;
	nGameOver = FALSE;													//�趨һ��ʼ������
	while (DynamicObjList.IsExist() && !nGameOver)
	{
		DynamicObjList.DrawHealthAndScore();							//��ӡ���ɼ�������ֵ
		if (_kbhit())													//��Ұ��¼���
		{
			int ESC = DynamicObjList.WhichOrder();
			if (ESC == 1)
			{
				int RetSuspend = SuspendGame(StaticObjList, DynamicObjList);
				if (RetSuspend == 3)  //�ص��˵�
				{
					nGameOver = TRUE;
				}
				else
					StaticObjList.DrawObj();  //�Ѷ����ٻ�һ��
			}
		}
		if (nGameOver)  //�����ͣ��ʱ��ѡ���˻ص��˵����Ͳ������´�ӡһ�鶯̬�������˳��ˡ�
		{
			break;
		}
		DynamicObjList.AllMove(StaticObjList, g_GrassPosList);
		if (pPlayer1->GetScore() == 4)//��������ķ��ˣ��Ǿͽ�����һ��
		{
			static int num = 0;
			num++;
			char szNum[2] = { 0 };
			sprintf_s(szNum, 2, "%d", num);
			char Buffer[10] = "Mission";
			strcat_s(Buffer, 10, szNum);
			system("cls");
			WriteChar({ Text_LOCX,Text_LOCY }, "��һ��", 0xf);
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
// ��������: SizeOfWindow
// ����˵��: ȫ��Ҫ�õ��ĺ���
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: HANDLE hConsoleOutput
// �� �� ֵ: SMALL_RECT
//*****************************************************************************************
SMALL_RECT SizeOfWindow(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hConsoleOutput, &info);
	return info.srWindow;
}

//*****************************************************************************************
// ��������: FullScreen
// ����˵��: ȫ���������Ҹ�������������Ϣ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: void
//*****************************************************************************************
void FullScreen()
{
	HANDLE hOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
	/************����ȫ��************/
	HWND hWnd = GetForegroundWindow();  //��ȡǰ�����ھ��  
	int nCx = GetSystemMetrics(SM_CXSCREEN);  //������Ļ���  
	int nCy = GetSystemMetrics(SM_CYSCREEN);  //������Ļ�߶�  
	LONG l_WinStyle = GetWindowLong(hWnd, GWL_STYLE);  //������ʽ  
	SetWindowLong(hWnd, GWL_STYLE, (l_WinStyle | WS_POPUP | WS_MAXIMIZE) & ~WS_CAPTION & ~WS_THICKFRAME & ~WS_BORDER);  // ���ô�����Ϣ ��� ȡ�����������߿�
	SetWindowPos(hWnd, HWND_TOP, 0, 0, nCx, nCy, 0);
	/************�����޹��************/
	CONSOLE_CURSOR_INFO cci;  //���ù����Ϣ�Ľṹ��
	cci.dwSize = 1;  //һ��Ҫ��ֵ��������
	cci.bVisible = FALSE;  //����Ϊ���ɼ�
	SetConsoleCursorInfo(hOutPut, &cci);  //���ù����Ϣ
	/************�����޹�����************/
	CONSOLE_SCREEN_BUFFER_INFO info;  //���û�������Ϣ�Ľṹ��
	GetConsoleScreenBufferInfo(hOutPut, &info);
	SMALL_RECT rect = SizeOfWindow(hOutPut);  //��/��ȡ����size
	COORD size = { rect.Right + 1,rect.Bottom + 1 };	//���建������С 
	SetConsoleScreenBufferSize(hOutPut, size);
	/**************һ��ʼ�������뷨ΪӢ��***************/
	keybd_event(VK_SHIFT, 0, 0, 0);
	Sleep(100);
	keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);  //�л�Ӣ��
}

//*****************************************************************************************
// ��������: DrawWelcome
// ����˵��: ��ӡ����ӭ����
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: void
//*****************************************************************************************
void DrawWelcome()
{
	system("cls");
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = {};
	SetConsoleCursorPosition(hOutput, position);
	SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "������������     ��         ��       �� ��    ��      " << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "    ����       ��  ��       �� ��    �� ��   ��       " << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "    ����      ��    ��      ��  ��   �� ��  ��     " << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "    ����     ����������     ��   ��  �� ����" << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "    ����    ��        ��    ��    �� �� ��  ��              " << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "    ����   ��          ��   ��     ���� ��    ��            " << endl;

	SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "    ���� ��              �� ��       �� ��      ��          " << endl;

	Sleep(1000);
}

//*****************************************************************************************
// ��������: PrintMenu
// ����˵��: ��ӡ���˵�
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: void
//*****************************************************************************************
void PrintMenu()
{
	while (true)
	{
		system("cls");
		CGameObj Game;
		system("cls");
		WriteChar({ Text_LOCX, Text_LOCY - 4 }, "1.��ʼ��Ϸ");
		WriteChar({ Text_LOCX, Text_LOCY - 2 }, "2.������Ϸ");
		WriteChar({ Text_LOCX, Text_LOCY + 0 }, "3.�Զ����ͼ");
		WriteChar({ Text_LOCX, Text_LOCY + 2 }, "4.˫����ս");
		WriteChar({ Text_LOCX, Text_LOCY + 4 }, "5.ɾ����ͼ");
		WriteChar({ Text_LOCX, Text_LOCY + 6 }, "6.�˳���Ϸ");
		char MenuChoice = _getch() - 0x30;
		switch (MenuChoice)
		{
		case 1:
			g_nFRIENDNUM = 1;		//�����������
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
			CStaticObjList test1;  //û�õģ�����������
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
// ��������: ScoreBoard
// ����˵��: ��ӡ���Ʒְ�
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: void
//*****************************************************************************************
void ScoreBoard()
{
	for (short i = g_ScoreX; i < g_ScoreX + 15; i++)
	{
		for (short j = 0; j < g_ScoreY; j++)
		{
			if (i == g_ScoreX || i == 74 || j == 0 || j == g_ScoreY - 1)
			{
				WriteChar({i, j}, "��", FOREGROUND_GREEN | FOREGROUND_RED);
			}
		}
	}
}

//*****************************************************************************************
// ��������: CustomMap
// ����˵��: �Զ����ͼ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: void
//*****************************************************************************************
void CustomMap()
{
	system("cls");
	CreateFolder();  //�����ļ�������ŵ�ͼ
	CDynamicObjList DynamicObjList;
	CStaticObjList StaticObjList;
	/********����ǽ*********/
	StaticObjList.CreateWall();	//����Χǽ
	StaticObjList.CreateHome(); //������
	StaticObjList.DrawObj();	//������
	Tips();					    //������ʾ��
	/**********��ȡ����¼��ͼ����¼�****************/
	HANDLE hInPut = GetStdHandle(STD_INPUT_HANDLE);  //��ȡ���
	INPUT_RECORD ir = {};  //���ýṹ��
	DWORD dwCount = 0;
	int nChoice = 0;
	bool keyDown = false;
	SetConsoleMode(hInPut, ENABLE_MOUSE_INPUT);
	while (true)
	{
		ReadConsoleInput(hInPut, &ir, 1, &dwCount);
		if (ir.EventType == MOUSE_EVENT)  //��ȡ����¼�
		{
			if (ir.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)  //������
			{
				COORD pos = ir.Event.MouseEvent.dwMousePosition;
				CStaticObjList::CStaticObjNode* pNode = new CStaticObjList::CStaticObjNode;
				if (pos.X / 2 >= g_ScoreX + 3 && pos.X / 2 <= g_ScoreX + 7 && pos.Y == 7)	   //�ж��Ƿ���ľͷ
				{
					nChoice = EARTH;
					keyDown = true;
					continue;
				}
				else if (pos.X / 2 >= g_ScoreX + 3 && pos.X / 2 <= g_ScoreX + 7 && pos.Y == 9)		//�ж��Ƿ�����ǽ
				{
					nChoice = IRON;
					keyDown = true;
					continue;
				}
				else if (pos.X / 2 >= g_ScoreX + 3 && pos.X / 2 <= g_ScoreX + 7 && pos.Y == 11)		//�ж��Ƿ��˺���
				{
					nChoice = WATER;
					keyDown = true;
					continue;
				}
				else if (pos.X / 2 >= g_ScoreX + 3 && pos.X / 2 <= g_ScoreX + 7 && pos.Y == 13)		//�ж��Ƿ��˲ݵ�
				{
					nChoice = GRASS;
					keyDown = true;
					continue;
				}
				if (!keyDown)  //���û�н���ѡ��Ļ����ǾͲ��ܻ���
				{
					continue;
				}
				if (pos.X / 2 > MaxX - 2 || pos.X / 2 < 1 || pos.Y > MaxY - 2 || pos.Y < 1)		  //�ж��Ƿ񳬹�Χǽ��
				{
					continue;
				}
				if ((pos.X/2 == 27&&pos.Y==42)|| (pos.X / 2 == 27 && pos.Y == 43) || (pos.X / 2 == 28 && pos.Y == 42) || (pos.X / 2 == 28 && pos.Y == 43))  //�Ƿ�����������
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
			if (ir.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)  //���ÿյأ������ϰ���
			{
				COORD pos = ir.Event.MouseEvent.dwMousePosition;
				CStaticObjList::CStaticObjNode* pNode = new CStaticObjList::CStaticObjNode;
				pNode = StaticObjList.m_pHead->m_next;
				if (pos.X / 2 > MaxX - 2 || pos.X / 2 < 1 || pos.Y > MaxY - 2 || pos.Y < 1)				//�ж��Ƿ񳬹�Χǽ��
				{
					continue;
				}
				if ((pos.X / 2 == 27 && pos.Y == 42) || (pos.X / 2 == 27 && pos.Y == 43) || (pos.X / 2 == 28 && pos.Y == 42) || (pos.X / 2 == 28 && pos.Y == 43))  //�Ƿ�����������
				{
					continue;
				}
				while (pNode)
				{
					if (pNode->m_Staticobj->IsBoundary({ pos.X / 2,pos.Y }))  //����ҵ��������ˣ���������ɾ����
					{
						pNode->m_Staticobj->Draw(CLEAN);
						if (!pNode->m_next)		//���ɾ�����һ����						
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
			if (ir.Event.KeyEvent.wVirtualKeyCode == VK_RETURN)  //������»س���
			{
				struct tm t;   //tm�ṹָ��
				time_t now;  //����time_t����ָ��
				time(&now);
				localtime_s(&t, &now);
				char tmp[64];
				char tmp2[64];
				strftime(tmp, sizeof(tmp), "Map\\%Y��%m��%d��%H��%M��%S��", &t);  //��ʱ�������ļ���
				strftime(tmp2, sizeof(tmp2), "Map\\%Y��%m��%d��%H��%M��%S��", &t);
				SaveMap(&StaticObjList, &DynamicObjList, tmp);  //�����ͼ
				SaveMapName(tmp2);  //�����ͼ��
				system("cls");
				break;
			}
			else if (ir.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)  //������
			{
				system("cls");
				break;
			}
		}
	}
}

//*****************************************************************************************
// ��������: CreateFolder
// ����˵��: �����ļ���
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: bool
//*****************************************************************************************
bool CreateFolder()
{
	//�ļ�������
	char folderName[] = "Map";
	// �ļ��в������򴴽��ļ���
	if (_access(folderName, 0) == -1)
	{
		_mkdir(folderName);
	}
	return false;
}

//*****************************************************************************************
// ��������: Tips
// ����˵��: ������ʾ��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: void
//*****************************************************************************************
void Tips()
{
	for (short i = g_ScoreX; i < 75; i++)
	{
		for (short j = 0; j < g_ScoreY; j++)
		{
			if (i == g_ScoreX || i == 74 || j == 0 || j == g_ScoreY - 1)
			{
				WriteChar({ i, j }, "��", FOREGROUND_RED);
			}
			if (i == (g_ScoreX + 3) && j == 3)
			{
				WriteChar({ i, j }, "�س�������");
				WriteChar({ i, j + 2 }, "ESC��������ͼ");
				WriteChar({ i, j + 4 }, "�� ��ǽ", FOREGROUND_GREEN | FOREGROUND_RED);
				WriteChar({ i, j + 6 }, "�� ��ǽ", FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
				WriteChar({ i, j + 8 }, "�� ����", FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				WriteChar({ i, j + 10 }, "�� �ݵ�", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			}
		}
	}
}

//*****************************************************************************************
// ��������: SaveMap
// ����˵��: �����ͼ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: CStaticObjList * StaticObjList
// ��    ��: CDynamicObjList * DynamicObjList
// ��    ��: const char * c_szFileName
// �� �� ֵ: void
//*****************************************************************************************
void SaveMap(CStaticObjList* StaticObjList, CDynamicObjList* DynamicObjList, const char* c_szFileName)
{
	CStaticObjList::CStaticObjNode* pStaticNode = StaticObjList->m_pHead->m_next;  //ָ���׽ڵ�
	FILE *pFile = NULL;
	fopen_s(&pFile, c_szFileName, "wb+");  //���ļ�
	char buffer[10] = { 0 };
	sprintf_s(buffer, "%d", StaticObjList->GetLen());
	fwrite(buffer, sizeof(buffer), 1, pFile); //д�뾲̬����ĳ���
	fclose(pFile);
	while (pStaticNode)
	{
		pStaticNode->m_Staticobj->Save(c_szFileName);
		pStaticNode = pStaticNode->m_next;
	}
	if (DynamicObjList)  //������ָ�벻��ָ�յĻ����Ǿͱ��涯̬��
	{
		FILE *pFile = NULL;
		fopen_s(&pFile, c_szFileName, "ab+");  //���ļ�
		CDynamicObjList::CDynamicObjNode* pDynamicNode = DynamicObjList->m_pHead->m_next;
		char buffer2[10] = { 0 };
		sprintf_s(buffer2, "%d", DynamicObjList->GetLen());
		fwrite(buffer2, sizeof(buffer2), 1, pFile); //д�붯̬����ĳ���
		fclose(pFile);
		while (pDynamicNode)
		{
			pDynamicNode->m_pDynamicObj->Save(c_szFileName);
			pDynamicNode = pDynamicNode->m_next;
		}
	}

}

//*****************************************************************************************
// ��������: SaveMapName
// ����˵��: �����ͼ��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: const char * c_szFileName
// �� �� ֵ: void
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
// ��������: ReadMap
// ����˵��: ��ȡ��ͼ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: const char * c_szFileName
// ��    ��: CStaticObjList & StaticObjList
// ��    ��: CGrassPosList & GrassPosList
// ��    ��: CDynamicObjList & DynamicObjList
// �� �� ֵ: void
//*****************************************************************************************
void ReadMap(const char* c_szFileName, CStaticObjList& StaticObjList, CGrassPosList& GrassPosList, CDynamicObjList& DynamicObjList)
{
	system("cls");
	FILE *pFile = NULL;
	fopen_s(&pFile, c_szFileName, "rb+");  //���ļ�
	char buffer[10] = { 0 };
	fread(buffer, sizeof(buffer), 1, pFile); //д�뾲̬����ĳ���
	int nStaticLen = atoi(buffer);			 //��ȡ������		
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
			fread((int*)pobj+1, sizeof(CWALL)-4, 1, pFile); //д�뾲̬����ĳ���
			pNode->m_Staticobj = pobj;
			break;
		}
		case EARTH:
		{
			CEarth* pobj = new CEarth;
			fread((int*)pobj + 1, sizeof(CEarth)-4, 1, pFile); //д�뾲̬����ĳ���
			pNode->m_Staticobj = pobj;
			break;
		}
		case IRON:
		{
			CIron* pobj = new CIron;
			fread((int*)pobj + 1, sizeof(CIron)-4, 1, pFile); //д�뾲̬����ĳ���
			pNode->m_Staticobj = pobj;
			break;
		}
		case WATER:
		{
			CWater* pobj = new CWater;
			fread((int*)pobj + 1, sizeof(CWater)-4, 1, pFile); //д�뾲̬����ĳ���
			pNode->m_Staticobj = pobj;
			break;
		}
		case GRASS:
		{
			CGrass* pobj = new CGrass;
			fread((int*)pobj + 1, sizeof(CGrass)-4, 1, pFile); //д�뾲̬����ĳ���
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
			fread((int*)pobj + 1, sizeof(CGen) - 4, 1, pFile); //д�뾲̬����ĳ���
			pNode->m_Staticobj = pobj;
			break;
		}
		default:
			break;
		}
		StaticObjList.InsertCStaticObjList(pNode);
		g_Map[pNode->m_Staticobj->GetPos().X][pNode->m_Staticobj->GetPos().Y] = STATIC;
	}
	StaticObjList.DrawObj();  //�������о�̬����

	char buffer2[10] = { 0 };
	fread(buffer2, sizeof(buffer), 1, pFile); //д�뾲̬����ĳ���
	int nStaticLen2 = atoi(buffer2);			 //��ȡ������		
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
				fread((int*)pobj + 1, sizeof(CBulletObj) - 4, 1, pFile); //д�뾲̬����ĳ���
				pobj->UpDate();
				pNode->m_pDynamicObj = pobj;
				break;
			}
			case TANK:
			{
				CTankObj* pobj = new CTankObj;
				fread((int*)pobj + 1, sizeof(CTankObj) - 4, 1, pFile); //д�뾲̬����ĳ���
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
// ��������: ChoiceMap
// ����˵��: ѡ���ͼ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: CStaticObjList & StaticObjList
// ��    ��: CDynamicObjList & DynamicObjList
// ��    ��: CGrassPosList & g_GrassPosList
// �� �� ֵ: int
//*****************************************************************************************
int ChoiceMap(CStaticObjList& StaticObjList, CDynamicObjList& DynamicObjList, CGrassPosList& g_GrassPosList)
{
	int nChoice = 0;
	while (TRUE)
	{
		system("cls");
		WriteChar({ Text_LOCX, Text_LOCY }, "1.ʹ�������ͼ");
		WriteChar({ Text_LOCX, Text_LOCY + 2 }, "2.ʹ���Զ����ͼ");
		WriteChar({ Text_LOCX, Text_LOCY + 4 }, "3.�ص��˵�");

		nChoice = _getch() - 0x30;
		switch (nChoice)
		{
		case 1:
			system("cls");
			StaticObjList.CreateWall();										    //����Χǽ
			StaticObjList.CreateHome();										    //������
			StaticObjList.CreateEarth(10);										//������ľ
			StaticObjList.CreateIron(20);								        //��������
			StaticObjList.CreateWater(10);										//����ˮ
			StaticObjList.CreateGrass(GrassNumber, &g_GrassPosList);			//�����ݵ�
			StaticObjList.DrawObj();											//������̬��Ʒ
			return 1;
		case 2:
			if (LoadDeleteMap(Load, StaticObjList, DynamicObjList, g_GrassPosList)) //ʹ���Զ����ͼ
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
// ��������: LoadDeleteMap
// ����˵��: ��ȡ����ɾ����ͼ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: int LoadOrDelete
// ��    ��: CStaticObjList & StaticObjList
// ��    ��: CDynamicObjList DynamicObjList
// ��    ��: CGrassPosList & GrassPosList
// �� �� ֵ: bool
//*****************************************************************************************
bool LoadDeleteMap(int LoadOrDelete, CStaticObjList& StaticObjList, CDynamicObjList DynamicObjList, CGrassPosList& GrassPosList)
{
	int nExit = 0;		 //�жϵ�ͼ�Ƿ����
	system("cls");
	FILE *pFile = NULL;
	fopen_s(&pFile, "Map\\MapName.txt", "rb");  //�򿪵�ͼ�������ļ�
	if (!pFile)  //���û������ļ�����ʾ
	{
		WriteChar({ Text_LOCX, Text_LOCY }, "�����Զ����ͼ");
		Sleep(1000);
		return false;
	}
	if (fgetc(pFile) == EOF)	//�����ͼɾ���˵Ļ��ͻ���ʾ
	{
		WriteChar({ Text_LOCX, Text_LOCY }, "�����Զ����ͼ");
		Sleep(1000);
		fclose(pFile);
		pFile = NULL;
		return false;
	}
	else
	{
		rewind(pFile);		  //����fgetc��ԭ��ʹ������pFile
		if (LoadOrDelete == Load)
		{
			WriteChar({ Text_LOCX - 5, Text_LOCY }, "������ʹ�õĵ�ͼ���");
		}
		else if (LoadOrDelete == Delete)
		{
			WriteChar({ Text_LOCX - 5, Text_LOCY }, "������ɾ����ͼ���");
		}
		PMapName MapnameList = (PMapName)malloc(sizeof(MapName) * 1);
		MapnameList->next = NULL;
		int ret = 0;		   //����Ƿ�����ļ�β
		int i = 1;				//��¼��ͼ���
		char MapNum[10];
		while (true)  //���ļ��ж�ȡ��ͼ��
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
		PMapName P = MapnameList;	   //����ָ��ָ������ͷ
		while (P->next)  //��ӡ��ͼ���б�
		{
			sprintf_s(MapNum, "%d", P->next->m_nChoice);
			WriteChar({(short) (Text_LOCX - 5), (short)(Text_LOCY + 2 * P->next->m_nChoice) }, MapNum);
			WriteChar({ (short)(Text_LOCX - 3), (short)(Text_LOCY + 2 * P->next->m_nChoice) }, P->next->m_szMapName);
			P = P->next;
			memset(MapNum, 0, 10);
		}
		P = MapnameList;				//����ָ��
		int MapChoice = _getch() - 0x30;  //��ȡ��ͼ���
		char szSeletedMapName[64] = { 0 };			   //�����ļ���
		while (P->next)
		{
			if (MapChoice == P->next->m_nChoice)
			{
				strcpy_s(szSeletedMapName, 64, P->next->m_szMapName);  //������������ȡ��ͼ����
				nExit = 1;
				break;
			}
			P = P->next;
		}
		if (!nExit)
		{
			system("cls");
			WriteChar({ Text_LOCX, Text_LOCY }, "��������ȷ�����");
			Sleep(1000);
			return false;
		}
		switch (LoadOrDelete)
		{
		case 1:  //1�Ǽ��ص�ͼ
		{
			ReadMap(szSeletedMapName, StaticObjList, GrassPosList, DynamicObjList);
			break;
		}
		case 2:  //2��ɾ����ͼ
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
			while (P->next)  //����д��ȥ
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
// ��������: SuspendGame
// ����˵��: ��ͣ��Ϸ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: CStaticObjList & StaticObjList
// ��    ��: CDynamicObjList & DynamicObjList
// �� �� ֵ: int
//*****************************************************************************************
int SuspendGame(CStaticObjList& StaticObjList, CDynamicObjList& DynamicObjList)
{
	system("cls");
	while (true)
	{
		WriteChar({ Text_LOCX, Text_LOCY }, "1.������Ϸ");
		WriteChar({ Text_LOCX, Text_LOCY + 2 }, "2.������Ϸ");
		WriteChar({ Text_LOCX, Text_LOCY + 4 }, "3.�ص��˵�");
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
