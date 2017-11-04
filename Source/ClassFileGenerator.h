#ifndef GENERATOR_H
#define GENERATOR_H

#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>

//这里本来想用 #include<cstdint> 为什么会报错呢？
#include <stdint.h>

#define HARDWARE_IS_LITTLE_ENDIAN

class Field;
class Method;
class ConstantInfo;
class ConstantValueAttribute;
class Attribute;
class CodeAttribute;
class MethodPara;
class VarPara;

class ClassFileGenerator
{
public:
		//语法分析器只需要调用前几个就足够了，越向下越底层，供内部调用
		ClassFileGenerator(std::string _className,bool _debug);
		~ClassFileGenerator();

		//获取当前类的索引
		uint16_t getThisIdx();
		//获取类名
		std::string getName();
		void setSuperClass(uint16_t classIdx);
		void setSuperClass(ClassFileGenerator *super);
		ClassFileGenerator *getSuperClass();


		void newField(const std::string& name, const std::string& descriptor,uint32_t val);
		void newField(const std::string& name, const std::string& descriptor,const std::string& val);
		void beginMethod(const std::string& name, const std::string&, bool isStatic);		//不可被索引
		void beginMethod(const std::string& name, const std::string&,int paraCount, bool isStatic);

		//向当前所在的方法写入代码
		void addCode(uint8_t op);
		void addCode(uint8_t op,uint16_t offset);
		void addCodeU16(uint16_t num);
		void addCodeU16(uint16_t num,int16_t offset);
		//获得当前代码的地址
		int16_t getOffset();
		void endMethod();
		void output();

		//获得函数信息
		MethodPara getMethodDecl(const std::string& methodName);
		//获得变量信息
		VarPara getVarDecl(const std::string& varName);


        uint16_t newClassConstant(const std::string& name);

    	/*
        FieldDescriptor := FieldType
		ComponentType := FieldType
		FieldType := BaseType|ObjectType|ArrayType
		BaseType := B|C|D|F|I|J|S|Z
		ObjectType := L<classname>;
		ArrayType := [ComponentType

		B 	byte 	signed byte
		C 	char 	Unicode character
		D 	double 	double-precision floating-point value
		F 	float 	single-precision floating-point value
		I 	int 	integer
		J 	long 	long integer
		L<classname>; 	reference 	an instance of class <classname>
		S 	short 	signed short
		Z 	boolean 	true or false
		[ 	reference 	one array dimension
    	*/
        uint16_t newFieldrefConstant(uint16_t classIdx,const std::string& fieldName,const std::string& fieldDescriptor);

        /*
		MethodDescriptor := (ParameterDescriptor*)ReturnDescriptor
		ParameterDescriptor := FieldType
		ReturnDescriptor := FieldType|V

		Note that internal forms of fully qualified names are used.
        */
        uint16_t newMethodrefConstant(uint16_t classIdx,const std::string& methodName,const std::string& methodDescriptor);

        uint16_t newStringConstantInfo(const std::string& str);
        uint16_t newIntegerConstantInfo(uint32_t num);
        uint16_t newNameAndTypeConstantInfo(const std::string& name,const std::string& sescriptor);
        uint16_t newUtf8ConstantInfo(const std::string &str);

        void writeByte(uint8_t _bt);
        void writeU16(uint16_t val);
        void writeU32(uint32_t val);
        void writeString(const std::string& str);
        void writeString(char *str);

private:
	uint32_t magic;
	uint16_t minor_version;
	uint16_t major_version;

	//uint16_t constant_pool_count;	equal to the number of entries in the constant_pool table plus one
	//The constant_pool table is indexed from 1 to constant_pool_count-1.
	std::vector<ConstantInfo*> constant_pool;

	uint16_t access_flags; 	//只能是public
	uint16_t this_class;	//指向常量池中的CONSTANT_Class_info
	uint16_t super_class;

	uint16_t interfaces_count;
	//uint16_t interfaces[interfaces_count];	无

	//uint16_t fields_count;
	std::vector<Field*> fields;


	//uint16_t methods_count;
	std::vector<Method*> methods;

	uint16_t attributes_count;
	//std::vector<Attribute> attributes; 本来只能加SourceFile 我们不支持

	/***************成员变量 成员函数的符号表******************/
	std::map<std::string,MethodPara> methodList;
	std::map<std::string,VarPara> varList;
	ClassFileGenerator *p_super;

	/***************以下为内部使用的变量******************/
	std::map<std::string,int> constantsMap;		//newUtf8ConstantInfo中用来判断是否重复
	Method* currentMethod;
	std::string className;
	std::ofstream target;
	bool debug;
};

class Field
{
public:
	Field(ClassFileGenerator* _bc,uint16_t nameIdx, uint16_t descripIdx);
	void addAttribute(Attribute* attr);
	void output();

private:
	uint16_t access_flags;	//只能是public
	uint16_t name_index;
	uint16_t descriptor_index;

	//只能放1个ConstantValue
	//uint16_t attributes_count;
	std::vector<Attribute*> attributes;

	ClassFileGenerator* bc;
};

class Method
{
public:
	Method(ClassFileGenerator* _bc,uint16_t nameIdx, uint16_t descripIdx,bool isStatic);
	void addCode(uint8_t op);
	void addCode(uint8_t op,int16_t offset);
	int16_t getOffset();
	void output();
	int getLength();

private:
	uint16_t access_flags;
	uint16_t name_index;
	uint16_t descriptor_index;

	//只能放CodeAttribute
	//uint16_t attributes_count;
	std::vector<Attribute*> attributes;

	CodeAttribute* code;	//避免在调用addCode()时的强制类型转换
	ClassFileGenerator* bc;
};


class MethodPara
{
public:
	MethodPara(std::string name_,std::string desc_,int count);
	MethodPara();
	std::string name;
	std::string desc;
	int paraCount;

};

class VarPara
{
public:
	VarPara(std::string name_,std::string desc_);
	VarPara();
	std::string name;
	std::string desc;
};


#include "Attribute.h"
#include "ConstantPool.h"





#endif
