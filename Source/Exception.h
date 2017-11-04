
/*
我决定本程序中所有错误反馈都用C++的异常机制
异常处理中最为困难的就是决定哪些异常作为类名，由编译器进行捕获，
从而决定程序处理流程；哪些异常采用同一个处理流程配合不同的字符串报错即可

我认为，有如下大类蕴含不同的异常处理流程
1.文件未打开（不存在） 磁盘系统错误 打开后读取错误 (外部捕获)
文件已经读至末尾，可能是真的完了（内层捕获）

2.写入错误

3.语法错误
4.未定义 重定义

*/

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <iostream>
#include <cassert>
#include <exception>

using std::exception;
using std::string;

class ReadError:public exception
{
public:
	explicit ReadError(const string& _msg);
	string msg;
	~ReadError() throw(){}
};

class SyntaxError:public exception
{
public:
	explicit SyntaxError(const string& _msg,int _jump);
	string msg;
	int jump;
	~SyntaxError() throw(){}
};


class TypeError:public exception
{
public:
	explicit TypeError(const string& _msg,int _jump);
	string msg;
	int jump;
	~TypeError() throw(){}
};


class EofError:public exception
{
public:
	explicit EofError();
	~EofError() throw(){}
};


#endif /* EXCEPTION_H_ */
