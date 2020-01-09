#include "pch.h"
#include "CDynamicObj.h"


int CTankObj::g_nCount = 0;

//*****************************************************************************************
// 函数名称: MoveObj
// 函数说明: 坦克移动函数
// 作    者: lracker
// 时    间: 2019/08/22
// 参    数: CStaticObjList & StaticObjList
// 参    数: CDynamicObjList & DynamicObjList
// 参    数: CGrassPosList g_GrassPosList
// 返 回 值: int
//*****************************************************************************************
int CTankObj::MoveObj(CStaticObjList &StaticObjList, CDynamicObjList &DynamicObjList, CGrassPosList g_GrassPosList)
{
	if (m_nHealth <= 0)  //如果挂了的话，那就不要移动了，什么都不能干
	{
		return 0;
	}
	if (!TimeOut(m_MoveTimer))
	{
		return 0;
	}
	if (m_nCamp == ENEMY)
	{
		/*m_cKeyboardInfo = g_cInput[rand() % 5];*/
		if (!m_vecPath.empty())
		{
			bool Flag = true;
			Node p;
			p = m_vecPath[m_vecPath.size() - 1];
			//这里要使用A*算法得到的路径坐标
			switch (p.m_nDir)
			{
			case UP:
				m_cKeyboardInfo = 'w';
				if (GetDir() != UP)
				{
					Flag = false;
				}
				break;
			case DOWN:
				m_cKeyboardInfo = 's';
				if (GetDir() != DOWN)
				{
					Flag = false;
				}
				break;
			case LEFT:
				m_cKeyboardInfo = 'a';
				if (GetDir() != LEFT)
				{
					Flag = false;
				}
				break;
			case RIGHT:
				m_cKeyboardInfo = 'd';
				if (GetDir() != RIGHT)
				{
					Flag = false;
				}
				break;
			default:
				break;
			}
			if (Flag)
			{
				m_vecPath.pop_back();
			}
		}
		else
		{
			m_cKeyboardInfo = 'f';
		}
	}
	Draw(CLEAN);
	COORD NewPos = m_pos;
	switch (m_cKeyboardInfo)
	{
	case 'w':case 'i':
		SetDir(UP);
		NewPos.Y--;
		break;
	case 's':case 'k':
		SetDir(DOWN);
		NewPos.Y++;
		break;
	case 'a':case 'j':
		SetDir(LEFT);
		NewPos.X--;
		break;
	case 'd':case 'l':
		SetDir(RIGHT);
		NewPos.X++;
		break;
	case 'f':case 'h':
		if (TimeOut(m_ShootTimer))
		{
			m_cKeyboardInfo = NULL;
			return 1;
		}
		break;
	default:
		break;
	}
	m_cKeyboardInfo = NULL;
	if (Isknock(StaticObjList, DynamicObjList, NewPos))  //碰到东西了
	{
		Draw(DRAW);
		return 0;
	}
	m_pos = NewPos;
	Draw(DRAW);
	CGrassPosList::GrassPos* q = g_GrassPosList.m_pHead->m_next;
	CStaticObjList::CStaticObjNode* p = StaticObjList.m_pHead->m_next;
	while (p)
	{
		if (!q)
		{
			break;
		}
		if (p->m_Staticobj->IsBoundary(q->pos))
		{
			q = q->m_next;
			p->m_Staticobj->Draw(DRAW);
			g_Map[p->m_Staticobj->GetPos().X][p->m_Staticobj->GetPos().Y] = STATIC;
		}
		p = p->m_next;
	}
	return 0;
}

//*****************************************************************************************
// 函数名称: Draw
// 函数说明: 描绘出坦克
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: int nOrder
// 返 回 值: void
//*****************************************************************************************
void CTankObj::Draw(int nOrder)
{
	COORD GunPos = RetPos();
	switch (GetDir())
	{
	case UP:
		GunPos.Y--;
		break;
	case DOWN:
		GunPos.Y++;
		break;
	case LEFT:
		GunPos.X--;
		break;
	case RIGHT:
		GunPos.X++;
		break;
	default:
		break;
	}
	for (short i = RetPos().X - 1; i <RetPos().X + 2; i++)
	{
		for (short j = RetPos().Y - 1; j < RetPos().Y + 2; j++)
		{
			if (nOrder == CLEAN)
			{
				if (i == GunPos.X&&j == GunPos.Y)
					WriteChar(GunPos, " ", FOREGROUND_GREEN);
				else
					WriteChar({ i,j }, " ", GetColor());
				g_Map[i][j] = BLANK;
			}
			else if (nOrder == DRAW)
			{
				if (i == GunPos.X&&j == GunPos.Y)
					WriteChar(GunPos, "★", FOREGROUND_GREEN);
				else
					WriteChar({ i,j }, GetStyle(), GetColor());
				g_Map[i][j] = DYNAMIC;
			}
		}
	}
}

//*****************************************************************************************
// 函数名称: SetControl
// 函数说明: 设置操控
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: char input
// 返 回 值: void
//*****************************************************************************************
void CTankObj::SetControl(char input)
{
	if(m_nHealth)
		m_cKeyboardInfo = input;
}

//*****************************************************************************************
// 函数名称: Isknock
// 函数说明: 坦克碰撞函数
// 作    者: lracker
// 时    间: 2019/08/22
// 参    数: CStaticObjList StaticObjList
// 参    数: CDynamicObjList & DynamicObjList
// 参    数: COORD NewPos
// 返 回 值: bool
//*****************************************************************************************
bool CTankObj::Isknock(CStaticObjList StaticObjList, CDynamicObjList &DynamicObjList, COORD NewPos)
{
	int k = 0;
	for (short i = NewPos.X - 1; i < NewPos.X + 2; i++)
	{
		for (short j = NewPos.Y - 1; j < NewPos.Y + 2; j++)
		{
			if (g_Map[i][j] != BLANK)  //只要有一处不是空地就代表了碰到东西了
			{
				if (g_Map[i][j] == STATIC)  //如果是静态物品的话
				{
					if (IsGrass(StaticObjList, { i,j }))  //如果是草地的话
					{
						continue;
					}
					return true;
				}
				if (g_Map[i][j] == DYNAMIC)  //如果有动态的东西，直接退出，不要移动
					return true;
			}
		}
	}
	return false;
}


//*****************************************************************************************
// 函数名称: IsBoundary
// 函数说明: 查看是否在此范围内
// 作    者: lracker
// 时    间: 2019/08/22
// 参    数: COORD pos
// 返 回 值: bool
//*****************************************************************************************
bool CTankObj::IsBoundary(COORD pos)
{
	for (int i = m_pos.X - 1; i < m_pos.X + 2; i++)
	{
		for (int j = m_pos.Y - 1; j < m_pos.Y + 2; j++)
		{
			if (i == pos.X&&j == pos.Y)
			{
				return true;
			}
		}
	}
	return false;
}

//*****************************************************************************************
// 函数名称: Beaten
// 函数说明: 坦克被打函数
// 作    者: lracker
// 时    间: 2019/08/22
// 返 回 值: bool
//*****************************************************************************************
bool CTankObj::Beaten(int nCamp)
{
	if (nCamp == GetCamp())
	{
		return false;
	}
	SetHealth(1);
	if (GetHealth() <= 0)
	{
		m_IsDie = true;  //真的挂了
		Draw(CLEAN);
		return true;
	}
	return false;
}

//*****************************************************************************************
// 函数名称: IsGrass
// 函数说明: 判断是否在草地
// 作    者: lracker
// 时    间: 2019/08/22
// 参    数: CStaticObjList StaticObjList
// 参    数: COORD NewPos
// 返 回 值: bool
//*****************************************************************************************
bool CTankObj::IsGrass(CStaticObjList StaticObjList, COORD NewPos)
{
	CStaticObjList::CStaticObjNode* p = StaticObjList.m_pHead->m_next;
	while (p)
	{
		if (p->m_Staticobj->IsBoundary(NewPos))
		{
			if (p->m_Staticobj->GetName() == GRASS)//如果这个坐标是草地的话，返回真
			{
				return true;
			}
			return false;
		}
		p = p->m_next;
	}
	return false;
}


//*****************************************************************************************
// 函数名称: Save
// 函数说明: 坦克保存函数
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: const char * c_szFileName
// 返 回 值: void
//*****************************************************************************************
void CTankObj::Save(const char* c_szFileName)
{
	FILE *pFile = NULL;
	fopen_s(&pFile, c_szFileName, "ab+");
	char szName[10] = { 0 };
	sprintf_s(szName, 10, "%d", m_nName);  //写入类型名
	fwrite(szName, sizeof(szName), 1, pFile);
	fwrite((int*)this + 1, sizeof(CTankObj) - 4, 1, pFile);  //再写入整个结构体
	fclose(pFile);
	pFile = NULL;
}

//*****************************************************************************************
// 函数名称: CreateTank
// 函数说明: 创建坦克
// 作    者: lracker
// 时    间: 2019/08/22
// 参    数: int n
// 参    数: int nCamp
// 返 回 值: void
//*****************************************************************************************
void CDynamicObjList::CreateTank(int n, int nCamp)
{
	for (int i = 0; i < n; i++)
	{
		CDynamicObjNode* pNode = new CDynamicObjNode;
		CTankObj *pPlayerTank = new CTankObj(UP, nCamp, i);  
		if (nCamp == ENEMY) 
		{
			pPlayerTank->InitHealth(i + 1);
			pPlayerTank->SetStyle(i+1);
			pPlayerTank->SetColor(i + 2);
		}
		else if(nCamp==FRIEND)
		{
			pPlayerTank->InitHealth(5);  //玩家初始生命值为5
			pPlayerTank->SetStyle(0);
			pPlayerTank->SetColor(i);
		}
		pNode->m_pDynamicObj = pPlayerTank;  //父类指针指向玩家坦克
		InsertNode(pNode);
	}
}

//*****************************************************************************************
// 函数名称: InsertNode
// 函数说明: 插入动态链表
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: CDynamicObjNode * & pNode
// 返 回 值: void
//*****************************************************************************************
void CDynamicObjList::InsertNode(CDynamicObjNode*& pNode)
{
	m_pRear->m_next = pNode;			 //使用尾插法
	pNode->m_prior = m_pRear;
	m_pRear = pNode;
}


//*****************************************************************************************
// 函数名称: CDynamicObjList
// 函数说明: 动态链表的构造函数
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: 
//*****************************************************************************************
CDynamicObjList::CDynamicObjList()
{
	CDynamicObjNode* pHeadNode = new CDynamicObjNode;  //定义头结点
	m_pHead = pHeadNode;
	m_pRear = m_pHead;  //初始化的时候把尾指针和头指针放在一起
	CTankObj::g_nCount = 0;
}


//*****************************************************************************************
// 函数名称: ~CDynamicObjList
// 函数说明: 析构函数
// 作    者: lracker
// 时    间: 2019/08/23
// 返 回 值: 
//*****************************************************************************************
CDynamicObjList::~CDynamicObjList()
{
	CDynamicObjNode* p;
	CDynamicObjNode* q;
	p = m_pHead->m_next;
	while (p)
	{
		q = p->m_next;
		delete p;
		p = q;
	}
	p = NULL;
	q = NULL;
	m_pHead = NULL;
	m_pRear = NULL;
}

//*****************************************************************************************
// 函数名称: CreatBullet
// 函数说明: 创建子弹
// 作    者: lracker
// 时    间: 2019/08/23
// 参    数: int nDir
// 参    数: int nCamp
// 参    数: COORD pos
// 参    数: int nColor
// 参    数: CStaticObjList & StaticObjList
// 参    数: int nId
// 返 回 值: void
//*****************************************************************************************
void CDynamicObjList::CreatBullet(int nDir,int nCamp,COORD pos,int nColor, CStaticObjList &StaticObjList, int nId)
{
	if(nCamp==FRIEND)
		PlaySoundA("Sound\\shoot.wav", NULL, SND_ASYNC | SND_NODEFAULT); // 玩家发射子弹播放音乐
	CDynamicObjNode* q = new CDynamicObjNode;
	CBulletObj* p = new CBulletObj(nDir, nCamp, pos, nColor, nId);
	q->m_pDynamicObj = p;
	if (!q->m_pDynamicObj->Isknock(StaticObjList, *this,p->RetPos()))
	{
		InsertNode(q);
		if(!p->GetHide())//如果没有隐藏的话
			q->m_pDynamicObj->Draw(DRAW);
		//如果隐藏了的话，那就不要画出来//譬如草地、水
	}
}


//*****************************************************************************************
// 函数名称: DrawDynmaicObjList
// 函数说明: 画出坦克
// 作    者: lracker
// 时    间: 2019/08/19
// 返 回 值: void
//*****************************************************************************************
void CDynamicObjList::DrawObj(int nOrder)
{
	CDynamicObjNode *pHead = m_pHead->m_next;
	while (pHead)
	{
		pHead->m_pDynamicObj->Draw(nOrder);
		pHead = pHead->m_next;
	}
}


int CDynamicObjList::GetLen()
{
	CDynamicObjNode *pHead = m_pHead->m_next;
	int i = 0;
	while (pHead)
	{
		i++;
		pHead = pHead->m_next;
	}
	return i;
}

void CDynamicObjList::AllMove(CStaticObjList &StaticObjList, CGrassPosList g_GrassPosList)
{
	CDynamicObjNode* p = m_pHead->m_next;
	while (p)
	{
		int ret = p->m_pDynamicObj->MoveObj(StaticObjList, *this, g_GrassPosList);
		if (ret == 1)
		{
		CreatBullet(p->m_pDynamicObj->GetDir(),
				p->m_pDynamicObj->GetCamp(),
				p->m_pDynamicObj->RetPos(),
				p->m_pDynamicObj->GetColor(), 
				StaticObjList,
				p->m_pDynamicObj->GetId());
				p = p->m_next;
		}
		else if (ret == 2 && p->m_pDynamicObj->GetName() == BULLET)  //碰到东西了
		{
			if (p == m_pRear)
			{
				m_pRear = p->m_prior;
				p->m_prior->m_next = p->m_next;
			}
			else
			{
				p->m_prior->m_next = p->m_next;
				p->m_next->m_prior = p->m_prior;
			}

			CDynamicObjNode* q = p;
			p = p->m_next;
			delete q;
			
		}
		else
		{
			p = p->m_next;
		}
	
	}
}

//*****************************************************************************************
// 函数名称: WhichOrder
// 函数说明: 判断是谁发出命令
// 作    者: lracker
// 时    间: 2019/08/22
// 返 回 值: void
//*****************************************************************************************
int CDynamicObjList::WhichOrder()
{
	char input = _getch();
	if (input == 27)
	{
		return 1;
	}
	if (input == 'w' || input == 's' || input == 'a' || input == 'd' || input == 'f')
	{
		SetPlayerControl1(input);
	}
	if (g_nFRIENDNUM == 2)
	{
		if (input == 'i' || input == 'k' || input == 'j' || input == 'l' || input == 'h')
		{
			SetPlayerControl2(input);
		}
	}
	return 0;
}

bool CDynamicObjList::IsExist()
{
	int j = 0;
	for (int i = 0; i < g_nFRIENDNUM; i++)
	{
		if (m_pPlayer[i]->m_pDynamicObj->GetIsDie())
			j++;
		if (j == g_nFRIENDNUM)
		{
			return false;  //如果两个都凉了那就返回不存在了
		}
	}
	return true;
}

void CDynamicObjList::InitPlayer()
{
	CDynamicObjNode* p = m_pHead->m_next;
	for (int i = 0; i < g_nFRIENDNUM; i++)
	{
		m_pPlayer[i] = p;
		p = p->m_next;
	}
}

//*****************************************************************************************
// 函数名称: DrawHealthAndScore
// 函数说明: 打印出生命值和成绩
// 作    者: lracker
// 时    间: 2019/08/22
// 返 回 值: void
//*****************************************************************************************
void CDynamicObjList::DrawHealthAndScore()
{
	for (short i = 0; i < g_nFRIENDNUM; i++)
	{
		WriteChar({ g_ScoreX + 4 , g_ScoreY - 8 - 3 * i }, "生命值");
		WriteChar({ g_ScoreX + 5 , g_ScoreY - 6 - 3 * i }, "     ");
		char szHealth[10];
		CTankObj* p = (CTankObj*)m_pPlayer[i]->m_pDynamicObj;
		sprintf_s(szHealth, "%d", p->GetHealth());
		WriteChar({ g_ScoreX + 5 , g_ScoreY - 6 - 3 * i }, szHealth);

		WriteChar({ g_ScoreX + 9 , g_ScoreY - 8 - 3 * i }, "成绩");
		WriteChar({ g_ScoreX + 10 , g_ScoreY - 6 - 3 * i }, "   ");
		char szScore[10];
		int test = p->GetScore();
		sprintf_s(szScore, "%d", p->GetScore());
		WriteChar({ g_ScoreX + 10 , g_ScoreY - 6 - 3 * i }, szScore);
	}
}

CTankObj::CTankObj(int nDir, int nCamp, int nId, int nScore) :m_nScore(nScore),CDynamicObj(nDir, nCamp, TANK,nId)
{
	if (nCamp == ENEMY)  //如果是敌人坦克的话
	{
		m_pos = Birth();  //那么在空地中生成坐标
		int nEndX = 27;   //将军坐标为27，42
		int nEndY = 38;  
		CAStar astar;
		astar.getPath(nDir, m_pos.X, m_pos.Y, nEndX, nEndY, m_vecPath);  //得到的路径点放在了vecPath里面
	}
	else 
	{
		m_pos = g_PlayerPos[CTankObj::g_nCount++];
	}
	for (int i = m_pos.X - 1; i < m_pos.X + 2; i++)
	{
		for (int j = m_pos.Y - 1; j < m_pos.Y + 2; j++)
		{
			g_Map[i][j] = DYNAMIC;
		}
	}
	InitTimer(m_MoveTimer);			//设置移动的计时器
	InitTimer(m_ShootTimer);
	SetdelayTime(m_MoveTimer,TKTime);		//设置超时时间
	SetdelayTime(m_ShootTimer, ShootTime);  //设置子弹间隔时间
}


CDynamicObjList::CDynamicObjNode::CDynamicObjNode()
{
	m_next = NULL;
	m_pDynamicObj = NULL;
}


COORD CTankObj::Birth()
{
	srand((int)time(NULL));
	COORD pos;
	int Flag = 0;
	do 
	{
		Flag = 0;
		pos.X = rand() % (MaxX - 2) + 1;
		pos.Y = rand() % (MaxY/2 - 2) + 1;  //在上半部分
		for (int i = pos.X - 1; i < pos.X + 2; i++)
		{
			for (int j = pos.Y - 1; j < pos.Y + 2; j++)
			{
				if(g_Map[i][j] == BLANK)
					Flag++;
			}
		}
	} while (Flag!=9);
	return pos;
}

COORD CBulletObj::Birth(COORD pos)
{
	COORD tempPos = pos;
	switch (m_nDir)
	{
	case UP:
		tempPos.Y -= 2;
		break;
	case DOWN:
		tempPos.Y += 2;
		break;
	case LEFT:
		tempPos.X -= 2;
		break;
	case RIGHT:
		tempPos.X += 2;
		break;
	default:
		break;
	}
	return tempPos;
}

CBulletObj::CBulletObj(int nDir, int nCamp, COORD pos, int nColor, int nId):CDynamicObj(nDir, nCamp, BULLET, nId, LIVE)
{
	InitTimer(m_MoveTimer);
	SetdelayTime(m_MoveTimer, BTTime);
	SetStyle(3);
	m_nColor = nColor;
	m_pos = Birth(pos);
	m_nHide = false;
}

int CBulletObj::MoveObj(CStaticObjList &StaticObjList, CDynamicObjList &DynamicObjList, CGrassPosList g_GrassPosList)
{
	if (!TimeOut(m_MoveTimer))
	{
		return 0;
	}
	if(!m_nHide)
		Draw(CLEAN);
	COORD NewPos = m_pos;
	switch (m_nDir)
	{
	case UP:
		NewPos.Y--;
		break;
	case DOWN:
		NewPos.Y++;
		break;
	case LEFT:
		NewPos.X--;
		break;
	case RIGHT:
		NewPos.X++;
		break;
	default:
		break;
	}
	if (Isknock(StaticObjList, DynamicObjList, NewPos))  //碰到东西了
	{
		return 2;
	}
	m_pos = NewPos;
	if (m_nHide)  //如果隐藏起来了
		return 0;
	Draw(DRAW);  //否则画出来
	return 0;
}

void CBulletObj::Draw(int nOrder)
{
	if (nOrder == CLEAN)
	{
		g_Map[m_pos.X][m_pos.Y] = BLANK;
		WriteChar(m_pos, "  ", m_nColor);
	}

	else if (nOrder == DRAW)
	{
		g_Map[m_pos.X][m_pos.Y] = DYNAMIC;
		WriteChar(m_pos, m_szStyle, m_nColor);
	}
}

//*****************************************************************************************
// 函数名称: Isknock
// 函数说明:
// 作    者: lracker
// 时    间: 2019/08/21
// 参    数: CStaticObjList & StaticObjList
// 参    数: CDynamicObjList & DynamicObjList
// 返 回 值: bool
//			 true代表碰到东西了
//			 false代表没有碰到东西
//*****************************************************************************************
bool CBulletObj::Isknock(CStaticObjList &StaticObjList, CDynamicObjList &DynamicObjList, COORD NewPos)
{
	m_nHide = false;
	if (g_Map[NewPos.X][NewPos.Y] == BLANK)			 //如果是空地的话
	{
		return false;
	}
	else if (g_Map[NewPos.X][NewPos.Y] == DYNAMIC)  //如果是动态类的东西
	{
		CDynamicObjList::CDynamicObjNode* p = DynamicObjList.m_pHead->m_next;  //指向当前遍历的结点
		while (p)
		{
			if (p->m_pDynamicObj->IsBoundary(NewPos))	//判断是否在该范围
			{
				if (p->m_pDynamicObj->Beaten(m_nCamp))  //判断是否打到产生效果了，譬如友方坦克互打是没用的
				{
					if (p->m_pDynamicObj->GetName() == TANK && p->m_pDynamicObj->GetCamp() == ENEMY)  //敌方坦克
					{
						for (int i = 0; i < g_nFRIENDNUM; i++)
						{
							if (DynamicObjList.m_pPlayer[i]->m_pDynamicObj->GetId() == GetId())  //如果ID相同
							{
								CTankObj* player = (CTankObj*)DynamicObjList.m_pPlayer[i]->m_pDynamicObj;
								player->SetScore();  //成绩++
							}
								
						}
					}
					if (p->m_pDynamicObj->GetName() == BULLET)  //判断是否要销毁该物体，这里只删除子弹  //坦克只清除，不在链表里删		
					{
						if (p == DynamicObjList.m_pRear)		//如果删到尾指针了						
						{
							DynamicObjList.m_pRear = p->m_prior;
							p->m_prior->m_next = p->m_next;
							p->m_prior = NULL;
						}
						else
						{
							p->m_prior->m_next = p->m_next;
							p->m_next->m_prior = p->m_prior;
						}
						delete p;
						return true;
					}
				}
				break;
			}
			p = p->m_next;
		}
		return true;
	}
	else if (g_Map[NewPos.X][NewPos.Y] == STATIC)
	{
		CStaticObjList::CStaticObjNode* p = StaticObjList.m_pHead->m_next;  //从静态链表的首节点开始遍历
		CStaticObjList::CStaticObjNode* q = StaticObjList.m_pHead;  //指向要被删掉的结点
		int ret = 0;  //等于0是什么都没发生，虽然这不应该存在，只是赋初值
		while (p)
		{
			if (p->m_Staticobj->IsBoundary(NewPos))
			{
				ret = p->m_Staticobj->Beaten();
				if (ret == 1)  //调用该物品被打的函数,并且确认要删掉的话
				{
					if (p->m_next == NULL)		//如果删到最后一个了					
					{
						p->m_prior->m_next = p->m_next;
						p->m_prior = NULL;
					}
					else
					{
						p->m_prior->m_next = p->m_next;
						p->m_next->m_prior = p->m_prior;
					}
					delete p;
					p = NULL;
				}
				break;
			}
			p = p->m_next;
			q = q->m_next;
		}
		if (ret == 2)  //代表了该物品不删，但是子弹删掉了  //铁墙
			return true;
		if (ret == 1)  //代表了该物品和子弹都要删掉，土墙的时候
			return true;
		if (ret == 3)
		{
			m_nHide = true;
			return false;  //代表了物品和子弹都不用删掉，河流和草地的时候
		}
	}
	return false;
}

bool CBulletObj::IsBoundary(COORD pos)
{
	if (m_pos.X == pos.X && m_pos.Y == pos.Y)
	{
		return true;
	}
	return false;
}

bool CBulletObj::Beaten(int m_nCamp)
{
	Draw(CLEAN);
	return true;
}



void CBulletObj::Save(const char* c_szFileName)
{
	FILE *pFile = NULL;
	fopen_s(&pFile, c_szFileName, "ab+");
	char szName[10] = { 0 };
	sprintf_s(szName, 10, "%d", m_nName);  //写入类型名
	fwrite(szName, sizeof(szName), 1, pFile);
	fwrite((int*)this + 1, sizeof(CBulletObj) - 4, 1, pFile);  //再写入整个结构体
	fclose(pFile);
	pFile = NULL;
}

int CDynamicObj::TimeOut(Timer& t)
{
	// 获取当前的时间
	clock_t now = clock();
	// 当前时间 - 上次超时时间
	// 得到的差如果小于间隔时间, 说明还没有超时. 
	if (now - t.time < t.timeout) {
		return 0;
	}

	// 超时之后, 需要更新最后一次的超时时间
	t.time = now;
	return 1;
}

void CDynamicObj::Save(const char* c_szFileName)
{
}
