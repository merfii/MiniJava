#include "ConstantPool.h"

ConstantInfo::ConstantInfo(ClassFileGenerator* _bc):
bc(_bc)
{
	;
}

ConstantInfo::~ConstantInfo()
{}

ClassConstantInfo::ClassConstantInfo(ClassFileGenerator* _bc,uint16_t idx):
ConstantInfo(_bc),tag(CONSTANT_Class),name_index(idx)
{

}

void ClassConstantInfo::output()
{
	bc->writeByte(tag);
	bc->writeU16(name_index);

}

ClassConstantInfo::~ClassConstantInfo()
{

}

FieldrefConstantInfo::FieldrefConstantInfo(ClassFileGenerator* _bc,uint16_t classIdx,uint16_t nameTypeIdx):
ConstantInfo(_bc),tag(CONSTANT_Fieldref),class_index(classIdx),name_and_type_index(nameTypeIdx)
{

}

void FieldrefConstantInfo::output()
{
	bc->writeByte(tag);
	bc->writeU16(class_index);
	bc->writeU16(name_and_type_index);
}

FieldrefConstantInfo::~FieldrefConstantInfo()
{

}

MethodrefConstantInfo::MethodrefConstantInfo(ClassFileGenerator* _bc, uint16_t classIndex, uint16_t nameTypeIndex):
ConstantInfo(_bc),tag(CONSTANT_Methodref),class_index(classIndex),name_and_type_index(nameTypeIndex)
{

}

void MethodrefConstantInfo::output()
{
	bc->writeByte(tag);
	bc->writeU16(class_index);
	bc->writeU16(name_and_type_index);
}

MethodrefConstantInfo::~MethodrefConstantInfo()
{

}

StringConstantInfo::StringConstantInfo(ClassFileGenerator* _bc,uint16_t stringIdx):
ConstantInfo(_bc),tag(CONSTANT_String),string_index(stringIdx)
{

}

void StringConstantInfo::output()
{
	bc->writeByte(tag);
	bc->writeU16(string_index);
}

StringConstantInfo::~StringConstantInfo()
{

}


IntegerConstantInfo::IntegerConstantInfo(ClassFileGenerator* _bc, uint32_t num):
ConstantInfo(_bc),tag(CONSTANT_Integer),bytes(num)
{


}

void IntegerConstantInfo::output()
{
	bc->writeByte(tag);
	bc->writeU32(bytes);
}

IntegerConstantInfo::~IntegerConstantInfo()
{

}

/*If the name of the method of a CONSTANT_Methodref_info structure
 * begins with a' <' ('\u003c'), then the name
 * must be the special name <init>,
 * representing an instance initialization method
 */
NameAndTypeConstantInfo::NameAndTypeConstantInfo(ClassFileGenerator* _bc,uint16_t nameIdx,uint16_t descriptIdx):
ConstantInfo(_bc),tag(CONSTANT_NameAndType),
name_index(nameIdx),
descriptor_index(descriptIdx)
{

}

void NameAndTypeConstantInfo::output()
{
	bc->writeByte(tag);
	bc->writeU16(name_index);
	bc->writeU16(descriptor_index);
}

NameAndTypeConstantInfo::~NameAndTypeConstantInfo()
{

}


Utf8ConstantInfo::Utf8ConstantInfo(ClassFileGenerator* _bc,const std::string& str):
ConstantInfo(_bc),tag(CONSTANT_Utf8)
{
	for(uint32_t i=0;i<str.length();i++)
		bytes.push_back(str[i]);
	length=bytes.size();
}

void Utf8ConstantInfo::output()
{
	bc->writeByte(tag);
	bc->writeU16(length);
	for(std::vector<uint8_t>::iterator it=bytes.begin();it!=bytes.end();it++)
		bc->writeByte(*it);
}

Utf8ConstantInfo::~Utf8ConstantInfo()
{

}
