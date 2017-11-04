#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <utility>

class SymDesc;
class InstanceDesc;
class IntDesc;
class BoolDesc;
class ArrayDesc;
class ClassFileGenerator;
typedef std::map<std::string,SymDesc> SymMap;

/*存储类、对象名
成员变量和成员函数的问题在于，定义和调用时不同
定义时只需要对当前类添加成员符号即可
调用时需要进行存在性检查
这里为了防止重复存储，利用前置的对象名去找对应类的常量池以判定存在性
当前类的不需要前置名也会默认查找
这样的话符号表只需要存储所有类名、对象名、局部变量
不需要存储成员变量对于类的依赖关系(由常量池负责)
*/
enum SYMBOL_TYPE
{
	SYM_NONE,		//不存在
	SYM_CLASS,		//类名
	SYM_INSTANCE,	//对象引用
	SYM_INT,		//整型
	SYM_BOOLEAN,	//布尔型
	SYM_ARRAY		//数组
	//SYM_METHOD,		miniJava不支持非成员方法
};

class SymbolTable
{

public:
	SymbolTable();
	~SymbolTable();
	void addClass(const std::string& className, ClassFileGenerator* generator);
	void addInstance(const std::string& instanceName, const InstanceDesc& insDesc);
	void addFieldInt(const std::string& fieldName, const IntDesc& intDesc);
	void addFieldBoolean(const std::string& fieldName, const BoolDesc& boolDesc);
	void addFieldArray(const std::string& fieldName,const ArrayDesc& arrayDesc);

	void blockPush();
	void blockPop();
	bool blockExisted(std::string sym);

	//用getType后,索引号内部自动锁定，紧接着可以获取当前信息
	enum SYMBOL_TYPE getType(const std::string& _sym,ClassFileGenerator *cfg);

	//紧跟getType后使用 是否是成员变量
	bool isInstanceVar();

	//紧跟getType后使用
	ClassFileGenerator* getClassGenerator();
	InstanceDesc& getInstance();
	IntDesc& getFieldInt();
	BoolDesc& getFieldBoolean();
	ArrayDesc& getFieldArray();

	bool checkStackTop();
	//用来遍历所有.class生成器
	//std::vector<SymMap>& getSymbolStack();

private:
	std::vector<SymMap> symbolStack;		//存符号map数组 实现栈

	std::vector<ClassFileGenerator*> generators_;
	std::vector<InstanceDesc> insDesc_;
	std::vector<IntDesc> intDesc_;
	std::vector<BoolDesc> boolDesc_;
	std::vector<ArrayDesc> arrayDesc_;

	int lastIndex;
	bool isInstance;
};

class SymDesc
{
public:
	SymDesc();
	SymDesc(enum SYMBOL_TYPE t,int i);
	enum SYMBOL_TYPE type;
	int index;
};

class InstanceDesc
{
public:
	InstanceDesc(std::string className_,uint8_t addr_,int line_);
	InstanceDesc(std::string className_,uint16_t fieldRefIdx_);

	std::string className;
	uint8_t addr;
	int line;
	uint16_t fieldRefIdx;	//仅供成员变量使用
};

class IntDesc
{
public:
	IntDesc(uint8_t addr_,int line_);
	IntDesc(uint16_t fieldRefIdx_);
	uint8_t addr;
	int line;
	uint16_t fieldRefIdx;	//仅供成员变量使用
};

class BoolDesc
{
public:
	BoolDesc(uint8_t addr_,int line_);
	BoolDesc(uint16_t fieldRefIdx_);

	uint8_t addr;
	int line;
	uint16_t fieldRefIdx;	//仅供成员变量使用
};

class ArrayDesc
{
public:
	ArrayDesc(uint8_t addr_,int line_);
	ArrayDesc(uint16_t fieldRefIdx_);

	uint8_t addr;
	int line;
	uint16_t fieldRefIdx;	//仅供成员变量使用
};


#endif /* SYMBOLTABLE_H_ */
