
/*
�Ҿ��������������д���������C++���쳣����
�쳣��������Ϊ���ѵľ��Ǿ�����Щ�쳣��Ϊ�������ɱ��������в���
�Ӷ��������������̣���Щ�쳣����ͬһ������������ϲ�ͬ���ַ���������

����Ϊ�������´����̺���ͬ���쳣��������
1.�ļ�δ�򿪣������ڣ� ����ϵͳ���� �򿪺��ȡ���� (�ⲿ����)
�ļ��Ѿ�����ĩβ��������������ˣ��ڲ㲶��

2.д�����

3.�﷨����
4.δ���� �ض���

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
