#pragma once
#include <math.h>
#include <vector>
#include "DataBase.h"
using std::vector;

class Node
{
public:
	int m_nX;  //x����
	int m_nY;  //y����
	int m_nF;  //Fֵ
	int m_nG;  //Gֵ���ƶ���ģ�����һ����Gֵ+1���������ʱgΪ0
	int m_nH;  //Hֵ����ǰ����յ��ֱ�߾��룬һ���ǵ�ǰ���x,y������յ��x��y�������֮��
	int m_nDir;  //����
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
	Node current;   //��ǰ��
	Node previous;  //��һ����
}Point;

class CAStar
{
	vector<Point> m_vecOpen;	//open�������ͼ����Щû���߹��ĵ�
	vector<Point> m_vecClose;	//close�������Ѿ��߹��ĵ�
	Node		  m_startNode;  //���
	Node		  m_endNode;    //�յ�
public:
	void setStartPos(int x, int y);			//���ÿ�ʼ����
	void setEndPos(int x, int y);			//���ý�������
	void SetStartDirection(int direction);  //���ó�ʼ����
	bool getPath(vector<Node>& vecPath);	//��ȡ·����������ȡ����·�����浽vecPath��
	bool getPath(int direction, int nStarX, int nStarY, int nEndX, int nEndY, vector<Node>& vecPath);				  //��ȡ·����������ȡ����·�����浽vecPath��

protected:
	bool searchPath(Point endNode, vector<Node>& vecPath);  //��close����������һ�������յ������·��
	bool isEndPos(const Node& node);						//�ж�һ���ǲ����յ�
	bool isValidNode(const Node& node);					    //�ж�һ�����ǲ�����Ч��
	void createNode(const Node& center, Point* pArray);     //�����ĵ���Χ������4����
	int getOpenMinNode();									//��open�����ҵ�һ����Сֵ�Ľڵ�
};

