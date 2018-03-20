# include "X86CodeGenerate.h"
#include <algorithm>
#ifndef WIN32
#include <sys/stat.h> 
#endif
using namespace std;
#define  Default_Repeat_Count 10
#ifdef WIN32
	#define mkdir(tmp) _mkdir(tmp.c_str());
#else
	#define mkdir(tmp) mkdir(tmp.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif	
static bool isInComma = false;//表示当前处于逗号表达式中
static int flag=0;
static int flag_Global = 0;//标识是否在输出GlobalVarCpp文件，初始化为0
//static bool FileRW = 0;//标识是否有文件读写actor

static bool isInFileReader = false;//表示当前处于FileReader中
static bool isInFileWriter = false;//表示当前处于FileWriter中
static bool FileRW = 0;//标识是否有文件读写actor
static bool FileR  = 0; // 标识是否有文件读 actor
static bool FileW  = 0; // 标识是否有文件写 actor
static bool existFileWriter = false;//用于标识是否存在FileWriter，默认为否

static bool isstructVar = false;//表示该变量为结构体内变量
static bool isUnionVar = false;
static bool isEnumVar = false;


//static bool existFileWriter = false, existFileReader = false;//用于标识是否存在FileWriter，默认为否
static bool isCGGlobalVar = false;//表示目前正处于生成全局变量
static FlatNode* curActor = NULL;//用于表示当前的生成actor文件
extern char* objName;
X86CodeGenerate::X86CodeGenerate(SchedulerSSG *Sssg, int nCpucore, const char *currentDir, StageAssignment *psa, Partition *Mptemp)
{
	pSa = psa;
	Mp=Mptemp;
	sssg_ = Sssg;
	flatNodes_ = Sssg->GetFlatNodes();
	pEdgeInfo = new ActorEdgeInfo(Sssg);
	nCpucore_ = nCpucore;
	nActors_ = flatNodes_.size();
	readerActor = writerActor = NULL;
	dir_ = currentDir;
	//建立代码生成目标文件的目录

	/* Begin ——wangliang*/
	string::size_type pos = dir_.find_last_of("\\");
	string tmp = dir_.substr(0, pos);
	mkdir(tmp);//目标文件夹没有则创建，有则下面会删除，主要是防止上一次运行产生的目标文件和这一次的混合，导致目标代码编译出错
	string command = "rmdir/s/q " + tmp;

#ifdef WIN32
	if (system(command.c_str()) == 0){//删除该文件夹
		//cout << "\n删除成功" << endl;
	}
	//else cout << "\n删除失败" << endl;
#else
	if (remove(tmp.c_str()) == 0){
		//cout << "\n删除成功" << endl;
	}
	//else cout << "\n删除失败" << endl;
#endif

	mkdir(tmp);
	/* End ——wangliang*/

	//建立mapActor2InEdge，mapActor2OutEdge以及readerActor、writeActor
	vector<FlatNode*>::iterator iter1,iter2,iter3;
	for (iter1=flatNodes_.begin();iter1!=flatNodes_.end();++iter1)
	{
		for (iter2=(*iter1)->inFlatNodes.begin();iter2!=(*iter1)->inFlatNodes.end();++iter2)
		{
			string tempstring1=(*iter2)->name+"_"+(*iter1)->name;//得到形如A_B的边名称
			mapActor2InEdge.insert(make_pair((*iter1),tempstring1));
		}
		for (iter3=(*iter1)->outFlatNodes.begin();iter3!=(*iter1)->outFlatNodes.end();++iter3)
		{
			string tempstring2=(*iter1)->name+"_"+(*iter3)->name;
			mapActor2OutEdge.insert(make_pair((*iter1),tempstring2));
		}
		if((*iter1)->name.find("FileReader") != -1)
		{
			readerActor = (*iter1);
			//existFileReader = true;
			FileR = 1;
		}			
		if((*iter1)->name.find("FileWriter") != -1)
		{	
			writerActor = (*iter1);
			existFileWriter = true;
			FileW = 1;
		}
	}
	//构造每个线程上的stage集合mapNum2Stage
	vector<FlatNode*>tempactors;
	vector<FlatNode*>::iterator iter;
	for (int i=0;i<nCpucore;i++)
	{
		set<int>tempstageset;
		tempstageset.clear();
		tempactors=Mp->findNodeSetInPartition(i);		//根据线程号找到actor集合
		//遍历actor
		for (iter=tempactors.begin();iter!=tempactors.end();++iter)
		{
			tempstageset.insert(pSa->FindStage(*iter));
		}
		mapNum2Stage.insert(make_pair(i,tempstageset));
	}

	extractDecl = false;
	isInParam = false;
}
void X86CodeGenerate::CGdeclList(FlatNode *actor, OperatorType ot, stringstream &buf)		//输出变量声明
{
	List *state = NULL;
	Node  *param = NULL;
	assert(actor);
	buf <<"\t// AST Variables\n";
	//state，var，param被常量传播消除
	state = actor->contents->body->u.operBody.state;


	extractDecl = true;//标志生成decl
	declList.str(""); // 清空declList内容
	buf<<"\tint steadyScheduleCount;\t//稳态时一次迭代的执行次数\n";
	buf<<"\tint initScheduleCount;\n";
	//state
	SPL2X86_List(state, 0);//输出state
	buf << "\t/* *****logic state***** */\n\t" << declList.str();
	extractDecl = false;
	//state init
	stateInit << declInitList.str();
	declInitList.str(""); // 清空declInitList内容
}
void X86CodeGenerate::CGinitVarAndState(FlatNode *actor, OperatorType ot, stringstream &buf)	//输出initVar和State
{
	buf << "\t// initVarAndState\n";
	buf << "\tvoid initVarAndState() {\n";

	declInitList << "\n";
	buf << declInitList.str();
	buf << "\n\t\t/**** State Init ****/\n";
	buf << stateInit.str();
	buf << "\n\t}\n"; // initVarAndState方法结束

	declInitList.str(""); // 清空
	stateInit.str(""); // 清空
}
void X86CodeGenerate::CGlogicInit(FlatNode *actor, OperatorType ot, stringstream &buf)
{
	declInitList.str(""); // 清空
	buf << "\t// init\n";
	buf << "\tvoid init()";
	Node *init = actor->contents->body->u.operBody.init;
	if (init)
	{
		SPL2X86_Node(init, 2);
		buf << declInitList.str(); // init结构必须带"{}"
	}
	else
	{
		buf << " {\n";
		buf << "\t}\n"; // '}'独占一行
	}
	declInitList.str(""); // 清空
}
void X86CodeGenerate::CGthis(FlatNode *actor, OperatorType ot, stringstream &buf)	//输出actor的构造函数
{
	buf <<"\t// Constructor\n";
	buf << "\t"<<actor->name<<"(" ;
	vector<FlatNode*>::iterator iter,end;	
	
	//遍历输出边
	iter = actor->outFlatNodes.begin();
	end = actor->outFlatNodes.end();
	List *outputList = actor->contents->decl->u.decl.type->u.operdcl.outputs;
	ListMarker output_maker;
	Node *outputNode = NULL;
	IterateList(&output_maker,outputList);/*int i=0;*/
	while(iter!=end)
	{ 
		NextOnList(&output_maker,(GenericREF)&outputNode);
		string outputString;
		if(outputNode->typ == Id) outputString = outputNode->u.id.text;
		else if (outputNode->typ == Decl) outputString = outputNode->u.decl.name;
		buf<<"Buffer<"<<pEdgeInfo->getEdgeInfo(actor,(*iter)).typeName<<">& "<<outputString<<",";
		iter++;
	}
	//遍历输入边
	iter = actor->inFlatNodes.begin();
	end = actor->inFlatNodes.end();
	//获取actor内输入边的名称
	List *inputList =actor->contents->decl->u.decl.type->u.operdcl.inputs;
	ListMarker input_maker;
	Node *inputNode = NULL;
	IterateList(&input_maker,inputList);
	while(iter!=end)
	{
		NextOnList(&input_maker,(GenericREF)&inputNode);
		string inputString;
		if(inputNode->typ == Id)inputString = inputNode->u.id.text;
		else if (inputNode->typ == Decl) inputString = inputNode->u.decl.name;
		buf<<"Buffer<"<<pEdgeInfo->getEdgeInfo((*iter),actor).typeName<<">& "<<inputString<<",";
		iter++;
	}
	buf<<parameterBuf.str();
	if (parameterBuf.str()=="")
	{
		buf.seekp((int)buf.tellp()-1);
	}
	else
	{
		buf.seekp((int)buf.tellp()-2);
	}
	buf<<"):";
	IterateList(&input_maker,inputList);
	while(NextOnList(&input_maker,(GenericREF)&inputNode))
	{
		string inputString;
		if(inputNode->typ == Id)inputString = inputNode->u.id.text;
		else if (inputNode->typ == Decl) inputString = inputNode->u.decl.name;
		buf<<inputString<<"("<<inputString<<"),";
	}
		IterateList(&output_maker,outputList);
	while(NextOnList(&output_maker,(GenericREF)&outputNode))
	{
		string outputString;
		if(outputNode->typ == Id) outputString = outputNode->u.id.text;
		else if (outputNode->typ == Decl) outputString = outputNode->u.decl.name;
		buf<<outputString<<"("<<outputString<<"),";
	}
	buf.seekp((int)buf.tellp()-1);
	buf<<"{\n";
	buf << thisBuf.str();
	//buf << "\t\tRepeatCount = rc;\n ";
	buf<< "\t\tsteadyScheduleCount = "<<sssg_->GetSteadyCount(actor)<<";\n";
	buf<< "\t\tinitScheduleCount = "<<sssg_->GetInitCount(actor)<<";\n";
	buf << "\t}\n"; // init方法结束
}


void X86CodeGenerate::CGinitWork(stringstream &buf)
{
	buf << "\t// initWork\n";
	buf << "\tvoid initWork() {\n";
	buf << "\t\tinitVarAndState();\n";
	buf << "\t\tinit();\n";

	buf << "\t}\n"; // initWork方法结束
}

void X86CodeGenerate::CGinitPeek(stringstream &buf, string initPeekBuf)
{
	buf <<"\t// initPeek\n";
	buf << "\tdef initPeek() {\n";

	buf << initPeekBuf;

	buf << "\t}\n"; // initPeek方法结束
}


void X86CodeGenerate::CGpopToken(FlatNode *actor,stringstream &buf)
{
	buf << "\t// popToken\n";
	buf << "\tvoid popToken() {";
	List *inputList =actor->contents->decl->u.decl.type->u.operdcl.inputs;
	List *outputList = actor->contents->decl->u.decl.type->u.operdcl.outputs;
	ListMarker input_maker;
	ListMarker output_maker;
	Node *inputNode = NULL;
	Node *outputNode = NULL;
	IterateList(&input_maker,inputList);
	vector<int>::iterator iter=actor->inPopWeights.begin();
	while(NextOnList(&input_maker,(GenericREF)&inputNode))
	{
		string inputString;
		if(inputNode->typ == Id)inputString = inputNode->u.id.text;
		else if (inputNode->typ == Decl) inputString = inputNode->u.decl.name;
		buf<<"\n\t"<<inputString<<".updatehead"<<"("<<*iter<<");\n";
		iter++;
	}
	buf << "\t}\n"; // popToken方法结束
}
void X86CodeGenerate::CGpushToken(FlatNode *actor,stringstream &buf)
{
	buf << "\t// pushToken\n";
	buf << "\tvoid pushToken() {";
	List *outputList = actor->contents->decl->u.decl.type->u.operdcl.outputs;
	ListMarker output_maker;
	Node *outputNode = NULL;
	IterateList(&output_maker,outputList);
	vector<int>::iterator iter=actor->outPushWeights.begin();
	while(NextOnList(&output_maker,(GenericREF)&outputNode))
	{
		string outputString;
		if(outputNode->typ == Id)outputString = outputNode->u.id.text;
		else if (outputNode->typ == Decl) outputString = outputNode->u.decl.name;
		buf<<"\n\t"<<outputString<<".updatetail"<<"("<<*iter<<");\n";
		iter++;
	}
	//buf << popingBuf;

	buf << "\t}\n"; // popToken方法结束
}

void X86CodeGenerate::CGpushToken(stringstream &buf, string pushingBuf)
{
	buf << "\t// pushToken\n";
	buf << "\tdef pushToken() {\n";

	buf << pushingBuf;

	buf << "\t}\n"; // pushToken方法结束
}
void X86CodeGenerate::CGrun(stringstream &buf, string initFun)
{
	buf <<"\t// run\n";
	buf << "\tpublic void run() {\n";//run方法,类中最主要的方法

	buf << "\t\t" << initFun << ";\n"; // 调用init方法

	buf << strScheduler.str();

	buf << "\t}\n"; // run方法结束
}
void X86CodeGenerate::CGrunInitScheduleWork(FlatNode *actor,stringstream &buf)
{
	buf <<"\t// runInitScheduleWork\n";
	buf << "\tvoid runInitScheduleWork() {\n";
	buf << "\t\tinitWork();\n"; // 调用initwork方法
	buf<<"\t\tfor(int i=0;i<initScheduleCount;i++)\n\t\t\twork();\n";
	if(NoCheckBuffer){
		//出边
		List *outputList = actor->contents->decl->u.decl.type->u.operdcl.outputs;
		ListMarker output_maker;
		Node *outputNode = NULL;
		IterateList(&output_maker,outputList);
		while(NextOnList(&output_maker,(GenericREF)&outputNode))
		{
			string outputString;
			if(outputNode->typ == Id)outputString = outputNode->u.id.text;
			else if (outputNode->typ == Decl) outputString = outputNode->u.decl.name;
			buf<<"\t\t"<<outputString<<".resetTail();\n";
		}
		//入边
		List *inputList =actor->contents->decl->u.decl.type->u.operdcl.inputs;
		ListMarker input_maker;
		Node *inputNode = NULL;
		IterateList(&input_maker,inputList);
		while(NextOnList(&input_maker,(GenericREF)&inputNode))
		{
			string inputString;
			if(inputNode->typ == Id)inputString = inputNode->u.id.text;
			else if (inputNode->typ == Decl) inputString = inputNode->u.decl.name;
			buf<<"\t\t"<<inputString<<".resetHead();\n";
		}
		//buf<<"\t\t//cout<<\""<<actor->name<<" init over\"<<endl;\n";
	}
	buf << "\t}\n"; // CGrunInitScheduleWork方法结束
}
void X86CodeGenerate::CGrunSteadyScheduleWork(FlatNode *actor,stringstream &buf)
{
	buf <<"\t// CGrunSteadyScheduleWork\n";
	buf << "\tvoid runSteadyScheduleWork() {\n";
	if(AmplifySchedule)
	{
		buf<<"\t\tfor(int j=0;i<AMPLIFYFACTOR;j++)\n\t\t{\n";
	}	
	buf<<"\t\tfor(int i=0;i<steadyScheduleCount;i++)\n\t\t\twork();\n";
	if(NoCheckBuffer)
	{
		//出边
		List *outputList = actor->contents->decl->u.decl.type->u.operdcl.outputs;
		ListMarker output_maker;
		Node *outputNode = NULL;
		IterateList(&output_maker,outputList);
		while(NextOnList(&output_maker,(GenericREF)&outputNode))
		{
			string outputString;
			if(outputNode->typ == Id)outputString = outputNode->u.id.text;
			else if (outputNode->typ == Decl) outputString = outputNode->u.decl.name;
			buf<<"\t\t"<<outputString<<".resetTail();\n";
		}
		//入边
		List *inputList =actor->contents->decl->u.decl.type->u.operdcl.inputs;
		ListMarker input_maker;
		Node *inputNode = NULL;
		IterateList(&input_maker,inputList);
		while(NextOnList(&input_maker,(GenericREF)&inputNode))
		{
			string inputString;
			if(inputNode->typ == Id)inputString = inputNode->u.id.text;
			else if (inputNode->typ == Decl) inputString = inputNode->u.decl.name;
			buf<<"\t\t"<<inputString<<".resetHead();\n";
		}
		//buf<<"\t\t//cout<<\""<<actor->name<<" steady over\"<<endl;\n";
	}
	if(AmplifySchedule)
		buf<<"\t\t}\n";
	buf << "\t}\n"; // // CGrunSteadyScheduleWork方法结束
}
void X86CodeGenerate::CGEdgeParam(FlatNode *actor,stringstream &buf)
{
	buf<<"\t//edge param\n\t";
	
	vector<FlatNode*>::iterator iter,end;	//遍历输入输出边，生成producer和consumer	
	//输出边
	iter = actor->outFlatNodes.begin();
	end = actor->outFlatNodes.end();
	//获取actor输出边的名称
	List *outputList = actor->contents->decl->u.decl.type->u.operdcl.outputs;
	ListMarker output_maker;
	Node *outputNode = NULL;
	IterateList(&output_maker,outputList);
	while(iter!=end)
	{
		NextOnList(&output_maker,(GenericREF)&outputNode);
		string outputString;
		if(outputNode->typ == Id) outputString = outputNode->u.id.text;
		else if (outputNode->typ == Decl) outputString = outputNode->u.decl.name;
		buf<<"\tProducer<"<<pEdgeInfo->getEdgeInfo(actor,(*iter)).typeName<<"> "<<outputString<<";\n";
		iter ++;
	}
	//入边
	iter = actor->inFlatNodes.begin();
	end = actor->inFlatNodes.end();
	//获取actor内输入边的名称
	List *inputList =actor->contents->decl->u.decl.type->u.operdcl.inputs;
	ListMarker input_maker;
	Node *inputNode = NULL;
	IterateList(&input_maker,inputList);
	while(iter!=end)
	{
		NextOnList(&input_maker,(GenericREF)&inputNode);
		string inputString;
		if(inputNode->typ == Id)inputString = inputNode->u.id.text;
		else if (inputNode->typ == Decl) inputString = inputNode->u.decl.name;
		buf<<"\tConsumer<"<<pEdgeInfo->getEdgeInfo((*iter),actor).typeName<<"> "<<inputString<<";\n";
		iter++;
	}

}

void X86CodeGenerate::CGGlobalvar()
{
	isCGGlobalVar = true;
	stringstream buf;
	declInitList.str("");
	//（1）对全局变量进行代码生成
	flag_Global = 1;
	SPL2X86_List(gDeclList, 1);
	//（2）对函数定义进行代码生成
	SPL2X86_List(gProcList, 1);
	if (CallModelEmbed)
	{
		buf << " #include \"" << string(objName) << "_GlobalVar.h\"\n";
		buf << " namespace COStream" << string(objName) << "{ \n";
	}
	else
	{
		buf << " #include \"GlobalVar.h\"\n";
	}
	//（3）相应内容写入输出缓冲区
	buf << declInitList.str() << "\n\n";
	//（4）清空并恢复步骤2之前内容，以得到每个结点initVarAndState正确内容	
	
	if (CallModelEmbed)
	{
		buf << "} \n";
	}

	declInitList.str("");
	flag_Global = 0;
	//输出到文件
	stringstream ss;
	isCGGlobalVar = false;
	if (CallModelEmbed)
	{
		ss << dir_ << string(objName) << "_GlobalVar.cpp";
	}
	else
		ss << dir_ << "GlobalVar.cpp";
	OutputToFile(ss.str(), buf.str());
}

void X86CodeGenerate::CGGlobalvarextern()
{
	stringstream ss;
	stringstream buf;
	if (CallModelEmbed)
	{
		buf << "#ifndef _" << string(objName) << "_GLOBALVAL_H\n";
		buf << "#define _" << string(objName) << "_GLOBALVAL_H\n";
		//buf << " namespace COStream" << string(objName) << "{ \n"; //delete by mobinsheng
		buf << globalvarbuf.str();
		//buf << "} \n"; // delete by mobinsheng
	}
	else
	{
		buf << "#ifndef GLOBALVAL_H\n";
		buf << "#define GLOBALVAL_H\n";
		buf << globalvarbuf.str();
	}
	buf << "\n#endif\n";
	if (CallModelEmbed)
	{
		ss << dir_ << string(objName) << "_GlobalVar.h";
	}
	else
		ss << dir_ << "GlobalVar.h";
	OutputToFile(ss.str(), buf.str());
}
void X86CodeGenerate::CGactor(FlatNode *actor, OperatorType ot)
{
	stringstream buf; //存放输出到actor头文件的字符串流
	stringstream srcBuf;//存放输出到actor源文件的字符串流，其中对actor内的var和state数据进行初始化
	vector<string>::iterator iter;
	string classNmae = actor->name;
	curactor=actor;
	assert(actor);
	//cout<<actor->name<<"       "<<endl;  //modify my wangliang
	string protect_macro = "_COStream" + string(objName) + classNmae + "_H_";  // add by mobinsheng for 保护性的宏定义，防止头文件被重复引用
	buf << "#ifndef " << protect_macro << " \n"; // add by mobinsheng
	buf << "#define " << protect_macro << " \n"; // add by mobinsheng
	actor->SetIOStreams();
	//注释文件名称
	buf <<"/**\n * Class "<<classNmae<<"\n */\n";
	//添加头文件
	buf<<"#include \"Buffer.h\"\n";
	buf<<"#include \"Consumer.h\"\n";
	buf<<"#include \"Producer.h\"\n";
	buf<<"#include <string>\n";
	buf<<"#include \"iostream\"\n";
	buf<<"using namespace std;\n";
	//buf<<"\tinclude \""<<classNmae<<".h\"\n";
	if (CallModelEmbed)
	{
		buf << "#include \"" << string(objName) << "_global.h\"\n";
		buf << "#include \"" << string(objName) << "_GlobalVar.h\"\n";
	}
	else
	{
		buf << "#include \"global.h\"\n";
		buf << "#include \"GlobalVar.h\"\n";
	}
	//if (CallModelEmbed)
		//buf << " namespace COStream" << string(objName) << "{ \n";//添加名字空间
	buf <<"class "<<classNmae<<"{\n"; // 类块开始	
	//定义actor的输入边consumer，输出边producer
	CGEdgeParam(actor,buf);
	/*FileReader和FileWriter另外生成*/		
	if(actor == readerActor)
	{
		SPL2X86_List(actor->contents->body->u.operBody.state, 0);
		isInFileReader = FALSE;
		buf<<"\tint dataCount;\n";
		buf<<"\tint steadyScheduleCount;\n";
		buf<<"\tint initScheduleCount;\n";
		CGpushToken(actor,buf);	
		CGFileReaderActor(buf);
	}
	else if(actor == writerActor)
	{
		SPL2X86_List(actor->contents->body->u.operBody.state, 0);
		isInFileWriter = FALSE;
		buf<<"\tint dataCount;\n";
		buf<<"\tint steadyScheduleCount;\n";
		buf<<"\tint initScheduleCount;\n";
		CGpopToken(actor,buf);
		CGFileWriterActor(buf);
	}
	else 
	{
		// 写入发送、接收信息
		parameterBuf.str(""); // 清空parameterBuf内容
		thisBuf.str(""); // 清空thisBuf内容
		buf<<"private:\n\t";
		// 写入语法树成员变量信息（param, var, state）
		CGdeclList(actor, ot, buf);
		// 写入 var, state 的初始化信息
		CGinitVarAndState(actor, ot, buf);
		// 写入 init 的初始化信息(考虑到init可以定义局部变量，所以不与上部分合并)
		CGlogicInit(actor, ot, buf);			//生成init函数
		// 写入popToken和pushToken函数，调整缓存区
		CGpopToken(actor,buf);
		CGpushToken(actor,buf);	
		// 写入initWork函数
		CGinitWork(buf);
		// 写入work函数
		CGwork(actor, ot, buf);
	}
	//以下生成每个actor的成员函数，包括构造函数，初态调度函数和稳态调度函数
	buf<<"public:\n\t";
	// 写入构造函数信息
	CGthis(actor, ot, buf);
	//写入runInitScheduleWork函数
	CGrunInitScheduleWork(actor,buf);
	//写入runSteadyScheduleWork函数
	CGrunSteadyScheduleWork(actor,buf);
		buf <<"};\n";//类块结束
	//if(CallModelEmbed)
	//	buf<<"}\n";//名字空间块结束

	buf << "#endif " << " \n"; // add by mobinsheng

	//构造源文件内容
	if (CallModelEmbed)
		srcBuf << "#include \"" << string(objName) << "_" << actor->name << ".h\"\n";//chenzhen 2015/1/20
	else
		srcBuf << "#include \"" << actor->name << ".h\"\n";
	for (iter=staticNameInit.begin();iter!=staticNameInit.end();++iter)
		srcBuf<<(*iter);
	//输出到文件
	stringstream headerFileName,srcFileName;	//分别保存头文件名和源文件名，源文件定义了该actor的静态成员
	if (CallModelEmbed)	 //chenzhen  2015/1/20
	{
		headerFileName << dir_ << string(objName) << "_" << classNmae << ".h";
		srcFileName << dir_ << string(objName) << "_" << classNmae << ".cpp";
	}
	else
	{
		headerFileName << dir_ << classNmae << ".h";
		srcFileName << dir_ << classNmae << ".cpp";
	}
	OutputToFile(headerFileName.str(),buf.str());	//头文件
	if (staticNameInit.size()!=0)		//若包含静态成员，则生成源文件初始化类成员
		OutputToFile(srcFileName.str(),srcBuf.str());
	staticNameInit.clear();
}
void X86CodeGenerate::CGwork(FlatNode *actor, OperatorType ot, stringstream &buf)
{
	vector<string>::iterator iter;
	Node *work = actor->contents->body->u.operBody.work;
	string actorname = actor->name;
	stringstream tmpBuf;
	buf << "\t// work\n";
	buf << "\tvoid work() {\n";
	SPL2X86_Node(work, 2);		//访问work将work中的内容输出到declInitList
	buf << declInitList.str();
	buf << "\n\t\tpushToken();\n";
	buf << "\n\t\tpopToken();\n";
	buf << "\t}\n"; // work方法结束
	declInitList.str(""); // 清空
	//ptrname.clear();//清空
}
void X86CodeGenerate::OutputToFile(string fileName, string oldContents)
{
	ofstream fw;
	try{
		fw.open(fileName.c_str());
		fw<<oldContents;
		fw.close();
	}catch(...){
		cout<<"error:output to file"<<endl;
	}
}
void X86CodeGenerate::CGactors()
{
	stringstream ss,buf;
	std::map<operatorNode *, string>::iterator pos;
	for (int i = 0; i < nActors_; ++i)//生成 各个 类模板
	{
		int len = ListLength(flatNodes_[i]->oldContents->decl->u.decl.type->u.operdcl.outputs);
		int nOut = flatNodes_[i]->nOut;
		OperatorType ot = flatNodes_[i]->oldContents->ot;
	   
		pos = mapOperator2ClassName.find(flatNodes_[i]->oldContents);
		//if (pos == mapOperator2ClassName.end()) // 新的类模板进入了
		{

			string name = flatNodes_[i]->name;
			int index = name.find_first_of('_');
			string tmp = name.substr(0, index);
			mapOperator2ClassName.insert(make_pair(flatNodes_[i]->oldContents, flatNodes_[i]->name));

			if (len == nOut)			//nOut为每个operator的输出边个数
			{
				if (CallModelEmbed)
				{
					buf << "#include \"" << string(objName) << "_" << flatNodes_[i]->name << ".h\"\n";
				}
				else
					buf << "#include \"" << flatNodes_[i]->name << ".h\"\n";
				if (strcmp(tmp.c_str(), "FileReader") == 0)
					isInFileReader = true;
				if (strcmp(tmp.c_str(), "FileWriter") == 0)
					isInFileWriter = true; 
				curActor = flatNodes_[i];
				CGactor(flatNodes_[i], ot);
			}
			else // 一条边被多个actor共用, 暂不处理
			{
				cout << "test" << endl;
				UNREACHABLE;
			}
			
		}
	
	}
	if (CallModelEmbed)
	{
		ss << dir_ << string(objName) << "_AllActorHeader.h";
	}
	else
	{
		ss << dir_ << "AllActorHeader.h";
	}
	//ss<<dir_<<"AllActorHeader.h";
	OutputToFile(ss.str(),buf.str());

}
string X86CodeGenerate::MakeTabs(int tabs)
{
	stringstream tmp;
	while (tabs--) tmp<<"\t";
	return tmp.str();
}
void X86CodeGenerate::OutputCRSpaceAndTabs(int tabs)
{
	declInitList<<"\n"; 
	while (tabs--) declInitList<<"\t";
}
void X86CodeGenerate::OutputArgList(List *list, int offset)
{ 
	ListMarker marker;
	Node *item;
	int i=0;
	IterateList(&marker, list);
	while (NextOnList(&marker, (GenericREF) &item))
	{
		if(i!=0) declInitList<<", ";
		SPL2X86_Node(item, offset);
		i++;
	}
}
void X86CodeGenerate::OutputTabs(int tabs)
{
	while (tabs--) declInitList<<"\t";
}
void X86CodeGenerate::AddSemicolon()
{

}
void X86CodeGenerate::CGglobalHeader()
{
	stringstream buf;
	string edgename;
	ListMarker marker;
	Node *node;
	vector<FlatNode *>::iterator iter_1,iter_2;
	if (CallModelEmbed)
	{
		buf << "#ifndef _" << string(objName) << "_GLOBAL_H\n";
		buf << "#define _" << string(objName) << "_GLOBAL_H\n";
	}
	else
	{
		buf << "#ifndef _GLOBAL_H\n";
		buf << "#define _GLOBAL_H\n";
	}
	buf<<"/*全局变量，用于存储边的信息*/\n";
	buf<<"/*边的命名规则：A_B,其中A->B*/\n\n";
	if (FileR || FileW)		//存在文件读写操作
	{
		buf<<"#include <sstream>\n";
		buf<<"#include <fstream>\n";
		buf<<"#include \"tinystr.h\"\n";
		buf<<"#include \"tinyxml.h\"\n";
	}
	IterateList(&marker, Program);
	while (NextOnList(&marker, (GenericREF) &node)) {
		if (node == NULL)
			continue;

		if (node->coord.includedp)
			continue;

		if (node->typ == Text) {

			buf<<node->u.text.text<<endl;
			continue;
		}
	}
	buf<<"#include \"Buffer.h\"\n";
	buf<<"#include \"MathExtension.h\"\n";
	buf<<"#include <string>\n";
	buf<<"using namespace std;\n";
	if (Win)
	{
		buf<<"#define MAX_ITER 10\n";
	}
	//if(CallModelEmbed)
		//buf << " namespace COStream" << string(objName) << "{ \n";//名字空间
	
																  //输出stream结构体类型定义 
	pEdgeInfo->DeclEdgeType(buf);
	//输出全局变量Buffer的声明
	vector<FlatNode *>::iterator iter;
	for (iter=flatNodes_.begin();iter!=flatNodes_.end();++iter)//遍历所有结点
	{
		for (iter_2=(*iter)->outFlatNodes.begin();iter_2!=(*iter)->outFlatNodes.end();iter_2++)
		{
			string edgename=(*iter)->name+"_"+(*iter_2)->name;
			buf<<"extern Buffer<";
			buf<<pEdgeInfo->getEdgeInfo((*iter),(*iter_2)).typeName<<"> "<<edgename<<";"<<endl;
		}	
	}
	if(readerActor)		//读文件Actor
	{		
		string typeName = pEdgeInfo->getEdgeInfo(readerActor,readerActor->outFlatNodes[0]).typeName;
		buf<<"extern "<<typeName <<"* source;\n";
		buf<<"int FileReader(string path,int mode);\n";
		buf<<"istream& operator>>(istream& is, "<<typeName<<" &object);\n";
	}
	if(writerActor)
	{
		string typeName = pEdgeInfo->getEdgeInfo(writerActor->inFlatNodes[0],writerActor).typeName;
		buf<<"extern "<<typeName <<"* sink;\n";
		buf<<"void FileWriter(string path,int mode);\n";
		buf<<"ostream& operator<<(ostream& os, const "<<typeName<<" &object);\n";
		buf<<"extern int outPutCount;\n";
	}
	//if(CallModelEmbed)
	//	buf<<"}\n";//名字空间结束
	buf<<"#endif\n";
	//输出到文件
	stringstream ss;
	if (CallModelEmbed)
	{
		ss << dir_ << string(objName) << "_global.h";
	}
	else
		ss << dir_ << "global.h";
	OutputToFile(ss.str(),  buf.str());
}
void X86CodeGenerate::CGglobalCpp()
{
	stringstream buf;			//存放输出到globalCpp的字符串流
	buf<<"/*cpp文件,全局变量，用于存储边的信息*/\n";
	buf<<"#include \"Buffer.h\"\n";
	if (!CallModelEmbed) // modify by mobinsheng
	{
		buf << "#include \"global.h\"\n";
	}
	
	buf<<"#include <vector>\n";
	buf<<"using namespace std;\n";
	if (CallModelEmbed)
	{
		buf << "#include \"" << string(objName) << "_global.h\"\n";
		//buf << " namespace COStream" << string(objName) << "{ \n";//名字空间
	}
	else
		buf << "#include \"global.h\"\n";
	int init1,init2;//发送actor和接受actor初态调度产生和接受的数据量
	for (vector<FlatNode *>::iterator iter_1=flatNodes_.begin();iter_1!=flatNodes_.end();++iter_1)//遍历所有结点
	{
		for (vector<FlatNode *>::iterator iter_2=(*iter_1)->outFlatNodes.begin();iter_2!=(*iter_1)->outFlatNodes.end();iter_2++)
		{
			int stageminus; //stageminus表示两个actor所分配的阶段差
			int size;	//缓冲区的大小
			string edgename=(*iter_1)->name+"_"+(*iter_2)->name;	//边的名称
			stageminus=pSa->FindStage(*iter_2)-pSa->FindStage(*iter_1);//发送方和接受方的软件流水阶段差
			int edgePos = iter_2 - (*iter_1)->outFlatNodes.begin();	//iter_2在iter_1的输出边的序号
			int perSteadyPushCount = sssg_->GetSteadyCount(*iter_1)*(*iter_1)->outPushWeights.at(edgePos);//发送actor每次调用steadywork需要push的个数,nc
			//根据NoCheckBuffer选择是否使用边界检查的缓冲区管理
			if(NoCheckBuffer)			//使用无边界检查的缓存区管理，利用数据拷贝实现
			{	 
				int copySize=0,copyStartPos=0;	//拷贝的数据大小，copy存放的开始位置
				for(int inEdgeIndex =0;inEdgeIndex<(*iter_2)->inFlatNodes.size();inEdgeIndex++)
					if((*iter_2)->inFlatNodes.at(inEdgeIndex) == (*iter_1))
					{
						int perWorkPeekCount = (*iter_2)->inPeekWeights[inEdgeIndex];//接收边actor每次peek的个数,b
						int perWorkPopCount = (*iter_2)->inPopWeights[inEdgeIndex];//接收边actor每次调用work需要pop的个数,c
						init1 = sssg_->GetInitCount(*iter_1)*(*iter_1)->outPushWeights.at(edgePos);//发送actor调用initwork产生的数据量
						init2 =sssg_->GetInitCount(*iter_2)*perWorkPopCount;
						size = init1 + perSteadyPushCount*(stageminus+2);
						if(perWorkPeekCount == perWorkPopCount)	//peek == pop
						{
							if(perSteadyPushCount){
								copySize = (init1-init2)%perSteadyPushCount;
								copyStartPos = init2%perSteadyPushCount;
							}
						}
						else
						{
							int leftnum =  ((init1-init2)%perSteadyPushCount+perSteadyPushCount-(perWorkPeekCount-perWorkPopCount)%perSteadyPushCount)%perSteadyPushCount;
							copySize = leftnum+perWorkPeekCount-perWorkPopCount;
							int addtime = copySize%perSteadyPushCount?copySize/perSteadyPushCount+1:copySize/perSteadyPushCount;
							copyStartPos = init2%perSteadyPushCount;
							size += addtime*perSteadyPushCount;
						}
						buf<<"Buffer<"<<pEdgeInfo->getEdgeInfo((*iter_1),(*iter_2)).typeName<<"> "<<edgename<<"("<<size<<","<<copySize<<","<<copyStartPos<<");\n";
						break;
					}
			}
			else		//使用边界检查，缓存区大小=（初态执行次数+（稳态阶段差+1）*稳态执行次数）* 每次push的数据量
			{
				size=(sssg_->GetInitCount(*iter_1)+sssg_->GetSteadyCount(*iter_1)*(stageminus+1))*(*iter_1)->outPushWeights.at(edgePos);
				int tempSize = 1;
				while(size>tempSize)
					tempSize <<= 1;
				size = tempSize;
				buf<<"Buffer<"<<pEdgeInfo->getEdgeInfo((*iter_1),(*iter_2)).typeName<<"> "<<edgename<<"("<<size<<");\n";
			}
		}
	}
	if(readerActor)		//读文件Actor
	{
		StreamEdgeInfo edgeInfo = pEdgeInfo->getEdgeInfo(readerActor,readerActor->outFlatNodes[0]);
		buf<<edgeInfo.typeName <<"* source;\n";
		buf<<"int FileReader(string path,int mode)\n{\n";
		buf<<"\tvector<"<<edgeInfo.typeName<<"> sourceDataVec;\n";
		//		buf<<"\tifstream ifile;\n\tifile.open(path.c_str());\n";
		buf<<"\t"<<edgeInfo.typeName<<" temp;\n";
		buf<<"\tstringstream instream;\n";

		buf<<"\tif(path.find(\".txt\")!=-1){\n";
		buf<<"\tifstream infile;\n\tinfile.open(path.c_str());\n";
		buf<<"\tif(!infile)\n\t{\n\t\tprintf(\"file open error\");\n\t\treturn -1;\n\t}\n\telse\n\t{\n";
		buf<<"\t\tdouble value;\n\t\twhile(infile>>value)\n\t\tinstream<<\"x \"<<value;\n\t\tinfile.close();\n\t}";
		buf<<"\t}\n\telse\n\t{\n\t\t";

		buf<<"TiXmlDocument *pDoc =  new TiXmlDocument(path.c_str());\n\t";
		buf<<"if(!pDoc->LoadFile())\n\t";
		buf<<"{\n\t\treturn 0;\n\t}\n\t";
		buf<<"TiXmlElement *RootElement = pDoc->RootElement();\n\t";
		buf<<"TiXmlElement *currentNode = RootElement;\n\t";
		buf<<"while(currentNode)\n\t{\n\t\t";
		buf<<"TiXmlElement *idElement = currentNode->FirstChildElement();\n\t\t";
		buf<<"while(idElement)\n\t\t{\n\t\t\t";
		//	buf<<"stringstream sstr;\n\t\t\tsstr << idElement->FirstChild()->Value();\n\t\t\tfloat a;\n\t\t\tsstr >> a;\n\t\t\t";	
		//	buf<<"instream <<\"x \"<< a;\n\t\t\tidElement = idElement->NextSiblingElement();\n\t\t}\n\t\t";
		buf<<"stringstream sstr;\n\t\t\tstring type;\n\t\t\tsstr << idElement->FirstChild()->Value();\n\t\t\ttype = const_cast<char *>(idElement->Value());\n\t\t\t";
		buf<<"if(type == \"int\")\n\t\t\t{\n\t\t\t\tint a;\n\t\t\t\tsstr >> a;\n\t\t\t\tinstream <<\"x \"<< a;\n\t\t\t}\n\t\t\t";
		buf<<"else if(type == \"float\")\n\t\t\t{\n\t\t\t\tfloat a;\n\t\t\t\tsstr >> a;\n\t\t\t\tinstream <<\"x \"<< a;\n\t\t\t}\n\t\t\t";
		buf<<"else if(type == \"double\")\n\t\t\t{\n\t\t\t\tdouble a;\n\t\t\t\tsstr >> a;\n\t\t\t\tinstream <<\"x \"<< a;\n\t\t\t}\n\t\t\t";
		buf<<"idElement = idElement->NextSiblingElement();\n\t\t}\n\t\t";
		buf<<"currentNode = currentNode->NextSiblingElement();\n\t}\n";

		buf<<"\t}\n";

		buf<<"\twhile(instream >> temp)\n\t\tsourceDataVec.push_back(temp);\n\tint size = sourceDataVec.size();\n\tsource = new "<< edgeInfo.typeName<<"[size];\n\tfor(int i=0;i<size;i++)\n\t\tsource[i] = sourceDataVec[i];\n";
		buf<<"\treturn size;\n}\n";
		buf<<"istream& operator>>(istream& is, "<<edgeInfo.typeName<<"& object)\n{\n";
		buf<<"\tstring s;\n\tis";
		int begin = edgeInfo.typedefine.find_first_of(' ');
		int end;
		while(begin!=-1)
		{
			end = edgeInfo.typedefine.find_first_of('\n',begin);
			buf<<">>s>>object."<< edgeInfo.typedefine.substr(begin+1,end-begin-2);
			begin = end;
			begin = edgeInfo.typedefine.find_first_of(' ',begin);
		}
		buf<<";\n\treturn is;\n";
		buf<<"}\n";
	}
	if(writerActor)
	{
		StreamEdgeInfo edgeInfo = pEdgeInfo->getEdgeInfo(writerActor->inFlatNodes[0],writerActor);
		buf<<edgeInfo.typeName <<"* sink;\n";
		buf<<"int outPutCount;\n";
		buf<<"void FileWriter(string path,int mode)\n{\n";
		buf<<"\tstringstream outstream;\n";
		buf<<"\tint i=0;\n\twhile(i<outPutCount)\n\t\toutstream <<sink[i++];\n\tdelete[] sink;\n\t";

		buf<<"TiXmlDocument doc;\n\t";
		buf<<"TiXmlElement *root = new TiXmlElement(\"stream\");\n\tdoc.LinkEndChild(root);\n\tstring s1, s2;\n\t";
		buf<<"while(outstream >> s1 >> s2)\n\t{\n\t\t";
		buf<<"const char *cc1 = s1.c_str();\n\t\tTiXmlElement *sub = new TiXmlElement(cc1);\n\t\tchar *c1 = const_cast<char *>(s2.c_str());\n\t\t";
		buf<<"TiXmlText *con = new TiXmlText(c1);\n\t\tsub->LinkEndChild(con);\n\t\troot->LinkEndChild(sub);\n\t}\n\t";
		buf<<"const char* pp = path.c_str();\n\t";
		buf<<"doc.SaveFile(pp);\n";

		buf << "\n}\n";
		buf<<"ostream& operator<<(ostream& os, const "<<edgeInfo.typeName<<"& object)\n{\n";
		buf<<"\tos";
		int begin = edgeInfo.typedefine.find_first_of(' ');
		int end;
		int bb = 0;
		while(begin!=-1)
		{
			end = edgeInfo.typedefine.find_first_of('\n',begin);
			buf<<"<< \" "<< edgeInfo.typedefine.substr(bb,begin-bb) << " \" <<object."<< edgeInfo.typedefine.substr(begin+1,end-begin-2);
			begin = end;
			bb = end + 1;
			begin = edgeInfo.typedefine.find_first_of(' ',begin);
		}
		buf<<"; \n\treturn os;\n";
		buf<<"}\n";
	}
	//if(CallModelEmbed)
	//	buf<<"}\n";
	//输出到文件
	stringstream ss;
	if (CallModelEmbed)
	{
		ss << dir_ << string(objName) << "_global.cpp";
	}
	else
		ss << dir_ << "global.cpp";
	OutputToFile(ss.str(),  buf.str());
	

}
void X86CodeGenerate::CGMakefile()
{
#ifndef WIN32
	stringstream buf;
	string path = COStreamPath;
	buf<<"PROGRAM := "<<objName<<"\n";
	buf<<"SOURCES := $(wildcard ./*.cpp)\n";
	buf<<"SOURCES += $(wildcard ./src/*.cpp)\n";
	buf<<"OBJS    := $(patsubst %.cpp,%.o,$(SOURCES))\n";
	buf<<"CC      := g++\n";
	buf<<"CFLAGS  := -ggdb -Wall\n";
	// buf<<"INCLUDE := -I . -I"<<path<<"/runtime/X86Lib2 . -I"<<path<<"/src/3rdpart/include\n";
	// buf<<"LIBPATH := -L"<<path<<"/src/3rdpart/lib\n";
	buf<<"INCLUDE := -I . -I"<<path<<"/runtime/X86Lib2\n";
	buf<<"LIBPATH := -L"<<path<<"/runtime/X86Lib2\n";
	buf<<"LIB     := -lpthread -ldl -llock\n\n\n";
	buf<<".PHONY: clean install\n\n";
	buf<<"$(PROGRAM): $(OBJS)\n";
	buf<<"\t$(CC) -o $@ $^ $(LIB) $(LIBPATH)\n";
	buf<<"%.o: %.cpp\n";
	buf<<"\t$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE)\n\n\n";
	buf<<"clean:\n";
	buf<<"\trm $(OBJS) $(PROGRAM) -f\n\n\n";
	buf<<"install: $(PROGRAM)\n";
	buf<<"\tcp $(PROGRAM) ./bin/\n";
	stringstream ss;
	ss<<dir_<<"Makefile";
	cout<<"***dir"<<dir_<<endl;
	OutputToFile(ss.str(),buf.str());
#endif
}

string X86CodeGenerate::CGEmbedModeThreadPool(int cpuCore)
{
#define TAB "\t"

	string task_thread_str;

	{
		task_thread_str = ""														"\n"
			TAB"class COStreamTask"													"\n"
			TAB"{"																		"\n"
			TAB"public:"																"\n"
			TAB"	virtual void Execute(){}"											"\n"
			TAB"};"																	"\n"
			TAB"typedef pthread_t ThreadHandle;"										"\n"
			TAB"class BaseThread"														"\n"
			TAB"{"																		"\n"
			TAB"private:"																"\n"
			TAB""																		"\n"
			TAB"	ThreadHandle thread;"												"\n"
			TAB"	"																	"\n"
			TAB"public:"																"\n"
			TAB""																		"\n"
			TAB"	BaseThread();"														"\n"
			TAB"	"																	"\n"
			TAB"	virtual ~BaseThread();"												"\n"
			TAB"	"																	"\n"
			TAB"	virtual void ThreadMain() = 0;"										"\n"
			TAB"	"																	"\n"
			TAB"	bool Start();"														"\n"
			TAB"	"																	"\n"
			TAB"	void Stop();"														"\n"
			TAB"};"																	"\n"
			TAB""																		"\n"
			TAB"static void *ThreadShim(void *opaque)"									"\n"
			TAB"{"																		"\n"
			TAB"	BaseThread *instance = reinterpret_cast<BaseThread *>(opaque);"		"\n"
			TAB"	"																	"\n"
			TAB"	instance->ThreadMain();"											"\n"
			TAB"	"																	"\n"
			TAB"	return NULL;"														"\n"
			TAB"}"																		"\n"
			TAB""																		"\n"
			TAB"bool BaseThread::Start()"												"\n"
			TAB"{"																		"\n"
			TAB"	if (pthread_create(&this->thread, NULL, ThreadShim, this))"			"\n"
			TAB"	{"																	"\n"
			TAB"		this->thread = 0;"												"\n"
			TAB"		"																"\n"
			TAB"		return false;"													"\n"
			TAB"	}"																	"\n"
			TAB"	"																	"\n"
			TAB"	return true;"														"\n"
			TAB"}"																		"\n"
			TAB""																		"\n"
			TAB"void BaseThread::Stop()"												"\n"
			TAB"{"																		"\n"
			TAB"	if (this->thread)"													"\n"
			TAB"	{"																	"\n"
			TAB"		pthread_join(this->thread, NULL);"								"\n"
			TAB"	}"																	"\n"
			TAB"}"																		"\n"
			TAB""																		"\n"
			TAB"BaseThread::~BaseThread() {}"											"\n"
			TAB""																		"\n"
			TAB""																		"\n"
			TAB"BaseThread::BaseThread()"												"\n"
			TAB"{""\n"
			TAB"	this->thread = 0;""\n"
			TAB"}""\n"
			TAB"pthread_cond_t notify_cond;""\n"
			TAB"pthread_mutex_t notify_lock;""\n"
			TAB"pthread_mutex_t compelete_lock;""\n"
			TAB"pthread_cond_t compelete_notify;""\n"
			TAB"int g_expect_compelete_num = %d;""\n"
			TAB"int g_compelete_num = 0;""\n"
			TAB"class COStreamThread* g_threads = 0;""\n"
			TAB"int g_threads_num = %d;""\n"
			TAB"class COStreamThread :public BaseThread""\n"
			TAB"{""\n"
			TAB"	deque<COStreamTask*> m_taskQueue;""\n"
			TAB"	pthread_cond_t m_notify_cond;""\n"
			TAB"	pthread_mutex_t m_notify_lock;""\n"
			TAB"	bool m_bLoop;""\n"
			TAB"public:""\n"
			TAB"""\n"
			TAB"	COStreamThread() :m_bLoop(true){""\n"
			TAB"		pthread_mutex_init(&m_notify_lock, 0);""\n"
			TAB"		pthread_cond_init(&m_notify_cond, 0);""\n"
			TAB"	}""\n"
			TAB"	""\n"
			TAB"	~COStreamThread()""\n"
			TAB"	{""\n"
			TAB"		pthread_mutex_destroy(&m_notify_lock);""\n"
			TAB"		pthread_cond_destroy(&m_notify_cond);""\n"
			TAB"	}""\n"
			TAB"	""\n"
			TAB"	void setTask(COStreamTask* task)""\n"
			TAB"	{""\n"
			TAB"		pthread_mutex_lock(&m_notify_lock);""\n"
			TAB"		m_taskQueue.push_back(task);""\n"
			TAB"		pthread_cond_signal(&m_notify_cond);""\n"
			TAB"		pthread_mutex_unlock(&m_notify_lock);""\n"
			TAB"	}""\n"
			TAB"	""\n"
			TAB"	virtual void ThreadMain()""\n"
			TAB"	{""\n"
			TAB"		while (true)""\n"
			TAB"		{""\n"
			TAB"			pthread_mutex_lock(&m_notify_lock);""\n"
			TAB"			""\n"
			TAB"			while (m_taskQueue.empty())""\n"
			TAB"				pthread_cond_wait(&m_notify_cond, &m_notify_lock);""\n"
			TAB"				""\n"
			TAB"			COStreamTask* task = m_taskQueue.front();""\n"
			TAB"			m_taskQueue.pop_front();""\n"
			TAB"			""\n"
			TAB"			if (task == 0)""\n"
			TAB"			{""\n"
			TAB"				pthread_mutex_unlock(&m_notify_lock);""\n"
			TAB"				break;""\n"
			TAB"			}""\n"
			TAB"			task->Execute();""\n"
			TAB"			""\n"
			TAB"			delete task;""\n"
			TAB"			""\n"
			TAB"			pthread_mutex_unlock(&m_notify_lock);""\n"
			TAB"			""\n"
			TAB"			""\n"
			TAB"			pthread_mutex_lock(&compelete_lock);""\n"
			TAB"			++g_compelete_num;""\n"
			TAB"			if (g_compelete_num == g_expect_compelete_num)""\n"
			TAB"			{""\n"
			TAB"				pthread_cond_signal(&compelete_notify);""\n"
			TAB"			}""\n"
			TAB"			pthread_mutex_unlock(&compelete_lock);""\n"
			TAB"		}""\n"
			TAB"	}""\n"
			TAB"};""\n"
			TAB"void CreateThreadPool()""\n"
			TAB"{""\n"
			TAB"	if (g_threads != 0)""\n"
			TAB"	{""\n"
			TAB"		return;""\n"
			TAB"	}""\n"
			TAB"	""\n"
			TAB"	g_threads = 0;""\n"
			TAB"	""\n"
			TAB"	pthread_mutex_init(&notify_lock, 0);""\n"
			TAB"	pthread_cond_init(&notify_cond, 0);""\n"
			TAB"	""\n"
			TAB"	pthread_mutex_init(&compelete_lock, 0);""\n"
			TAB"	pthread_cond_init(&compelete_notify, 0);""\n"
			TAB"	""\n"
			TAB"	""\n"
			TAB"	g_threads = new COStreamThread[g_threads_num];""\n"
			TAB"	""\n"
			TAB"	for (int i = 0; i < g_threads_num; ++i)""\n"
			TAB"	{""\n"
			TAB"		g_threads[i].Start();""\n"
			TAB"	}""\n"
			TAB"	""\n"
			TAB"}""\n"
			TAB"""\n"
			TAB"void DistroyThreadPool()""\n"
			TAB"{""\n"
			TAB"	if (g_threads != 0)""\n"
			TAB"	{""\n"
			TAB"		for (int i = 0; i < g_threads_num; ++i)""\n"
			TAB"		{""\n"
			TAB"			g_threads[i].Stop();""\n"
			TAB"		}""\n"
			TAB"		""\n"
			TAB"		delete[] g_threads;""\n"
			TAB"		g_threads = 0;""\n"
			TAB"	}""\n"
			TAB"	""\n"
			TAB"	g_threads = 0;""\n"
			TAB"	g_threads_num = 0;""\n"
			TAB"	""\n"
			TAB"	""\n"
			TAB"	pthread_mutex_destroy(&notify_lock);""\n"
			TAB"	pthread_cond_destroy(&notify_cond);""\n"
			TAB"	""\n"
			TAB"	pthread_mutex_destroy(&compelete_lock);""\n"
			TAB"	pthread_cond_destroy(&compelete_notify);""\n"
			TAB"}""\n"
			TAB"""\n"
			TAB"""\n";
	}

	char* tempBuffer = new char[1024 * 8];
	sprintf(tempBuffer, task_thread_str.c_str(), cpuCore, cpuCore);
	task_thread_str = tempBuffer;
	delete[] tempBuffer;
	task_thread_str += GetTaskStr(cpuCore);
	task_thread_str += GetNotifyExecuteStr(cpuCore);
	task_thread_str += GetNotifyCloseStr(cpuCore);

	return task_thread_str;
}

string X86CodeGenerate::GetTaskStr(int threadNums)
{
	const int LEN = 1024 ;

	char buffer[LEN] = { 0 };
	string str_task_x = 
		TAB""										"\n"
		TAB"class Task%d :public COStreamTask"		"\n"
		TAB"{"										"\n"
		TAB"public:"								"\n"
		TAB"	virtual void Execute()"				"\n"
		TAB"	{"									"\n"
		TAB"		set_cpu(%d);"					"\n"
		TAB"		thread_%d_fun();"				"\n"
		TAB"	}"									"\n"
		TAB"};"									"\n"
		TAB""										"\n";
	string ret = "";
	for (int i = 0; i < threadNums; ++i)
	{
		sprintf(buffer, str_task_x.c_str(), i, i, i);
		ret += buffer;
		memset(buffer, 0, LEN);
	}
	
	return ret;
}

string X86CodeGenerate::GetNotifyExecuteStr(int threadNums)
{
	string strHead = 
		TAB""												"\n"
		TAB"void NotifyExecute()"								"\n"
		TAB"{"													"\n"
		TAB"	g_compelete_num = 0;"							"\n"
		TAB"	"												"\n"
		TAB"	for (int i = 0; i < g_threads_num; ++i){"		"\n"
		TAB"		COStreamTask* task = 0;"					"\n"
		TAB"		switch (i)"									"\n"
		TAB"		{"											"\n"
		TAB"		"											"\n";

	string strMid = "";
	for (int i = 0; i < threadNums; ++i)
	{
		char buffer[1024] = { 0 };
		string fmt = 
			TAB"		case %d:"					"\n"
			TAB	TAB"			task = new Task%d;"		"\n"
			TAB"			break;"					"\n";					//"case %d:task = new Task%d;break;";;
			TAB"";
		sprintf(buffer, fmt.c_str(), i, i);
		strMid += buffer;
	}
	strMid += 
		TAB"		default: "		"\n"
		TAB"			break;"		"\n"
		TAB"		}"				"\n";

	strMid += 
		TAB"	g_threads[i].setTask(task);"	"\n"
		TAB"    }	"							"\n"
		;

	string strTail = 
		TAB"	pthread_cond_broadcast(&notify_cond);"						"\n"
		TAB"	pthread_mutex_lock(&compelete_lock);"						"\n"
		TAB"	while (g_compelete_num < g_expect_compelete_num)"			"\n"
		TAB"	{"															"\n"
		TAB"		pthread_cond_wait(&compelete_notify, &compelete_lock);"	"\n"
		TAB"	}"															"\n"
		TAB"	pthread_mutex_unlock(&compelete_lock); "					"\n"
		TAB"}"																"\n";

	string ret = strHead + strMid + strTail;

	return ret;
}

string X86CodeGenerate::GetNotifyCloseStr(int threadNums)
{
	string strHead = 
		TAB"void NotifyClose()"							"\n"
		TAB"{"												"\n"
		TAB"	for (int i = 0; i < g_threads_num; ++i){"	"\n"
		TAB"		COStreamTask* task = 0;"				"\n"
		TAB"		g_threads[i].setTask(task);"			"\n"
		TAB"	}"											"\n"
		TAB"}"												"\n";
	return strHead;
}

void X86CodeGenerate::CGThreads()   //生成线程代码
{
	string threadname;
	stringstream buf; 
	for (int i=0;i<nCpucore_;i++)
	{
		buf<<"/*该文件定义各thread的入口函数，在函数内部完成软件流水迭代*/\n";
		stringstream ss;
		if (CallModelEmbed)  // chenzhen
		{
			ss << dir_ << (string)objName << "_thread_" << i << ".cpp";
		}
		else
		{
			ss << dir_ << "thread_" << i << ".cpp";
		}
		CGThread(i,buf);			//生成线程i
		OutputToFile(ss.str(),buf.str());
		ss.str("");
		buf.str("");
	}

}
void X86CodeGenerate::CGThread(int index,stringstream&buf)
{
	int totalstagenum=pSa->MaxStageNum();//最大阶段号
	pair<multimap<FlatNode *,string>::iterator,multimap<FlatNode *,string>::iterator>pos1,pos2;
	//添加头文件
	buf<<"#include \"Buffer.h\"\n";
	buf<<"#include \"Producer.h\"\n";
	buf<<"#include \"Consumer.h\"\n";
	//buf<<"#include \"global.h\"\n";
	//buf<<"#include \"AllActorHeader.h\"\t//包含所有actor的头文件\n";
	buf<<"#include \"lock_free_barrier.h\"\t//包含barrier函数\n";
	buf<<"#include \"rdtsc.h\"\n";
	buf<<"#include <fstream>\n";
	if (CallModelEmbed)   //chenzhen
	{
		buf << "#include \"" << (string)objName << "_global.h\"\n";
		buf << "#include \"" << (string)objName << "_AllActorHeader.h\"\t//包含所有actor的头文件\n";
	}
	else
	{
		buf << "#include \"global.h\"\n";
		buf << "#include \"AllActorHeader.h\"\t//包含所有actor的头文件\n";
	}
	if(MakeProfile)
	{
		buf<<"#include  <sys/stat.h>\n";
		buf<<"#include <sys/types.h>\n";
	}
	if(CHECKEACHACTORTIME || MakeProfile)		//记录每个actor的执行时间
	{	
		buf<<"#include <sstream>\n";
		
	}
	//if(CallModelEmbed)		//嵌入在名字空间COStream中
		//buf << " namespace COStream" << string(objName) << "{ \n";
	//声明外部定义的全局变量
	if (Linux)
	{	
		buf<<"extern int MAX_ITER;\n";//执行次数，用作迭代上限
	}
	if (TRACE)
	{
		buf<<"extern double (*deltatimes)["<<nCpucore_<<"][2];\n";	//如果为trace的话，则记录每个线程每次迭代的同步和计算时间长度
		buf<<"#define	MEASURE_RUNS 100000\n";					//MEASURE_RUNS为时钟长度单位，为10M时钟周期
	}
	else if(CALRATIO)
		buf<<"#define	MEASURE_RUNS 100000\n";
	if (index)			//除线程0外，其他线程调用workSync等待线程0，线程0进行累计其他线程是否结束，并设置标志位
	{
		buf<<"void thread_"<<index<<"_fun()\n{\n";
		buf<<"\tworkerSync("<<index<<");\n";
	}
	else
	{
		buf<<"void thread_"<<index<<"_fun()\n{\n";
		buf<<"\tmasterSync("<<nCpucore_<<");\n";
	}	
	if(TRACE||CALRATIO){
		buf<<"\ttsc_counter c0, c1;\n";	
	}
	if(CALRATIO)
		buf<<"\tdouble cal=0,total=0;\n";
	if(CHECKEACHACTORTIME)						//输出每个actor在每个stage上的steadywork时间
	{
		buf<<"\tofstream txtfw;\n\tstringstream ss;\n";
		buf<<"\tss<<\"该文件记录该线程上每个actor稳态执行的时间，第一列为actor的名称，第二列为该次稳态执行的时钟周期数（单位为10^6）\"<<endl;\n";
		buf<<"\ttsc_counter c2, c3;\n";	
	}
	else if(CALRATIO)
		buf<<"\tofstream txtfw;\n";
	if(MakeProfile)            //cwb，记录每个actor的稳态的执行时间
	{
		buf<<"\tofstream txtpf;\n\tstringstream sst;\n";
		buf<<"\ttsc_counter cc2, cc3;\n";	
	}

	vector<FlatNode *>tempactorset = Mp->findNodeSetInPartition(index);//tempactorset存放线程index上所有actor的集合
	//遍历tempactorset所有actor，生成其类型实例
	for (vector<FlatNode *>::iterator iter=tempactorset.begin();iter!=tempactorset.end();++iter)	
	{
		buf<<"\t"<<(*iter)->name<<" "<<(*iter)->name<<"_obj(";//定义actor对象，actor->name_obj,调用构造函数，参数为输入输出边的全局变量
		//mapActor2InEdge和mapActor2OutEdge存放该actor所对应的输入输出边buffer的名称
		pos1=mapActor2InEdge.equal_range(*iter);		
		pos2=mapActor2OutEdge.equal_range(*iter);
		while (pos2.first!=pos2.second)
		{
			buf<<pos2.first->second<<",";
			++pos2.first;
		}
		while(pos1.first!=pos1.second)
		{
			buf<<pos1.first->second<<",";
			++pos1.first;
		}
		//稳态iad
		buf.seekp((int)buf.tellp()-1);
		buf<<");\n";
		if(MakeProfile)		//做profile，累加执行时间
		{
			buf<<"\tdouble "<<(*iter)->name<<"_time = 0;"<<endl;
		}
	}
	//stage表示阶段号数组，初始除0外都为0
	buf<<"\tchar stage["<<totalstagenum<<"]={0};\n";
	buf<<"\tstage[0]=1;\n";
	//遍历该线程上的所有的阶段号，在对应的阶段号内调用每个actor的initwork
	buf<<"\tfor(int _stageNum=0;_stageNum<"<<totalstagenum<<";_stageNum++)\n";
	buf<<"\t{\n";
	set<int>* ptempstagenum=&mapNum2Stage.find(index)->second;		//查找该thread对应的阶段号集合
	set<int>::iterator endIter = ptempstagenum->end();//标记阶段号集合的End
	if(CALRATIO||TRACE)			
			buf<<"\t\tRDTSC(c0);\n";
		for (int i=totalstagenum-1;i>=0;i--)	//迭代stage Num
		{
			set<int>::iterator stageiter = ptempstagenum->find(i);		//查找该线程对应在阶段i是否有actor
			if(stageiter!=endIter){					//该stage在该thread上
				buf<<"\t\tif("<<i<<"==_stageNum)\n\t\t{\n";
				vector<FlatNode*> flatVec = pSa->FindActor(i);	//取得在该阶段的所有actor集合
				for(vector<FlatNode*>::iterator iter1=flatVec.begin();iter1!=flatVec.end();++iter1)//遍历actor，调用初态initScheduleWork
				{
					if(index == Mp->findPartitionNumForFlatNode(*iter1))
					{
						buf<<"\t\t\t"<<(*iter1)->name<<"_obj.runInitScheduleWork();\n";
					}
				}
				buf<<"\t\t}\n";
			}
		}	
	if (TRACE)		//记录时间
	{
		buf<<"\t\tRDTSC(c1);\n";
		if(CALRATIO)
			buf<<"\t\tcal += deltatimes[_stageNum]["<<index<<"][0] = COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
		else
			buf<<"\t\tdeltatimes[_stageNum]["<<index<<"][0] = COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
		buf<<"\t\tc0 = c1;\n";
	}
	else if(CALRATIO)
	{
		buf<<"\t\tRDTSC(c1);\n";
		buf<<"\t\tcal += COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
	}
	//区分线程0和其他线程
	if(index)
		buf<<"\t\n\t\tworkerSync("<<index<<");\n";
	else
		buf<<"\t\n\t\tmasterSync("<<nCpucore_<<");\n";
	if(TRACE){
		buf<<"\t\tRDTSC(c1);\n";
		if(CALRATIO)
			buf<<"\t\ttotal += deltatimes[_stageNum]["<<index<<"][1] = COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
		else
			buf<<"\t\tdeltatimes[_stageNum]["<<index<<"][1] = COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
		buf<<"\t\tc0 = c1;\n";		
	}
	else if(CALRATIO)
	{
		buf<<"\t\tRDTSC(c1);\n";
		buf<<"\t\ttotal += COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
	}
	//初态调度完成
	buf<<"\t}\n";
	//以下开始输出该线程每个阶段每个actor的稳态调度
	buf<<"\tfor(int _stageNum="<<totalstagenum<<";_stageNum<2*"<<totalstagenum<<"+MAX_ITER-1;_stageNum++)\n\t{\n";
	if(CALRATIO||TRACE)
		buf<<"\t\tRDTSC(c0);\n";
		
	if(CHECKEACHACTORTIME)
	{
		buf<<"\t\tss<<\"stage:\\t\"<<_stageNum<<\"\t\"<<\"actorname\\t\"<<\"time(cycles)\"<<\"steadycount\\t\"<<\"averagetime\"<<endl;\n";
		
	}
	if(MakeProfile)       //cwb
	{
		buf<<"\t\tint steadycount;\n";
	}
	for (int i=totalstagenum-1;i>=0;i--)	//迭代stage
	{			

		set<int>::iterator stageiter = ptempstagenum->find(i);
		if(stageiter!=endIter){					//该stage在该thread上
			buf<<"\t\tif(stage["<<i<<"])\n\t\t{\n";

			vector<FlatNode*> flatVec = pSa->FindActor(i);
			vector<FlatNode*>::iterator iter1;
			for(iter1=flatVec.begin();iter1!=flatVec.end();++iter1){
				if(index == Mp->findPartitionNumForFlatNode(*iter1))
				{
					if(CHECKEACHACTORTIME)
					{
						buf<<"\t\t\tRDTSC(c2);\n";
					}
					if(MakeProfile)                         //cwb
					{
						buf<<"\t\t\tRDTSC(cc2);\n";
					}
					buf<<"\t\t\t"<<(*iter1)->name<<"_obj.runSteadyScheduleWork();\n";
					if(CHECKEACHACTORTIME)
					{
						buf<<"\t\t\tRDTSC(c3);\n";
						buf<<"\t\t\tss<<\"\\t"<<(*iter1)->name<<":\"<<COUNTER_DIFF(c3,c2,CYCLES)<<endl;\n";
					}
					if(MakeProfile)                    //cwb,输出该actor该次work的所需的时钟周期
					{
						buf<<"\t\t\tRDTSC(cc3);\n";
						buf<<"\t\t\tsteadycount = "<<sssg_->GetSteadyCount((*iter1))<<";\n";
						buf<<"\t\t\t"<<(*iter1)->name<<"_time += COUNTER_DIFF(cc3,cc2,CYCLES)/steadycount;\n";
					}
				}
			}
			buf<<"\t\t}\n";
		}
	}
	if(CHECKEACHACTORTIME)
		buf<<"\t\tss<<endl;\n";
	buf<<"\t\tfor(int index="<<totalstagenum-1<<"; index>= 1; --index)\n";
	buf<<"\t\t\tstage[index] = stage[index-1];\n\t\tif(_stageNum == (MAX_ITER - 1 + "<<totalstagenum<<"))\n\t\t{\n\t\t\tstage[0]=0;\n\t\t}\n";
	if (TRACE)
	{
		buf<<"\t\tRDTSC(c1);\n";
		if(CALRATIO)
			buf<<"\t\tcal += deltatimes[_stageNum]["<<index<<"][0] = COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
		else
			buf<<"\t\tdeltatimes[_stageNum]["<<index<<"][0] = COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
		buf<<"\t\tc0 = c1;\n";
	}
	else if(CALRATIO)
	{
		buf<<"\t\tRDTSC(c1);\n";
		buf<<"\t\tcal += COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
	}
	if(index)
		buf<<"\t\n\t\tworkerSync("<<index<<");\n";
	else
		buf<<"\t\n\t\tmasterSync("<<nCpucore_<<");\n";
	if (TRACE)
	{
		buf<<"\t\tRDTSC(c1);\n";
		if(CALRATIO)
			buf<<"\t\ttotal += deltatimes[_stageNum]["<<index<<"][1] = COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
		else
			buf<<"\t\tdeltatimes[_stageNum]["<<index<<"][1] = COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
//		buf<<"\t\tdeltatimes["<<pSa->MaxStageNum()<<"+_stageNum]["<<index<<"][1] = COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
		buf<<"\t\tc0 = c1;\n";
	}
	else if(CALRATIO)
	{
		buf<<"\t\tRDTSC(c1);\n";
		buf<<"\t\ttotal += COUNTER_DIFF(c1, c0, CYCLES)/MEASURE_RUNS;\n";
	}
	buf<<"\t}\n";//稳态调度截止
	if(CHECKEACHACTORTIME)
	{
		buf<<"\ttry\n\t{\n\t\ttxtfw.open(\"thread "<<index<<"'s stage.txt\");\n\t\ttxtfw<<ss.str();\n\t\ttxtfw.close();\n\t}\n\tcatch(...)\n\t\t{cout<<\"error:output to file\"<<endl;\n\t}\n";
	}
	if(MakeProfile)    //cwb
	{
		//输出每个actor的平均计算时间
		for (vector<FlatNode *>::iterator iter=tempactorset.begin();iter!=tempactorset.end();++iter)
			buf<<"\tsst<<\""<<(*iter)->name<<"\		\"<<(int)"<<(*iter)->name<<"_time/MAX_ITER<<endl;\n";
		mkdir(string("profileresult"));
		buf<<"\ttry\n\t{\n\t\tmkdir(\"profile\",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);\n\t\ttxtpf.open(\""<<string(objName)<<".profile\");\n\t\ttxtpf<<sst.str();\n\t\ttxtpf.close();\n\t\tsystem(\"mv "<<string(objName)<<".profile ./profile\");\n\t}\n\tcatch(...)\n\t\t{cout<<\"error:output to file\"<<endl;\n\t}\n";
	}
	if(CALRATIO)
		buf<<"\ttry\n\t{\n\t\ttxtfw.open(\"thread "<<index<<"'s CALRATIO.txt\");\n\t\ttxtfw<<\"计算:\t\"<<cal<<endl<<\"总时间:\t\"<<total<<endl;\n\t\ttxtfw.close();\n\t}\n\tcatch(...)\n\t\t{cout<<\"error:output to file\"<<endl;\n\t}\n";
	buf<<"}\n";
	//if(CallModelEmbed)
	//	buf<<"}";
}
void X86CodeGenerate::CGAllActorHeader()
{
	vector<FlatNode*>::iterator iter;
	stringstream buf,ss;
	buf<<"/*包含所有actor的头文件，主要是为了方便主文件包含*/\n\n";
	if (CallModelEmbed)
	{
		for (iter = flatNodes_.begin(); iter != flatNodes_.end(); ++iter)
		{
			buf << "#include \"" << string(objName) << "_" << (*iter)->name << ".h\"\n";
			ss << dir_ << string(objName) << "_AllActorHeader.h";
		}
	}
	else
	{
		for (iter = flatNodes_.begin(); iter != flatNodes_.end(); ++iter)
		{

			buf << "#include \"" << (*iter)->name << ".h\"\n";
			ss << dir_ << "AllActorHeader.h";
		}
	}
	OutputToFile(ss.str(),buf.str());
}
void X86CodeGenerate::CGAllActorCpp()
{

}
//生成main.cpp，包含main函数的定义
void X86CodeGenerate::CGMain()
{
	stringstream buf,ss;
	char a[10];
	if(TRACE)
	{
		buf<<"#include \"rdtsc.h\"\n";
		buf<<"#include <sstream>\n";
		buf<<"#include <fstream>\n";
	}
	string Tab;
	if(CallModelEmbed)
	{
		buf << "#include \"" << string(objName) << "_RunCOStream.h\"\n";  //chenzhen
		buf << "#include \<iostream>\n";
		buf << "#include <pthread.h>\n";
		buf << "#include \"setCpu.h\"\n";
		buf << "#include \"lock_free_barrier.h\"	//包含barrier函数\n";
		buf << "#include <deque>\n"; // add by mobinsheng
		buf << "#include <stdio.h>""\n"; // add by mobinsheng
		buf << "#include \"memory.h\"\n"; //add by cmt 2017 memset 头文件
		buf << "using namespace std;\n"; // add by mobinsheng
		//buf << " namespace COStream" << string(objName) << "{ \n";
		Tab = "\t";
	}
	else
	{
		buf<<"#include \"iostream\"\n";
		buf<<"#include \"stdlib.h\"\n";
		buf<<"#include <pthread.h>\n";
		buf<<"#include \"setCpu.h\"\n";
		buf<<"#include \"lock_free_barrier.h\"	//包含barrier函数\n";
		buf<<"#include \"global.h\"\n";
		if(gfrtaCallList!=NULL){
			buf<<"#include \"frta.h\"\n";
			buf<<"#include \"GlobalVar.h\"\n";
		}
		buf<<"using namespace std;\n";
	}
	buf<<Tab<<"int MAX_ITER=1;//默认的执行次数是1\n";
	if (TRACE)
	{
		buf<<Tab<<"double (*deltatimes)["<<nCpucore_<<"][2];\n";
		buf<<Tab<<"typedef double time_array["<<nCpucore_<<"][2];\n";
	}
	for (int i=0;i<nCpucore_;i++)
	{
		buf<<Tab<<"extern void thread_"<<i<<"_fun();\n";
	}	
	for(int i=1;i<nCpucore_;i++)
	{
		buf<<Tab<<"void* thread_"<<i<<"_fun_start(void *)\n"<<Tab<<"{\n\t"<<Tab<<"set_cpu("<<i<<");\n\t"<<Tab<<"thread_"<<i<<"_fun();\n\t"<<Tab<<"return 0;\n"<<Tab<<"}\n";
	}
	if (CallModelEmbed) {
		if (readerActor)	//存在filereader，构造以输入流in来创建流程序的数据源
		{
			int fileReaderPush, fileReaderSteadyCount, fileReaderInitCount;
			StreamEdgeInfo  readerEdgeInfo = pEdgeInfo->getEdgeInfo(readerActor, readerActor->outFlatNodes[0]);
			fileReaderInitCount = sssg_->GetInitCount(readerActor);
			fileReaderSteadyCount = sssg_->GetSteadyCount(readerActor);
			fileReaderPush = readerActor->outPushWeights[0];
			buf << "\tRunCOStream::RunCOStream(stringstream& in)\n\t{\n";
			buf << "\t\tvector<" << readerEdgeInfo.typeName << "> sourceDataVec;\n";
			buf << "\t\t" << readerEdgeInfo.typeName << " temp;\n";
			buf << "\t\twhile(in >> temp)\n\t\t\tsourceDataVec.push_back(temp);\n\t\tint size = sourceDataVec.size();\n\t\tsource = new " << readerEdgeInfo.typeName << "[size];\n\t\tfor(int i=0;i<size;i++)\n\t\t\tsource[i] = sourceDataVec[i];\n";
			buf << "\t\tMAX_ITER = (size-" << fileReaderInitCount << "*" << fileReaderPush << ")/(" << fileReaderSteadyCount << "*" << fileReaderPush << ");\n";//设置迭代执行次数 (size - 初态执行次数*push)/(稳态执行次数*push)

			StreamEdgeInfo  writerEdgeInfo;
			int fileWriterPush, fileWriterSteadyCount, fileWriterInitCount;
			if (writerActor)	//存在filewriter，构造输出数组指针
			{
				writerEdgeInfo = pEdgeInfo->getEdgeInfo(writerActor->inFlatNodes[0], writerActor);
				fileWriterInitCount = sssg_->GetInitCount(writerActor);
				fileWriterSteadyCount = sssg_->GetSteadyCount(writerActor);
				fileWriterPush = writerActor->inPopWeights[0];
				buf << "\t\tsink = new " << writerEdgeInfo.typeName << "[MAX_ITER*" << (fileWriterInitCount + fileWriterSteadyCount)*fileWriterPush << "];\n";	//输出数组大小
				buf << "\t\toutputNum = MAX_ITER*" << (fileWriterInitCount + fileWriterSteadyCount)*fileWriterPush << ";\n";
			}
			buf << "\t}\n";

			/*源数据数组指针构造*/
			buf << "\tRunCOStream::RunCOStream(void* pSrc,int srcDataCount)\n\t{\n";
			buf << "\t\tsource = (" << readerEdgeInfo.typeName << "*)pSrc;\n";
			buf << "\t\tMAX_ITER = (srcDataCount-" << fileReaderInitCount << "*" << fileReaderPush << ")/(" << fileReaderSteadyCount << "*" << fileReaderPush << ");\n";//设置迭代执行次数 (size - 初态执行次数*push)/(稳态执行次数*push)
			if (writerActor)
			{
				buf << "\t\tsink = new " << writerEdgeInfo.typeName << "[MAX_ITER*" << (fileWriterInitCount + fileWriterSteadyCount)*fileWriterPush << "];\n";
				buf << "\t\toutputNum = MAX_ITER*" << (fileWriterInitCount + fileWriterSteadyCount)*fileWriterPush << ";\n";
			}
			buf << "\t}\n";

			/*通过文件路径构造*/
			buf << "\tRunCOStream::RunCOStream(string path)\n\t{\n";
			buf << "\t\tifstream ifile;\n";
			buf << "\t\tifile.open(path.c_str());\n";
			buf << "\t\tstringstream instream;\n";
			buf << "\t\tinstream<<ifile.rdbuf();\n";
			buf << "\t\tifile.close();\n";
			buf << "\t\tvector<" << readerEdgeInfo.typeName << "> sourceDataVec;\n";
			buf << "\t\t" << readerEdgeInfo.typeName << " temp;\n";
			buf << "\t\twhile(instream >> temp)\n\t\t\tsourceDataVec.push_back(temp);\n\t\tint size = sourceDataVec.size();\n\t\tsource = new " << readerEdgeInfo.typeName << "[size];\n\t\tfor(int i=0;i<size;i++)\n\t\t\tsource[i] = sourceDataVec[i];\n";
			buf << "\t\tMAX_ITER = (size-" << fileReaderInitCount << "*" << fileReaderPush << ")/(" << fileReaderSteadyCount << "*" << fileReaderPush << ");\n";//设置迭代执行次数 (size - 初态执行次数*push)/(稳态执行次数*push)
			if (writerActor)
			{
				buf << "\t\tsink = new " << writerEdgeInfo.typeName << "[MAX_ITER*" << (fileWriterInitCount + fileWriterSteadyCount)*fileWriterPush << "];\n";
				buf << "\t\toutputNum = MAX_ITER*" << (fileWriterInitCount + fileWriterSteadyCount)*fileWriterPush << ";\n";
			}
			buf << "\t}\n";
		}//存在filereader end
		else 
		{//没有filereader节点，表明数据流程序自己生成数据源，只需要定义默认构造函数
			if (EmbedModeUseThreadPool)
			{
				buf << CGEmbedModeThreadPool(nCpucore_); // add by mobinsheng
				buf << "\tRunCOStream::RunCOStream()\n\t{\n \tCreateThreadPool();\t outArray = 0;" 
					<< "allocBarrier(" << nCpucore_ << ");"
					<<"\n \t}\n";
				/*
				//处理从C++main函数中传入的数据 add  by donghui
				buf << "\tRunCOStream::RunCOStream(stringstream& in)\n\t{\n\t\n";

				FlatNode *topleverNode = sssg_->GetTopLevel();
				StreamEdgeInfo  EdgeInfo = pEdgeInfo->getEdgeInfo(topleverNode, topleverNode->outFlatNodes[0]);

				buf << "\t\t" << EdgeInfo.typeName << " temp;\n";
				//从输入流中传入的数据是基本类型，没有封装在struct中的
				//将temp结构体中的变量名取出来
				string name = EdgeInfo.typeName;
				string val = name.substr(name.find("_") + 1);

				buf << "\t\twhile(in >> temp." << val << ")\n\t\t\tsourceDataVec.push_back(temp);\n";
				buf << "\t}\n";
				*/
				buf << "\tRunCOStream::~RunCOStream()\n\t{\n \t\t NotifyClose();DistroyThreadPool();delete[] outArray;\n}\n";
			}
			else
			{	
				//处理从C++main函数中传入的数据
				//删除 2017/11/14 cmt
				/*
				buf << "\tRunCOStream::RunCOStream(stringstream& in)\n\t{\n\t\n";

				FlatNode *topleverNode = sssg_->GetTopLevel();
				StreamEdgeInfo  EdgeInfo = pEdgeInfo->getEdgeInfo(topleverNode, topleverNode->outFlatNodes[0]);

				buf << "\t\t" << EdgeInfo.typeName << " temp;\n";
				//从输入流中传入的数据是基本类型，没有封装在struct中的
				//将temp结构体中的变量名取出来
				string name = EdgeInfo.typeName;
				string val = name.substr(name.find("_") + 1);

				buf << "\t\twhile(in >> temp." << val << ")\n\t\t\tsourceDataVec.push_back(temp);\n";
				buf << "\t}\n";
				*/

				buf << "\tRunCOStream::RunCOStream()\n\t{\n\t\toutArray = 0; \n \t}\n";
				buf << "\tRunCOStream::~RunCOStream()\n\t{\n\t\tdelete[] outArray;\n  \t}\n";
			}
		   buf << "\tvoid RunCOStream::Run(int iter_num)\n\t{\n";
           buf << "\t\tMAX_ITER = iter_num;\n";
			if (EmbedModeUseThreadPool)
			{

				buf << "\t\tmemset((void*)barrierBuffer,0,sizeof(volatile char) *" << nCpucore_ << ");\n";
				buf << "\t\tNotifyExecute();\n";
			}
			else
			{
		
                buf << "\t\tpthread_t tid[" << nCpucore_ - 1 << "];\n";
				buf << "\t\tallocBarrier(" << nCpucore_ << ");\n";
				for (int i = 1; i < nCpucore_; i++)
				{
					buf << "\t\tpthread_create (&tid[" << i - 1 << "], NULL, thread_" << i << "_fun_start, (void*)NULL);\n";
				}
				buf << "\t\tthread_0_fun();\n";
			}
			
			//buf << "\t}\n";

			//add donghui
			
			//buf << "\tvoid RunCOStream::Run()\n\t{\n";
			//FlatNode *topleverNode = sssg_->GetTopLevel();
			//StreamEdgeInfo  EdgeInfo = pEdgeInfo->getEdgeInfo(topleverNode, topleverNode->outFlatNodes[0]);
			//string name = EdgeInfo.typeName;
			//string val = name.substr(name.find("_") + 1);

			//将接收到的数据拷贝到全局数组中,
			//sourceData[] 不存在, 2017/11/14 cmt 删除
			/*
			buf << "\t\tfor(int i = 0;i < sourceDataVec.size();i++){\n";
			buf << "\t\t\tsourceData[i] = sourceDataVec[i]." << name << ";\n";
			buf << "\t\t}\n";
			*/
			//实现数据的输出，通过全局变量将数据返回
			

			if (CALRATIO){
				buf << "\t\t tsc_counter c0, c1;//标记整个程序run的时间 lihe\n";
				buf << "\t\t long totalCal,totalSyn;\n";
			}


			//		buf<<"\t\tMAX_ITER = iter_num;\n";
			//		if(needExternType)
			//			buf<<"\t\tsplExtern = splPoint;\n";
			//for linux thread

			//buf << "\t\tpthread_t tid[" << nCpucore_ - 1 << "];\n";//2017 cmt 删除
			//buf<<"\t\tset_cpu(0);\n";
			if (CALRATIO)
				buf << "\t\tRDTSC(c0);//程序开始时间， lihe\n";

			/* run()函数的，东辉版本删除
			buf << "\t\tallocBarrier(" << nCpucore_ << ");\n";
			for (int i = 1; i<nCpucore_; i++)
			{
				buf << "\t\tpthread_create (&tid[" << i - 1 << "], NULL, thread_" << i << "_fun_start, (void*)NULL);\n";
			}
			buf << "\t\tthread_0_fun();\n";
			*/
			if (CALRATIO){
				buf << "\t\tRDTSC(c1);//程序结束时间， lihe\n";
				buf << "\t\tlong timeCount = COUNTER_DIFF(c1, c0, CYCLES);//得到程序运行总时间 lihe\n";
				buf << "\t\tcout<<\"totalTime:	\"<<timeCount<<endl;//输出 lihe\n";
				buf << "\t\tfor(int i=0;i<" << nCpucore_ << ";i++){\n";
				buf << "\t\t\tcout<<calArr[i]<<\"	\"<<synArr[i]<<endl;//输出计算时间和同步时间， lihe\n";
				buf << "\t\t\ttotalCal += calArr[i];//累加整个程序的计算时间， lihe\n";
				buf << "\t\t\ttotalSyn += synArr[i];//累加整个程序的同步时间， lihe\n";
				buf << "\t\t}\n";
				buf << "\t\tcout<<totalCal<<\"	\"<<totalSyn<<endl;//输出整个程序的计算时间和同步时间， lihe\n";
			}
			if (writerActor)
				buf << "\t\toutArray = sink;\n";
			buf << "\t}\n";

			
			//buf << "}\n";
			ss << dir_ << string(objName) << "_RunCOStream.cpp";  //chenzhen
		}

	}
	else				//独立模式，生成main
	{
		buf<<"int main(int argc,char **argv)\n{\n";
		buf<<"\tvoid setRunIterCount(int,char**);\n";
		if(gfrtaCallList!=NULL){
			SPL2X86_List(gfrtaCallList,0);
			buf<<"\t";
			buf<<declInitList.str();
			buf<<"\n";
			declInitList.str(""); // 清空declInitList内容
			CGFrta();
		}
		buf<<"\tsetRunIterCount(argc,argv);\n";
		buf<<"\tset_cpu(0);\n";	
		buf<<"\tallocBarrier("<<nCpucore_<<");\n";	
		buf<<"\tpthread_t tid["<<nCpucore_-1<<"];\n";
		for (int i=1;i<nCpucore_;i++)
		{
			buf<<"\tpthread_create (&tid["<<i-1<<"], NULL, thread_"<<i<<"_fun_start, (void*)NULL);\n";
		}
		buf<<"\tthread_0_fun();\n";	
		if (FileW)
		{
			OutputPath = outfileName;
			buf<<"\tstring outpath = "<<OutputPath<<";\n";
			buf<<"\tFileWriter(outpath,0);\n";
		}
		if (TRACE)			
		{
			buf<<"\tstring txtDataFileName = \"traceDat.txt\";\n\tstringstream txtDataContent;\n";
			buf<<"\tdouble perThreadTotalTime["<<nCpucore_<<"]={0},perThreadCalucatorTime["<<nCpucore_<<"]={0};\n";
			buf<<"\ttxtDataContent<<\"该文件记录每个线程每次迭代计算和同步所需要的时钟周期数，单位为10^6\"<<endl;\n";
			buf<<"\tfor(int threadindex=0;threadindex<"<<nCpucore_<<";threadindex++)\n\t{\n";
			buf<<"\t\ttxtDataContent<<\"线程\"<<threadindex<<\"的执行时间如下:\"<<endl;\n";
			buf<<"\t\ttxtDataContent<<\"initworkstage:\\t\\t计算时间\\t\\t同步时间\"<<endl;\n";
			buf<<"\t\tfor(int stageindex=0;stageindex<"<<pSa->MaxStageNum()<<";stageindex++)\n\t\t{\n";
			buf<<"\t\t\ttxtDataContent<<stageindex<<\":\t\"<<deltatimes[stageindex][threadindex][0]<<\"\t\t\t\"<<deltatimes[stageindex][threadindex][1]<<endl;\n\n";
			buf<<"\t\t\tperThreadCalucatorTime[threadindex] += deltatimes[stageindex][threadindex][0];\n";
			buf<<"\t\t\tperThreadTotalTime[threadindex] += (deltatimes[stageindex][threadindex][0]+deltatimes[stageindex][threadindex][1]);\n\t\t}\n";
			buf<<"\t\ttxtDataContent<<\"steadyworkstage:\\t\\t计算时间\\t\\t同步时间\"<<endl;\n";
			buf<<"\t\tfor(int stageindex=0;stageindex<MAX_ITER+"<<pSa->MaxStageNum()-1<<";stageindex++)\n\t\t{\n";
			buf<<"\t\t\ttxtDataContent<<stageindex<<\":\t\"<<deltatimes[stageindex+"<<pSa->MaxStageNum()<<"][threadindex][0]<<\"\t\t\t\"<<deltatimes[stageindex+"<<pSa->MaxStageNum()<<"][threadindex][1]<<endl;\n";
			buf<<"\t\t\tperThreadCalucatorTime[threadindex] += deltatimes[stageindex+"<<pSa->MaxStageNum()<<"][threadindex][0];\n";
			buf<<"\t\t\tperThreadTotalTime[threadindex] += (deltatimes[stageindex+"<<pSa->MaxStageNum()<<"][threadindex][0]+deltatimes[stageindex+"<<pSa->MaxStageNum()<<"][threadindex][1]);\n\t\t}\n\t}\n";
			buf<<"\ttxtDataContent<<\"\\n\\n线程运行时间分布如下：\"<<endl;\n";
			buf<<"\ttxtDataContent<<\"线程		计算时间		同步时间		计算百分比\"<<endl;\n";
			buf<<"\tfor(int threadindex=0;threadindex<"<<nCpucore_<<";threadindex++)\n";
			buf<<"\t\ttxtDataContent<<\"线程\"<<threadindex<<\":		\"<<perThreadTotalTime[threadindex]<<\"		\"<<(perThreadTotalTime[threadindex]- perThreadCalucatorTime[threadindex])<<\"		\"<<100*perThreadCalucatorTime[threadindex]/perThreadTotalTime[threadindex]<<\"%\"<<endl;\n";
			buf<<"\tofstream txtfw;\n\ttry\n\t{\n\t\ttxtfw.open(txtDataFileName.c_str());\n\t\ttxtfw<<txtDataContent.str();\n\t\ttxtfw.close();\n\t}\n\tcatch(...)\n\t{\n\t\tcout<<\"error:output to file\"<<endl;\n\t}\n";
			//trace for image
			buf<<"\tstring imageDataFileName = \"traceDat.dat\";\n\tstringstream imageDataContent;\n\tdouble base;\n";
			buf<<"\tint rectangleIndex = 1;\n\tdouble rectangleHeight = 0.02;\n\timageDataContent<<\"reset;\";\n";
			buf<<"\tfor(int threadindex=0;threadindex<"<<nCpucore_<<";threadindex++)\n\t{\n";
			buf<<"\t\tbase = 0;\n";
			buf<<"\t\tfor(int stageindex=0;stageindex<MAX_ITER+"<<2*pSa->MaxStageNum()-1<<";stageindex++)\n\t\t{\n";
			buf<<"\timageDataContent<<\"set object \"<<rectangleIndex++<<\" rectangle from \"<<base<<\",\"<<threadindex*rectangleHeight<<\" to \";\n";
			buf<<"\t\t\tbase += deltatimes[stageindex][threadindex][0];\n";
			buf<<"\timageDataContent<<base<<\",\"<<(threadindex+1)*rectangleHeight<<\" fc rgb \\\"red\\\";\\n\";\n";
			buf<<"\timageDataContent<<\"set object \"<<rectangleIndex++<<\" rectangle from \"<<base<<\",\"<<threadindex*rectangleHeight<<\" to \";\n";
			buf<<"\t\t\tbase += deltatimes[stageindex][threadindex][1];\n";
			buf<<"\timageDataContent<<base<<\",\"<<(threadindex+1)*rectangleHeight<<\" fc rgb \\\"green\\\";\\n\";\n\t\t}\n\t}\n";
			buf<<"\timageDataContent<<\"plot [0:\"<<int(base+2)<<\"][0:\"<<"<<nCpucore_<<"*rectangleHeight<<\"] 10000000 notitle;\\n\"; \n";
			buf<<"\tofstream imagefw;\n\ttry\n\t{\n\t\timagefw.open(imageDataFileName.c_str());\n\t\timagefw<<imageDataContent.str();\n\t\timagefw.close();\n\t}\n\tcatch(...)\n\t{\n\t\tcout<<\"error:output to file\"<<endl;\n\t}\n";
		}
		buf<<"\treturn 0;\n";
		buf<<"\n}\n";
		buf<<"void setRunIterCount(int argc,char **argv)\n{\n";
		buf<<"\tint oc;\n";
		buf<<"\tchar *b_opt_arg;\n";
		buf<<"\twhile((oc=getopt(argc,argv,\"i:\"))!=-1)\n";
		buf<<"\t{\n";
		buf<<"\t\tswitch(oc)\n";
		buf<<"\t\t{\n";
		buf<<"\t\t\tcase 'i':\n";
		buf<<"\t\t\t\tMAX_ITER=atoi(optarg);\n";
		buf<<"\t\t\t\tbreak;\n";
		buf<<"\t\t}\n";
		buf<<"\t}\n";
		if (FileR)
		{
			InputPath = infileName;
			if (existFileWriter)
			{
				buf<<"\tstring inpath = "<<InputPath<<";\n";
				buf<<"\tint sourceCount = FileReader(inpath,0);\n";
			}
			else
			{
				buf<<"\tstring inpath = "<<InputPath<<";\n";
				buf<<"\tint sourceCount = FileReader(inpath,0);\n";
			}
			int initworkcount = sssg_->GetInitCount(flatNodes_[0]);
			int steadyworkcount = sssg_->GetSteadyCount(flatNodes_[0]);;
			int perWorkItem = flatNodes_[0]->outPushWeights[0];
			buf<<"\tint maxtimes = (sourceCount-"<<perWorkItem<<"*"<<initworkcount<<")/("<<steadyworkcount<<"*"<<perWorkItem<<");\n";//设置迭代执行次数
			buf<<"\tif(MAX_ITER>maxtimes)\n\t{\t\tcout<<\"文件中数据不够执行\"<<MAX_ITER<<\"次,实际只执行\"<<maxtimes<<\"次\";\n\t\tMAX_ITER = maxtimes;\n\t}\n";

		}
		if(writerActor && FileW)
		{
			int initworkcount = sssg_->GetInitCount(flatNodes_[0]);
			int steadyworkcount = sssg_->GetSteadyCount(flatNodes_[0]);;
			int perWorkItem = flatNodes_[0]->outPushWeights[0];
			StreamEdgeInfo  edgeInfo =  pEdgeInfo->getEdgeInfo(writerActor->inFlatNodes[0],writerActor);
			initworkcount = sssg_->GetInitCount(writerActor);
			steadyworkcount = sssg_->GetSteadyCount(writerActor);
			perWorkItem = writerActor->inPopWeights[0];
			buf<<"\tsink = new "<<edgeInfo.typeName<<"[MAX_ITER*"<<(initworkcount+steadyworkcount)*perWorkItem<<"];\n";
			buf<<"\toutPutCount = MAX_ITER*"<<(initworkcount+steadyworkcount)*perWorkItem<<";\n";
		}
		if(TRACE)
			buf<<"\t\tdeltatimes = new time_array[MAX_ITER-1+2*"<<pSa->MaxStageNum()<<"];\n";
	
		buf<<"}\n";
		ss<<dir_<<"main.cpp";
	}
	OutputToFile(ss.str(),buf.str());

	if(CallModelEmbed)	//生成RunCOStream.h和COStream.h
	{
		//生成RunCOStream.h
		buf.str("");
		ss.str("");

		string protect_macro = "COStream" + string(objName) + "RunCOStream_H_"; // add by mobinsheng for 防止重复引用头文件
		buf << "#ifndef " << protect_macro << " \n"; // add by mobinsheng
		buf << "#define " << protect_macro << " \n"; // add by mobinsheng
		buf << "#include \"" << string(objName) << "_COStream.h\"\n"; 
		buf<<"#include \"iostream\"\n";
		buf<<"#include \"stdlib.h\"\n";
		buf<<"#include <pthread.h>\n";
		buf<<"#include \"lock_free_barrier.h\"	//包含barrier函数\n";
		buf << "#include \"" << string(objName) << "_global.h\"\n"; 
		buf<<"#include <vector>\n";
		buf<<"using namespace std;\n";

		//buf << " namespace COStream" << string(objName) << "{ \n";
		buf<<"\tclass RunCOStream\n\t{\n\tpublic:\n";
		buf<<"\t\tRunCOStream(stringstream& in);\n";
		buf<<"\t\tRunCOStream();\n";
		buf<<"\t\tRunCOStream(string path);\n";
		//buf << "\t\tvoid Run(void* pSrc,int srcDataCount);\n";
		buf << "\t\tvoid Run(int iter_num);\n"; // add by mobinsheng

		//cmt 2017 声明可以留这
		buf << "\t\tvoid Run();\n";//add by donghui 

		/*GetOutputNum，取得输出数据个数*/

		buf<<"\t\tint GetOutputNum()\n\t\t{\n\t\t\treturn outputNum;\n\t\t}\n";
		/*GetOutputArray,取得输出数据数组指针*/
		buf<<"\t\tvoid* GetOutputArray()\n\t\t{\n\t\t\treturn outArray;\n\t\t}\n";

		FlatNode *topleverNode = sssg_->GetTopLevel();
		StreamEdgeInfo  EdgeInfo = pEdgeInfo->getEdgeInfo(topleverNode, topleverNode->outFlatNodes[0]);
		string name = EdgeInfo.typeName;
		string valtype = name.substr(0, name.find("_"));
		//string val = name.substr(name.find("_") + 1);

		//返回值直接返回的是vector，vector里是数据类型不是封装在struct中的结构
		//buf << "\n\t\tvector<" << name<< ">";//EdgeInfo.typeName==int_x;
		//buf << " GetOutputArray()\n\t\t{\n";
		//buf << "\t\tfor(int i=0;i<outputNum;i++){\n";
		//buf << "\t\t\t EdegeInfo.typeName temp;\n";
		//buf << "\t\t\t temp."<<val<<"=resultData[i];\n";
		//buf << "\t\t \tresultDataVec.push_back(temp);\n";
		//buf << "\t\t}\n";
		
		
		/*混合编程版本修改
		*2017/11/14修改，删除成员函数vector<> GetOutputArray()

		buf << "\t\tvector<" << name<< "> temp;\n";
		buf << "\t\tfor(int i=0;i<outputNum;i++){\n";
		//buf << "\t\t\ttemp.push_back(resultData[i]." << val <<");\n";
		buf << "\t\ttemp.push_back(resultData[i]);\n";
		buf << "\t\t}\n";
		buf << "\t\treturn temp;\n";
		buf << "\t\t}\n";
		*/

		//析构函数
		//buf<<"\t\t~RunCOStream()\n\t\t{\n\t\t\tdelete[] outArray;\n\t\t}\n"; delete by mobinsheng
		buf << "\t\t~RunCOStream();\n"; //modify by mobinsheng
		/*私有成员变量*/
		buf << "\tprivate:\n\t\tint outputNum;\n";
		buf << "\t\tvoid * outArray;\n};\n";
		//删除，回复原本2017.1.16版本 cmt 2017
		//buf << "\n\t\tvector<" << EdgeInfo.typeName << "> sourceDataVec;\n";
		//buf << "\n\t\tvector<" << EdgeInfo.typeName << "> resultDataVec;\n";

		//buf<<"}\n";
		buf << "#endif " << " \n"; // add by mobisnheng

		ss << dir_ << string(objName) << "_RunCOStream.h";
		OutputToFile(ss.str(),buf.str());
		//生成COStream.h
		buf.str("");
		ss.str("");
		protect_macro = "COStream" + string(objName) + "_H_"; // add by mobinsheng for保护头文件
		buf << "#ifndef " << protect_macro << " \n";  // add by mobinsheng for保护头文件
		buf << "#define " << protect_macro << " \n"; // add by mobinsheng for保护头文件
		buf << " namespace COStream" << string(objName) << "{ \n";
		buf<<"\tclass RunCOStream;\n}\n";
		buf << "#endif " << " \n"; // add by mobinsheng for保护头文件
		ss << dir_ << string(objName) << "_COStream.h";
		OutputToFile(ss.str(),buf.str());
	}
}
//生成FileReaderActor的initwork和work函数
void X86CodeGenerate::CGFileReaderActor(stringstream &buf)		
{
	/*生成initWork()*/
	buf<<"\tvoid initWork()\n\t{\n\t\tdataCount = 0;\n\t}\n";
	/*FileReader的work函数*/
	buf<<"\tvoid work()\n\t{\n";
	buf<<"\t\t"<<readerActor->pushString[0]<<"[0] = source[dataCount++];\n\t\tpushToken();\n\t}\n";
}
//生成FileWriterActor的initwork和work函数
void X86CodeGenerate::CGFileWriterActor(stringstream &buf)
{
	/*生成initWork()*/
	buf<<"\tvoid initWork()\n\t{\n\t\tdataCount = 0;\n\t}\n";
	/*Filewriter的work函数*/
	buf<<"\tvoid work()\n\t{\n";
	//	buf<<"\t\t"<<writerActor->peekString[0]<<"[0] = sink[dataCount++];\n\t\tpopToken();\n\t}\n";
	buf<<"\t\t"<<"sink[dataCount++] = " << writerActor->peekString[0]<<"[0];\n\t\tpopToken();\n\t}\n";
}

void X86CodeGenerate::CGFrta()
{
	
	stringstream buf;
	int n = nDeclDim.size()-1;
	buf<<"#include <string>\n";
	buf<<"#include \"fstream\"\n\n";
	buf<<"using namespace std;\n\n";
	buf<<"int frta(double cc";
	for(int i=0;i<n;i++)
		buf<<"["<<nDeclDim[i+1]<<"]";
	buf<<", string path){\n";
	assert(n<=3);
	if(n>0)
		buf<<"\tint i";
	if(n>1)
		buf<<",j";
	if(n>2)
		buf<<",k";
	buf<<";\n";
	buf<<"\tifstream fin;\n";
	buf<<"\tfin.open(path.c_str());\n";
	buf<<"\tif(!fin.is_open()){\n";
	buf<<"\t\texit(EXIT_FAILURE);\n\t}\n";
	if(n>0)
		buf<<"\tfor(i=0;i<"<<nDeclDim[1]<<";i++)\n";
	if(n>1)
		buf<<"\t\tfor(j=0;j<"<<nDeclDim[2]<<";j++)\n";
	if(n>2)
		buf<<"\t\t\tfor(k=0;k<"<<nDeclDim[3]<<";k++)\n";
	for(int i=0;i<=n;i++)
		buf<<"\t";
	buf<<"fin>>cc";
	if(n>0)
		buf<<"[i]";
	if(n>1)
		buf<<"[j]";
	if(n>2)
		buf<<"[k]";
	buf<<";\n";
	buf<<"\tfin.close();\n";
	buf<<"\treturn 0;\n}\n";

	
	//输出到文件
	stringstream ss;
	ss<<dir_<<"frta.h";
	OutputToFile(ss.str(),  buf.str());
}
int X86CodeGenerate::OutputChar(char val)
{
	switch(val) 
	{
	case '\n': declInitList<<"\\n";break;
	case '\t': declInitList<<"\\t";break;
	case '\v': declInitList<<"\\v";break;
	case '\b': declInitList<<"\\b";break;
	case '\r': declInitList<<"\\r";break;
	case '\f': declInitList<<"\\f";break;
	case '\a': declInitList<<"\\a";break;
	case '\\': declInitList<<"\\\\";break;
	case '\?': declInitList<<"\\\?";break;
	case '\"': declInitList<<"\\\"";break;
	case '\'': declInitList<<"\\\'";break;
	default:
		if (isprint(val)) //判断val是否为可打印字符
		{
			declInitList<<(val);
		} else 
		{
			declInitList<<"\\"<<val;
		}
	}

	return 1;
}
int X86CodeGenerate::OutputString(const char *s)
{ 
	int len = 0;

	declInitList<<"\"";
	while (*s != 0) 
	{
		len += OutputChar(*s++);
	}
	declInitList<<"\"";

	return len + 2;
}
void X86CodeGenerate::CharToText(char *str, unsigned char val){ 
	if (val < ' ') 
	{
		static const char *names[32] = {
			"nul","soh","stx","etx","eot","enq","ack","bel",
			"\\b", "\\t", "\\n", "\\v", "ff", "cr", "so", "si",
			"dle","dc1","dc2","dc3","dc4","nak","syn","etb",
			"can","em", "sub","esc","fs", "gs", "rs", "us" };
			sprintf(str, "0x%02x (%s)", val, names[val]);
	} else if (val < 0x7f) 
	{
		sprintf(str, "'%c'", val);
	} else if (val == 0x7f) 
	{
		strcpy(str, "0x7f (del)");
	} else 
	{ /* val >= 0x80 */
		sprintf(str, "0x%x", val);
	}
}
//生成actor内的语句
void X86CodeGenerate::OutputStmt(Node *node, int offset)
{
	if (node->typ != Block)
		OutputTabs(offset);
	if (node == NULL)
	{
		declInitList << ";";
		return;
	}
	//遍历语句node
	SPL2X86_Node(node, offset);
	switch (node->typ)
	{
	case Id:
	case Const:
	case Binop:
	case Unary:
	case Cast:
	case Ternary:
	case Comma:
	case Call:
	case Array:
	case ImplicitCast:
		//OutputExpr(out, node);
		declInitList << ";\n";
		break;
	case Label:

		break;
	case Switch:

		break;
	case Case:

		break;
	case Default:

		break;
	case If:

		break;
	case IfElse:

		break;
	case While:

		break;
	case Do:

		break;
	case For:

		break;	
	case Goto:

		break;
	case Continue:

		break;
	case Break:

		break;
	case Return:

		break;
	case Block:

		break;
	case Text:

		break;
	default:
		fprintf(stderr, "Internal error: unexpected node");
		PrintNode(stderr, node, 2);
		UNREACHABLE;
	}

}
void X86CodeGenerate::OutputStmtList(List *list, int offset)
{
	ListMarker marker; 
	Node *item = NULL; 

	IterateList(&marker, list); 
	while (NextOnList(&marker, (GenericREF)&item))
	{ 
		OutputStmt(item, offset);
	}
}
//多维数组初始化过程（递归）
void X86CodeGenerate::RecursiveAdclInit(List *init)
{
	//多维数组则init是一个多维的链表
	ListMarker marker;
	Node *item;
	int i=1;
	int len = ListLength(init);
	IterateList(&marker, init);
	while ( NextOnList(&marker, (GenericREF) &item) ) 
	{
		if(i==1) declInitList<<"{";
		if (item->typ == Unary)
		{
			if(i!=1) declInitList<<",";
			declInitList<<GetOpType(item->u.unary.op);
			SPL2X86_Node(item->u.unary.expr,0);
		}
		else if(item->typ == Const) // 如果数组成员是基本类型
		{
			if(i!=1) declInitList<<",";
			SPL2X86_Node(item,0);
		}
		else if (item->typ == Initializer)//如果数组成员是一个数组则递归
		{
			RecursiveAdclInit(item->u.initializer.exprs);
			if(i != len)
			{
				declInitList<<",";
				OutputCRSpaceAndTabs(4);
			}
		}
		else if (item->typ == ImplicitCast)//基本类型的隐式转换
		{
			if(i!=1) declInitList<<",";
			SPL2X86_Node(item->u.implicitcast.value,0);
		}
		i++;
	}
	declInitList<<"}";
}
void X86CodeGenerate::AdclInit(Node *from, int offset)
{
	Node *arrayNode = from->u.decl.type;
	Node *tmpNode = from->u.decl.type;
	Node *initNode = from->u.decl.init;//变量初始化node
	string name = from->u.decl.name;
	string arrayType = GetArrayDataType(tmpNode->u.adcl.type);
	string dim = GetArrayDim(tmpNode->u.adcl.dim);
	bool isGlobal = FALSE;
	if (initNode == NULL) //如果没有初始化，则按数组类型进行初始化
	{
		declInitList << ";";
		if(from->u.decl.tq==T_TOP_DECL){
			nDeclDim.push_back(name);
			isGlobal = TRUE;
			Node* tempNode = from->u.decl.type;
			declList << arrayType;
			while (tempNode->u.decl.type && tempNode->u.decl.type->typ != Sdcl) {
				dim = GetArrayDim(tempNode->u.adcl.dim);
				if (isGlobal)
					nDeclDim.push_back(dim);
				declList << "[" << dim << "]";
				tempNode = tempNode->u.adcl.type;
			}
			declList << ";\n";
			if (tempNode->u.decl.type && tempNode->u.decl.type->typ != Sdcl)
				declInitList << declInitList_temp.str() << "={0};\n";
		}
		declInitList_temp.str("");
	}
	else if(initNode->typ==Call){
		declInitList<<";"; 
	}
	else//如果存在初始化则初始化为指定值
	{
		//declInitList<<declInitList_temp.str();
		declInitList_temp.str("");
		Node* tempNode = from->u.decl.type;
		declList<<arrayType;
		while(tempNode->u.decl.type){
			dim = GetArrayDim(tempNode->u.adcl.dim);
			declList<<"["<<dim<<"]";
			tempNode = tempNode->u.adcl.type;
		}
		declList<<";\n";   
		List *tmp = initNode->u.initializer.exprs;
		int n = ListLength(tmp);
		if(n == 1) // 如果初始化个数1个单一值时，则将数组所有成员初始化为该值
		{
			Node *item = (Node *)FirstItem(tmp);
			if(item->typ == Const)
			{
				declInitList<<" = ";
				RecursiveAdclInit(tmp);
				declInitList<<";\n";
			}
		}
		else //初始的个数和数组维数一致，则可以采取这样的赋值形式：val pp:Array[Int](1) = [1,2,3,4,5];
		{	
			declInitList<<" = ";
			RecursiveAdclInit(tmp);
			declInitList<<";\n";

		}
	}
}
void X86CodeGenerate::OutputConstant(Node *c, Bool with_name)
{
	int len = 0;
	const char *tmpString = c->u.Const.text;

	switch (c->u.Const.type->typ)
	{
	case Prim:
		switch (c->u.Const.type->u.prim.basic) 
		{
		case Sint:
			declInitList<< c->u.Const.value.i;
			break;
		case Uint:
			declInitList<< c->u.Const.value.u;
			break;
		case Slong:
			declInitList<< c->u.Const.value.l<<"L";
			break;
		case Ulong:
			declInitList<< c->u.Const.value.ul<<"UL";
			break;
		case Float:
			if(c->u.Const.value.d==0.0) declInitList<<"0.0";
			else
				declInitList<<c->u.Const.value.f;
			break;
		case Double:
			if(c->u.Const.value.d==0.0) declInitList<<"0.0";
			else if(c->u.Const.value.d - (int)c->u.Const.value.d == 0)
				//declInitList<<c->u.Const.value.d<<".0";
				declInitList<<c->u.Const.value.d;
			else 
				declInitList<<c->u.Const.value.d;
			break;
		case Char:
		case Schar:
		case Uchar:
			OutputChar(c->u.Const.value.i);
			break;

		default:
			Fail(__FILE__, __LINE__, "");
			return ;
		}
		break;
	case Ptr:
		UNREACHABLE;
		// declInitList<< c->u.Const.value.u;//spl没指针，可以忽略
		break;
		/* Used for strings */
	case Adcl:
		OutputString(c->u.Const.value.s);
		break;
	default:
		assert(("Unrecognized constant type", TRUE));
		return ;
	}
}

void X86CodeGenerate::OutputEnumConstant(Node *c, Bool with_name)
{
	int len = 0;
	const char *tmpString = c->u.Const.text;

	switch (c->u.Const.type->typ)
	{
	case Prim:
		switch (c->u.Const.type->u.prim.basic)
		{
		case Sint:
			globalvarbuf << c->u.Const.value.i;
			break;
		case Uint:
			globalvarbuf << c->u.Const.value.u;
			break;
		case Slong:
			globalvarbuf << c->u.Const.value.l << "L";
			break;
		case Ulong:
			globalvarbuf << c->u.Const.value.ul << "UL";
			break;
		case Float:
			if (c->u.Const.value.d == 0.0) declInitList << "0.0";
			else
				globalvarbuf << c->u.Const.value.f;
			break;
		case Double:
			if (c->u.Const.value.d == 0.0) declInitList << "0.0";
			else if (c->u.Const.value.d - (int)c->u.Const.value.d == 0)
				//declInitList<<c->u.Const.value.d<<".0";
				globalvarbuf << c->u.Const.value.d;
			else
				globalvarbuf << c->u.Const.value.d;
			break;
		case Char:
		case Schar:
		case Uchar:
			OutputChar(c->u.Const.value.i);
			break;

		default:
			/*删除最后一个多余的分号*/
			string orignal = globalvarbuf.str();
			//清空declInitList
			globalvarbuf.str("");
			orignal = orignal.substr(0, orignal.size() - 2);

			globalvarbuf << orignal;
			return;
		}
		break;
	case Ptr:
		UNREACHABLE;
		// declInitList<< c->u.Const.value.u;//spl没指针，可以忽略
		break;
		/* Used for strings */
		//	case Adcl:暂不支持数组
		//		OutputString(c->u.Const.value.s);
		break;
	default:
		assert(("Unrecognized constant type", TRUE));
		return;
	}
}


void X86CodeGenerate::SPL2X86_Node(Node *node, int offset)
{
	if (node == NULL) return;

	if(node->parenthesized == TRUE) declInitList << "("; //加括号保证逻辑性

#define CODE(name, node, union) SPL2X86_##name(node, union, offset)
	ASTSWITCH(node, CODE)
#undef CODE

	if(node->parenthesized == TRUE) declInitList << ")"; //加括号保证逻辑性
}

//void X86CodeGenerate::SPL2X86_List(List *list, int offset)
//{
//	ListMarker marker; 
//	Node *item = NULL; 
//
//	IterateList(&marker, list); 
//	while (NextOnList(&marker, (GenericREF)&item))
//	{ 
//		string name = item->u.decl.name;
//		if (strcmp(name.c_str(),"path")==0)
//		{
//			if(isInFileReader)
//				infileName = item->u.decl.init->u.Const.text;
//			if(isInFileWriter)
//				outfileName = item->u.decl.init->u.Const.text;
//			return;
//		}
//		SPL2X86_Node(item, offset);	
//	}
//}

//修改之后代码
void X86CodeGenerate::SPL2X86_List(List *list, int offset)
{
	ListMarker marker;
	Node *item = NULL;

	IterateList(&marker, list);
	int commaCount = 0; // 用来标记逗号运算符的个数
	while (NextOnList(&marker, (GenericREF)&item))
	{
		string name;
		if (item->typ == Unary || item->typ == Binop)
		{
			name = item->u.decl.type->u.decl.name;
			if (isInComma)
			{
				++commaCount;
				if (commaCount == 2)
				{
					declInitList << ",";
					commaCount--;
				}//if
			}//if
		}
		//单独判断结构体 联合体 枚举判断  2015-06-12 yy添加
		else if (item->typ == Sdcl)
		{
			name = item->u.sdcl.type->name;
		}

		else if (item->typ == Udcl)
		{
			name = item->u.edcl.type->name;
		}
		else if (item->typ == Edcl)
		{
			name = item->u.udcl.type->name;
		}
		else
		{
			name = item->u.decl.name;
		}//els

		if (strcmp(name.c_str(), "path") == 0)
		{
			if (isInFileReader)
				infileName = item->u.decl.init->u.Const.text;
			if (isInFileWriter)
				outfileName = item->u.decl.init->u.Const.text;
			return;
		}
		SPL2X86_Node(item, offset);
	}
}
/*******************************************************************

********************************************************************/
void X86CodeGenerate::SPL2X86_STRdcl(Node *node, strdclNode *u, int offset)
{
}

void X86CodeGenerate::SPL2X86_Const(Node *node, ConstNode *u, int offset)
{
	if (isEnumVar == true)
		OutputEnumConstant(node, TRUE);
	else
		OutputConstant(node, TRUE);
}

void X86CodeGenerate::SPL2X86_Id(Node *node, idNode *u, int offset)
{
	declInitList << u->text;
}
string X86CodeGenerate::GetOpType(OpType op)
{
	switch(op)
	{
	case '.':return ".";
	case '&':return "&";
	case '*':return " * ";
	case UPLUS:return "+";
	case '+':return " + ";
	case UMINUS:return "-";
	case '-':return " - ";
	case '~':return "~";
	case '!':return "!";
	case '/':return " / ";
	case '%':return " % ";
	case '<':return " < ";
	case '>':return " > ";
	case '^':return "^";
	case '|':return " | ";
	case '?':return " ? ";
	case ':':return " : ";
	case ';':return "; ";
	case '=':return " = ";
	case SIZEOF:return "sizeof";
	case ARROW:return "->";
	case PREINC:
	case POSTINC:return "++";
	case POSTDEC:
	case PREDEC:return "--";
	case LS:return " << ";
	case RS:return " >> ";
	case LE:return " <= ";
	case GE:return " >= ";
	case EQ:return " == ";
	case NE:return " != ";
	case ANDAND:return " && ";
	case OROR:return " || ";
	case MULTassign:return " *= ";
	case DIVassign:return " /= ";
	case MODassign:return " %= ";
	case PLUSassign:return " += ";
	case MINUSassign:return " -= ";
	case LSassign:return " <<= ";
	case RSassign:return " >>= ";
	case ANDassign:return " &= ";
	case ERassign:return " ^= ";
	case ORassign:return " |= ";
	case ADDRESS:return " & ";
	default:
		FAIL("Unrecognized node type in CodeGeneration!"); break; 
	}
}
void X86CodeGenerate::SPL2X86_Binop(Node *node, binopNode *u, int offset)//对非结构体类型的Stream进行简化,lihe,2012-09-04
{
	//OutputTabs(offset);
	if (node->u.binop.left) 
		SPL2X86_Node(node->u.binop.left,offset); 
	string op = GetOpType(node->u.binop.op);
	declInitList << GetOpType(node->u.binop.op);
	if (node->u.binop.right) 
		SPL2X86_Node(node->u.binop.right, offset);
	
	//declInitList << ";\n";
}

void X86CodeGenerate::SPL2X86_Unary(Node *node, unaryNode *u, int offset)
{
	if (node->u.unary.op != POSTINC && node->u.unary.op != POSTDEC)
		declInitList << GetOpType(node->u.binop.op);
	if (node->u.unary.expr)
	{
		if (node->u.unary.op == SIZEOF)  //chenzhen2016/3/18
		{
			node->u.unary.expr->parenthesized = true;
		}
		SPL2X86_Node(node->u.unary.expr, offset);
	}
	if(node->u.unary.op == POSTINC || node->u.unary.op == POSTDEC)
		declInitList << GetOpType(node->u.binop.op);
}
//string X86CodeGenerate::GetPrimDataType(Node *from)//类型定义
//{
//	string type;
//
//	switch(from->u.prim.basic){
//		 case Sshort:
//		 case Sint:
//			 type = "int";;
//			 break;
//			 /*Manish 2/3 hack to print pointer constants */
//		 case Uint:
//		 case Ushort:
//			 type = "UInt";;
//			 break;
//		 case Slong:
//			 type = "Long";;
//			 break;
//		 case Ulong:
//			 type = "ULong";;
//			 break;
//		 case Float:
//			 type = "float";
//			 break;
//		 case Double:
//			 type = "double";
//			 break;
//		 case Char:
//		 case Schar:
//		 case Uchar:
//			type = "char";
//			 break;
//		 default: type = "Any";
//			break;
//	}
//	return type;
//}

//取数据成员的初始值
string X86CodeGenerate::GetDataInitVal(string type)
{
	string s ;
	if (type == "Char")
	{
		s = "''";
	}else if (type == "UInt")
	{
		s = "0u";
	}
	else if (type == "Int")
	{
		s = "0";
	}else if (type == "Long")
	{
		s = "0l";
	}
	else if (type == "ULong")
	{
		s = "0ul";
	}
	else if (type == "Float" || type == "Double")
	{
		s = "0.0";
	}
	else if (type == "String")
	{
		s = "\"\"";
	}
	else if (type == "Any")
	{
		s = "null";
	}else//如果数组成员非基本类型则初始化为null，作为一个未知类的初值，例如：数组成员也是个数组
		s = "null";
	return s;
}
//取数组的维数
string X86CodeGenerate::GetArrayDim(Node *from)
{
	string dim;
	if (from->typ == Const)//如果维数节点类型为常量，例如a[10]
	{
		if(from->u.Const.text)dim = from->u.Const.text;
		else 
		{
			char *tmpdim = (char *)malloc(20);
			sprintf(tmpdim,"%d",from->u.Const.value.l);//20120322 zww添加
			dim = tmpdim;
		}
	}
	else if (from->typ == Id)
	{
		dim = from->u.id.text;
	}
	else if(from->typ == ImplicitCast && from->u.implicitcast.value->typ ==Const)
	{
		if(from->u.implicitcast.value->u.Const.text)dim = from->u.implicitcast.value->u.Const.text;
		else 
		{
			char *tmpdim = (char *)malloc(20);
			sprintf(tmpdim,"%d",from->u.implicitcast.value->u.Const.value.l);
			dim = tmpdim;
		}
	}
	else if(from->typ == Binop)
	{
		string tmp = declInitList.str(); // 保存
		stringstream tmp2;

		declInitList.str("");
		SPL2X86_Node(from, 0);
		tmp2 << "(" << declInitList.str() << ")";
		dim = tmp2.str();
		declInitList.str("");
		declInitList << tmp; // 恢复 
	}
	else
		UNREACHABLE;

	return dim;
}

//string X86CodeGenerate::GetArrayDataType(Node *node)
//{
//	string type;
//	if (node->typ == Prim) //基本类型
//	{
//		type = GetPrimDataType(node);
//	}
//	else if (node->typ == Ptr) //chenzhen20151208
//	{
//
//		if (node->u.ptr.type->typ == Ptr)  //多级指针，递归执行
//		{
//			type = GetArrayDataType(node->u.ptr.type);
//		}
//		else if (node->u.ptr.type->typ == Tdef)
//		{
//			type = node->u.ptr.type->u.tdef.name;
//		}
//		else if (node->u.ptr.type->typ == Prim)
//		{
//			type = GetPrimDataType(node->u.ptr.type);
//		}
//		else
//		{
//
//		}
//
//		type += "*";
//	}
//	else if (node->typ == Tdef)  //chenzhen20151208
//	{
//		type = node->u.tdef.name;
//	}
//	else if (node->typ == Adcl) // 也是个数组则递归查找类型
//	{
//		stringstream ss;
//		ss<<GetArrayDataType(node->u.adcl.type);
//		type = ss.str();
//	}
//	else // 如果数组的成员是复杂类型，则有待扩展
//	{
//		Warning(1,"this arrayDataType can not be handle!");
//		type = "Any";// 暂时返回一种通用类型
//		UNREACHABLE;
//	}
//	return type;
//}

void X86CodeGenerate::ExtractDeclVariables(Node *from)
{
	stringstream tempdeclList, tempdeclInitList;
	if (from->u.decl.type->typ == Prim) // 基本类型
	{
		Node *typeNode = from->u.decl.type;
		Node *initNode = from->u.decl.init;
		string type = GetPrimDataType(typeNode);
		string name = from->u.decl.name;
		char tempvalude[20];
		declList << "\t" << type << " " << name << ";\n";
		if (initNode) // 存在初始化则进行初始化
		{
			declInitList << "\t\t" << name << " = ";
			SPL2X86_Node(initNode, 0);
			declInitList << ";\n";
		}

		if (isInParam)
		{
			parameterBuf << type << " " << name << ", ";
			thisBuf << "\t\tthis." << name << " = " << name << ";\n";
		}
	}
	else if (from->u.decl.type->typ == Adcl) // 数组, 最多处理二维数组, 高维待扩展
	{
		Node *tmpNode = from->u.decl.type;
		Node *initNode = from->u.decl.init;
		string name = from->u.decl.name;
		string arrayType = GetArrayDataType(tmpNode->u.adcl.type);
		string dim;
		if (initNode == NULL) //如果没有初始化，则按数组类型进行初始化
		{
			string dim = GetArrayDim(tmpNode->u.adcl.dim);
			stringstream sdim;
			sdim << dim;
			int dimNum;
			bool dynamicArray = false;

			if (sdim >> dimNum)		//为数组
				declList << "\t" << arrayType << " " << name << "[" << dim << "]";
			else		//数组维数不确定，使用new动态生成
			{
				dynamicArray = true;
				declList << "\t" << arrayType << "*" << name;
				declInitList << "\t\t" << name << " = new " << arrayType << "[" << dim << "]";
			}
			tmpNode = tmpNode->u.adcl.type;
			while (tmpNode->typ == Adcl)
			{
				dim = GetArrayDim(tmpNode->u.adcl.dim);
				declList << "[" << dim << "]";
				if (dynamicArray)
					declInitList << "[" << dim << "]";
				tmpNode = tmpNode->u.adcl.type;
			}
			declList << ";\n";
			if (dynamicArray)
				declInitList << "();\n";
		}
		else//如果存在初始化则初始化为指定值
		{
			declList << "static " << arrayType << " " << name;
			tempdeclList << arrayType << " " << curactor->name << "::" << name;
			while (tmpNode->typ == Adcl)
			{
				string dim = GetArrayDim(tmpNode->u.adcl.dim);
				declList << "[" << dim << "]";
				tempdeclList << "[" << dim << "]";
				tmpNode = tmpNode->u.adcl.type;
			}
			declList << ";\n";
			tempdeclList << "=";
			List *tmp = initNode->u.initializer.exprs;
			int n = ListLength(tmp);
			if (n == 1) // 如果初始化个数1个单一值时，则将数组所有成员初始化为该值
			{
				Node *item = (Node *)FirstItem(tmp);
				if (item->typ == Const)
				{
					stringstream ss;
					ss << item->u.Const.text;
					if (ss.str() == "NULL")
					{
						declInitList << arrayType << " " << name << "[" << dim << "];\n";
					}
					else
					{
						declInitList << arrayType << " " << name << "[" << dim << "]={" << ss.str() << "};\n";
					}
				}
			}
			else //初始的个数和数组维数一致，则可以采取这样的赋值形式：val pp:Array[Int](1) = [1,2,3,4,5];
			{
				RecursiveAdclInit(tmp);
				tempdeclInitList << tempdeclList.str() << declInitList.str() << ";\n";
				declInitList.str("");


			}
			staticNameInit.push_back(tempdeclInitList.str());
		}

		if (isInParam)
		{
			//parameterBuf << name << " :Array[" << arrayType << "](1);\n";
			parameterBuf << arrayType << " " << name << "[];\n";
			thisBuf << "\t\tthis." << name << " = " << name << ";\n";
		}
	}
	else if (from->u.decl.type->typ == Ptr) // 指针，只能出现在param中
	{
		Node *typeNode = from->u.decl.type;
		Node *initNode = from->u.decl.init;
		string type;
		string ptrOp = "*"; //cz 4_5
		if (typeNode->u.ptr.type->typ == Prim)
			type = GetPrimDataType(typeNode->u.ptr.type);
		else if (typeNode->u.ptr.type->typ == Tdef)
			type = typeNode->u.ptr.type->u.tdef.name;
		else if (typeNode->u.ptr.type->typ == Ptr)  //cz 4-5
		{
			//SPL2X86_Node(typeNode, 0);

			while (typeNode->u.ptr.type->typ == Ptr)
			{
				typeNode = typeNode->u.ptr.type;

				ptrOp += "*";
			}
			if (typeNode->u.ptr.type->typ == Prim)
				type = GetPrimDataType(typeNode->u.ptr.type);
			if (typeNode->u.ptr.type->typ == Tdef)
				type = typeNode->u.ptr.type->u.tdef.name;
		}
		else UNREACHABLE;			//其他类型的暂不支持

		string name = from->u.decl.name;
		char tempvalude[20];
		//declList<<"\t"<<type<<" "<<"*"<<name<<";\n";
		declList << "\t" << type << " " << ptrOp << name << ";\n";
		if (initNode) // 存在初始化则进行初始化
		{
			declInitList << "\t\t" << name << " = ";
			SPL2X86_Node(initNode, 0);
			declInitList << ";\n";
		}

		if (isInParam)
		{
			parameterBuf << type << " " << "*" << name << ", ";
			thisBuf << "\t\tthis." << name << " = " << name << ";\n";
		}
	}
	else if (from->u.decl.type->typ == Tdef)  //直接输出声明，暂时不提供初始化
	{
		string name = from->u.decl.name;
		declList << "\t" << from->u.decl.type->u.tdef.name << " " << name << ";\n";
	}
	else
		UNREACHABLE;
}

void X86CodeGenerate::SPL2X86_Cast(Node *node, castNode *u, int offset)
{
	if(node->u.cast.type->typ==Ptr&&node->u.cast.expr->typ==Const){//添加对NULL的判断
		if(node->u.cast.expr->u.Const.value.i==0)
			declInitList<<"NULL";
	}
	else if (node->u.cast.type->typ == Ptr)
	{
		declInitList << "(";
		SPL2X86_Node(node->u.cast.type, offset);  //多级指针递归调用
		declInitList << ")";
		SPL2X86_Node(node->u.cast.expr, offset);
	}
	else if (node->u.cast.type->typ == Tdef)
	{
		declInitList << "(";
		declInitList << (node->u.cast.type->u.tdef.name) << ")";
		SPL2X86_Node(node->u.cast.expr, offset);
	}
	else{
		declInitList<<"(";
		declInitList<<GetPrimDataType(node->u.cast.type)<<")";
		SPL2X86_Node(node->u.cast.expr,offset);
	}
}

void X86CodeGenerate::SPL2X86_Comma(Node *node, commaNode *u, int offset)
{
	isInComma = true;//正处于逗号表达式中
	SPL2X86_List(u->exprs,offset);
	isInComma = false;
}

void X86CodeGenerate::SPL2X86_Ternary(Node *node, ternaryNode *u, int offset)
{
	SPL2X86_Node(u->cond,offset);
	declInitList<<" ? ";
	SPL2X86_Node(u->true_,offset);
	declInitList<<":";
	SPL2X86_Node(u->false_,offset);
}

void X86CodeGenerate::SPL2X86_Array(Node *node, arrayNode *u, int offset)
{
	SPL2X86_Node(u->name,offset);
	List *tmp = u->dims;
	while(tmp != NULL)//可能是多维数组，需要遍历dim这个list
	{
		declInitList<<"[";
		Node *item = (Node *)FirstItem(tmp);
		SPL2X86_Node(item,offset);
		declInitList<<"]";
		tmp = Rest(tmp);
	}
}

void X86CodeGenerate::SPL2X86_Call(Node *node, callNode *u, int offset)
{
	assert(u->name->typ == Id);
	{
		int flag=1;//标识是否加括号
		const char *ident = u->name->u.id.text;
		if (strcmp(ident,"acos") == 0) declInitList<<"acos";
		else if (strcmp(ident,"acosh")==0) declInitList<<"acosh";
		else if (strcmp(ident,"acosh")==0) declInitList<<"acosh";
		else if (strcmp(ident,"asin")==0) declInitList<<"asin";
		else if (strcmp(ident,"asinh")==0) declInitList<<"asinh";
		else if (strcmp(ident,"atan")==0) declInitList<<"atan";
		else if (strcmp(ident,"atan2")==0) declInitList<<"atan2";
		else if (strcmp(ident,"atanh")==0) declInitList<<"atanh";
		else if (strcmp(ident,"ceil")==0) declInitList<<"ceil";
		else if (strcmp(ident,"cos")==0) declInitList<<"cos";
		else if (strcmp(ident,"cosh")==0) declInitList<<"cosh";
		else if (strcmp(ident,"exp")==0) declInitList<<"exp";
		else if (strcmp(ident,"expm1")==0) declInitList<<"expm1";
		else if (strcmp(ident,"floor")==0) declInitList<<"floor";
		else if (strcmp(ident,"fmod")==0) declInitList<<"fmod";
		else if (strcmp(ident,"frexp")==0) declInitList<<"frexp";
		else if (strcmp(ident,"log")==0) declInitList<<"log";
		else if (strcmp(ident,"log10")==0) declInitList<<"log10";
		else if (strcmp(ident,"log1p")==0) declInitList<<"log1p";
		else if (strcmp(ident,"modf")==0) declInitList<<"modf";
		else if (strcmp(ident,"pow")==0) declInitList<<"pow";
		else if (strcmp(ident,"sin")==0) declInitList<<"sin";
		else if (strcmp(ident,"sinh")==0) declInitList<<"sinh";
		else if (strcmp(ident,"sqrt")==0) declInitList<<"sqrt";
		else if (strcmp(ident,"tan")==0) declInitList<<"tan";
		else if (strcmp(ident,"tanh")==0) declInitList<<"tanh";
		// not from profiling: round(x) is currently macro for floor((x)+0.5)
		else if (strcmp(ident,"round")==0) declInitList<<"round";
		// not from profiling: just stuck in here to keep compilation of gmti 
		// from spewing warnings.
		else if (strcmp(ident,"abs")==0) declInitList<<"abs";
		else if (strcmp(ident,"max")==0) declInitList<<"max";
		else if (strcmp(ident,"min")==0) declInitList<<"min";
		else if (strcmp(ident,"frta")==0) declInitList<<"frta";
		else if (strcmp(ident,"println")==0) {declInitList<<"cout<<fixed<<";flag=2;}
		else if (strcmp(ident,"printf")==0) {declInitList<<"cout<<";flag=3;}
		else if (strcmp(ident,"print")==0) {declInitList<<"cout<<";flag=4;}
		else //unkonwn methods
		{
			declInitList<<u->name->u.id.text;//输出函数名
		}
		if (flag==1)
		{
			declInitList<<"(";
			OutputArgList(u->args,offset);//参数
 			declInitList<<")";
			if (strcmp(ident,"frta")==0)
				declInitList<<";";
		}
		else if(flag==2)
		{
			OutputArgList(u->args,offset);//参数
			declInitList<<"<<endl";
		}
		else
		{
			OutputArgList(u->args,offset);//参数
		}
	}
}

void X86CodeGenerate::SPL2X86_Initializer(Node *node, initializerNode *u, int offset)
{
}

void X86CodeGenerate::SPL2X86_ImplicitCast(Node *node, implicitcastNode *u, int offset)
{
	if (isEnumVar == true)
	{
		/*删除Enum类型未初始化时的“=”*/
		string orignal = globalvarbuf.str();
		//清空declInitList
		globalvarbuf.str("");
		orignal = orignal.substr(0, orignal.size() - 3);

		globalvarbuf << orignal;
	}
	else
		SPL2X86_Node(u->expr, offset);
}

void X86CodeGenerate::SPL2X86_Label(Node *node, labelNode *u, int offset)
{
}

void X86CodeGenerate::SPL2X86_Switch(Node *node, SwitchNode *u, int offset)
{
	declInitList<<"switch (";
	SPL2X86_Node(u->expr,offset);
	declInitList<<")";
	SPL2X86_Node(u->stmt,offset);
}

void X86CodeGenerate::SPL2X86_Case(Node *node, CaseNode *u, int offset)
{
	declInitList<<"case ";
	SPL2X86_Node(u->expr,offset);
	declInitList<<":";
	OutputCRSpaceAndTabs(offset+1);
	SPL2X86_Node(u->stmt,offset);
}

void X86CodeGenerate::SPL2X86_Default(Node *node, DefaultNode *u, int offset)
{
	declInitList<<"default: ";
	OutputCRSpaceAndTabs(offset);
	SPL2X86_Node(u->stmt,offset);
}

void X86CodeGenerate::SPL2X86_If(Node *node, IfNode *u, int offset)
{
	declInitList<<"if (";
	SPL2X86_Node(u->expr,offset);
	declInitList<<")";
	if (u->stmt->typ != Block) // 如果是非block结点，则需要换行对齐
	{
		OutputCRSpaceAndTabs(offset+1);
	}
	SPL2X86_Node(u->stmt,offset+1);
	if(u->stmt->typ == Binop || u->stmt->typ == Unary || u->stmt->typ == Ternary || u->stmt->typ == Call || u->stmt->typ == Decl)//如果是表达式结点则需要在末位添加分号表示结束
		declInitList<<";";
}

void X86CodeGenerate::SPL2X86_IfElse(Node *node, IfElseNode *u, int offset)
{
	declInitList<<"if (";
	SPL2X86_Node(u->expr,offset);
	declInitList<<")";
	if (u->true_->typ != Block)//如果是非block结点，则需要换行对齐
	{
		OutputCRSpaceAndTabs(offset+1);
	}
	SPL2X86_Node(u->true_,offset);
	if(u->true_->typ == Binop || u->true_->typ == Unary || u->true_->typ == Ternary || u->true_->typ == Call || u->true_->typ == Decl)//如果是表达式结点则需要在末位添加分号表示结束
		declInitList<<";";
	OutputCRSpaceAndTabs(offset);
	declInitList<<"else ";
	if (u->false_->typ != Block && u->false_->typ != IfElse)//如果是非block结点或者ifelse结点，则需要换行对齐
	{
		OutputCRSpaceAndTabs(offset+1);
	}
	SPL2X86_Node(u->false_,offset);
	if(u->false_->typ == Binop || u->false_->typ == Unary || u->false_->typ == Ternary || u->false_->typ == Call || u->false_->typ == Decl)//如果是表达式结点则需要在末位添加分号表示结束
		declInitList<<";";
}

void X86CodeGenerate::SPL2X86_While(Node *node, WhileNode *u, int offset)
{
	declInitList<<"while (";
	SPL2X86_Node(u->expr,offset);
	declInitList<<")";
	if (u->stmt->typ != Block)//如果是非block结点，则需要换行对齐
	{
		OutputCRSpaceAndTabs(offset+1);
	}
	SPL2X86_Node(u->stmt,offset);
	if(u->stmt->typ == Binop || u->stmt->typ == Unary || u->stmt->typ == Ternary || u->stmt->typ == Call || u->stmt->typ == Decl)//如果是表达式结点则需要在末位添加分号表示结束
		declInitList<<";";
}

void X86CodeGenerate::SPL2X86_Do(Node *node, DoNode *u, int offset)
{
	declInitList<<"do";
	SPL2X86_Node(u->stmt,offset);
	declInitList<<"while (";
	SPL2X86_Node(u->expr,offset);
	declInitList<<");";
}

void X86CodeGenerate::SPL2X86_For(Node *node, ForNode *u, int offset)
{
	declInitList<<"for (";
	SPL2X86_Node(u->init,offset);
	declInitList<<";";
	SPL2X86_Node(u->cond,offset);
	declInitList<<";";
	SPL2X86_Node(u->next,offset);
	declInitList<<")";
	if (u->stmt->typ != Block)//如果是非block结点，则需要换行对齐
	{
		declInitList<<"\n";
	}
	OutputStmt(u->stmt,offset+1);
}

void X86CodeGenerate::SPL2X86_Goto(Node *node, GotoNode *u, int offset)
{
}

void X86CodeGenerate::SPL2X86_Continue(Node *node, ContinueNode *u, int offset)
{
}

void X86CodeGenerate::SPL2X86_Break(Node *node, BreakNode *u, int offset)
{
	declInitList<<"break;";
}

void X86CodeGenerate::SPL2X86_Return(Node *node, ReturnNode *u, int offset)
{
}

void X86CodeGenerate::SPL2X86_Block(Node *node, BlockNode *u, int offset)
{
	declInitList <<"{\n";
	//输出decl
	SPL2X86_List(u->decl,  offset);
	declInitList<<"\n"; // 另起一行

	OutputStmtList(u->stmts, offset);

	OutputCRSpaceAndTabs(offset);
	declInitList<<"}\n"; // '}'独占一行
}

void X86CodeGenerate::SPL2X86_Prim(Node *node, primNode *u, int offset)
{
	//declInitList << GetPrimDataType(node);
	//globalvarbuf<<"extern "<<GetPrimDataType(node)<<" ";
	////cout<<GetPrimDataType(node)<<endl;
	if (!isstructVar && !isUnionVar && !isEnumVar)
		declInitList << GetPrimDataType(node);
	if (!isstructVar && !isUnionVar && !isEnumVar)//使结构体变量不输出extern
		globalvarbuf << "extern ";
	if (isEnumVar == false)
		globalvarbuf << GetPrimDataType(node) << " ";
}

void X86CodeGenerate::SPL2X86_Tdef(Node *node, tdefNode *u, int offset)
{
	if (isCGGlobalVar)
	{
		if (isstructVar)  //嵌套在其他结构体中，只需要输出重命名即可
			globalvarbuf << u->name << " ";
		else
		{
			globalvarbuf << "typedef ";
			if (u->type->typ == Sdcl)  //原类型为struct，则重新定义；
			{
				if (strcmp(u->type->u.sdcl.type->name, "___sue1") == 0)
				{
					SPL2X86_Node(u->type, offset);
					string orignal = globalvarbuf.str();
					//清空declInitList
					globalvarbuf.str("");
					orignal = orignal.substr(0, orignal.size() - 3);
					globalvarbuf << orignal << u->name << ";\n";
				}
				else
					globalvarbuf << "struct " << u->type->u.sdcl.type->name << " " << u->name << ";\n\n";  //原结构体已先单独定义过，然后再进行类型定义；对于同时进行类型和结构体定义的情形暂不支持。
			}

			else
			{

			}
		}
	}

	else
	{
		declInitList << u->name << " ";
	}
}

void X86CodeGenerate::SPL2X86_Ptr(Node *node, ptrNode *u, int offset)
{
	 //declInitList<< GetArrayDataType(node->u.adcl.type)<<"* ";
	 //globalvarbuf<< GetArrayDataType(node->u.adcl.type)<<"* ";
	if (u->type->typ == Tdef)
	{
		declInitList << u->type->u.tdef.name << "*";
		if (isCGGlobalVar) globalvarbuf << u->type->u.tdef.name << "*";
		
	}
	else if (u->type->typ == Sdcl)
	{
		if (!isstructVar && !isUnionVar && !isEnumVar)
		{
			declInitList << "struct " << u->type->u.sdcl.type->name << " *";
		}
		if (isCGGlobalVar) globalvarbuf << "struct " << u->type->u.sdcl.type->name << " *";
	}
	else if (u->type->typ == Ptr)
	{
		//declInitList << u->type->u.ptr.type->u.tdef.name << "** ";
		//globalvarbuf << u->type->u.ptr.type->u.tdef.name << "** "; 
		//多级指针的递归调用  chenzhen 2015/5/13
		SPL2X86_Node(u->type, offset);
		if (!isstructVar && !isUnionVar && !isEnumVar)
		{
			declInitList << "*";  //chenzhen
		}
		//globalvarbuf << GetArrayDataType(node->u.adcl.type) << "*";
		if (isCGGlobalVar) globalvarbuf << "*";  //chenzhen
		
	}
	else  //基本类型
	{
		
		if (!isstructVar && !isUnionVar && !isEnumVar)
		{
			//declInitList << GetArrayDataType(node->u.adcl.type) << "*";
			declInitList << GetPrimDataType(node->u.adcl.type) << "*";
		}
		//globalvarbuf<< GetArrayDataType(node->u.adcl.type)<<"*";
		if (isCGGlobalVar) globalvarbuf << GetPrimDataType(node->u.adcl.type) << "*";
		
	}
}

void X86CodeGenerate::SPL2X86_Adcl(Node *node, adclNode *u, int offset)
{
	//string arrayType = GetArrayDataType(node->u.adcl.type);
	//declInitList<<arrayType<<" ";
	//globalvarbuf<<"extern "<<arrayType<<" ";
	string arrayType = GetArrayDataType(node->u.adcl.type);
	if (!isstructVar && !isUnionVar && !isEnumVar)
		declInitList << arrayType << " ";
	if (!isstructVar && !isUnionVar && !isEnumVar)//使结构体变量不输出extern
		globalvarbuf << "extern ";
	globalvarbuf << arrayType << " ";
}

//2015-05-08添加函数声明代码生成模块实现
void X86CodeGenerate::SPL2X86_Fdcl(Node *node, fdclNode *u, int offset)
{
	//（1）更新函数声明个数记录
	n_fdcl++;
	//（2）获取函数声明返回值类型
	string funType = GetArrayDataType(node->u.fdcl.returns);
	//（3）获取函数声明参数列表
	ListMarker argList;
	IterateList(&argList, node->u.fdcl.args);
	Node *item = NULL;
	//（4）将类型与参数信息写入globalvarbuf流
	globalvarbuf << "extern " << funType << " " << "(";
	while (NextOnList(&argList, (GenericREF)&item))
	{
		string argType = "";
		if (item->u.decl.type->typ == Prim)
		{
			argType = GetPrimDataType(item->u.decl.type);
		}
		else {
			argType = GetArrayDataType(item->u.decl.type);
		}
		globalvarbuf << argType << " " << item->u.decl.name << " ,";
	}

	string temp = globalvarbuf.str();
	//删除最后一个逗号
	if (temp[temp.length() - 1] == ',')
	{
		globalvarbuf.str("");
		globalvarbuf << temp.substr(0, temp.length() - 1);
	}
	globalvarbuf << ");\n";
}

void X86CodeGenerate::SPL2X86_Sdcl(Node *node, sdclNode *u, int offset)
{
	string sdclType = u->type->name;
	if (isCGGlobalVar)
	{
		globalvarbuf << "struct ";
		if (isstructVar && u->type->typ == Sdcl)  //当前类型为结构体，且isstructVar已为真，说明该结构体嵌套在其他的结构体中，此时不需要输出该结构体的具体定义；
			globalvarbuf << sdclType << " ";
		else
		{
			globalvarbuf << sdclType << "{\n";

			isstructVar = true;//控制变量的输出

			SPL2X86_List(u->type->fields, 0);

			globalvarbuf << "};\n\n";
			//declInitList<<sdclType<<" ";//输出到函数中
			isstructVar = false;
		}

	}
	else
		declInitList << sdclType;
}

void X86CodeGenerate::SPL2X86_Udcl(Node *node, udclNode *u, int offset)
{
	string udclType = u->type->name;
	if (isCGGlobalVar)
	{
		globalvarbuf << "union ";
		globalvarbuf << udclType << "{\n";
		isUnionVar = true;//控制变量的输出
		SPL2X86_List(u->type->fields, 0);
		isUnionVar = false;
		globalvarbuf << "};";
	}
	else
		declInitList << udclType << " ";//输出到函数中
}

void X86CodeGenerate::SPL2X86_Edcl(Node *node, edclNode *u, int offset)
{

	string edclType = u->type->name;
	if (isCGGlobalVar)
	{
		globalvarbuf << "enum ";
		globalvarbuf << edclType << "{\n";
		isEnumVar = true;//控制变量的输出
		SPL2X86_List(u->type->fields, 0);
		isEnumVar = false;

		/*删除最后一个多余的分号*/
		string orignal = globalvarbuf.str();
		//清空declInitList
		globalvarbuf.str("");
		orignal = orignal.substr(0, orignal.size() - 2);
		globalvarbuf << orignal << "\n};\n";
	}
	else
		declInitList << edclType << " ";//输出到函数中
}


void X86CodeGenerate::SPL2X86_Decl(Node *node, declNode *u, int offset)
{
	
	if (extractDecl)
	{
		ExtractDeclVariables(node);
		return;
	}
	else
	{
		if (!isstructVar && !isUnionVar && !isEnumVar && !isCGGlobalVar)
			OutputCRSpaceAndTabs(offset);
		if(isCGGlobalVar && STORAGE_CLASS(node->u.decl.tq) == T_EXTERN)
		{
			declInitList<<"extern ";
			globalvarbuf<<"extern ";
		}
		SPL2X86_Node(u->type, offset);
		if(u->type->typ == Adcl)	//多维数组
		{
			Node *tempNode = u->type;
			globalvarbuf<<node->u.decl.name;
			if (!isstructVar && !isUnionVar && !isEnumVar)
			{
				if (flag_Global)			//输出在GlobalVar中，为整个流程序的全局变量
					declInitList << node->u.decl.name;
				else
					declInitList_temp << node->u.decl.name;
			}
			while(tempNode->typ == Adcl)
			{
				if(STORAGE_CLASS(node->u.decl.tq) == T_EXTERN){
					if (!isstructVar && !isUnionVar && !isEnumVar)
					{

						if (flag_Global)
							declInitList << "[" << "];";
						else
							declInitList_temp << "[" << "];";
					}
					globalvarbuf << "[" << "]";
				}else{
					string dim = GetArrayDim(tempNode->u.adcl.dim);
					if (!isstructVar && !isUnionVar && !isEnumVar)
					{

						if (flag_Global)
							declInitList << "[" << dim << "]";
						else
							declInitList_temp << "[" << dim << "]";
					}
					globalvarbuf << "[" << dim << "]";
				}
				tempNode = tempNode->u.adcl.type;
			}
			temp_declInitList<<declInitList.str()<<";\n";
			globalvarbuf<<";\n";
			if (!isstructVar && !isUnionVar && !isEnumVar)
			{
				declInitList << declInitList_temp.str();  //chenzhen 2015/5/19	
			}
		}
		else if (u->type->typ == Fdcl) { //2015-05-08 添加 
			int count = 0;
			//获取全局函数名,将函数名插入到"("前面
			string temp = globalvarbuf.str();
			int i, begin, end;
			for (i = 0; i < temp.size(); i++)
			{
				if (temp[i] == '(')
				{
					begin = i;
				}
				if (temp[i] == ')')
				{
					end = i;
					count++;
				}
				//判断是否处理的当前函数声明
				if (count == n_fdcl)
					break;
				else
					continue;
			}
			//最终的声明信息
			string declInfo(temp);
			//获得函数声明的标准形式在（之前插入函数名，在）之前删除多余的，号
			declInfo = temp.substr(0, begin) + node->u.decl.name + temp.substr(begin, end - begin) + temp.substr(end);
			globalvarbuf.str("");
			globalvarbuf << declInfo;
		}
		else if (node->u.decl.type->typ == Sdcl || node->u.decl.type->typ == Udcl || node->u.decl.type->typ == Edcl&&isCGGlobalVar)//处理SUE类型的节点
		{
			if (isCGGlobalVar)
			{/*删除多余的一个分号，类型命名*/
				string orignal = globalvarbuf.str();
				//清空declInitList
				globalvarbuf.str("");
				//orignal = orignal.substr(0, orignal.size() - 1);
				globalvarbuf << orignal;
				globalvarbuf << node->u.decl.name << ";\n";
			}
			else
			{
				declInitList << " " << node->u.decl.name << ";";   //变量名

				temp_declInitList << declInitList.str() << ";\n";
			}
		}
		else {
			if (!isstructVar && !isUnionVar && !isEnumVar)//防止SUE类型的变量在Globalvar.cpp文件中
				declInitList << " " << node->u.decl.name;   //变量名

			temp_declInitList << declInitList.str() << ";\n";
			globalvarbuf << node->u.decl.name;
			if (isEnumVar == false)
				globalvarbuf << ";\n";//声明变量				
		}//else
	}

	if(u->type->typ == Adcl)
	{
		if(STORAGE_CLASS(node->u.decl.tq) != T_EXTERN)
			if (!isstructVar && !isUnionVar)
				AdclInit(node, offset);		//初始化数组
	}
	else
	{
		if (node->u.decl.init && node->u.decl.init->typ != Array)
		{
			if (u->type->typ == Prim && u->type->u.prim.basic == Char)//如果是个字符声明
			{
				declInitList << " = " << u->init->u.implicitcast.expr->u.Const.text;
			}
			else
			{
				if (isEnumVar == true)
					globalvarbuf << " = ";			//初始化变量 如int a = 2;

				else
					declInitList << " = ";
				SPL2X86_Node(u->init, offset);
			}
		}
		if (node->u.decl.init && node->u.decl.init->typ == Array)
		{
			declInitList << " = ";
			SPL2X86_Node(u->init, offset);
		}
	}
	if (u->type->typ != Adcl && u->type->typ != Fdcl && !isstructVar && !isUnionVar && !isEnumVar&&node->u.decl.type->typ != Sdcl&&node->u.decl.type->typ != Udcl&&node->u.decl.type->typ != Edcl)
	{
		declInitList << ";\n";
		if (isCGGlobalVar) declInitList << "\n";
	}
	if (isEnumVar == true)   //带有初始化时，需先输出初始化值再输出符号
		globalvarbuf << ",\n";
}

//void X86CodeGenerate::SPL2X86_Decl(Node *node, declNode *u, int offset)
//{
//
//	if (extractDecl)
//	{
//		ExtractDeclVariables(node);
//		return;
//	}
//	else
//	{
//		OutputCRSpaceAndTabs(offset);
//		if (isCGGlobalVar && STORAGE_CLASS(node->u.decl.tq) == T_EXTERN)
//		{
//			declInitList << "extern ";
//			globalvarbuf << "extern ";
//		}
//		SPL2X86_Node(u->type, offset);
//		if (u->type->typ == Adcl)	//多维数组
//		{
//			Node *tempNode = u->type;
//			globalvarbuf << node->u.decl.name;
//			if (flag_Global)			//输出在GlobalVar中，为整个流程序的全局变量
//				declInitList << node->u.decl.name;
//			else
//				declInitList_temp << node->u.decl.name;
//			while (tempNode->typ == Adcl)
//			{
//				if (STORAGE_CLASS(node->u.decl.tq) == T_EXTERN) {
//					if (flag_Global)
//						declInitList << "[" << "];";
//					else
//						declInitList_temp << "[" << "];";
//					globalvarbuf << "[" << "]";
//				}
//				else {
//					string dim = GetArrayDim(tempNode->u.adcl.dim);
//					if (flag_Global)
//						declInitList << "[" << dim << "]";
//					else
//						declInitList_temp << "[" << dim << "]";
//					globalvarbuf << "[" << dim << "]";
//				}
//				tempNode = tempNode->u.adcl.type;
//			}
//			temp_declInitList << declInitList.str() << ";\n";
//			globalvarbuf << ";\n";
//		}
//		else		//标量
//		{
//			declInitList << " " << node->u.decl.name;   //变量名
//			temp_declInitList << declInitList.str() << ";\n";
//			globalvarbuf << node->u.decl.name << ";\n";//声明变量
//		}
//	}
//
//	if (u->type->typ == Adcl)
//	{
//		if (STORAGE_CLASS(node->u.decl.tq) != T_EXTERN)
//			AdclInit(node, offset);		//初始化数组
//	}
//	else
//	{
//		if (node->u.decl.init)
//		{
//			if (u->type->typ == Prim && u->type->u.prim.basic == Char)//如果是个字符声明
//			{
//				declInitList << " = " << u->init->u.implicitcast.expr->u.Const.text;
//			}
//			else
//			{
//				declInitList << " = ";			//初始化变量 如int a = 2;
//				SPL2X86_Node(u->init, offset);
//			}
//		}
//	}
//	if (u->type->typ != Adcl)
//		declInitList << ";";
//}

void X86CodeGenerate::SPL2X86_Attrib(Node *node, attribNode *u, int offset)
{
}

//2015-05-08添加函数定义代码生成模块实现      
void X86CodeGenerate::SPL2X86_Proc(Node *node, procNode *u, int offset)
{
	//（1）若程序运行到此处，更改标志flag值为true，并为declInitLen赋值
	if (!flag)
	{
		flag = 1;
		declInitLen = declInitList.str().size();
	}
	//(2)获取函数名
	string funName = u->decl->u.decl.name;
	//(3)获取函数返回类型
	string returnType = "";
	if (u->decl->u.fdcl.returns->typ == Prim)
	{
		returnType = GetPrimDataType(u->decl->u.fdcl.returns->u.decl.type);
	}
	else {
		returnType = GetArrayDataType(u->decl->u.fdcl.returns->u.decl.type);
	}
	//(4)写入字符流
	declInitList << returnType << " " << funName << "(";
	ListMarker argList;
	IterateList(&argList, node->u.proc.decl->u.decl.type->u.fdcl.args);
	Node *item = NULL;
	while (NextOnList(&argList, (GenericREF)&item))
	{
		string argType = "";
		if (item->u.decl.type->typ == Prim)
		{
			argType = GetPrimDataType(item->u.decl.type);
		}
		else {
			argType = GetArrayDataType(item->u.decl.type);
		}
		string argName = item->u.decl.name;
		declInitList << argType << " " << argName << " ,";
	}
	//(5)删除最后一个多余的逗号
	string orignal = declInitList.str();
	declInitList.str("");
	orignal = orignal.substr(0, orignal.size() - 1);
	declInitList << orignal << ")";
	temp_declInitList << declInitList.str() << "\n";
	//(6)处理并输出函数体代码生成
	declInitList << "\t{ \n";
	if (u->body->typ != Block)//如果是非block结点，则需要换行对齐
	{
		declInitList << "\n\t";
	}
	OutputStmt(u->body, offset + 1);
	declInitList << "\n\t} \n";
}

void X86CodeGenerate::SPL2X86_Text(Node *node, textNode *u, int offset)
{
}
