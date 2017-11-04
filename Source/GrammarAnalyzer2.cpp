#include "GrammarAnalyzer.h"
#include "InstructionSet.h"

void GrammarAnalyzer::doVarAndStatement()
{
	DEBUG_BEGIN(VarAndStatement);

	/*
	var可能有
		int[] bool identifier  +fieldName
	statement可能有
		{ if while system
		identifier= 赋值
		identifier[ 赋值
	 */
	std::string id1;
	bool var=true;
	while(var)
	{
		switch(word.getSeriNo())
		{
		case Word::INT:
		case Word::BOOLEAN:
			try {doVarDeclar();}
			catch(SyntaxError& e){
				while(word.getSeriNo()!=Word::SEMICOLON&&
						word.getSeriNo()!=Word::RIGHT_BLOCK&&
						word.getSeriNo()!=Word::RETURN)
				{
						NEXTSYM;
				}
			}catch(TypeError& e){
				while(word.getSeriNo()!=Word::SEMICOLON&&
						word.getSeriNo()!=Word::RETURN)
						NEXTSYM;
			}
			break;
		case Word::ID_STRING:
			id1=word.getString();
			NEXTSYM;
			switch(word.getSeriNo())
			{
				case Word::EQUAL:
				case Word::LEFT_SQUARE:
					try{doAssignmentStatement(id1);}
					catch(SyntaxError& e){
					while(word.getSeriNo()!=Word::SEMICOLON&&
							word.getSeriNo()!=Word::RIGHT_BLOCK&&
							word.getSeriNo()!=Word::RETURN)
							NEXTSYM;}catch(TypeError& e){
								while(word.getSeriNo()!=Word::SEMICOLON&&
										word.getSeriNo()!=Word::RIGHT_BLOCK&&
										word.getSeriNo()!=Word::RETURN)
										NEXTSYM;}
					var=false;
					break;
				case Word::ID_STRING:
					try{doVarDeclar(id1);}
					catch(SyntaxError& e){
						while(word.getSeriNo()!=Word::SEMICOLON&&
								word.getSeriNo()!=Word::RIGHT_BLOCK&&
								word.getSeriNo()!=Word::RETURN)
								NEXTSYM;}catch(TypeError& e){
									while(word.getSeriNo()!=Word::SEMICOLON&&
											word.getSeriNo()!=Word::RIGHT_BLOCK&&
											word.getSeriNo()!=Word::RETURN)
											NEXTSYM;}
					break;
				default:
					errorAndJumpout(SyntaxError("Illegal syntax ",LINENUMBER));
					break;
			}
			break;

		case Word::IF:
		case Word::WHILE:
		case Word::SYSTEM:
			try{doStatement();}
			catch(SyntaxError& e){
			while(word.getSeriNo()!=Word::SEMICOLON&&
					word.getSeriNo()!=Word::RIGHT_BLOCK&&
					word.getSeriNo()!=Word::RETURN)
					NEXTSYM;}catch(TypeError& e){
						while(word.getSeriNo()!=Word::SEMICOLON&&
								word.getSeriNo()!=Word::RIGHT_BLOCK&&
								word.getSeriNo()!=Word::RETURN)
								NEXTSYM;}
			var=false;
			break;
		case Word::RETURN:
		case Word::RIGHT_BLOCK:
			DEBUG_END(VarAndStatement);
			return;
		}
	}
	DEBUG_END(VarAndStatement);
}

void GrammarAnalyzer::doStatement()
{
	DEBUG_BEGIN(Statement);

	if(word.getSeriNo()==Word::SEMICOLON)
	{
		NEXTSYM;
		return ;
	}

	switch(word.getSeriNo())
	{
	case Word::IF:
		doIfStatement();
		break;
	case Word::WHILE:
		doWhileStatement();
		break;
	case Word::SYSTEM:
		doPrintStatement();
		break;
	case Word::LEFT_BLOCK:
		doBlock();
		break;

	case Word::ID_STRING:
		doAssignmentStatement();
		break;
	default:

		errorAndJumpout(SyntaxError("Illegal statement",LINENUMBER));
	}

	instanceRef.has=false;
	DEBUG_END(Statement);

	return;
}

void GrammarAnalyzer::doAssignmentStatement()
{
	DEBUG_BEGIN(AssignmentStatement);

	string fieldName;
	if(word.getSeriNo()==Word::ID_STRING)
	{
		fieldName=word.getString();
		NEXTSYM;
		doAssignmentStatement(fieldName);
	}else
		errorAndJumpout(SyntaxError("Illegal filed name",LINENUMBER));
	DEBUG_END(AssignmentStatement);

}

void GrammarAnalyzer::doAssignmentStatement(string fieldName)
{
	DEBUG_BEGIN(AssignmentStatementString);

	if(word.getSeriNo()==Word::EQUAL)
	{
		switch(symbolTable.getType(fieldName,curClass))
		{
			case SYM_NONE:
				errorAndContinue(SyntaxError("Undefined variable "+fieldName,LINENUMBER));

			case SYM_INT:
				if(symbolTable.isInstanceVar())
				{
					uint16_t addr=symbolTable.getFieldInt().fieldRefIdx;
					//成员变量
					NEXTSYM;
					curClass->addCode(aload_0);
					if(doExpression()!=SYM_INT)
						errorAndContinue(SyntaxError("Inconsistent assignment type ",LINENUMBER));
					curClass->addCode(putfield);
					curClass->addCodeU16(addr);
				}else
				{
					uint8_t addr=symbolTable.getFieldInt().addr;
					NEXTSYM;
					if(doExpression()!=SYM_INT)
						errorAndContinue(SyntaxError("Inconsistent assignment type ",LINENUMBER));
					curClass->addCode(istore);
					curClass->addCode(addr);
				}
				break;

			case SYM_BOOLEAN:
				if(symbolTable.isInstanceVar())
				{
					uint16_t addr=symbolTable.getFieldBoolean().fieldRefIdx;
					//成员变量
					NEXTSYM;
					curClass->addCode(aload_0);
					if(doExpression()!=SYM_BOOLEAN)
						errorAndContinue(SyntaxError("Inconsistent assignment type ",LINENUMBER));
					curClass->addCode(putfield);
					curClass->addCodeU16(addr);

				}else
				{
					uint8_t addr=symbolTable.getFieldBoolean().addr;
					NEXTSYM;

					if(doExpression()!=SYM_BOOLEAN)
					{
						errorAndContinue(SyntaxError("Inconsistent assignment type ",LINENUMBER));
					}

					curClass->addCode(istore);
					curClass->addCode(addr);
				}
				break;

			case SYM_INSTANCE:
				if(symbolTable.isInstanceVar())
				{
					//成员变量
					uint16_t addr=symbolTable.getInstance().fieldRefIdx;
					NEXTSYM;
					curClass->addCode(aload_0);
					if(doExpression()!=SYM_INSTANCE)
						errorAndContinue(SyntaxError("Inconsistent assignment type ",LINENUMBER));
					curClass->addCode(putfield);
					curClass->addCodeU16(addr);
				}else
				{
					uint8_t addr=symbolTable.getInstance().addr;
					NEXTSYM;
					if(doExpression()!=SYM_INSTANCE)
						errorAndContinue(SyntaxError("Inconsistent assignment type ",LINENUMBER));
					curClass->addCode(istore);
					curClass->addCode(addr);
				}
				break;
			case SYM_ARRAY:
				if(symbolTable.isInstanceVar())
				{
					uint16_t addr=symbolTable.getFieldArray().fieldRefIdx;
					//成员变量
					NEXTSYM;
					curClass->addCode(aload_0);
					if(doExpression()!=SYM_ARRAY)
						errorAndContinue(SyntaxError("Inconsistent assignment type ",LINENUMBER));
					curClass->addCode(putfield);
					curClass->addCodeU16(addr);
				}else
				{
					uint8_t addr=symbolTable.getFieldArray().addr;
					NEXTSYM;
					if(doExpression()!=SYM_ARRAY)
						errorAndContinue(SyntaxError("Inconsistent assignment type ",LINENUMBER));
					curClass->addCode(astore);
					curClass->addCode(addr);
				}
				break;
			default:
				errorAndJumpout(TypeError("Wrong type ",LINENUMBER));
		}

	}else if(word.getSeriNo()==Word::LEFT_SQUARE)
	{
		if(symbolTable.getType(fieldName,curClass)!=SYM_ARRAY)
			errorAndJumpout(TypeError("Wrong type ",LINENUMBER));
		NEXTSYM;


		if(symbolTable.isInstanceVar())
		{
			//arrayref
			curClass->addCode(aload_0);
			curClass->addCode(getfield);
			curClass->addCodeU16(symbolTable.getFieldArray().fieldRefIdx);

			//index
			if(doExpression()!=SYM_INT)
				errorAndContinue(TypeError("Index must be int",LINENUMBER));
			if(word.getSeriNo()==Word::RIGHT_SQUARE)
				NEXTSYM;
			else
				errorAndJumpout(SyntaxError("Expect ] ",LINENUMBER));

			if(word.getSeriNo()==Word::EQUAL)
				NEXTSYM;
			else
				errorAndJumpout(SyntaxError("Expect = ",LINENUMBER));

			//value
			if(doExpression()!=SYM_INT)
				errorAndContinue(SyntaxError("Inconsistent assignment type ",LINENUMBER));

			curClass->addCode(iastore);
		}
		else
		{
			curClass->addCode(aload);
			//arrayref
			curClass->addCode(symbolTable.getFieldArray().addr);

			//index
			if(doExpression()!=SYM_INT)
				errorAndContinue(TypeError("Index must be int",LINENUMBER));
			if(word.getSeriNo()==Word::RIGHT_SQUARE)
				NEXTSYM;
			else
				errorAndJumpout(SyntaxError("Expect ] ",LINENUMBER));

			if(word.getSeriNo()==Word::EQUAL)
				NEXTSYM;
			else
				errorAndJumpout(SyntaxError("Expect = ",LINENUMBER));

			//value
			if(doExpression()!=SYM_INT)
				errorAndContinue(SyntaxError("Inconsistent assignment type ",LINENUMBER));

			curClass->addCode(iastore);
		}

	}else
		errorAndJumpout(SyntaxError("Illegal assignment statement ",LINENUMBER));

	if(word.getSeriNo()==Word::SEMICOLON)
		NEXTSYM;
	else
		errorAndJumpout(SyntaxError("Expect ; ",LINENUMBER));

	DEBUG_END(AssignmentStatementString);

}

void GrammarAnalyzer::doIfStatement()
{
	DEBUG_BEGIN(AssignmentStatementString);

	int16_t offsetFalse,offsetOut;
	NEXTSYM;
	if(word.getSeriNo()!=Word::LEFT_BRACKET)
		errorAndJumpout(SyntaxError("Expect ( ",LINENUMBER));
	NEXTSYM;
	if(doExpression()!=SYM_BOOLEAN)
		errorAndContinue(SyntaxError("The condition should be bool",LINENUMBER));
	if(word.getSeriNo()!=Word::RIGHT_BRACKET)
	{
		errorAndContinue(SyntaxError("Expect ) ",LINENUMBER));
	}else
		NEXTSYM;

	curClass->addCode(ifle);
	curClass->addCode(0x00);
	offsetFalse=curClass->getOffset();
	curClass->addCode(0x00);

	doStatement();
	curClass->addCode(igoto);
	curClass->addCode(0x00);
	offsetOut=curClass->getOffset();
	curClass->addCode(0x00);

	if(word.getSeriNo()!=Word::ELSE)
		errorAndJumpout(SyntaxError("Expect \"else\" ",LINENUMBER));
	NEXTSYM;

	//回写地址
	curClass->addCodeU16((uint16_t)(curClass->getOffset()-offsetFalse+2),offsetFalse);
	doStatement();

	curClass->addCodeU16((uint16_t)(curClass->getOffset()-offsetOut+2),offsetOut);
	DEBUG_END(AssignmentStatementString);

}

void GrammarAnalyzer::doWhileStatement()
{
	DEBUG_BEGIN(WhileStatement);

	NEXTSYM;
	int16_t offsetBegin,offsetBreak;
	if(word.getSeriNo()!=Word::LEFT_BRACKET)
		errorAndJumpout(SyntaxError("Expect ( ",LINENUMBER));
	NEXTSYM;
	curClass->addCode(nop);		//为了防止第一行就是while
	offsetBegin=curClass->getOffset();

	if(doExpression()!=SYM_BOOLEAN)
		errorAndContinue(SyntaxError("The condition should be bool",LINENUMBER));
	if(word.getSeriNo()!=Word::RIGHT_BRACKET)
	{
		errorAndContinue(SyntaxError("Expect ) ",LINENUMBER));
	}else
		NEXTSYM;
	curClass->addCode(ifle);
	curClass->addCode(0x00);
	offsetBreak=curClass->getOffset();
	curClass->addCode(0x00);
	doStatement();
	curClass->addCode(igoto);
	curClass->addCodeU16((uint16_t)(offsetBegin-curClass->getOffset()+1));
	curClass->addCodeU16((uint16_t)(curClass->getOffset()-offsetBreak+2),offsetBreak);
	DEBUG_END(WhileStatement);
}


void GrammarAnalyzer::doPrintStatement()
{
	DEBUG_BEGIN(PrintStatement);

	//生成代码
	uint16_t systemIdx=curClass->newClassConstant(string("java/lang/System"));
	uint16_t outIdx=curClass->newFieldrefConstant(systemIdx,string("out"),string("Ljava/io/PrintStream;"));
	uint16_t printStreamIdx=curClass->newClassConstant(string("java/io/PrintStream"));
	uint16_t methodIdx=curClass->newMethodrefConstant(printStreamIdx,string("println"),string("(I)V"));
	do{
		if(word.getSeriNo()!=Word::SYSTEM)
			break;
		NEXTSYM;
		if(word.getSeriNo()!=Word::DOT)
			break;
		NEXTSYM;
		if(word.getSeriNo()!=Word::ID_STRING)
			break;
		if(!(word.getString()=="out"))
			break;
		NEXTSYM;
		if(word.getSeriNo()!=Word::DOT)
			break;
		NEXTSYM;
		if(word.getSeriNo()!=Word::ID_STRING)
			break;
		if(!(word.getString()=="println"))
			break;
		NEXTSYM;
		if(word.getSeriNo()!=Word::LEFT_BRACKET)
			break;
		curClass->addCode(getstatic);
		curClass->addCodeU16(outIdx);

		NEXTSYM;
		doExpression();
		if(word.getSeriNo()!=Word::RIGHT_BRACKET)
			break;

		curClass->addCode(invokevirtual);
		curClass->addCodeU16(methodIdx);

		NEXTSYM;
		if(word.getSeriNo()!=Word::SEMICOLON)
			errorAndJumpout(SyntaxError("Expect ;",LINENUMBER));
		else
			NEXTSYM;

		return;
	}while(0);
	errorAndContinue(SyntaxError("Print statement grammar error",LINENUMBER));
	while(word.getSeriNo()!=Word::SEMICOLON&&word.getSeriNo()!=Word::RIGHT_BLOCK)
		NEXTSYM;
	if(word.getSeriNo()==Word::SEMICOLON)
		NEXTSYM;

	DEBUG_END(PrintStatement);

}


void GrammarAnalyzer::doBlock()
{
	DEBUG_BEGIN(Block);

	if(word.getSeriNo()!=Word::LEFT_BLOCK)
	{
		errorAndJumpout(SyntaxError("Missing { ",LINENUMBER));
	}else
	{
		NEXTSYM;
		symbolTable.blockPush();

		while(word.getSeriNo()!=Word::RIGHT_BLOCK)
		{
			try{
				doStatement();
			}
			catch(SyntaxError& e)
			{
				while(word.getSeriNo()!=Word::SEMICOLON && word.getSeriNo()!=Word::RIGHT_BLOCK)
					NEXTSYM;
				if(word.getSeriNo()==Word::SEMICOLON)
					NEXTSYM;
			}
		}
		symbolTable.blockPop();
		NEXTSYM;
	}

	DEBUG_END(Block);

}


