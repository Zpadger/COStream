#include "CodeGeneration.h"
#include "X86CodeGenerate.h"
using namespace std;
#ifndef WIN32
#include <sys/stat.h> 
#endif
#ifdef WIN32
#define mkdir(tmp) _mkdir(tmp.c_str());

#else
#define mkdir(tmp) mkdir(tmp.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
char* objName;
//输入为已完成调度的静态数据流图
GLOBAL void CodeGeneration(char *currentDir, SchedulerSSG *sssg, string substring, StageAssignment *psa, Partition *Mp, MAFLPartition *maflp)
{//20121127日根据lihe新版本代码修改
	assert(currentDir);
	assert(sssg);
	//选择代码生成后端类型
	if (X10Backend == TRUE)//x10
	{
		string dir = currentDir;
		dir += "\\X10DistCode\\";
		dir += substring;
		cout<<"-----------生成x10代码-----------"<<endl;
		//取得命令行指定的place个数，若无指定则设置成与程序actor个数一致
		int nCpucore = CpuCoreNum > 0 ? CpuCoreNum : sssg->GetFlatNodes().size();

		cout<<"简单工厂模式...";
		X10CodeGenerate *X10Code = new X10CodeGenerate(sssg, nCpucore, Buffer_Size, dir.c_str());

		// 生成接口文件, 含各个流类型的声明
		X10Code->CGinterface();
		// 生成各个类文件
		X10Code->CGactors();
		// 生成主控文件
		X10Code->SimpleScheduler();
		X10Code->CGmain();

		// 拷贝lib文件
		X10LibCopy tmp;
		tmp.Run(dir.c_str());

		cout<<"done!"<<endl;

	}
	else if (X86Backend == TRUE)
	{
		string dir = currentDir;
#ifdef WIN32
		if (Win)
		{
			dir += "\\X86StaticDistCode_Win\\";
		}
		else dir +="\\X86StaticDistCode_Linux\\";
		dir += substring;
		dir += "\\";
		objName = new char[substring.size()+1];
		strcpy(objName,substring.c_str());

#else
		if(dir == "./")	//若是没有指定目录，则在当前目录下新建文件夹，新文件夹的名字为当前spl
		{
			dir += substring;
			dir += "/";
			mkdir(dir);

		}
		int first,end;
		first = 0;
		first = dir.find_first_of('/',first);
		end = dir.find_first_of('/',first+1);
		while(end !=-1)
		{
			mkdir(dir.substr(0,end));
			first = end;
			end =  dir.find_first_of('/',first+1);
		}
		cout<<"dir_______"<<dir<<endl;
		if(dir.at(dir.size()-1) != '/')
		{
			mkdir(dir);
		}
		objName = new char[substring.size()+1];
		strcpy(objName,substring.c_str());
#endif

		cout<<dir<<"*******************"<<endl;
		cout<<"-----------生成x86代码-----------"<<endl;
		//取得命令行指定的place个数，若无指定则设置成与程序actor个数一致
		int nCpucore = CpuCoreNum > 0 ? CpuCoreNum : sssg->GetFlatNodes().size();
		
		//Begin ——wangliang
		//下面处理空任务核的调度问题
		if (realnCpucore){
			int k = Mp->getParts();//划分的份数
			//	assert(nCpucore == k);

			nCpucore = Mp->orderPartitionResult();
		}
		//End ——wangliang
		cout<<"简单工厂模式...\n";
		X86CodeGenerate *X86Code = new X86CodeGenerate(sssg, nCpucore, dir.c_str(),psa,Mp);
		X86Code->CGGlobalvar();//生成流程序引入的全局变量定义文件	GlobalVar.cpp
		X86Code->CGGlobalvarextern();//生成流程序引入的全局变量的声明文件 GlobalVar.h
		X86Code->CGglobalHeader();	//生成stream流类型和全局数据流缓存区的声明
		X86Code->CGglobalCpp();		//生成流缓冲区的定义
		X86Code->CGThreads();		//生成所有线程
		X86Code->CGactors();		//生成以类表示的计算单元actor
		X86Code->CGMain();//生成启动线程的main文件
#ifdef WIN32
		X86LibCopy tmp;
		tmp.Run(dir.c_str());
#else		
		cout<<"cgmakefile"<<endl;
		X86Code->CGMakefile();
#endif

		cout<<"done!"<<endl;
		//X86Code->CGAllActorHeader();
	}
	else if (GPUBackend)
	{
		string dir = currentDir;
		if (Win)
		{
			dir += "\\GPUDistCode_Win\\";
		}
		else dir +="\\GPUDistCode_Linux\\";
		dir += substring;
		dir += "\\";
		cout<<dir<<"*******************"<<endl;
		cout<<"-----------生成GPU(OpenCL)代码-----------"<<endl;
		//取得命令行指定的place个数，若无指定则设置成与程序actor个数一致
		int ngpu = GpuNum > 0 ? GpuNum : sssg->GetFlatNodes().size();

		
		//cout << nCpucore << endl;
		cout<<"简单工厂模式...";
		GPUCodeGenerate *GPUCode = new GPUCodeGenerate(sssg, ngpu, Buffer_Size, dir.c_str(),psa,maflp);
		GPUCode->CGGlobalvar();//生成全局变量
		GPUCode->CGGlobalvarextern();
		GPUCode->CGThreads();
		GPUCode->CGglobalHeader();
		GPUCode->CGactors();
		GPUCode->CGAllKernel();
		GPUCode->CGglobalCpp();
		GPUCode->CGMain();//生成启动线程的main文件
		GPULibCopy tmp;
		tmp.Run(dir.c_str());
		cout<<"done!"<<endl;
	}
}
