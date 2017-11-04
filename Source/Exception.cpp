#include "Exception.h"

ReadError::ReadError(const string& _msg)
{
	msg=_msg;
}

SyntaxError::SyntaxError(const string &_msg,int _jump)
{
	msg=_msg;
	jump=_jump;
}


TypeError::TypeError(const string &_msg,int _jump)
{
	msg=_msg;
	jump=_jump;
}


EofError::EofError()
{
}

