#include "Attribute.h"

Attribute::Attribute(ClassFileGenerator* _bc):
bc(_bc)
{
}

Attribute::~Attribute()
{
}

CodeAttribute::CodeAttribute(ClassFileGenerator* _bc):
Attribute(_bc)
{
	std::string id("Code");
	name_index=bc->newUtf8ConstantInfo(id);
}


void CodeAttribute::addCode(uint8_t op)
{
	code.push_back(op);
}

void CodeAttribute::addCode(uint8_t op,uint16_t offset)
{
	code[offset]=op;

}

int16_t CodeAttribute::getOffset()
{
	assert(code.size());
	return code.size()-1;
}

void CodeAttribute::output()
{
	if(code.size()==0)
		addCode(0);	//不允许方法内代码为空 填充Nop
	bc->writeU16(name_index);
	bc->writeU32(2+2+4+code.size()+2+0+2+0);
	bc->writeU16(MAX_STACK);	//max_stack应该怎么算?
	bc->writeU16(MAX_LOCAL);	//max_locals应该怎么算?
	bc->writeU32(code.size());
	for(std::vector<uint8_t>::iterator it=code.begin();it!=code.end();it++)
		bc->writeByte(*it);

	bc->writeU16(0);
	//write exception_table

	bc->writeU16(0);
	//write attributes
}
CodeAttribute::~CodeAttribute()
{

}

ConstantValueAttribute::ConstantValueAttribute(ClassFileGenerator *_bc, uint16_t constantValueIdx):
Attribute(_bc),length(2),constantvalue_index(constantValueIdx)
{
	std::string id("ConstantValue");
	name_index=bc->newUtf8ConstantInfo(id);
}


void ConstantValueAttribute::output()
{
	bc->writeU16(name_index);
	bc->writeU32(length);
	bc->writeU16(constantvalue_index);
}

ConstantValueAttribute::~ConstantValueAttribute()
{

}
