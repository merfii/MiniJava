#include "GrammarAnalyzer.h"
#include "InstructionSet.h"


void GrammarAnalyzer::doClassDeclaration()
{
	DEBUG_BEGIN(ClassDeclaration);

	NEXTSYM;
	if(word.getSeriNo()!=Word::ID_STRING)
		errorAndJumpout(SyntaxError("Illegal identifier"+word.getMemoniStr(),LINENUMBER));
	std::string className=word.getString();
	std::string superName;
	curClass=new ClassFileGenerator(className,false);
	generators.push_back(curClass);
	symbolTable.addClass(className,curClass);

	NEXTSYM;
	if(word.getSeriNo()==Word::EXTENDS)
	{
		//继承类
		NEXTSYM;
		superName=word.getString();
		if(symbolTable.getType(superName,NULL)!=SYM_CLASS)
		{
			errorAndContinue(SyntaxError("Undefined super class",LINENUMBER));
		}else
			curClass->setSuperClass(symbolTable.getClassGenerator());
		NEXTSYM;
	}else
		superName="java/lang/Object";
	uint16_t superIdx=curClass->newClassConstant(superName);

	if(word.getSeriNo()==Word::LEFT_BLOCK)
	{
		symbolTable.blockPush();

		//构造函数
		uint16_t methodInitIdx=curClass->newMethodrefConstant(superIdx,string("<init>"),string("()V"));
		curClass->setSuperClass(superIdx);
		curClass->beginMethod(string("<init>"),string("()V"),false);
			curClass->addCode(aload_0);	//将当前类句柄从local variable 压入operand
			curClass->addCode(invokespecial);	//调用继承的构造函数
			curClass->addCodeU16(methodInitIdx);
			curClass->addCode(vreturn);
		curClass->endMethod();

		//this变量名 基本作用是利用其存在性标识类指针
		/*
		string desc;
		desc="L";desc+=className;desc.push_back(';');
		curClass->newField(string("this"),desc,0);
		symbolTable.addInstance(string("this"),InstanceDesc(className,-1));
*/
	}else
	{
		errorAndJumpout(SyntaxError("Missing {",LINENUMBER));
	}
	NEXTSYM;
	bool var=true;
	while(word.getSeriNo()!=Word::EOF)
	{
		if(word.getSeriNo()==Word::PUBLIC)
		{
			var=false;
			try{
				doMethodDeclaration();
			}catch(SyntaxError& e)
			{
				while(word.getSeriNo()!=Word::SEMICOLON)
					NEXTSYM;
				NEXTSYM;
			}
		}
		else if(word.getSeriNo()==Word::RIGHT_BLOCK)
			break;
		else if(var)
			doInstanceVarDeclar();
		else
			NEXTSYM;
	}

	if(word.getSeriNo()==Word::EOF)
		errorAndJumpout(SyntaxError("Expect } before end",LINENUMBER));
	if(word.getSeriNo()==Word::RIGHT_BLOCK)
	{
		symbolTable.blockPop();
		NEXTSYM;
	}

	DEBUG_END(ClassDeclaration);
	if(_debug)std::cout<<std::endl;
}


void GrammarAnalyzer::doMethodDeclaration()
{
	DEBUG_BEGIN(MethodDeclaration);

	assert(word.getSeriNo()==Word::PUBLIC);
	localVarCount=1;
	string methodName,retType,desc;
	int i;

	NEXTSYM;
	switch(word.getSeriNo())
	{
	case Word::INT:
		NEXTSYM;
		if(word.getSeriNo()==Word::LEFT_SQUARE)
		{
			//数组
			NEXTSYM;
			if(word.getSeriNo()!=Word::RIGHT_SQUARE)
				errorAndContinue(SyntaxError("Mising ] ",LINENUMBER));
			else
				NEXTSYM;

			retType=string("[I");
		}else
		{
			//整型变量
			retType=string("I");
		}
		break;
	case Word::BOOLEAN:
		NEXTSYM;
		retType=string("Z");
		break;
	case Word::ID_STRING:
		NEXTSYM;
		retType=string("L")+word.getString();
		retType.append(";");
		break;

	default:
		errorAndContinue(SyntaxError("Need return type declearation ",LINENUMBER));
		i=15;
		while(i-->0)
		{
			if(word.getSeriNo()==Word::PUBLIC)
			{
				return;
			}
			else if(word.getSeriNo()==Word::LEFT_BLOCK)
			{
				doVarAndStatement();
				while(word.getSeriNo()!=Word::RIGHT_BLOCK)
					NEXTSYM;
				NEXTSYM;
				return;
			}
			NEXTSYM;
		}
	}

	methodName=word.getString();
	//判断重复
	NEXTSYM;
	if(word.getSeriNo()==Word::LEFT_BRACKET)
		NEXTSYM;
	else
		errorAndJumpout(SyntaxError("Missing ( ",LINENUMBER));

	symbolTable.blockPush();
	desc=doFormalParameterList();
	int count;
	count=desc[desc.size()-1];
	desc.resize(desc.size()-1);
	desc+=retType;
	if(word.getSeriNo()==Word::RIGHT_BRACKET)
		NEXTSYM;
	else
		errorAndJumpout(SyntaxError("Missing ) ",LINENUMBER));

	if(word.getSeriNo()==Word::LEFT_BLOCK)
			NEXTSYM;
	else
		errorAndJumpout(SyntaxError("Missing {",LINENUMBER));

	curClass->beginMethod(methodName,desc,count,false);


	try{
		doVarAndStatement();
	}catch(SyntaxError& e)
	{
		while(word.getSeriNo()!=Word::SEMICOLON&&
				word.getSeriNo()!=Word::RIGHT_BLOCK&&
				word.getSeriNo()!=Word::RETURN)
				NEXTSYM;
	}
	if(word.getSeriNo()==Word::SEMICOLON)
		NEXTSYM;

	while(word.getSeriNo()!=Word::RIGHT_BLOCK&&
			word.getSeriNo()!=Word::RETURN)
	{
		try{
			doStatement();
		}catch(SyntaxError& e)
		{
			while(word.getSeriNo()!=Word::SEMICOLON&&
					word.getSeriNo()!=Word::RIGHT_BLOCK&&
					word.getSeriNo()!=Word::RETURN)
					NEXTSYM;
		}catch(TypeError& e)
		{
			while(word.getSeriNo()!=Word::SEMICOLON&&
					word.getSeriNo()!=Word::RIGHT_BLOCK&&
					word.getSeriNo()!=Word::RETURN)
					NEXTSYM;
		}

		if(word.getSeriNo()==Word::SEMICOLON)
			NEXTSYM;
	}

	if(word.getSeriNo()==Word::RETURN)
	{
		NEXTSYM;
	}
	else
	{
			errorAndContinue(SyntaxError("Expect return",LINENUMBER));
			while(word.getSeriNo()!=Word::RIGHT_BLOCK)
				NEXTSYM;
			NEXTSYM;
			return;
	}

	doExpression();
	curClass->addCode(ireturn);

	if(word.getSeriNo()==Word::SEMICOLON)
		NEXTSYM;
	else
		errorAndContinue(SyntaxError("Missing ;",LINENUMBER));

	while(word.getSeriNo()!=Word::RIGHT_BLOCK)
		NEXTSYM;
	NEXTSYM;

	symbolTable.blockPop();
	curClass->endMethod();

	DEBUG_END(MethodDeclaration);
	if(_debug)std::cout<<std::endl;
}


std::string GrammarAnalyzer::doFormalParameterList()
{
	DEBUG_BEGIN(FormalParameterList);

	if(word.getSeriNo()==Word::RIGHT_BRACKET)
	{
		DEBUG_END(FormalParameterList);
		string desc("()");
		desc.push_back(0);
		return desc;
	}
	else
	{
		char count=0;
		string paraDesc("(");
		while(word.getSeriNo()!=Word::RIGHT_BRACKET &&
				word.getSeriNo()!=Word::RIGHT_BLOCK )
		{
			paraDesc+=doFormalParameter();
			count++;
			if(word.getSeriNo()!=Word::COMMA)
			{
				while(word.getSeriNo()!=Word::RIGHT_BRACKET &&
						word.getSeriNo()!=Word::RIGHT_BLOCK &&
						word.getSeriNo()!=Word::COMMA)
					NEXTSYM;

			}else
				NEXTSYM;
		}
		paraDesc+=")";
		paraDesc.push_back(count);

		DEBUG_END(FormalParameterList);
		return paraDesc;
	}
}

std::string GrammarAnalyzer::doFormalParameter()
{
	DEBUG_BEGIN(FormalParameter);
		string ret=__doFormalParameter();
	DEBUG_END(FormalParameter);
	return ret;
}


std::string GrammarAnalyzer::__doFormalParameter()
{

	string fieldName;
	switch(word.getSeriNo())
	{
	case Word::INT:
		NEXTSYM;
		if(word.getSeriNo()==Word::LEFT_SQUARE)
		{
			//数组
			NEXTSYM;
			if(word.getSeriNo()!=Word::RIGHT_SQUARE)
				errorAndContinue(SyntaxError("Mising ] ",LINENUMBER));
			else
				NEXTSYM;
			fieldName=word.getString();
			if(symbolTable.blockExisted(fieldName))
				errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));
			symbolTable.addFieldArray(fieldName,ArrayDesc(localVarCount++,LINENUMBER));
			NEXTSYM;
			return string("[I");
		}else if(word.getSeriNo()==Word::ID_STRING)
		{
			//整型变量
			fieldName=word.getString();
			if(symbolTable.blockExisted(fieldName))
							errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));
			symbolTable.addFieldInt(fieldName,IntDesc(localVarCount++,LINENUMBER));
			NEXTSYM;
			return string("I");
		}else
		{
			errorAndContinue(SyntaxError("SyntaxError",LINENUMBER));
			while(word.getSeriNo()!=Word::COMMA && word.getSeriNo()!=Word::RIGHT_BRACKET)
				NEXTSYM;
			return string();
		}

	case Word::BOOLEAN:
		NEXTSYM;
		fieldName=word.getString();
		if(symbolTable.blockExisted(fieldName))
						errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));
		symbolTable.addFieldBoolean(fieldName,BoolDesc(localVarCount++,LINENUMBER));
		NEXTSYM;
		return string("Z");

	case Word::ID_STRING:
	{
		//这里本来需要进行类型存在性检查
		string cname;
		cname=word.getString();
		if(symbolTable.getType(cname,curClass)!=Word::CLASS)
			errorAndContinue(SyntaxError("Expect class name",LINENUMBER));
		NEXTSYM;
		if(word.getSeriNo()!=Word::ID_STRING)
		{
			errorAndContinue(SyntaxError("Expect field name",LINENUMBER));
			while(word.getSeriNo()!=Word::COMMA && word.getSeriNo()!=Word::RIGHT_BRACKET)
				NEXTSYM;
			return string();
		}
		if(symbolTable.blockExisted(fieldName))
			errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));
		fieldName=word.getString();
		string fieldDesc=string("L<")+cname;
		fieldDesc.append(">;");
			NEXTSYM;
		return fieldDesc;
	}
	default:
		return string();
	}


	return string();
}



void GrammarAnalyzer::doVarDeclar()
{
	DEBUG_BEGIN(VarDeclar);
	string fieldName,typeName;
	switch(word.getSeriNo())
	{
		case Word::INT:
			NEXTSYM;
			if(word.getSeriNo()==Word::LEFT_SQUARE)
			{
				//数组
				NEXTSYM;
				if(word.getSeriNo()!=Word::RIGHT_SQUARE)
					errorAndContinue(SyntaxError("Mising ] ",LINENUMBER));
				else
					NEXTSYM;

				if(word.getSeriNo()==Word::ID_STRING)
					fieldName=word.getString();
				else
					errorAndJumpout(SyntaxError("Mising field name",LINENUMBER));

				if(symbolTable.blockExisted(fieldName))
					errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));
				else
					symbolTable.addFieldArray(fieldName,ArrayDesc(localVarCount++,LINENUMBER));

				NEXTSYM;

				if(word.getSeriNo()==Word::SEMICOLON)
					NEXTSYM;
				else
					errorAndJumpout(SyntaxError("Mising ; ",LINENUMBER));

			}else if(word.getSeriNo()==Word::ID_STRING)
			{
				//整型变量
				fieldName=word.getString();
				if(symbolTable.blockExisted(fieldName))
					errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));
				else
				{
					symbolTable.addFieldInt(fieldName,IntDesc(localVarCount++,LINENUMBER));
				}
					NEXTSYM;

				if(word.getSeriNo()==Word::SEMICOLON)
					NEXTSYM;
				else
					errorAndJumpout(SyntaxError("Expect ; ",LINENUMBER));

			}else
				errorAndJumpout(SyntaxError("Illegal defination ",LINENUMBER));
			break;

		case Word::BOOLEAN:
			//布尔变量
			NEXTSYM;
			if(word.getSeriNo()==Word::ID_STRING)
			{

				fieldName=word.getString();
				if(symbolTable.blockExisted(fieldName))
					errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));
				else
				{
					symbolTable.addFieldBoolean(fieldName,BoolDesc(localVarCount++,LINENUMBER));
				}
					NEXTSYM;

				if(word.getSeriNo()==Word::SEMICOLON)
					NEXTSYM;
				else
					errorAndJumpout(SyntaxError("Mising ; ",LINENUMBER));
			}else
			{
				errorAndJumpout(SyntaxError("Illegal defination ",LINENUMBER));
			}
			break;

		case Word::ID_STRING:

			typeName=word.getString();
			NEXTSYM;

			if(word.getSeriNo()!=Word::ID_STRING)
			{
				errorAndJumpout(SyntaxError("Illeagan defination",LINENUMBER));
			}else
			{
				fieldName=word.getString();
				doVarDeclar(fieldName);
			}
			break;

		default:
			assert(word.getSeriNo()==Word::VOID);
	}
	DEBUG_END(VarDeclar);


}

void GrammarAnalyzer::doVarDeclar(string type)
{
	DEBUG_BEGIN(VarDeclarString);
	assert(word.getSeriNo()==Word::ID_STRING);
	string fieldName;
	fieldName=word.getString();

	if(symbolTable.blockExisted(fieldName))
		errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));
	else
		symbolTable.addInstance(fieldName,InstanceDesc(type,localVarCount++,LINENUMBER));

	NEXTSYM;
	if(word.getSeriNo()==Word::SEMICOLON)
	{
		NEXTSYM;
	}else
		errorAndJumpout(SyntaxError("Missing ; ",LINENUMBER));
	DEBUG_END(VarDeclarString);
}

void GrammarAnalyzer::doInstanceVarDeclar()
{
	DEBUG_BEGIN(InstanceVarDeclar);

	std::string fieldName,fieldDesc;
	uint16_t fieldref;
	switch(word.getSeriNo())
	{
	case Word::INT:
		NEXTSYM;
		if(word.getSeriNo()==Word::LEFT_SQUARE)
		{
			//数组
			NEXTSYM;
			if(word.getSeriNo()!=Word::RIGHT_SQUARE)
				errorAndContinue(SyntaxError("Mising ] ",LINENUMBER));
			else
				NEXTSYM;

			fieldName=word.getString();
			if(symbolTable.blockExisted(fieldName))
				errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));
			fieldDesc=string("[I");
			curClass->newField(fieldName,fieldDesc,0);
			fieldref=curClass->newFieldrefConstant(curClass->getThisIdx(),fieldName,fieldDesc);
			symbolTable.addFieldArray(fieldName,ArrayDesc(fieldref));
		}else
		{
			//整型变量
			fieldName=word.getString();
			if(symbolTable.blockExisted(fieldName))
				errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));

			fieldDesc=string("I");
			curClass->newField(fieldName,fieldDesc,0);
			fieldref=curClass->newFieldrefConstant(curClass->getThisIdx(),fieldName,fieldDesc);
			symbolTable.addFieldInt(fieldName,IntDesc(fieldref));
		}
		break;
	case Word::BOOLEAN:
		NEXTSYM;
		fieldName=word.getString();
		if(symbolTable.blockExisted(fieldName))
			errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));
		fieldDesc=string("Z");
		curClass->newField(fieldName,fieldDesc,0);
		fieldref=curClass->newFieldrefConstant(curClass->getThisIdx(),fieldName,fieldDesc);
		symbolTable.addFieldBoolean(fieldName,BoolDesc(fieldref));
		break;
	case Word::ID_STRING:
		string cname=word.getString();
		//类型存在性检查
		if(symbolTable.getType(cname,curClass)!=SYM_CLASS)
			errorAndContinue(SyntaxError("Class undefined",LINENUMBER));
		NEXTSYM;
		if(word.getSeriNo()!=Word::ID_STRING)
			errorAndJumpout(SyntaxError("Expect field name",LINENUMBER));
		if(symbolTable.blockExisted(fieldName))
			errorAndContinue(SyntaxError("Redundant field name "+fieldName,LINENUMBER));
		fieldName=word.getString();
		fieldDesc=string("L")+cname;
		fieldDesc.append(";");
		curClass->newField(fieldName,fieldDesc,0);
		fieldref=curClass->newFieldrefConstant(curClass->getThisIdx(),fieldName,fieldDesc);
		symbolTable.addInstance(fieldName,InstanceDesc(cname,fieldref));
		break;
	}
	NEXTSYM;
	if(word.getSeriNo()!=Word::SEMICOLON)
		errorAndContinue(SyntaxError("Mising ; ",LINENUMBER));
	else
		NEXTSYM;
	DEBUG_END(InstanceVarDeclar);
}


