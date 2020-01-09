#include "pch.h"
#include "CDynamicObj.h"


int CTankObj::g_nCount = 0;

//*****************************************************************************************
// ��������: MoveObj
// ����˵��: ̹���ƶ�����
// ��    ��: lracker
// ʱ    ��: 2019/08/22
// ��    ��: CStaticObjList & StaticObjList
// ��    ��: CDynamicObjList & DynamicObjList
// ��    ��: CGrassPosList g_GrassPosList
// �� �� ֵ: int
//*****************************************************************************************
int CTankObj::MoveObj(CStaticObjList &StaticObjList, CDynamicObjList &DynamicObjList, CGrassPosList g_GrassPosList)
{
	if (m_nHealth <= 0)  //������˵Ļ����ǾͲ�Ҫ�ƶ��ˣ�ʲô�����ܸ�
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
			//����Ҫʹ��A*�㷨�õ���·������
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
	if (Isknock(StaticObjList, DynamicObjList, NewPos))  //����������
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
// ��������: Draw
// ����˵��: ����̹��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: int nOrder
// �� �� ֵ: void
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
					WriteChar(GunPos, "��", FOREGROUND_GREEN);
				else
					WriteChar({ i,j }, GetStyle(), GetColor());
				g_Map[i][j] = DYNAMIC;
			}
		}
	}
}

//*****************************************************************************************
// ��������: SetControl
// ����˵��: ���òٿ�
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: char input
// �� �� ֵ: void
//*****************************************************************************************
void CTankObj::SetControl(char input)
{
	if(m_nHealth)
		m_cKeyboardInfo = input;
}

//*****************************************************************************************
// ��������: Isknock
// ����˵��: ̹����ײ����
// ��    ��: lracker
// ʱ    ��: 2019/08/22
// ��    ��: CStaticObjList StaticObjList
// ��    ��: CDynamicObjList & DynamicObjList
// ��    ��: COORD NewPos
// �� �� ֵ: bool
//*****************************************************************************************
bool CTankObj::Isknock(CStaticObjList StaticObjList, CDynamicObjList &DynamicObjList, COORD NewPos)
{
	int k = 0;
	for (short i = NewPos.X - 1; i < NewPos.X + 2; i++)
	{
		for (short j = NewPos.Y - 1; j < NewPos.Y + 2; j++)
		{
			if (g_Map[i][j] != BLANK)  //ֻҪ��һ�����ǿյؾʹ���������������
			{
				if (g_Map[i][j] == STATIC)  //����Ǿ�̬��Ʒ�Ļ�
				{
					if (IsGrass(StaticObjList, { i,j }))  //����ǲݵصĻ�
					{
						continue;
					}
					return true;
				}
				if (g_Map[i][j] == DYNAMIC)  //����ж�̬�Ķ�����ֱ���˳�����Ҫ�ƶ�
					return true;
			}
		}
	}
	return false;
}


//*****************************************************************************************
// ��������: IsBoundary
// ����˵��: �鿴�Ƿ��ڴ˷�Χ��
// ��    ��: lracker
// ʱ    ��: 2019/08/22
// ��    ��: COORD pos
// �� �� ֵ: bool
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
// ��������: Beaten
// ����˵��: ̹�˱�����
// ��    ��: lracker
// ʱ    ��: 2019/08/22
// �� �� ֵ: bool
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
		m_IsDie = true;  //��Ĺ���
		Draw(CLEAN);
		return true;
	}
	return false;
}

//*****************************************************************************************
// ��������: IsGrass
// ����˵��: �ж��Ƿ��ڲݵ�
// ��    ��: lracker
// ʱ    ��: 2019/08/22
// ��    ��: CStaticObjList StaticObjList
// ��    ��: COORD NewPos
// �� �� ֵ: bool
//*****************************************************************************************
bool CTankObj::IsGrass(CStaticObjList StaticObjList, COORD NewPos)
{
	CStaticObjList::CStaticObjNode* p = StaticObjList.m_pHead->m_next;
	while (p)
	{
		if (p->m_Staticobj->IsBoundary(NewPos))
		{
			if (p->m_Staticobj->GetName() == GRASS)//�����������ǲݵصĻ���������
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
// ��������: Save
// ����˵��: ̹�˱��溯��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: const char * c_szFileName
// �� �� ֵ: void
//*****************************************************************************************
void CTankObj::Save(const char* c_szFileName)
{
	FILE *pFile = NULL;
	fopen_s(&pFile, c_szFileName, "ab+");
	char szName[10] = { 0 };
	sprintf_s(szName, 10, "%d", m_nName);  //д��������
	fwrite(szName, sizeof(szName), 1, pFile);
	fwrite((int*)this + 1, sizeof(CTankObj) - 4, 1, pFile);  //��д�������ṹ��
	fclose(pFile);
	pFile = NULL;
}

//*****************************************************************************************
// ��������: CreateTank
// ����˵��: ����̹��
// ��    ��: lracker
// ʱ    ��: 2019/08/22
// ��    ��: int n
// ��    ��: int nCamp
// �� �� ֵ: void
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
			pPlayerTank->InitHealth(5);  //��ҳ�ʼ����ֵΪ5
			pPlayerTank->SetStyle(0);
			pPlayerTank->SetColor(i);
		}
		pNode->m_pDynamicObj = pPlayerTank;  //����ָ��ָ�����̹��
		InsertNode(pNode);
	}
}

//*****************************************************************************************
// ��������: InsertNode
// ����˵��: ���붯̬����
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: CDynamicObjNode * & pNode
// �� �� ֵ: void
//*****************************************************************************************
void CDynamicObjList::InsertNode(CDynamicObjNode*& pNode)
{
	m_pRear->m_next = pNode;			 //ʹ��β�巨
	pNode->m_prior = m_pRear;
	m_pRear = pNode;
}


//*****************************************************************************************
// ��������: CDynamicObjList
// ����˵��: ��̬����Ĺ��캯��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: 
//*****************************************************************************************
CDynamicObjList::CDynamicObjList()
{
	CDynamicObjNode* pHeadNode = new CDynamicObjNode;  //����ͷ���
	m_pHead = pHeadNode;
	m_pRear = m_pHead;  //��ʼ����ʱ���βָ���ͷָ�����һ��
	CTankObj::g_nCount = 0;
}


//*****************************************************************************************
// ��������: ~CDynamicObjList
// ����˵��: ��������
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: 
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
// ��������: CreatBullet
// ����˵��: �����ӵ�
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: int nDir
// ��    ��: int nCamp
// ��    ��: COORD pos
// ��    ��: int nColor
// ��    ��: CStaticObjList & StaticObjList
// ��    ��: int nId
// �� �� ֵ: void
//*****************************************************************************************
void CDynamicObjList::CreatBullet(int nDir,int nCamp,COORD pos,int nColor, CStaticObjList &StaticObjList, int nId)
{
	if(nCamp==FRIEND)
		PlaySoundA("Sound\\shoot.wav", NULL, SND_ASYNC | SND_NODEFAULT); // ��ҷ����ӵ���������
	CDynamicObjNode* q = new CDynamicObjNode;
	CBulletObj* p = new CBulletObj(nDir, nCamp, pos, nColor, nId);
	q->m_pDynamicObj = p;
	if (!q->m_pDynamicObj->Isknock(StaticObjList, *this,p->RetPos()))
	{
		InsertNode(q);
		if(!p->GetHide())//���û�����صĻ�
			q->m_pDynamicObj->Draw(DRAW);
		//��������˵Ļ����ǾͲ�Ҫ������//Ʃ��ݵء�ˮ
	}
}


//*****************************************************************************************
// ��������: DrawDynmaicObjList
// ����˵��: ����̹��
// ��    ��: lracker
// ʱ    ��: 2019/08/19
// �� �� ֵ: void
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
		else if (ret == 2 && p->m_pDynamicObj->GetName() == BULLET)  //����������
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
// ��������: WhichOrder
// ����˵��: �ж���˭��������
// ��    ��: lracker
// ʱ    ��: 2019/08/22
// �� �� ֵ: void
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
			return false;  //��������������Ǿͷ��ز�������
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
// ��������: DrawHealthAndScore
// ����˵��: ��ӡ������ֵ�ͳɼ�
// ��    ��: lracker
// ʱ    ��: 2019/08/22
// �� �� ֵ: void
//*****************************************************************************************
void CDynamicObjList::DrawHealthAndScore()
{
	for (short i = 0; i < g_nFRIENDNUM; i++)
	{
		WriteChar({ g_ScoreX + 4 , g_ScoreY - 8 - 3 * i }, "����ֵ");
		WriteChar({ g_ScoreX + 5 , g_ScoreY - 6 - 3 * i }, "     ");
		char szHealth[10];
		CTankObj* p = (CTankObj*)m_pPlayer[i]->m_pDynamicObj;
		sprintf_s(szHealth, "%d", p->GetHealth());
		WriteChar({ g_ScoreX + 5 , g_ScoreY - 6 - 3 * i }, szHealth);

		WriteChar({ g_ScoreX + 9 , g_ScoreY - 8 - 3 * i }, "�ɼ�");
		WriteChar({ g_ScoreX + 10 , g_ScoreY - 6 - 3 * i }, "   ");
		char szScore[10];
		int test = p->GetScore();
		sprintf_s(szScore, "%d", p->GetScore());
		WriteChar({ g_ScoreX + 10 , g_ScoreY - 6 - 3 * i }, szScore);
	}
}

CTankObj::CTankObj(int nDir, int nCamp, int nId, int nScore) :m_nScore(nScore),CDynamicObj(nDir, nCamp, TANK,nId)
{
	if (nCamp == ENEMY)  //����ǵ���̹�˵Ļ�
	{
		m_pos = Birth();  //��ô�ڿյ�����������
		int nEndX = 27;   //��������Ϊ27��42
		int nEndY = 38;  
		CAStar astar;
		astar.getPath(nDir, m_pos.X, m_pos.Y, nEndX, nEndY, m_vecPath);  //�õ���·���������vecPath����
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
	InitTimer(m_MoveTimer);			//�����ƶ��ļ�ʱ��
	InitTimer(m_ShootTimer);
	SetdelayTime(m_MoveTimer,TKTime);		//���ó�ʱʱ��
	SetdelayTime(m_ShootTimer, ShootTime);  //�����ӵ����ʱ��
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
		pos.Y = rand() % (MaxY/2 - 2) + 1;  //���ϰ벿��
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
	if (Isknock(StaticObjList, DynamicObjList, NewPos))  //����������
	{
		return 2;
	}
	m_pos = NewPos;
	if (m_nHide)  //�������������
		return 0;
	Draw(DRAW);  //���򻭳���
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
// ��������: Isknock
// ����˵��:
// ��    ��: lracker
// ʱ    ��: 2019/08/21
// ��    ��: CStaticObjList & StaticObjList
// ��    ��: CDynamicObjList & DynamicObjList
// �� �� ֵ: bool
//			 true��������������
//			 false����û����������
//*****************************************************************************************
bool CBulletObj::Isknock(CStaticObjList &StaticObjList, CDynamicObjList &DynamicObjList, COORD NewPos)
{
	m_nHide = false;
	if (g_Map[NewPos.X][NewPos.Y] == BLANK)			 //����ǿյصĻ�
	{
		return false;
	}
	else if (g_Map[NewPos.X][NewPos.Y] == DYNAMIC)  //����Ƕ�̬��Ķ���
	{
		CDynamicObjList::CDynamicObjNode* p = DynamicObjList.m_pHead->m_next;  //ָ��ǰ�����Ľ��
		while (p)
		{
			if (p->m_pDynamicObj->IsBoundary(NewPos))	//�ж��Ƿ��ڸ÷�Χ
			{
				if (p->m_pDynamicObj->Beaten(m_nCamp))  //�ж��Ƿ�򵽲���Ч���ˣ�Ʃ���ѷ�̹�˻�����û�õ�
				{
					if (p->m_pDynamicObj->GetName() == TANK && p->m_pDynamicObj->GetCamp() == ENEMY)  //�з�̹��
					{
						for (int i = 0; i < g_nFRIENDNUM; i++)
						{
							if (DynamicObjList.m_pPlayer[i]->m_pDynamicObj->GetId() == GetId())  //���ID��ͬ
							{
								CTankObj* player = (CTankObj*)DynamicObjList.m_pPlayer[i]->m_pDynamicObj;
								player->SetScore();  //�ɼ�++
							}
								
						}
					}
					if (p->m_pDynamicObj->GetName() == BULLET)  //�ж��Ƿ�Ҫ���ٸ����壬����ֻɾ���ӵ�  //̹��ֻ���������������ɾ		
					{
						if (p == DynamicObjList.m_pRear)		//���ɾ��βָ����						
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
		CStaticObjList::CStaticObjNode* p = StaticObjList.m_pHead->m_next;  //�Ӿ�̬������׽ڵ㿪ʼ����
		CStaticObjList::CStaticObjNode* q = StaticObjList.m_pHead;  //ָ��Ҫ��ɾ���Ľ��
		int ret = 0;  //����0��ʲô��û��������Ȼ�ⲻӦ�ô��ڣ�ֻ�Ǹ���ֵ
		while (p)
		{
			if (p->m_Staticobj->IsBoundary(NewPos))
			{
				ret = p->m_Staticobj->Beaten();
				if (ret == 1)  //���ø���Ʒ����ĺ���,����ȷ��Ҫɾ���Ļ�
				{
					if (p->m_next == NULL)		//���ɾ�����һ����					
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
		if (ret == 2)  //�����˸���Ʒ��ɾ�������ӵ�ɾ����  //��ǽ
			return true;
		if (ret == 1)  //�����˸���Ʒ���ӵ���Ҫɾ������ǽ��ʱ��
			return true;
		if (ret == 3)
		{
			m_nHide = true;
			return false;  //��������Ʒ���ӵ�������ɾ���������Ͳݵص�ʱ��
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
	sprintf_s(szName, 10, "%d", m_nName);  //д��������
	fwrite(szName, sizeof(szName), 1, pFile);
	fwrite((int*)this + 1, sizeof(CBulletObj) - 4, 1, pFile);  //��д�������ṹ��
	fclose(pFile);
	pFile = NULL;
}

int CDynamicObj::TimeOut(Timer& t)
{
	// ��ȡ��ǰ��ʱ��
	clock_t now = clock();
	// ��ǰʱ�� - �ϴγ�ʱʱ��
	// �õ��Ĳ����С�ڼ��ʱ��, ˵����û�г�ʱ. 
	if (now - t.time < t.timeout) {
		return 0;
	}

	// ��ʱ֮��, ��Ҫ�������һ�εĳ�ʱʱ��
	t.time = now;
	return 1;
}

void CDynamicObj::Save(const char* c_szFileName)
{
}
