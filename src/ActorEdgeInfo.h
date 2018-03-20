//提取SDF中actor间消息的类型及大小
#ifndef _ACTOREDGEINFO_H_
#define _ACTOREDGEINFO_H_
#include "flatNode.h"
#include "Partition.h"
#include <set>

using namespace std;
struct StreamEdgeInfo	
{
	string typedefine;
	int size;//大小
	string typeName;//类型名称
	StreamEdgeInfo(int size,string typeName,string typedefine):size(size),typeName(typeName),typedefine(typedefine){}
	StreamEdgeInfo(){}
};

class ActorEdgeInfo
{
public:
	ActorEdgeInfo(SchedulerSSG*);
	
	ActorEdgeInfo(DividedStaticGraph*);
	StreamEdgeInfo getEdgeInfo(FlatNode*,FlatNode*);
	void DeclEdgeType(stringstream &buf);

private:	
	vector<FlatNode *> flatNodes_;
	map<pair<FlatNode*,FlatNode*>,StreamEdgeInfo> mapEdge2TypeInfo;//存储每条边到其类型信息的映射
	vector<StreamEdgeInfo> vStreamTypeInfo;	//存放各种类型边的信息
	
};
//改为公共函数
extern string GetPrimDataType(Node *from, int *);
extern string GetPrimDataType(Node *from);
extern string GetArrayDataType(Node *node);
extern string GetArrayDim(Node *node);
#endif