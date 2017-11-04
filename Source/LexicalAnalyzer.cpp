#include "LexicalAnalyzer.h"
#include <string>

int LexicalAnalyzer::eofCounter=0;

LexicalAnalyzer::LexicalAnalyzer():
ch(' ')
{
	for(int i=0;i<HASH_SIZE;i++)
		keyWordHash[i]=0;
	for(int i=1;i<20;i++)
	{
		int sum=0;
		string s=Word::strTable[i];
		for(string::iterator it=s.begin();it<s.end();it++)
		{
			sum+=(*it);
			//这里的hash是凑出来的 正好没有重叠
			sum=(sum*17)&(0x0FFFF);
		}
		sum%=HASH_SIZE;
		assert(keyWordHash[sum]==0);
		keyWordHash[sum]=i;
	}
	//System.out.println 只查找了System 后面的归于语法分析

	//对于运算符号进行倒索引
	for(int i=0;i<128;i++)
		keySignHash[i]=0;
	for(int i=20;i<=35;i++)
	{
		assert(keySignHash[0+Word::strTable[i].at(0)]==0);
		keySignHash[0+Word::strTable[i].at(0)]=i;
	}
}

LexicalAnalyzer::~LexicalAnalyzer()
{
	source.close();
}

void LexicalAnalyzer::openSourceFile(std::string fileName)
{
	source.open(fileName.c_str(),ios::in);
	if(!source.is_open())
	{
		throw ReadError("Open source file ERROR!");
	}
}

#include <cctype>
Word LexicalAnalyzer::getWord()
{
	Word ret;
	sym.clear();
	while(isspace(ch)&&source.getch(ch));
	if(!source.good())
	{
		if(eofCounter++ >1)
			throw EofError();
		return Word(0,source.getLineNumber());
	}

	if(isalpha(ch))
	{
		do{
			sym.push_back(ch);
		}while(source.getch(ch)&&isalnum(ch));

		int n;
		if((n=matchKeyWord(sym)))
		{
			//关键字
			ret=Word(n,Word::strTable[n],source.getLineNumber());
		}else
		{
			//标识符
			ret=Word(Word::ID_STRING,sym,source.getLineNumber());
		}

	}else if(isdigit(ch))
	{
		int n=0;
		do{
			sym.push_back(ch);
			n*=10;
			n+=ch-'0';
		}while(source.getch(ch)&&isdigit(ch));

		ret=Word(Word::ID_INTEGER,sym,n,source.getLineNumber());

	}else if(matchKeySign(ch))
	{
		int n=matchKeySign(ch);
		if(Word::AND==n)	// &&
		{
			source.getch(ch);
			if(ch==Word::strTable[Word::AND].at(1))
			{
				source.getch(ch);
				ret=Word(Word::AND,Word::strTable[Word::AND],source.getLineNumber());
			}else
			{
				throw SyntaxError("Missing the second & after one &",getLineNumber());
			}
		}else
		{
			ret=Word(n,Word::strTable[n],source.getLineNumber());
			source.getch(ch);
		}

	}else
	{
		if(source.eof())
		{
			if(eofCounter++ >2)
				throw EofError();
			return Word(0,source.getLineNumber());
		}
		else
			return Word(Word::UNKNOWN,source.getLineNumber());
	}
	return ret;
}

int LexicalAnalyzer::getLineNumber()
{
	return source.getLineNumber();

}

int LexicalAnalyzer::matchKeyWord(string s)
{
	int sum=0;
	for(string::iterator it=s.begin();
			it<s.end();it++)
	{
		sum+=(*it);
		sum=(sum*17)&(0x0FFFF);
	}
	sum%=HASH_SIZE;
	if(keyWordHash[sum]&&s.compare(Word::strTable[keyWordHash[sum]])==0)
	{
		return keyWordHash[sum];
	}
	else return 0;
}

int LexicalAnalyzer::matchKeySign(char _ch)
{
	assert(_ch<128);
	return keySignHash[0x7F&_ch];

}

Word::Word():
seriNo(0),num(0)
{
}

Word::Word(int No,int line):
seriNo(No),num(0),lineNumber(line)
{
	;
}

Word::Word(int No,string s,int line):
seriNo(No),num(0),str(s),lineNumber(line)
{
	;
}

Word::Word(int No,string s,int n,int line):
seriNo(No),num(n),str(s),lineNumber(line)
{
	;
}

int Word::getSeriNo()
{
	return seriNo;
}

int Word::getLineNumber()
{
	return lineNumber;
}

string Word::getMemoniStr()
{
	return memoniTable[seriNo];

}

std::string Word::getString()
{
	if(seriNo==36||seriNo==37)
		return str;
	assert(seriNo>=0&&seriNo<=35);
	return strTable[seriNo];
}


unsigned int Word::getNum()
{
	assert(seriNo==37);
	return num;

}

const std::string Word::strTable[36]={
		"<EOF>",
		"boolean",
		"class",
		"else",
		"extends",
		"false",
		"if",
		"int",
		"length",
		"main",
		"new",
		"public",
		"return",
		"static",
		"this",
		"true",
		"void",
		"while",
		"String",
		"System",
		"=",
		"!",
		"&&",
		"+",
		"-",
		"*",
		"<",
		".",
		";",
		",",
		"[",
		"]",
		"(",
		")",
		"{",
		"}",
};

const std::string Word::memoniTable[38]={
"EOF",
"BOOLEAN",
"CLASS",
"ELSE",
"EXTENDS",
"FALSE",
"IF",
"INT",
"LENGTH",
"MAIN",
"NEW",
"PUBLIC",
"RETURN",
"STATIC",
"THIS",
"TRUE",
"VOID",
"WHILE",
"STRING",
"SYSTEM",
"EQUAL",
"NOT",
"AND",
"PLUS",
"MINUS",
"TIMES",
"COMPARE",
"DOT",
"SEMICOLON",
"COMMA",
"LEFT_SQUARE",
"RIGHT_SQUARE",
"LEFT_BRACKET",
"RIGHT_BRACKET",
"LEFT_BLOCK",
"RIGHT_BLOCK",
"ID_STRING",
"ID_INTEGER"
};

ifstreamWithLineNum::ifstreamWithLineNum():
std::ifstream(),posInOneLine(0),lineLength(0),lineNumber(0)
{
}

#include <cstring>
bool ifstreamWithLineNum::getch(char &c)
{
	if(posInOneLine>=lineLength)
	{
		do{
			getline(oneLine,MAX_LENGTH);
			lineNumber++;
			posInOneLine=0;
			lineLength=strlen(oneLine);

			if(eof())
				return false;
		}while(!lineLength);
		c=' ';
		return true;
	}else
	{
		c=oneLine[posInOneLine];
		posInOneLine++;
		return true;
	}
}


int ifstreamWithLineNum::getLineNumber()
{
	return lineNumber;

}
