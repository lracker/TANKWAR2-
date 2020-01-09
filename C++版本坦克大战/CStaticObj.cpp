#include "pch.h"
#include "CStaticObj.h"


//*****************************************************************************************
// ��������: CStaticObj
// ����˵��: ���캯��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: int n
// �� �� ֵ: 
//*****************************************************************************************
CStaticObj::CStaticObj(int n)
{
	m_nName = n;
}

//*****************************************************************************************
// ��������: Birth
// ����˵��: ��Ƴ����ص㺯��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: COORD pos
// �� �� ֵ: void
//*****************************************************************************************
void CStaticObj::Birth(COORD pos)
{
	m_pos = pos;
}

//*****************************************************************************************
// ��������: IsBoundary
// ����˵��:
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: COORD pos
// �� �� ֵ: bool
//*****************************************************************************************
bool CStaticObj::IsBoundary(COORD pos)
{
	if (m_pos.X == pos.X && m_pos.Y == pos.Y)
	{
		return true;
	}
	return false;
}

//*****************************************************************************************
// ��������: Save
// ����˵��: ����ṹ��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: const char * c_szFileName
// �� �� ֵ: void
//*****************************************************************************************
void CStaticObj::Save(const char* c_szFileName)
{
	FILE *pFile = NULL;
	fopen_s(&pFile, c_szFileName, "ab+");
	char szName[10] = { 0 };
	sprintf_s(szName, 10, "%d", m_nName);  //д��������
	fwrite(szName, sizeof(szName), 1, pFile);  
	fwrite((int*)this+1, sizeof(*this)-4, 1, pFile);  //��д�������ṹ��
	fclose(pFile);
	pFile = NULL;
}

//*****************************************************************************************
// ��������: Beaten
// ����˵��: ǽ����ĺ���
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: int
//*****************************************************************************************
int CWALL::Beaten()
{
	return 2;  //��ǽʲô��������
}

//*****************************************************************************************
// ��������: Draw
// ����˵��: ����ǽ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: int nOrder
// �� �� ֵ: void
//*****************************************************************************************
void CWALL::Draw(int nOrder)
{
	if (nOrder == DRAW && g_Map[m_pos.X][m_pos.Y] == STATIC)
		WriteChar(m_pos, "��", FOREGROUND_RED);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_GREEN);
}

//*****************************************************************************************
// ��������: Beaten
// ����˵��: ��ǽ����ĺ���
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: int
//*****************************************************************************************
int CEarth::Beaten()    //��ǽ������
{
	g_Map[m_pos.X][m_pos.Y] = BLANK;  //��Ŀ�����Ҫ�������
	Draw(CLEAN);
	return 1;
}

//*****************************************************************************************
// ��������: Draw
// ����˵��: ������ǽ�ĺ���
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: int nOrder
// �� �� ֵ: void
//*****************************************************************************************
void CEarth::Draw(int nOrder)
{
	if (nOrder == DRAW && g_Map[m_pos.X][m_pos.Y]==STATIC)
		WriteChar(m_pos, "��", FOREGROUND_GREEN | FOREGROUND_RED);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_GREEN | FOREGROUND_RED);
}

//*****************************************************************************************
// ��������: CStaticObjNode
// ����˵��: �ṹ��Ĺ��캯��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: 
//*****************************************************************************************
CStaticObjList::CStaticObjNode::CStaticObjNode()
{
	m_Staticobj = NULL;
	m_next = NULL;
}

int CGrass::nGRASSNUMBER;
//*****************************************************************************************
// ��������: RandomCreateObj
// ����˵��: ���������Ʒ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: int n
// ��    ��: int Static
// ��    ��: CGrassPosList * g_GrassPosList
// �� �� ֵ: void
//*****************************************************************************************
void CStaticObjList::RandomCreateObj(int n ,int Static, CGrassPosList* g_GrassPosList)
{
	srand((int)time(NULL));
	COORD pos;
	for (int i = 0; i < n; i++)
	{
		CStaticObjNode* StaticObj = new CStaticObjNode;
		if (Static == EARTH)
		{
			CEarth* obj = new CEarth;
			StaticObj->m_Staticobj = obj;
		}
		else if (Static == IRON)
		{
			CIron* obj = new CIron;
			StaticObj->m_Staticobj = obj;
		}
		else if (Static == WATER)
		{
			CWater* obj = new CWater;
			StaticObj->m_Staticobj = obj;
		}
		else if (Static == GRASS)
		{
			CGrass* obj = new CGrass;
			StaticObj->m_Staticobj = obj;
		}
		while (true)
		{
			pos.X = rand() % (MaxX - 2) + 1;
			pos.Y = rand() % (MaxY - 2) + 1;
			if (g_Map[pos.X][pos.Y] == BLANK)
			{
				break;
			}
		}
		StaticObj->m_Staticobj->Birth(pos);
		InsertCStaticObjList(StaticObj);
		g_Map[pos.X][pos.Y] = STATIC;
		if (Static == GRASS)
		{
			CGrassPosList::GrassPos* pPos = new CGrassPosList::GrassPos;
			pPos->pos = pos;
			g_GrassPosList->GrassPosInsert(pPos);
			CGrass::nGRASSNUMBER++;
		}
	}
	
}

//*****************************************************************************************
// ��������: CStaticObjList
// ����˵��: ��̬����Ĺ��캯��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: 
//*****************************************************************************************
CStaticObjList::CStaticObjList()
{
	CStaticObjNode* pHeadNode = new CStaticObjNode;  //����ͷ���
	m_pHead = pHeadNode;
}

//*****************************************************************************************
// ��������: ~CStaticObjList
// ����˵��: ��̬�������������
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: 
//*****************************************************************************************
CStaticObjList::~CStaticObjList()
{
	CStaticObjNode* p;
	CStaticObjNode* q;
	p = m_pHead->m_next;
/*
	while (p)
	{
		q = p->m_next;
		delete p;
		p = q;
	}*/
	p = NULL;
	q = NULL;
	m_pHead = NULL;
}

//*****************************************************************************************
// ��������: InsertCStaticObjList
// ����˵��: ��̬����Ĳ��뺯��
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: CStaticObjNode * & StaticObj
// �� �� ֵ: void
//*****************************************************************************************
void CStaticObjList::InsertCStaticObjList(CStaticObjNode*& StaticObj)
{
	if (m_pHead->m_next)  //�ж�һ�£�ͷ�ڵ�next�ǲ���null
	{
		StaticObj->m_next = m_pHead->m_next;
		m_pHead->m_next->m_prior = StaticObj;
		m_pHead->m_next = StaticObj;
		StaticObj->m_prior = m_pHead;
	}
	else
	{
		StaticObj->m_next = m_pHead->m_next;
		StaticObj->m_prior = m_pHead;
		m_pHead->m_next = StaticObj;
	}
}

//*****************************************************************************************
// ��������: CreateWall
// ����˵��: ����ǽ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: void
//*****************************************************************************************
void CStaticObjList::CreateWall()
{
	for (short i = 0; i < MaxX; i++)
	{
		for (short j = 0; j < MaxY; j++)
		{
			if (i == 0 || i == MaxX - 1 || j == 0 || j == MaxY - 1)
			{
				CStaticObjNode* StaticObj = new CStaticObjNode;
				CWALL* Wall = new CWALL;
				Wall->Birth({ i,j });
				StaticObj->m_Staticobj = Wall;
				InsertCStaticObjList(StaticObj);
				g_Map[i][j] = STATIC;
			}
		}
	}
}


//*****************************************************************************************
// ��������: CreateHome
// ����˵��: ������
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: void
//*****************************************************************************************
void CStaticObjList::CreateHome()
{
	int k = 0;
	for (short i = 25; i < 27; i++)
	{
		for (short j = MaxY - 2; j > MaxY - 6; j--)
		{
			CStaticObjNode* pNode = new CStaticObjNode;
			CEarth* q = new CEarth;
			q->Birth({ i,j });
			pNode->m_Staticobj = q;
			q->Draw(DRAW);
			g_Map[i][j] = STATIC;
			InsertCStaticObjList(pNode);
			k++;
		}
	}
	for (short i = 29; i < 31; i++)
	{
		for (short j = MaxY - 2; j > MaxY - 6; j--)
		{
			CStaticObjNode* pNode = new CStaticObjNode;
			CEarth* q = new CEarth;
			q->Birth({ i,j });
			pNode->m_Staticobj = q;
			q->Draw(DRAW);
			g_Map[i][j] = STATIC;
			InsertCStaticObjList(pNode);
			k++;
		}
	}
	for (short i = 25; i < 31; i++)
	{
		for (short j = MaxY - 5; j < MaxY - 3; j++)
		{
			CStaticObjNode* pNode = new CStaticObjNode;
			CEarth* q = new CEarth;
			q->Birth({ i,j });
			pNode->m_Staticobj = q;
			q->Draw(DRAW);
			g_Map[i][j] = STATIC;
			InsertCStaticObjList(pNode);
			k++;
		}
	}
	for (short i = 27; i < 29; i++)  //��������
	{
		for (short j = MaxY - 3; j < MaxY - 1; j++)
		{
			CStaticObjNode* pNode = new CStaticObjNode;  
			CGen* q = new CGen;
			q->Birth({ i,j });
			pNode->m_Staticobj = q;
			q->Draw(DRAW);
			g_Map[i][j] = STATIC;
			InsertCStaticObjList(pNode);
			k++;
		}
	}
}

//*****************************************************************************************
// ��������: DrawObj
// ����˵��: ������Ʒ
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: void
//*****************************************************************************************
void CStaticObjList::DrawObj()
{
	CStaticObjNode* pHead = m_pHead->m_next;
	int k = 0;
	while (pHead)
	{
		pHead->m_Staticobj->Draw(DRAW);
		pHead = pHead->m_next;
		k++;
	}
}

//*****************************************************************************************
// ��������: GetLen
// ����˵��: ��ȡ������
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: int
//*****************************************************************************************
int CStaticObjList::GetLen()
{
	CStaticObjNode* pHead = m_pHead->m_next;
	int i = 0;
	while (pHead)
	{
		i++;
		pHead = pHead->m_next;
	}
	return i;
}

//*****************************************************************************************
// ��������: Beaten
// ����˵��: ��������
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: int
//*****************************************************************************************
int CIron::Beaten()
{
	return 2;
}

//*****************************************************************************************
// ��������: Draw
// ����˵��: �������ĺ���
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: int nOrder
// �� �� ֵ: void
//*****************************************************************************************
void CIron::Draw(int nOrder)
{
	if (nOrder == DRAW && g_Map[m_pos.X][m_pos.Y] == STATIC)
		WriteChar(m_pos, "��", FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

//*****************************************************************************************
// ��������: Beaten
// ����˵��: ˮ����ĺ���
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: int
//*****************************************************************************************
int CWater::Beaten()
{
	return 3;
}

//*****************************************************************************************
// ��������: Draw
// ����˵��: ����ˮ�ĺ���
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: int nOrder
// �� �� ֵ: void
//*****************************************************************************************
void CWater::Draw(int nOrder)
{
	if (nOrder == DRAW && g_Map[m_pos.X][m_pos.Y] == STATIC)
		WriteChar(m_pos, "��", FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

//*****************************************************************************************
// ��������: Beaten
// ����˵��: �ݵر���ĺ���
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// �� �� ֵ: int
//*****************************************************************************************
int CGrass::Beaten()
{
	return 3;
}

//*****************************************************************************************
// ��������: Draw
// ����˵��: �����ݵصĺ���
// ��    ��: lracker
// ʱ    ��: 2019/08/23
// ��    ��: int nOrder
// �� �� ֵ: void
//*****************************************************************************************
void CGrass::Draw(int nOrder)
{
	if (nOrder == DRAW)
		WriteChar(m_pos, "��", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}


CGrassPosList::CGrassPosList()
{
	GrassPos* p = new GrassPos;
	m_pHead = p;
	CGrass::nGRASSNUMBER = 0;
}

void CGrassPosList::GrassPosInsert(GrassPos*& pNode)
{
	pNode->m_next = m_pHead->m_next;
	m_pHead->m_next = pNode;
}

CGrassPosList::GrassPos::GrassPos()
{
	COORD pos = { 0,0 };
	m_next = NULL;
}

int CGen::Beaten()
{
	nGameOver = TRUE;
	return true;
}

void CGen::Draw(int nOrder)
{
	if (nOrder == DRAW && g_Map[m_pos.X][m_pos.Y] == STATIC)
		WriteChar(m_pos, "��", FOREGROUND_RED | FOREGROUND_INTENSITY);
	else if (nOrder == CLEAN)
		WriteChar(m_pos, "  ", FOREGROUND_RED | FOREGROUND_INTENSITY);
}
