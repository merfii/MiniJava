/*
 * 编译原理大作业 MiniJava
 * 高等工程学院 马政飞 12231036
 *
 */
//我决定本程序中所有错误反馈都用C++的异常机制
#include <iostream>
#include <iomanip>
#include <fstream>
#include "Exception.h"
#include "LexicalAnalyzer.h"
#include "ClassFileGenerator.h"
#include "GrammarAnalyzer.h"

using namespace std;


int main(int argc,char **argv)
{
	string srcFile;
	bool debug=false;
	//多种输入方式
	if(argc==2)
	{
		srcFile.append(argv[1]);
	}
	else if(argc==3)
	{
		if(string(argv[1])==string("-debug"))
		{
			srcFile.append(argv[2]);
			debug=true;

		}else if(string(argv[2])==string("-debug"))
		{
			srcFile.append(argv[1]);
			debug=true;
		}else
		{
			cout<<"Argument error. Please run with: Minijava [-debug] source file name "<<endl;
			return 1;
		}
	}
	else
	{
		cout<<"Input source file name:"<<endl;
		cin>>srcFile;
	}

	GrammarAnalyzer grammarAnalyzer(srcFile,debug);
	try{
		grammarAnalyzer.startAnalyze();
		grammarAnalyzer.generateClassFile();
	}catch(...)
	{
		cout<<"Error near end "<<endl;
	}
	return 0;

}
