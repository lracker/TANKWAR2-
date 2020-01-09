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
	if (isEndPos(m_startNode))  //如果一开始就是终点了，那就返回
		return false;
	m_vecOpen.clear();  //清空两个表
	m_vecClose.clear();
	Point point = { m_startNode };  
	m_vecOpen.push_back(point);  //放入初始坐标
	Point stcArray[4];   // 生成四个点的坐标
	while (!m_vecOpen.empty())
	{
		int nMinIndex = getOpenMinNode();
		createNode(m_vecOpen[nMinIndex].current, stcArray);
		m_vecClose.push_back(m_vecOpen[nMinIndex]);  //将该点加入到close表
		m_vecOpen.erase(m_vecOpen.begin() + nMinIndex);  //将该点从open表里删除
		for (int i = 0; i < 4; ++i)
		{
			if (isEndPos(stcArray[i].current))
			{
				return searchPath(stcArray[i], vecPath);
			}
			else if (isValidNode(stcArray[i].current))  //判断该点是否有效
			{
				m_vecOpen.push_back(stcArray[i]);
			}
		}
	}
	return false;
}

bool CAStar::getPath(int direction, int nStarX, int nStarY, int nEndX, int nEndY, vector<Node>& vecPath)
{
	setStartPos(nStarX, nStarY);  //设置开始坐标
	setEndPos(nEndX, nEndY);	  //设置终点坐标
	SetStartDirection(direction); //设置初始方向
	return getPath(vecPath);      //获取路径
}

bool CAStar::searchPath(Point endNode, vector<Node>& vecPath)
{
	for (unsigned int i = 0; i < m_vecClose.size(); ++i)
	{
		if (m_vecClose[i].current == endNode.previous)  //如果这个点是终点的上一个点，那么就是了，保存到vecPath里面
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
		if (node == m_vecOpen[i].current)  //看看是否在open表里了
			return false;
	}
	for (unsigned int i = 0; i < m_vecClose.size(); ++i)
	{
		if (node == m_vecClose[i].current)  //看看是否在close表里了
			return false;
	}
/*
	if (g_Map[node.m_nX][node.m_nY] != BLANK)  //如果不是空地就是有障碍物了
		return false;*/
	switch (node.m_nDir)  //判断该范围内是不是有东西
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
	int nStepX[4] = { 0,0,-1,1 };  //因为坦克 9个点的，
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
