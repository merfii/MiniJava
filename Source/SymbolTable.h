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

/*�洢�ࡢ������
��Ա�����ͳ�Ա�������������ڣ�����͵���ʱ��ͬ
����ʱֻ��Ҫ�Ե�ǰ����ӳ�Ա���ż���
����ʱ��Ҫ���д����Լ��
����Ϊ�˷�ֹ�ظ��洢������ǰ�õĶ�����ȥ�Ҷ�Ӧ��ĳ��������ж�������
��ǰ��Ĳ���Ҫǰ����Ҳ��Ĭ�ϲ���
�����Ļ����ű�ֻ��Ҫ�洢�������������������ֲ�����
����Ҫ�洢��Ա�����������������ϵ(�ɳ����ظ���)
*/
enum SYMBOL_TYPE
{
	SYM_NONE,		//������
	SYM_CLASS,		//����
	SYM_INSTANCE,	//��������
	SYM_INT,		//����
	SYM_BOOLEAN,	//������
	SYM_ARRAY		//����
	//SYM_METHOD,		miniJava��֧�ַǳ�Ա����
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

	//��getType��,�������ڲ��Զ������������ſ��Ի�ȡ��ǰ��Ϣ
	enum SYMBOL_TYPE getType(const std::string& _sym,ClassFileGenerator *cfg);

	//����getType��ʹ�� �Ƿ��ǳ�Ա����
	bool isInstanceVar();

	//����getType��ʹ��
	ClassFileGenerator* getClassGenerator();
	InstanceDesc& getInstance();
	IntDesc& getFieldInt();
	BoolDesc& getFieldBoolean();
	ArrayDesc& getFieldArray();

	bool checkStackTop();
	//������������.class������
	//std::vector<SymMap>& getSymbolStack();

private:
	std::vector<SymMap> symbolStack;		//�����map���� ʵ��ջ

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
	uint16_t fieldRefIdx;	//������Ա����ʹ��
};

class IntDesc
{
public:
	IntDesc(uint8_t addr_,int line_);
	IntDesc(uint16_t fieldRefIdx_);
	uint8_t addr;
	int line;
	uint16_t fieldRefIdx;	//������Ա����ʹ��
};

class BoolDesc
{
public:
	BoolDesc(uint8_t addr_,int line_);
	BoolDesc(uint16_t fieldRefIdx_);

	uint8_t addr;
	int line;
	uint16_t fieldRefIdx;	//������Ա����ʹ��
};

class ArrayDesc
{
public:
	ArrayDesc(uint8_t addr_,int line_);
	ArrayDesc(uint16_t fieldRefIdx_);

	uint8_t addr;
	int line;
	uint16_t fieldRefIdx;	//������Ա����ʹ��
};


#endif /* SYMBOLTABLE_H_ */
