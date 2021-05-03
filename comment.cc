#include "lexer.h"

Token LexicalAnalyzer::InlineCmt() 
{
	char c;
	input.GetChar(c);
	if (c == '/') 
	{
		tmp.lexeme += c;
		input.GetChar(c);
		if (c == '/') 
		{
			while (!input.EndOfInput()) 
			{
				if (c == '\n') break;
				tmp.lexeme += c;
				input.GetChar(c);
			}
			tmp.token_type = INLINECMT;
		}
	}
	return tmp;
}
Token LexicalAnalyzer::BlockCmt() {
	char c;
	input.GetChar(c);
	if (c == '/') {
		tmp.lexeme += c;
		input.GetChar(c);
		if (c == '*') 
		{
			bool close = false;
			while (!input.EndOfInput()) 
			{
				tmp.lexeme += c;
				input.GetChar(c);
				if (c == '*') 
				{
					tmp.lexeme += c;
					input.GetChar(c);
					if (c == '/') 
					{
						tmp.lexeme += c;
						close = true;
						break;
					}
					else continue;
				}
				if (c == '\n' && input.EndOfInput())
				{
					tmp.lexeme += c;
					break;
				}
			}
			if(close) tmp.token_type = BLOCKCMT;
			else 
			{
				input.UngetString(tmp.lexeme);
				return ScanIdOrKeyword();
			}
		}
		else tmp.token_type = ERROR;
	}
	return tmp;
}