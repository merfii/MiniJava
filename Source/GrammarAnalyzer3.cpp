#include "GrammarAnalyzer.h"
#include "InstructionSet.h"



//封装 为了从不同路径都有end显示
enum SYMBOL_TYPE GrammarAnalyzer::doExpression()
{
	DEBUG_BEGIN(Expression);
	enum SYMBOL_TYPE ret=__doExpression();
	DEBUG_END(Expression);
	return ret;
}

/*
Expression::=
  AndExpression
| CompareExpression
| PlusExpression
| MinusExpression
| TimesExpression
| ArrayLookup
| ArrayLength
| MessageSend
| PrimaryExpression
*/

SYMBOL_TYPE GrammarAnalyzer::__doExpression()
{
	SYMBOL_TYPE t1=doPrimaryExpression();

	//这里需要求PrimaryExpression的follow集
/*
Follow(PrimatyExpression)=
&&
<
+
-
*
[
]
.
Follow(Expression)
*/
	switch(word.getSeriNo())
	{
	case Word::AND:	//短路与
	{
		int16_t offsetA,offsetB,offsetGoto,offset0;
		if(t1!=SYM_BOOLEAN)
			errorAndContinue(TypeError("Expect boolean value",LINENUMBER));
		curClass->addCode(ifeq);
		curClass->addCode(0x00);
		offsetA=curClass->getOffset();
		curClass->addCode(0x00);

		NEXTSYM;
		if(doPrimaryExpression()!=SYM_BOOLEAN)
			errorAndContinue(TypeError("Expect boolean value",LINENUMBER));

		curClass->addCode(ifeq);
		curClass->addCode(0x00);
		offsetB=curClass->getOffset();
		curClass->addCode(0x00);

		curClass->addCode(iconst_1);
		curClass->addCode(igoto);
		curClass->addCode(0x00);
		offsetGoto=curClass->getOffset();
		curClass->addCode(0x00);
		curClass->addCode(iconst_0);
		offset0=curClass->getOffset();

		//回写地址
		curClass->addCodeU16((uint16_t)(offset0+1-offsetA),offsetA);
		curClass->addCodeU16((uint16_t)(offset0+1-offsetB),offsetB);
		curClass->addCodeU16(4,offsetGoto);
		return SYM_BOOLEAN;
	}
		break;

	case Word::COMPARE:	// 只有<
		if(t1!=SYM_INT)
			errorAndContinue(TypeError("Expect int value",LINENUMBER));
		NEXTSYM;
		if(doPrimaryExpression()!=SYM_INT)
			errorAndContinue(TypeError("Expect int value",LINENUMBER));

		curClass->addCode(if_icmpge);
		curClass->addCodeU16(7);
		curClass->addCode(iconst_1);
		curClass->addCode(igoto);
		curClass->addCodeU16(4);
		curClass->addCode(iconst_0);
		return SYM_BOOLEAN;

	case Word::PLUS:
		if(t1!=SYM_INT)
			errorAndContinue(TypeError("Expect int value",LINENUMBER));
		NEXTSYM;
		if(doPrimaryExpression()!=SYM_INT)
			errorAndContinue(TypeError("Expect int value",LINENUMBER));

		curClass->addCode(iadd);
		return SYM_INT;

	case Word::MINUS:
		if(t1!=SYM_INT)
			errorAndContinue(TypeError("Expect int value",LINENUMBER));
		NEXTSYM;
		if(doPrimaryExpression()!=SYM_INT)
			errorAndContinue(TypeError("Expect int value",LINENUMBER));

		curClass->addCode(isub);
		return SYM_INT;

	case Word::TIMES:
		if(t1!=SYM_INT)
			errorAndContinue(TypeError("Expect int value",LINENUMBER));
		NEXTSYM;
		if(doPrimaryExpression()!=SYM_INT)
			errorAndContinue(TypeError("Expect int value",LINENUMBER));

		curClass->addCode(imul);
		return SYM_INT;

	case Word::LEFT_SQUARE:
		if(t1!=SYM_ARRAY)
			errorAndContinue(TypeError("Expect array type",LINENUMBER));
		NEXTSYM;
		if(doPrimaryExpression()!=SYM_INT)
			errorAndContinue(TypeError("Expect int value",LINENUMBER));
		curClass->addCode(iaload);
		if(word.getSeriNo()!=Word::RIGHT_SQUARE)
			errorAndContinue(TypeError("Expect ] ",LINENUMBER));
		else
			NEXTSYM;

		return SYM_INT;

	case Word::DOT:
		if(t1==SYM_INSTANCE&&instanceRef.has)
		{
			//函数调用 两种情况 instance.func()或(new ClassName()).func()
			NEXTSYM;
			if(word.getSeriNo()!=Word::ID_STRING)
				 errorAndJumpout(TypeError("Expect MessageSend operation ",LINENUMBER));
			if(symbolTable.getType(instanceRef.className,curClass)==SYM_CLASS)
			{
				//该类已定义
				MethodPara para=symbolTable.getClassGenerator()->getMethodDecl(word.getString());
				if(para.name.empty())
					errorAndJumpout(TypeError("Undecleared function",LINENUMBER));

				uint16_t classIdx=curClass->newClassConstant(instanceRef.className);
				uint16_t methodrefIdx=curClass->newMethodrefConstant(
						classIdx,
						word.getString(),
						para.desc);
				//参数

				NEXTSYM;
				if(word.getSeriNo()!=Word::LEFT_BRACKET)
					errorAndJumpout(SyntaxError("Expect (",LINENUMBER));
				else
					NEXTSYM;

				int paraCount;
				if(word.getSeriNo()==Word::RIGHT_BRACKET)
				{
					paraCount=0;
				}
				else
				{
					paraCount=doExpressionList();
				}
				NEXTSYM;
				if(paraCount!=para.paraCount)
				{
					errorAndContinue(SyntaxError("Parameter mismatch",LINENUMBER));
				}
				curClass->addCode(invokevirtual);
				curClass->addCodeU16(methodrefIdx);

				size_t m=para.desc.find_last_of(')')+1;
				switch(para.desc[m])
				{
				case 'I':
					return SYM_INT;
				case 'Z':
					return SYM_BOOLEAN;
				case '[':
					return SYM_ARRAY;
				case 'L':
					return SYM_INSTANCE;
				}

			}else
			{
				//该类尚未定义，需要扫描参数列表以确定函数调用形式
				string methodName=word.getString();
				NEXTSYM;
				string desc=doScanParameter();
				uint16_t classIdx=curClass->newClassConstant(instanceRef.className);
				uint16_t methodrefIdx=curClass->newMethodrefConstant(
						classIdx,
						methodName,
						desc);
				curClass->addCode(invokevirtual);
				curClass->addCodeU16(methodrefIdx);
				return SYM_INT;
			}
		}else if(t1==SYM_ARRAY)
		{
			NEXTSYM;
			if(word.getSeriNo()==Word::LENGTH)
			{

				curClass->addCode(arraylength);
				NEXTSYM;
				return SYM_INT;
			}
			else
				errorAndJumpout(TypeError("Expect length operation ",LINENUMBER));

		}else
		{
			errorAndJumpout(TypeError(string("Can't not apply . operation"),LINENUMBER));
		}
		return SYM_INT;

	case Word::SEMICOLON:
	case Word::COMMA:
	case Word::RIGHT_SQUARE:
	case Word::RIGHT_BRACKET:

		return t1;
	}

/*
Follow(Expression)=
Follow(PrimatyExpression)
;
]
)
,
*/
	errorAndJumpout(SyntaxError("Illegal expression",LINENUMBER));
	return SYM_NONE;
}



enum SYMBOL_TYPE GrammarAnalyzer::doPrimaryExpression()
{
	DEBUG_BEGIN(PrimaryExpression);
	SYMBOL_TYPE ret=__doPrimaryExpression();
	DEBUG_END(PrimaryExpression);
	return ret;
}


/*
PrimaryExpression::=
 IntegerLiteral
|TrueLiteral
|FalseLiteral
|Identifier
|ThisExpression
|ArrayAllocationExpression
|AllocationExpression
|NotExpression
|BracketExpression
*/
enum SYMBOL_TYPE GrammarAnalyzer::__doPrimaryExpression()
{
	int tmp;
	SYMBOL_TYPE ret;
	string fieldName;
	switch(word.getSeriNo())
	{
	case Word::ID_INTEGER:
		tmp=static_cast<uint16_t>(word.getNum());
		curClass->addCode(sipush);
		curClass->addCodeU16(tmp);
		NEXTSYM;
		return SYM_INT;

	case Word::TRUE:
		curClass->addCode(iconst_1);
		NEXTSYM;
		return SYM_BOOLEAN;

	case Word::FALSE:
		curClass->addCode(iconst_0);
		NEXTSYM;
		return SYM_BOOLEAN;


	case Word::NEW:
		if(doAllocationExpression()==SYM_ARRAY)
		{
			//数组引用储存在操作栈顶
			return SYM_ARRAY;
		}else
		{
			//对象引用储存在操作栈顶
			return SYM_INSTANCE;
		}

	case Word::NOT:
		NEXTSYM;
		doExpression();
		curClass->addCode(ifgt);
		curClass->addCodeU16(+7);
		curClass->addCode(iconst_1);
		curClass->addCode(igoto);
		curClass->addCodeU16(4);
		curClass->addCode(iconst_0);
		return SYM_BOOLEAN;

	case Word::LEFT_BRACKET:
		NEXTSYM;
		ret=doExpression();
		if(word.getSeriNo()!=Word::RIGHT_BRACKET)
		{
			errorAndJumpout(SyntaxError("Syntax error, wish ) .",LINENUMBER));
			return ret;
		}
		NEXTSYM;
		return ret;

	case Word::THIS:
		instanceRef.has=true;
		instanceRef.name=string("this");
		instanceRef.className=curClass->getName();
		curClass->addCode(aload_0);
		NEXTSYM;
		return SYM_INSTANCE;

	case Word::ID_STRING:
		fieldName=word.getString();
		switch(symbolTable.getType(fieldName,curClass))
		{
			case SYM_INT:
				if(symbolTable.isInstanceVar())
				{
					//访问常量池获取成员变量
					curClass->addCode(aload_0);
					curClass->addCode(getfield);
					curClass->addCodeU16(symbolTable.getFieldInt().fieldRefIdx);
				}else
				{
					curClass->addCode(iload);
					curClass->addCode(symbolTable.getFieldInt().addr);
				}
				NEXTSYM;
				return SYM_INT;

			case SYM_BOOLEAN:
				if(symbolTable.isInstanceVar())
				{
					//访问常量池获取成员变量
					curClass->addCode(aload_0);
					curClass->addCode(getfield);
					curClass->addCodeU16(symbolTable.getFieldBoolean().fieldRefIdx);
				}else
				{
					curClass->addCode(iload);
					curClass->addCode(symbolTable.getFieldBoolean().addr);
				}
				NEXTSYM;
				return SYM_BOOLEAN;

			case SYM_INSTANCE:
				instanceRef.has=true;
				instanceRef.className=symbolTable.getInstance().className;
				instanceRef.name=fieldName;
				if(symbolTable.isInstanceVar())
				{
					//访问常量池获取成员变量
					curClass->addCode(aload_0);
					curClass->addCode(getfield);
					curClass->addCodeU16(symbolTable.getInstance().fieldRefIdx);
				}else
				{
					curClass->addCode(aload);
					curClass->addCode(symbolTable.getInstance().addr);
				}
				NEXTSYM;
				return SYM_INSTANCE;

			case SYM_ARRAY:
				if(symbolTable.isInstanceVar())
				{
					//访问常量池获取成员变量
					curClass->addCode(aload_0);
					curClass->addCode(getfield);
					curClass->addCodeU16(symbolTable.getFieldArray().fieldRefIdx);
				}else
				{
					curClass->addCode(aload);
					curClass->addCode(symbolTable.getFieldArray().addr);
				}
				NEXTSYM;
				return SYM_ARRAY;

			default:
				errorAndContinue(SyntaxError("Undefined variable "+word.getString(),LINENUMBER));
				NEXTSYM;
				return SYM_NONE;
		}

	default:
		errorAndJumpout(SyntaxError("Illegal PrimaryExpression",LINENUMBER));
		return SYM_NONE;
	}
}

int GrammarAnalyzer::doExpressionList()
{
	DEBUG_BEGIN(ExpressionList);

	int count=0;
	doExpression();
	count++;
	while(word.getSeriNo()!=Word::RIGHT_BRACKET&&
			word.getSeriNo()!=Word::SEMICOLON)
	{
		while(word.getSeriNo()==Word::COMMA)
			NEXTSYM;

		try{
			doExpression();
			count++;
		}catch(SyntaxError& e){
			while(word.getSeriNo()!=Word::RIGHT_BRACKET&&
					word.getSeriNo()!=Word::COMMA&&
					word.getSeriNo()!=Word::SEMICOLON)
				NEXTSYM;
		}catch(TypeError& e){
			while(word.getSeriNo()!=Word::RIGHT_BRACKET&&
					word.getSeriNo()!=Word::COMMA&&
					word.getSeriNo()!=Word::SEMICOLON)
				NEXTSYM;}
	}
	DEBUG_END(ExpressionList);

	return count;
}


//AllocationExpression ::= "new" Identifier "(" ")"
//ArrayAllocationExpression ::= "new" "int" "[" Expression "]"
SYMBOL_TYPE GrammarAnalyzer::doAllocationExpression()
{
	DEBUG_BEGIN(AllocationExpression);
	do{
		assert(word.getSeriNo()==Word::NEW);
		NEXTSYM;
		if(word.getSeriNo()==Word::INT)
		{
			NEXTSYM;
			if(word.getSeriNo()!=Word::LEFT_SQUARE)
				errorAndJumpout(SyntaxError("Expect array decleration",LINENUMBER));
			NEXTSYM;
			if(doExpression()!=SYM_INT)
			{
				errorAndContinue(TypeError("Expect a int number for array size",LINENUMBER));
			}
			if(word.getSeriNo()!=Word::RIGHT_SQUARE)
				errorAndJumpout(SyntaxError("Expect ]",LINENUMBER));

			//数组长度储存在操作栈顶
			curClass->addCode(newarray);
			curClass->addCode(10);		//int
			NEXTSYM;
			DEBUG_END(AllocationExpression);
			return SYM_ARRAY;

		}else if(word.getSeriNo()==Word::ID_STRING)
		{
			instanceRef.has=true;
			instanceRef.className=word.getString();
			instanceRef.name=string();

			NEXTSYM;
			if(word.getSeriNo()!=Word::LEFT_BRACKET)
				errorAndContinue(SyntaxError("Expect ( ",LINENUMBER));

			NEXTSYM;
			if(word.getSeriNo()!=Word::RIGHT_BRACKET)
				errorAndContinue(SyntaxError("Expect ) ",LINENUMBER));

			uint16_t classIdx=curClass->newClassConstant(instanceRef.className);
			uint16_t methodIdx1=curClass->newMethodrefConstant(classIdx,string("<init>"),string("()V"));

			//生成新对象 引用置于栈顶
			curClass->addCode(inew);
			curClass->addCodeU16(classIdx);
			curClass->addCode(dup);
			curClass->addCode(invokespecial);	//调用构造函数
			curClass->addCodeU16(methodIdx1);
			NEXTSYM;
			DEBUG_END(AllocationExpression);
			return SYM_INSTANCE;
		}

	}while(0);
		errorAndJumpout(SyntaxError("Syntax error.",LINENUMBER));
	return SYM_NONE;
}

string GrammarAnalyzer::doScanParameter()
{
	DEBUG_BEGIN(doScanParameter);

	string desc;
	if(word.getSeriNo()==Word::LEFT_BRACKET)
		NEXTSYM;
	else
		errorAndJumpout(SyntaxError("Expect (",LINENUMBER));

	if(word.getSeriNo()==Word::RIGHT_BRACKET)
	{
		desc="()I";
	}
	else
	{
		desc="(";
		while(word.getSeriNo()!=Word::RIGHT_BRACKET)
		{
			while(word.getSeriNo()==Word::COMMA)
				NEXTSYM;
			switch(doExpression())
			{
			case SYM_INT:
				desc+="I";
				break;
			case SYM_BOOLEAN:
				desc+="Z";
				break;
			case SYM_ARRAY:
				desc+="[I";
				break;
			default:
				errorAndJumpout(TypeError("Unsupport parameter type",LINENUMBER));
			}
		}
		desc+=")I";
	}

	NEXTSYM;

	DEBUG_END(ScanParameter);
	return desc;

}

