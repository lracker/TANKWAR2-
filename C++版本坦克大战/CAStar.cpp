#include "pch.h"
#include "CAStar.h"
#include "Draw.h"


void CAStar::setStartPos(int x, int y)
{
	m_startNode.m_nX = x;
	m_startNode.m_nY = y;
}

void CAStar::setEndPos(int x, int y)
{
	m_endNode.m_nX = x;
	m_endNode.m_nY = y;
}

void CAStar::SetStartDirection(int direction)
{
	m_startNode.m_nDir = direction;
}

bool CAStar::getPath(vector<Node>& vecPath)
{
	if (isEndPos(m_startNode))  //���һ��ʼ�����յ��ˣ��Ǿͷ���
		return false;
	m_vecOpen.clear();  //���������
	m_vecClose.clear();
	Point point = { m_startNode };  
	m_vecOpen.push_back(point);  //�����ʼ����
	Point stcArray[4];   // �����ĸ��������
	while (!m_vecOpen.empty())
	{
		int nMinIndex = getOpenMinNode();
		createNode(m_vecOpen[nMinIndex].current, stcArray);
		m_vecClose.push_back(m_vecOpen[nMinIndex]);  //���õ���뵽close��
		m_vecOpen.erase(m_vecOpen.begin() + nMinIndex);  //���õ��open����ɾ��
		for (int i = 0; i < 4; ++i)
		{
			if (isEndPos(stcArray[i].current))
			{
				return searchPath(stcArray[i], vecPath);
			}
			else if (isValidNode(stcArray[i].current))  //�жϸõ��Ƿ���Ч
			{
				m_vecOpen.push_back(stcArray[i]);
			}
		}
	}
	return false;
}

bool CAStar::getPath(int direction, int nStarX, int nStarY, int nEndX, int nEndY, vector<Node>& vecPath)
{
	setStartPos(nStarX, nStarY);  //���ÿ�ʼ����
	setEndPos(nEndX, nEndY);	  //�����յ�����
	SetStartDirection(direction); //���ó�ʼ����
	return getPath(vecPath);      //��ȡ·��
}

bool CAStar::searchPath(Point endNode, vector<Node>& vecPath)
{
	for (unsigned int i = 0; i < m_vecClose.size(); ++i)
	{
		if (m_vecClose[i].current == endNode.previous)  //�����������յ����һ���㣬��ô�����ˣ����浽vecPath����
		{
			vecPath.push_back(m_vecClose[i].current);  
			endNode = m_vecClose[i];
			i = 0;
		}
	}
	return !vecPath.empty();
}

bool CAStar::isEndPos(const Node& node)
{
	return m_endNode == node;
}

bool CAStar::isValidNode(const Node& node)
{
	for (unsigned int i = 0; i < m_vecOpen.size(); ++i)
	{
		if (node == m_vecOpen[i].current)  //�����Ƿ���open������
			return false;
	}
	for (unsigned int i = 0; i < m_vecClose.size(); ++i)
	{
		if (node == m_vecClose[i].current)  //�����Ƿ���close������
			return false;
	}
/*
	if (g_Map[node.m_nX][node.m_nY] != BLANK)  //������ǿյؾ������ϰ�����
		return false;*/
	switch (node.m_nDir)  //�жϸ÷�Χ���ǲ����ж���
	{
	case UP:
		if (g_Map[node.m_nX - 1][node.m_nY - 1] != BLANK ||
			g_Map[node.m_nX + 1][node.m_nY - 1] != BLANK ||
			g_Map[node.m_nX][node.m_nY - 1] != BLANK)
			return false;
	case DOWN:
		if (g_Map[node.m_nX - 1][node.m_nY + 1] != BLANK ||
			g_Map[node.m_nX + 1][node.m_nY + 1] != BLANK ||
			g_Map[node.m_nX][node.m_nY + 1] != BLANK)
			return false;
	case LEFT:
		if (g_Map[node.m_nX - 1][node.m_nY - 1] != BLANK ||
			g_Map[node.m_nX - 1][node.m_nY + 1] != BLANK ||
			g_Map[node.m_nX - 1][node.m_nY] != BLANK)
			return false;
	case RIGHT:
		if (g_Map[node.m_nX + 1][node.m_nY - 1] != BLANK ||
			g_Map[node.m_nX + 1][node.m_nY + 1] != BLANK ||
			g_Map[node.m_nX + 1][node.m_nY] != BLANK)
			return false;
	}
	return true;
}

void CAStar::createNode(const Node& center, Point* pArray)
{
	int nStepX[4] = { 0,0,-1,1 };  //��Ϊ̹�� 9����ģ�
	int nStepY[4] = { -1,1,0,0 };
	for (int i = 0; i < 4; ++i)
	{
		pArray[i].previous = center;
		pArray[i].current.m_nDir = i;
		pArray[i].current.m_nG = center.m_nG + 1;
		pArray[i].current.m_nX = center.m_nX + nStepX[i];
		pArray[i].current.m_nY = center.m_nY + nStepY[i];
		pArray[i].current.SetH(m_endNode.m_nX, m_endNode.m_nY);
	}
}

int CAStar::getOpenMinNode()
{
	int minIndex = 0;
	int F = m_vecOpen.begin()->current.m_nF;
	for (unsigned int i = 0; i < m_vecOpen.size(); ++i)
	{
		if (F >= m_vecOpen[i].current.m_nF)
		{
			F = m_vecOpen[i].current.m_nF;
			minIndex = i;
		}
	}
	return minIndex;
}
