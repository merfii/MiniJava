#ifndef LEXICALANALYZER_H_
#define LEXICALANALYZER_H_
#include <iostream>
#include <fstream>
#include "Exception.h"

#define HASH_SIZE 100

using std::exception;
using std::ios;
using std::string;

class Word;

#define MAX_LENGTH 128

class ifstreamWithLineNum : public std::ifstream
{
public:
	ifstreamWithLineNum();
	bool getch(char &c);
	int getLineNumber();

private:
	ifstreamWithLineNum(const ifstreamWithLineNum&);
	char oneLine[MAX_LENGTH];
	int posInOneLine;
	int lineLength;
	int lineNumber;
};

class LexicalAnalyzer
{
public:
	LexicalAnalyzer();
	~LexicalAnalyzer();

	void openSourceFile(std::string fileName);
	Word getWord();
	int getLineNumber();

private:
	LexicalAnalyzer(const LexicalAnalyzer&);
	char ch;
	string sym;
	ifstreamWithLineNum source;
	int keyWordHash[HASH_SIZE];
	int keySignHash[128];
	int matchKeyWord(string s);
	int matchKeySign(char _ch);

	//防止死循环
	static int eofCounter;
};

#undef EOF

class Word{
public:
	Word();
	Word(int No,int line);
	Word(int No,string s,int line);
	Word(int No,string s,int n,int line);

	int getSeriNo();
	int getLineNumber();
	string getMemoniStr();

	//当seriNo!=36、37时为保留字符串
	std::string getString();

	//只能当seriNo=37时使用
	unsigned int getNum();

	static const int
	EOF=0,
	BOOLEAN=1,
	CLASS=2,
	ELSE=3,
	EXTENDS=4,
	FALSE=5,
	IF=6,
	INT=7,
	LENGTH=8,
	MAIN=9,
	NEW=10,
	PUBLIC=11,
	RETURN=12,
	STATIC=13,
	THIS=14,
	TRUE=15,
	VOID=16,
	WHILE=17,
	STRING=18,
	SYSTEM=19,
	EQUAL=20,
	NOT=21,
	AND=22,
	PLUS=23,
	MINUS=24,
	TIMES=25,
	COMPARE=26,
	DOT=27,
	SEMICOLON=28,
	COMMA=29,
	LEFT_SQUARE=30,
	RIGHT_SQUARE=31,
	LEFT_BRACKET=32,
	RIGHT_BRACKET=33,
	LEFT_BLOCK=34,
	RIGHT_BLOCK=35,
	ID_STRING=36,
	ID_INTEGER=37,
	UNKNOWN=38;

	const static std::string strTable[36];
	const static std::string memoniTable[38];

private:
	int seriNo;
	int num;
	std::string str;
	int lineNumber;

};



#endif /* LEXICALANALYSIS_H_ */
