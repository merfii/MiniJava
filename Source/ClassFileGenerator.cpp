#include "ClassFileGenerator.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

ClassFileGenerator::ClassFileGenerator(std::string _className,bool _debug)
{
	magic=0xCAFEBABE;
	minor_version=0;
	major_version=48;	//不能大于49

	access_flags=0x0001; 	//public; ACC_STATIC对应0x0008

	interfaces_count=0;
	attributes_count=0;
	currentMethod=NULL;
	className=_className;
	this_class=newClassConstant(_className);
	p_super=NULL;

	debug=_debug;
}

ClassFileGenerator::~ClassFileGenerator()
{
	if(target.is_open())
		target.close();
	//释放所有vector中用new分配的内存
}


void ClassFileGenerator::setSuperClass(uint16_t classIdx)
{
	super_class=classIdx;
}

void ClassFileGenerator::setSuperClass(ClassFileGenerator *super)
{
	p_super=super;
}

ClassFileGenerator* ClassFileGenerator::getSuperClass()
{
	return p_super;
}

void ClassFileGenerator::newField(const std::string& name, const std::string& descriptor,uint32_t val)
{
	fields.push_back(new Field(this,
					newUtf8ConstantInfo(name),
					newUtf8ConstantInfo(descriptor))
					);

	fields[fields.size()-1]->addAttribute(
			new ConstantValueAttribute(this,
				newIntegerConstantInfo(val)
								));
	varList[name]=VarPara(name,descriptor);
}

void ClassFileGenerator::newField(const std::string& name,const std::string& descriptor, const std::string& val)
{
	fields.push_back(new Field(this,
					newUtf8ConstantInfo(name),
					newUtf8ConstantInfo(descriptor))
					);
	fields[fields.size()-1]->addAttribute(
			new ConstantValueAttribute(this,
				newStringConstantInfo(val)
								));
	varList[name]=VarPara(name,descriptor);
}

void ClassFileGenerator::beginMethod(const std::string& name,const std::string& descriptor, bool isStatic)
{
	assert(currentMethod==NULL);
	currentMethod=new Method(this,
			newUtf8ConstantInfo(name),
			newUtf8ConstantInfo(descriptor),isStatic);
	methods.push_back(currentMethod);

}

void ClassFileGenerator::beginMethod(const std::string& name,const std::string& descriptor,int paraCount, bool isStatic)
{
	beginMethod(name,descriptor,isStatic);
	methodList[name]=MethodPara(name,descriptor,paraCount);

}


void ClassFileGenerator::addCode(uint8_t op)
{
	assert(currentMethod);
	currentMethod->addCode(op);
}
void ClassFileGenerator::addCode(uint8_t op,uint16_t offset)
{
	assert(currentMethod);
	currentMethod->addCode(op,offset);
}

void ClassFileGenerator::addCodeU16(uint16_t num)
{
	assert(currentMethod);
#ifdef HARDWARE_IS_LITTLE_ENDIAN
	currentMethod->addCode(((uint8_t *)&num)[1]);
	currentMethod->addCode(((uint8_t *)&num)[0]);
#else
#error LITTLE_ENDIAN_NEED!
#endif
}

void ClassFileGenerator::addCodeU16(uint16_t num,int16_t offset)
{
	assert(currentMethod);
#ifdef HARDWARE_IS_LITTLE_ENDIAN
	currentMethod->addCode(((uint8_t *)&num)[1],offset);
	currentMethod->addCode(((uint8_t *)&num)[0],offset+1);
#else
#error LITTLE_ENDIAN_NEED!
#endif

}


int16_t ClassFileGenerator::getOffset()
{
	return currentMethod->getOffset();
}


uint16_t ClassFileGenerator::getThisIdx()
{
	return this_class;

}

std::string ClassFileGenerator::getName()
{
	return className;
}


void ClassFileGenerator::endMethod()
{
	currentMethod=NULL;
}


MethodPara ClassFileGenerator::getMethodDecl(const std::string& methodName)
{
	if(methodList.count(methodName))
		return methodList[methodName];
	else if(p_super)
		return p_super->getMethodDecl(methodName);
	else
		return MethodPara();
}
		//获得变量信息
VarPara ClassFileGenerator::getVarDecl(const std::string& varName)
{
	if(varList.count(varName))
		return varList[varName];
	else if(p_super)
		return p_super->getVarDecl(varName);
	else
		return VarPara();

}

uint16_t ClassFileGenerator::newClassConstant(const std::string& name)
{

	constant_pool.push_back(
			new ClassConstantInfo(this,
					newUtf8ConstantInfo(name)
					));
	return constant_pool.size();

}

uint16_t ClassFileGenerator::newFieldrefConstant(uint16_t classIdx,const std::string& fieldName, const std::string& fieldDescriptor)
{
	constant_pool.push_back(
			new FieldrefConstantInfo(this,classIdx,
					newNameAndTypeConstantInfo(fieldName,fieldDescriptor)
					)
							);
	return constant_pool.size();
}

uint16_t ClassFileGenerator::newMethodrefConstant(uint16_t classIdx,const std::string& methodName,const std::string& methodDescriptor)
{
	constant_pool.push_back(
		new MethodrefConstantInfo(this,classIdx,
			newNameAndTypeConstantInfo(methodName,methodDescriptor)
								)
							);
	return constant_pool.size();
}

uint16_t ClassFileGenerator::newStringConstantInfo(const std::string& str)
{
	constant_pool.push_back(
			new StringConstantInfo(this,newUtf8ConstantInfo(str))
			);
	return constant_pool.size();
}

uint16_t ClassFileGenerator::newIntegerConstantInfo(uint32_t num)
{
	constant_pool.push_back(
			new IntegerConstantInfo(this,num)

	);
	return constant_pool.size();
}

uint16_t ClassFileGenerator::newNameAndTypeConstantInfo(const std::string& name, const std::string& descriptor)
{

	constant_pool.push_back(
			new NameAndTypeConstantInfo(this,
					newUtf8ConstantInfo(name),
					newUtf8ConstantInfo(descriptor)
									)
			);
	return constant_pool.size();
}

uint16_t ClassFileGenerator::newUtf8ConstantInfo(const std::string& str)
{
	assert(!str.empty());

	//判断符号是否重复
	if(constantsMap.count(str))
	{
		return constantsMap[str];
	}else		//未出现过
	{
		constant_pool.push_back(new Utf8ConstantInfo(this,str));
		constantsMap[str]=constant_pool.size();
		return constant_pool.size();
	}
}

void ClassFileGenerator::output()
{

	std::string fileName;
	fileName=className+".class";
	target.open(fileName.c_str(),std::ios::out|std::ios::trunc|std::ios::binary);
	if(!target.is_open())
	{
		std::cerr<<"Can't open target file!"<<std::endl;
		exit(-1);
	}
	writeU32(magic);
	writeU16(minor_version);
	writeU16(major_version);
	writeU16(constant_pool.size()+1);

	for(std::vector<ConstantInfo*>::iterator it=constant_pool.begin();it!=constant_pool.end();it++)
	{	
		(*it)->output();
	}
	writeU16(access_flags);
	writeU16(this_class);
	writeU16(super_class);
	writeU16(interfaces_count);		//0
	//write interfaces
	writeU16(fields.size());
	for(std::vector<Field*>::iterator it=fields.begin();it!=fields.end();it++)
		(*it)->output();
	writeU16(methods.size());
	for(std::vector<Method*>::iterator it=methods.begin();it!=methods.end();it++)
		(*it)->output();
	writeU16(attributes_count);
	//write attributes
}

void ClassFileGenerator::writeByte(uint8_t bt)
{
	target.put(bt);

	if(debug)
	{
		static int n;
		if(n%16==0)
				std::cout<<std::endl;
		if(bt>16)
			std::cout<<"0x"<<std::hex<<(int)bt<<" ";
		else	//前导0
			std::cout<<"0x0"<<std::hex<<(int)bt<<" ";
		n++;
	}
}

void ClassFileGenerator::writeU16(uint16_t val)
//这里会遇到大端与小端优先问题，跨平台注意！！
{
#ifdef HARDWARE_IS_LITTLE_ENDIAN

	writeByte(((uint8_t *)&val)[1]);
	writeByte(((uint8_t *)&val)[0]);
#else
	#error please implements big endian!
#endif
}

void ClassFileGenerator::writeU32(uint32_t val)
//这里会遇到大端与小端优先问题，跨平台注意！！
{
#ifdef HARDWARE_IS_LITTLE_ENDIAN

	writeByte(((uint8_t *)&val)[3]);
	writeByte(((uint8_t *)&val)[2]);
	writeByte(((uint8_t *)&val)[1]);
	writeByte(((uint8_t *)&val)[0]);

#else
	#error please implements big endian!
#endif
}

void ClassFileGenerator::writeString(const std::string& str)
{
	for(size_t i=0;i<str.length();i++)
		writeByte(str[i]);
}

void ClassFileGenerator::writeString(char *str)
{
	int i=0;
	while(str[i])
		writeByte(str[i++]);
}


Field::Field(ClassFileGenerator* _bc,uint16_t nameIdx, uint16_t descripIdx):
access_flags(0x0001),name_index(nameIdx),descriptor_index(descripIdx),bc(_bc)
{
}

void Field::addAttribute(Attribute* attr)
{
	attributes.push_back(attr);

}

void Field::output()
{
	bc->writeU16(access_flags);
	bc->writeU16(name_index);
	bc->writeU16(descriptor_index);
	if(attributes.empty())
	{
		bc->writeU16(0);
		return;
	}else
	{
		assert(attributes.size()==1);
		bc->writeU16(1);
		attributes[0]->output();
	}
}


Method::Method(ClassFileGenerator* _bc,uint16_t nameIdx, uint16_t descripIdx,bool isStatic):
access_flags(0x0001),name_index(nameIdx),descriptor_index(descripIdx),bc(_bc)
{
	code=new CodeAttribute(bc);
	attributes.push_back(code);
	if(isStatic)
		access_flags|=0x0008;
}

void Method::addCode(uint8_t op)
{
	code->addCode(op);
}
void Method::addCode(uint8_t op,int16_t offset)
{
	code->addCode(op,offset);
}

int16_t Method::getOffset()
{
	return code->getOffset();
}

void Method::output()
{
	bc->writeU16(access_flags);
	bc->writeU16(name_index);
	bc->writeU16(descriptor_index);
	assert(attributes.size()==1);
	bc->writeU16(1);
	code->output();
}


MethodPara::MethodPara(std::string name_,std::string desc_,int count)
{
	name=name_;
	desc=desc_;
	paraCount=count;
}

MethodPara::MethodPara()
{
}

VarPara::VarPara(std::string name_,std::string desc_)
{
	name=name_;
	desc=desc_;
}

VarPara::VarPara()
{
}
