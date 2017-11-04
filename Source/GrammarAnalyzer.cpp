#include "GrammarAnalyzer.h"
#include "InstructionSet.h"

GrammarAnalyzer::GrammarAnalyzer(std::string sourceName,bool debug):
hasError(false),_debug(debug)
{
	//localVarCount在doMethodDecleration里声明
	lexicalAnalyzer.openSourceFile(sourceName);

	__debug_level=0;

}

GrammarAnalyzer::~GrammarAnalyzer()
{

}

void GrammarAnalyzer::startAnalyze()
{

	std::cout<<"=============== Start compile ==============="<<std::endl;
	NEXTSYM;
	if(word.getSeriNo()==Word::EOF)
	{
		std::cout<<"Source file is empty."<<std::endl;
		hasError=true;
	}
	else
		doGoal();

}

bool GrammarAnalyzer::generateClassFile()
{
	if(hasError)
	{
		std::cout<<std::endl<<"=============== Build failed ==============="<<std::endl;
		return false;
	}else
	{
		if(symbolTable.checkStackTop())
		{
			std::cout<<std::endl<<"=============== Building ==============="<<std::endl;
			for(std::vector<ClassFileGenerator*>::iterator it=generators.begin();it!=generators.end();it++)
			{
				(*it)->output();
				delete (*it);
			}
			std::cout<<std::endl<<"================ Done ================="<<std::endl;
			return true;

		}else
			return false;
	}
}

//	Goal ::= MainClass ( TypeDeclaration )* <EOF>
void GrammarAnalyzer::doGoal()
{
	DEBUG_BEGIN(Goal);

	if(word.getSeriNo()==Word::CLASS)
	{
		doMainClass();
	}else
	{
		errorAndContinue(SyntaxError("Expect main class",LINENUMBER));
		while(word.getSeriNo()!=Word::CLASS)
			word=lexicalAnalyzer.getWord();
	}
	while(word.getSeriNo()!=Word::EOF)
	{
		try{
			if(word.getSeriNo()==Word::CLASS)
				doClassDeclaration();
		}catch(TypeError& e){}catch(SyntaxError &e){}
		while(word.getSeriNo()!=Word::EOF &&
				word.getSeriNo()!=Word::CLASS)
		NEXTSYM;

	}
	DEBUG_END(Goal);
}

//	MainClass	::=	"class" Identifier
//	"{" "public" "static" "void" "main"
//	"(" "String" "[" "]" Identifier ")"
//	"{" PrintStatement "}" "}"
void GrammarAnalyzer::doMainClass()
{
	DEBUG_BEGIN(MainClass);

	assert(word.getSeriNo()==Word::CLASS);
	NEXTSYM;
	if(word.getSeriNo()==Word::ID_STRING)
	{
		assert(generators.size()==0);
		std::string className=word.getString();
		curClass=new ClassFileGenerator(className,false);
		generators.push_back(curClass);
		symbolTable.addClass(className,curClass);

	}else
	{
		errorAndJumpout(SyntaxError("Identify code error",LINENUMBER));
	}
	uint16_t superIdx=curClass->newClassConstant(std::string("java/lang/Object"));
	uint16_t methodIdx1=curClass->newMethodrefConstant(superIdx,string("<init>"),string("()V"));
	curClass->setSuperClass(superIdx);
	curClass->beginMethod(string("<init>"),string("()V"),false);
		curClass->addCode(aload_0);	//将当前类句柄从local variable 压入operand
		curClass->addCode(invokespecial);	//调用继承的构造函数
		curClass->addCodeU16(methodIdx1);
		curClass->addCode(vreturn);
	curClass->endMethod();

	NEXTSYM;
	if(word.getSeriNo()==Word::LEFT_BLOCK)
	{
		symbolTable.blockPush();
	}else
	{
		errorAndJumpout(SyntaxError("Missing {",LINENUMBER));
	}
	NEXTSYM;
	if(word.getSeriNo()!=Word::PUBLIC)
		errorAndJumpout(SyntaxError("Missing \"public\"",LINENUMBER));

	NEXTSYM;
	if(word.getSeriNo()!=Word::STATIC)
		errorAndJumpout(SyntaxError("Missing \"static\"",LINENUMBER));

	NEXTSYM;
	if(word.getSeriNo()!=Word::VOID)
		errorAndJumpout(SyntaxError("Missing \"void\"",LINENUMBER));

	NEXTSYM;
	if(word.getSeriNo()!=Word::MAIN)
		errorAndJumpout(SyntaxError("Missing \"main\"",LINENUMBER));

	NEXTSYM;
	if(word.getSeriNo()!=Word::LEFT_BRACKET)
		errorAndJumpout(SyntaxError("Missing (",LINENUMBER));

	NEXTSYM;
	if(word.getSeriNo()!=Word::STRING)
		errorAndJumpout(SyntaxError("Missing \"string\"",LINENUMBER));

	NEXTSYM;
	if(word.getSeriNo()!=Word::LEFT_SQUARE)
		errorAndJumpout(SyntaxError("missing [ ",LINENUMBER));

	NEXTSYM;
	if(word.getSeriNo()!=Word::RIGHT_SQUARE)
		errorAndJumpout(SyntaxError("missing ] ",LINENUMBER));

	NEXTSYM;
	if(word.getSeriNo()!=Word::ID_STRING)
		errorAndJumpout(SyntaxError("missing id ",LINENUMBER));

	std::string varName=word.getString();

	//用不到这个参数
	NEXTSYM;
	if(word.getSeriNo()!=Word::RIGHT_BRACKET)
		errorAndJumpout(SyntaxError("missing ) ",LINENUMBER));

	NEXTSYM;
	if(word.getSeriNo()!=Word::LEFT_BLOCK)
		errorAndJumpout(SyntaxError("missing { ",LINENUMBER));

	curClass->beginMethod(string("main"),string("([Ljava/lang/String;)V"),true);

	NEXTSYM;
	doPrintStatement();
	if(word.getSeriNo()!=Word::RIGHT_BLOCK)
		errorAndJumpout(SyntaxError("missing } ",LINENUMBER));

	curClass->addCode(vreturn);		//return
	curClass->endMethod();

	NEXTSYM;
	if(word.getSeriNo()!=Word::RIGHT_BLOCK)
		errorAndJumpout(SyntaxError("missing } ",LINENUMBER));

	symbolTable.blockPop();
	NEXTSYM;
	DEBUG_END(MainClass);
	if(_debug)std::cout<<std::endl;
}


void GrammarAnalyzer::errorAndContinue(SyntaxError err)
{
	hasError=true;
	if(_debug)std::cout<<"Error: "+err.msg<<"  Line "<<err.jump<<std::endl;

}

void GrammarAnalyzer::errorAndContinue(TypeError err)
{
	hasError=true;
	std::cout<<"Error: "+err.msg<<"  Line "<<err.jump<<std::endl;

}

void GrammarAnalyzer::errorAndJumpout(SyntaxError err)
{
	hasError=true;
	std::cout<<"Error: "+err.msg<<"  Line "<<err.jump<<std::endl;
	throw err;

}

void GrammarAnalyzer::errorAndJumpout(TypeError err)
{
	hasError=true;
	std::cout<<"Error: "+err.msg<<"  Line "<<err.jump<<std::endl;
	throw err;

}
