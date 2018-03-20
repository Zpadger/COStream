#ifndef _X86_LIB_COPY_H
#define _X86_LIB_COPY_H
#include "schedulerSSG.h"

using namespace std;

class X86LibCopy
{
private:

	char *libDir_;
	char *producer_file_name_;
	char *consumer_file_name_;
	char *buffer_file_name_;
	char *lock_free_h_;
	char *lock_free_c_;
	char *Math_name_;
	char *makefile_name_;
	char *content_;
	char *time_file_name_;
	char *setCpu_name_;
	char *setCpu_src_name_; // add by mobinsheng 把setCpu函数从头文件提取到源文件，避免了编译错误
	char *tinystrh;
	char *tinyxmlh;
	char *tinystrcpp;
	char *tinyxmlcpp;
	char *tinyxmlerrorcpp;
	char *tinyxmlparsercpp;
	//DSG-YSZ
	//char* DSGproducer_file_name;
	//char* DSGconsumer_file_name;
	char *DSGname;
public:
	int TextFileWrite(char *file_name, char *content);
	char *TextFileRead(const char *file_name);

	X86LibCopy();
	~X86LibCopy(){};
	void Run(const char *dir);
};
extern "C"{
	extern Bool Win;
	extern Bool Linux;
	extern Bool TRACE;
	extern Bool CHECKBARRIERTIME;
	extern Bool CHECKEACHACTORTIME;
	extern Bool MakeProfile;
 	extern GLOBAL const char *output_file;
 	extern Bool CallModelEmbed; 
 	extern GLOBAL const char *infileName;
 	extern GLOBAL const char *outfileName ;
 	extern GLOBAL Bool NoCheckBuffer;
 	extern GLOBAL Bool AmplifySchedule;
 	extern GLOBAL int AmplifyFactor;
 	extern GLOBAL Bool CALRATIO;
	extern GLOBAL Bool PrintResult;
 };
#endif