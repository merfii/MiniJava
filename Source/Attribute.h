
#ifndef ATTRIBUTES_H_
#define ATTRIBUTES_H_

#include<stdint.h>
#include<vector>
#include<string>
#define MAX_STACK 255
#define MAX_LOCAL 127


class ClassFileGenerator;

class Attribute
{
public:
	Attribute(ClassFileGenerator* _bc);	//bc���ڷ������ֽ����������������

	virtual ~Attribute() =0;
	virtual void output() =0;
//	virtual int getLength()=0;
protected:
	ClassFileGenerator* bc;
};


/************************************************************/
class CodeAttribute:public Attribute
{
public:
	CodeAttribute(ClassFileGenerator* _bc);
	~CodeAttribute();
	//�ڼ������jvmָ��
	void addCode(uint8_t op);
	//��op���ǵ�offsetλ�õ�ԭ��ָ��
	void addCode(uint8_t op,uint16_t offset);
	//������д������һ��ָ��ĵ�ַ
	int16_t getOffset();
	void output();
private:
	uint16_t name_index;
	/*uint32_t length;
	The value of the attribute_length item
	indicates the length of the attribute,
	excluding the initial six bytes.
	*/
	//uint16_t max_stack;
	//uint16_t max_locals;

	//uint32_t code_length;
	std::vector<uint8_t> code;

	//uint16_t exception_table_length;
	//exception_table[exception_table_length];  exceptionTable;

	//uint16_t attributes_count;
	//attributes[attributes_count]; LineNumberTable and LocalVariableTable
};



/************************************************************/
class  ConstantValueAttribute:public Attribute
{
public:
	ConstantValueAttribute(ClassFileGenerator* _bc, uint16_t constantIdx);
	~ConstantValueAttribute();
	void output();
private:
	uint16_t name_index;
	uint32_t length;
	uint16_t constantvalue_index;
};

/************************************************************/
//class ExceptionsAttribute ʡ�ԡ�����

#include "ClassFileGenerator.h"


#endif /* ATTRIBUTES_H_ */
