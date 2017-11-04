#ifndef CONSTANT_H_
#define CONSTANT_H_

#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <stdint.h>

#include "ClassFileGenerator.h"


enum CONSTANT_INFO{
	CONSTANT_Class=7,
	CONSTANT_Fieldref=9,
	CONSTANT_Methodref=10,
	CONSTANT_InterfaceMethodref=11,
	CONSTANT_String=8,
	CONSTANT_Integer=3,
	CONSTANT_Float=4,
	CONSTANT_Long=5,
	CONSTANT_Double=6,
	CONSTANT_NameAndType=12,
	CONSTANT_Utf8=1
};

class ConstantInfo
{
public:
	ConstantInfo(ClassFileGenerator* _bc);
	virtual ~ConstantInfo() =0;
	virtual void output() =0;
//	virtual int getLength();

protected:
	ClassFileGenerator* bc;	//bc用于获得向上引用字节输出函数及常量池
};

/************************************************************/
class ClassConstantInfo:public ConstantInfo
{
public:
	ClassConstantInfo(ClassFileGenerator* _bc,uint16_t nameIdx);
	~ClassConstantInfo();
	void output();
private:
	uint8_t tag;
	uint16_t name_index;
};

/************************************************************/
class  FieldrefConstantInfo:public ConstantInfo
{
public:
	FieldrefConstantInfo(ClassFileGenerator* _bc, uint16_t classIdx, uint16_t nameTypeIdx);
	~FieldrefConstantInfo();
	void output();
private:
	uint8_t tag;
	uint16_t class_index;
	uint16_t name_and_type_index;
};



class  MethodrefConstantInfo:public ConstantInfo
{
public:
	MethodrefConstantInfo(ClassFileGenerator* _bc, uint16_t classIdx, uint16_t nameTypeIdx);
	~MethodrefConstantInfo();
	void output();
private:
	uint8_t tag;
	uint16_t class_index;
	uint16_t name_and_type_index;
};

/*Not used*/
class  InterfaceMethodrefConstantInfo:public ConstantInfo
{
public:
	InterfaceMethodrefConstantInfo(ClassFileGenerator* _bc);
	~InterfaceMethodrefConstantInfo();
	void output();
private:
	uint8_t tag;
	uint16_t class_index;
	uint16_t name_and_type_index;
};



/************************************************************/

class StringConstantInfo:public ConstantInfo
{
public:
	StringConstantInfo(ClassFileGenerator* _bc,uint16_t stringIdx);
	~StringConstantInfo();
	void output();
private:
	uint8_t tag;
	uint16_t string_index;
};

/************************************************************/

class IntegerConstantInfo:public ConstantInfo
{
public:
	IntegerConstantInfo(ClassFileGenerator* _bc, uint32_t val);
	~IntegerConstantInfo();
	void output();
private:
	uint8_t tag;
	uint32_t bytes;
};

//class FloatConstantInfo 省略。。。

/************************************************************/
//class LongConstantInfo 省略。。。


/************************************************************/
class NameAndTypeConstantInfo:public ConstantInfo
{
public:
	NameAndTypeConstantInfo(ClassFileGenerator* _bc,uint16_t nameIdx,uint16_t descriptIdx);
	~NameAndTypeConstantInfo();
	void output();
private:
	uint8_t tag;
	uint16_t name_index;
	uint16_t descriptor_index;
};

/************************************************************/
class Utf8ConstantInfo:public ConstantInfo
{
public:
	Utf8ConstantInfo(ClassFileGenerator* _bc,const std::string& str);
	~Utf8ConstantInfo();
	void output();
private:
	uint8_t tag;
	uint16_t length;
	std::vector<uint8_t> bytes;		//not null-terminated
};



#endif /* CONSTANT_H_ */
