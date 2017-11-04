#ifndef GRAMMARANALYZER_H_
#define GRAMMARANALYZER_H_

#include <stdint.h>
#include "LexicalAnalyzer.h"
#include "SymbolTable.h"
#include "ClassFileGenerator.h"

#define NEXTSYM word=lexicalAnalyzer.getWord()
#define LINENUMBER word.getLineNumber()

class GrammarAnalyzer
{
public:
	GrammarAnalyzer(std::string sourceName,bool debug);
	~GrammarAnalyzer();

	void startAnalyze();
	bool generateClassFile();


private:
	void doGoal();
	void doMainClass();
	void doClassDeclaration();
	void doInstanceVarDeclar();
	void doVarDeclar();
	void doMethodDeclaration();
	std::string doFormalParameterList();
	std::string doFormalParameter();
	std::string __doFormalParameter();

	void doVarAndStatement();
		//针对公共前缀
		void doVarDeclar(string type);
		void doStatement(string name);

	void doStatement();
	void doAssignmentStatement();
	void doAssignmentStatement(string fieldName);
	void doIfStatement();
	void doWhileStatement();
	void doPrintStatement();
	void doBlock();


	enum SYMBOL_TYPE doExpression();
		enum SYMBOL_TYPE __doExpression();
	enum SYMBOL_TYPE doPrimaryExpression();
		enum SYMBOL_TYPE __doPrimaryExpression();

	string doScanParameter();
	int doExpressionList();

	enum SYMBOL_TYPE doAllocationExpression();


	void errorAndContinue(SyntaxError err);
	void errorAndContinue(TypeError err);
	void errorAndJumpout(SyntaxError err);
	void errorAndJumpout(TypeError err);

	LexicalAnalyzer lexicalAnalyzer;
	SymbolTable symbolTable;
	ClassFileGenerator *curClass;
	std::vector<ClassFileGenerator*> generators;

	Word word;		//当前字段
	uint8_t localVarCount;		//局部变量分配标号

	class InstanceRef
	{
	public:
		bool has;
		string name;
		string className;
	}instanceRef;

	bool hasError;

	bool _debug;
	int __debug_level;
};

#include <iomanip>
#define DEBUG_BEGIN(NAME) \
		int __debug_current_level=__debug_level;\
		__debug_level++;\
		if(_debug)std::cout<<std::setw(4*__debug_current_level)<<" "<<"BEGIN "<< #NAME <<lexicalAnalyzer.getLineNumber()<<std::endl

#define DEBUG_END(NAME) \
		__debug_level=__debug_current_level;\
		if(_debug)std::cout<<std::setw(4*__debug_current_level)<<" "<<"END "<<#NAME<<lexicalAnalyzer.getLineNumber()<<std::endl


#endif /* GRAMMARANALYSIS_H_ */
