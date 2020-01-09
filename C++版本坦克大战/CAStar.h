#pragma once
#include <math.h>
#include <vector>
#include "DataBase.h"
using std::vector;

class Node
{
public:
	int m_nX;  //x坐标
	int m_nY;  //y坐标
	int m_nF;  //F值
	int m_nG;  //G值，移动损耗，是上一步的G值+1，处于起点时g为0
	int m_nH;  //H值，当前点和终点的直线距离，一般是当前点的x,y坐标和终点的x，y坐标相加之和
	int m_nDir;  //方向
public:
	Node() :m_nX(), m_nY(), m_nF(), m_nG(), m_nH(), m_nDir() {}
	Node(int nX, int nY, int nG, int nDir) :m_nX(nX), m_nY(nY), m_nG(nG), m_nDir(nDir) {}
	void SetH(int nEndX, int nEndY) {
		m_nH = abs(m_nX - nEndX) + abs(m_nY - nEndY);
		m_nF = m_nG + m_nH;
	}
	friend bool operator==(const Node& rLeft, const Node& rRight) {
		return (rLeft.m_nX == rRight.m_nX) && (rLeft.m_nY == rRight.m_nY);
	}
};

typedef struct _POINT
{
	Node current;   //当前点
	Node previous;  //上一个点
}Point;

class CAStar
{
	vector<Point> m_vecOpen;	//open表，保存地图上那些没有走过的点
	vector<Point> m_vecClose;	//close表，保存已经走过的点
	Node		  m_startNode;  //起点
	Node		  m_endNode;    //终点
public:
	void setStartPos(int x, int y);			//设置开始坐标
	void setEndPos(int x, int y);			//设置结束坐标
	void SetStartDirection(int direction);  //设置初始方向
	bool getPath(vector<Node>& vecPath);	//获取路径，并将获取到的路径保存到vecPath中
	bool getPath(int direction, int nStarX, int nStarY, int nEndX, int nEndY, vector<Node>& vecPath);				  //获取路径，并将获取到的路径保存到vecPath中

protected:
	bool searchPath(Point endNode, vector<Node>& vecPath);  //在close表中搜索出一条连接终点和起点的路径
	bool isEndPos(const Node& node);						//判断一个是不是终点
	bool isValidNode(const Node& node);					    //判断一个点是不是有效点
	void createNode(const Node& center, Point* pArray);     //在中心点周围创建出4个点
	int getOpenMinNode();									//在open表中找到一个最小值的节点
};

