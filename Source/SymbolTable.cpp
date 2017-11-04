#include "SymbolTable.h"
#include "ClassFileGenerator.h"
#include <cassert>

SymbolTable::SymbolTable()
{
	symbolStack.push_back(SymMap());
	lastIndex=0;
}


SymbolTable::~SymbolTable()
{
	//for(std::vector<SymMap*>::iterator it=symbolStack.begin();it!=symbolStack.end();it++)
		//delete (*it);
}


#include <iostream>
void SymbolTable::addClass(const std::string& className,ClassFileGenerator* generator)
{
	SymMap &symbolTable=symbolStack.at(symbolStack.size()-1);
	assert(symbolTable.count(className)==0);
	generators_.push_back(generator);
	symbolTable[className]=SymDesc(SYM_CLASS, generators_.size()-1);
}

void SymbolTable::addInstance(const std::string& instanceName,const InstanceDesc& insDesc)
{
	SymMap &symbolTable=symbolStack.at(symbolStack.size()-1);
	assert(symbolTable.count(instanceName)==0);
	insDesc_.push_back(insDesc);
	symbolTable[instanceName]=SymDesc(SYM_INSTANCE, insDesc_.size()-1);

}


void SymbolTable::addFieldInt(const std::string& fieldName, const IntDesc& intDesc)
{
	SymMap &symbolTable=symbolStack.at(symbolStack.size()-1);
	assert(symbolTable.count(fieldName)==0);
	intDesc_.push_back(intDesc);
	symbolTable[fieldName]=SymDesc(SYM_INT, intDesc_.size()-1);

}


void SymbolTable::addFieldBoolean(const std::string& fieldName, const BoolDesc& boolDesc)
{
	SymMap &symbolTable=symbolStack.at(symbolStack.size()-1);
	assert(symbolTable.count(fieldName)==0);
	boolDesc_.push_back(boolDesc);
	symbolTable[fieldName]=SymDesc(SYM_BOOLEAN, boolDesc_.size()-1);
}


void SymbolTable::addFieldArray(const std::string& fieldName,const ArrayDesc& arrayDesc)
{
	SymMap &symbolTable=symbolStack.at(symbolStack.size()-1);
	assert(symbolTable.count(fieldName)==0);
	arrayDesc_.push_back(arrayDesc);
	symbolTable[fieldName]=SymDesc(SYM_ARRAY, arrayDesc_.size()-1);
}

void SymbolTable::blockPush()
{
	symbolStack.push_back(SymMap());
}

void SymbolTable::blockPop()
{
	assert(symbolStack.size()>0);
	symbolStack.resize(symbolStack.size()-1);
}

bool SymbolTable::blockExisted(std::string sym)
{
	SymMap &symbolTable=symbolStack.at(symbolStack.size()-1);
	return symbolTable.count(sym)>0;
}


enum SYMBOL_TYPE SymbolTable::getType(const std::string& _sym,ClassFileGenerator *cfg)
{
#ifdef _DEBUG
	std::cout<<"\t\t\t##Find \""<<_sym<<"\" in symbol table"<<std::endl;
#endif
	for(size_t i=symbolStack.size();i>0;i--)
	{
		if(symbolStack.at(i-1).count(_sym))
		{
			//用getType后,索引号内部自动锁定，紧接着可以获取当前信息
			lastIndex=(symbolStack.at(i-1))[_sym].index;
			if(i==2)
				isInstance=true;
			else
				isInstance=false;

			return (symbolStack.at(i-1))[_sym].type;
		}
	}

	VarPara vp;
	if(cfg)
	{
		vp=cfg->getVarDecl(_sym);
		if(vp.name.empty())
		{
			isInstance=false;
			return SYM_NONE;
		}
		else
		{
			isInstance=true;
			uint16_t fieldref=cfg->newFieldrefConstant(cfg->getThisIdx(),vp.name,vp.desc);
			SymMap &symbolt=symbolStack.at(1);
			switch(vp.desc[0])
			{
				case 'I':
					intDesc_.push_back(IntDesc(fieldref));
					symbolt[vp.name]=SymDesc(SYM_INT, intDesc_.size()-1);
					lastIndex=(symbolStack.at(1))[vp.name].index;
					return SYM_INT;

				case 'Z':
					boolDesc_.push_back(BoolDesc(fieldref));
					symbolt[vp.name]=SymDesc(SYM_BOOLEAN, boolDesc_.size()-1);
					lastIndex=(symbolStack.at(symbolStack.size()-1))[vp.name].index;
					return SYM_BOOLEAN;

				case '[':
					arrayDesc_.push_back(ArrayDesc(fieldref));
					symbolt[vp.name]=SymDesc(SYM_ARRAY, arrayDesc_.size()-1);
					lastIndex=(symbolStack.at(symbolStack.size()-1))[vp.name].index;
					return SYM_ARRAY;

				case 'L':
					insDesc_.push_back(InstanceDesc(vp.desc.substr(1,vp.desc.length()-2),fieldref));
					symbolt[vp.name]=SymDesc(SYM_INSTANCE, insDesc_.size()-1);
					lastIndex=(symbolStack.at(symbolStack.size()-1))[vp.name].index;
					return SYM_INSTANCE;
			}
		}

	}

	return SYM_NONE;
}

bool SymbolTable::isInstanceVar()
{
	return isInstance;
}


ClassFileGenerator* SymbolTable::getClassGenerator()
{
	return generators_.at(lastIndex);
}


InstanceDesc& SymbolTable::getInstance()
{
	return insDesc_.at(lastIndex);
}


IntDesc& SymbolTable::getFieldInt()
{
	return intDesc_.at(lastIndex);
}


BoolDesc& SymbolTable::getFieldBoolean()
{
	return boolDesc_.at(lastIndex);
}


ArrayDesc& SymbolTable::getFieldArray()
{
	return arrayDesc_.at(lastIndex);
}

#include <iostream>
bool SymbolTable::checkStackTop()
{
	assert(symbolStack.size()==1);
	//扫描完成后，符号栈最外一层应该只有类名
	for(SymMap::iterator it=symbolStack[0].begin();it!=symbolStack[0].end();it++)
	{
		if(it->second.type!=SYM_CLASS)
		{
			std::cout<<"The most outer symbol is not SYM_CLASS. It's "<<it->second.type<<std::endl;
			return false;
		}
	}
	return true;
}

SymDesc::SymDesc(enum SYMBOL_TYPE t,int i):
type(t),index(i)
{
}

SymDesc::SymDesc():
type(SYM_NONE),index(0)
{

}


ArrayDesc::ArrayDesc(uint8_t addr_,int line_):
addr(addr_),line(line_),fieldRefIdx(0)
{

}

ArrayDesc::ArrayDesc(uint16_t fieldRefIdx_):
addr(0),fieldRefIdx(fieldRefIdx_)
{

}

BoolDesc::BoolDesc(uint8_t addr_,int line_):
addr(addr_),line(line_),fieldRefIdx(0)
{

}

BoolDesc::BoolDesc(uint16_t fieldRefIdx_):
addr(0),fieldRefIdx(fieldRefIdx_)
{

}

IntDesc::IntDesc(uint8_t addr_,int line_):
addr(addr_),line(line_),fieldRefIdx(0)
{

}

IntDesc::IntDesc(uint16_t fieldRefIdx_):
addr(0),fieldRefIdx(fieldRefIdx_)
{
}

InstanceDesc::InstanceDesc(std::string className_,uint8_t addr_,int line_):
className(className_),addr(addr_),line(line_),fieldRefIdx(0)
{

}
InstanceDesc::InstanceDesc(std::string className_,uint16_t fieldRefIdx_):
className(className_),addr(0),fieldRefIdx(fieldRefIdx_)
{

}


