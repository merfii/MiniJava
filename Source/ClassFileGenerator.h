#ifndef GENERATOR_H
#define GENERATOR_H

#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>

//���ﱾ������ #include<cstdint> Ϊʲô�ᱨ���أ�
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
		//�﷨������ֻ��Ҫ����ǰ�������㹻�ˣ�Խ����Խ�ײ㣬���ڲ�����
		ClassFileGenerator(std::string _className,bool _debug);
		~ClassFileGenerator();

		//��ȡ��ǰ�������
		uint16_t getThisIdx();
		//��ȡ����
		std::string getName();
		void setSuperClass(uint16_t classIdx);
		void setSuperClass(ClassFileGenerator *super);
		ClassFileGenerator *getSuperClass();


		void newField(const std::string& name, const std::string& descriptor,uint32_t val);
		void newField(const std::string& name, const std::string& descriptor,const std::string& val);
		void beginMethod(const std::string& name, const std::string&, bool isStatic);		//���ɱ�����
		void beginMethod(const std::string& name, const std::string&,int paraCount, bool isStatic);

		//��ǰ���ڵķ���д�����
		void addCode(uint8_t op);
		void addCode(uint8_t op,uint16_t offset);
		void addCodeU16(uint16_t num);
		void addCodeU16(uint16_t num,int16_t offset);
		//��õ�ǰ����ĵ�ַ
		int16_t getOffset();
		void endMethod();
		void output();

		//��ú�����Ϣ
		MethodPara getMethodDecl(const std::string& methodName);
		//��ñ�����Ϣ
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

	uint16_t access_flags; 	//ֻ����public
	uint16_t this_class;	//ָ�������е�CONSTANT_Class_info
	uint16_t super_class;

	uint16_t interfaces_count;
	//uint16_t interfaces[interfaces_count];	��

	//uint16_t fields_count;
	std::vector<Field*> fields;


	//uint16_t methods_count;
	std::vector<Method*> methods;

	uint16_t attributes_count;
	//std::vector<Attribute> attributes; ����ֻ�ܼ�SourceFile ���ǲ�֧��

	/***************��Ա���� ��Ա�����ķ��ű�******************/
	std::map<std::string,MethodPara> methodList;
	std::map<std::string,VarPara> varList;
	ClassFileGenerator *p_super;

	/***************����Ϊ�ڲ�ʹ�õı���******************/
	std::map<std::string,int> constantsMap;		//newUtf8ConstantInfo�������ж��Ƿ��ظ�
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
	uint16_t access_flags;	//ֻ����public
	uint16_t name_index;
	uint16_t descriptor_index;

	//ֻ�ܷ�1��ConstantValue
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

	//ֻ�ܷ�CodeAttribute
	//uint16_t attributes_count;
	std::vector<Attribute*> attributes;

	CodeAttribute* code;	//�����ڵ���addCode()ʱ��ǿ������ת��
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
