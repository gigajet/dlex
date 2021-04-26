#include "lexer.h"

Token LexicalAnalyzer::WysString()
{
    std::string val;
    Token tmp;
    char c;
    input.GetChar(c);
    val.push_back(c);
    //truong hop su dung ' ' 
    if (c == '\'')
    {
        bool isEndLoop = false;
        while (!input.EndOfInput() && !isEndLoop)
        {
            input.GetChar(c);
            val.push_back(c);
            if (c == '\\')
            {
                if (!input.EndOfInput())
                {
                    input.GetChar(c);
                    //xu ly add chuoi
                    val.push_back(c);
                    continue;
                }
            }
            if (c=='\'')
            {
                isEndLoop = true;
                break;
            }
            
        }
        if (!isEndLoop)
        {
            tmp.line_no = line_no;
            tmp.token_type = ERROR;
            return tmp;
        }
        else 
        {
            if (!input.EndOfInput())
            {
                input.GetChar(c);
                if (isDelimiterOfToken(c))
                {
                    input.UngetChar(c);
                    tmp.lexeme = val;
                    tmp.line_no = line_no;
                    tmp.token_type = WYSIWYGSTR;
                    return tmp;
                }
                else if (isStringPostfix(c))
                {
                    val.push_back(c);
                    tmp.lexeme = val;
                    tmp.line_no = line_no;
                    tmp.token_type = WYSIWYGSTR;
                    return tmp;
                }
                else
                {
                    input.UngetChar(c);
                    tmp.line_no = line_no;
                    tmp.token_type = ERROR;
                    return tmp;
                }
            }
            else 
            {
                tmp.lexeme = val;
                tmp.line_no = line_no;
                tmp.token_type = WYSIWYGSTR;
                return tmp;
            }
        }
    }
    else if (c == 'r')
    {
        input.GetChar(c);
        val.push_back(c);
        if (c == '\"')
        {
            bool isEndLoop = false;
            while (!input.EndOfInput() && !isEndLoop)
            {
                input.GetChar(c);
                val.push_back(c);
                if (c == '\\')
                {
                    if (!input.EndOfInput())
                    {
                        input.GetChar(c);
                        val.push_back(c);
                        continue;
                    }
                    else 
                    {
                        break;
                    }
                }
                if (c=='\"')
                {
                    isEndLoop = true;
                    break;
                }
                
            }
            if (!isEndLoop)
            {
                tmp.line_no = line_no;
                tmp.token_type = ERROR;
                return tmp;
            }
            else 
            {
                if (!input.EndOfInput())
                {
                    input.GetChar(c);
                    if (isDelimiterOfToken(c))
                    {
                        input.UngetChar(c);
                        tmp.lexeme = val;
                        tmp.line_no = line_no;
                        tmp.token_type = WYSIWYGSTR;
                        return tmp;
                    }
                    else if (isStringPostfix(c))
                    {
                        val.push_back(c);
                        tmp.lexeme = val;
                        tmp.line_no = line_no;
                        tmp.token_type = WYSIWYGSTR;
                        return tmp;
                    }
                    else
                    {
                        input.UngetChar(c);
                        tmp.line_no = line_no;
                        tmp.token_type = ERROR;
                        return tmp;
                    }
                }
                else 
                {
                    tmp.lexeme = val;
                    tmp.line_no = line_no;
                    tmp.token_type = WYSIWYGSTR;
                    return tmp;
                }
            }
        }
        else 
        {
            for (int i = val.size()-1; i>=0; --i)
                input.UngetChar(val[i]);
            return ScanIdOrKeyword();
        }
    }
}

Token LexicalAnalyzer::DoubleQuoteString()
{
    char c;
    input.GetChar(c);
    std::string val;
    val.push_back(c);
    Token tmp;

    bool isString = false;
    bool isEndLoop = false;
    while (!input.EndOfInput() && !isEndLoop)
    {
        input.GetChar(c);
        val.push_back(c);
        if (c == '\\')
        {
            if (!input.EndOfInput())
            {
                input.GetChar(c);
                //xu ly add chuoi
                val.push_back(c);
                continue;
            }
        }
        if (c=='\"')
        {
            isEndLoop = true;
            break;
        }
        
    }
    if (!isEndLoop)
    {
        tmp.line_no = line_no;
        tmp.token_type = ERROR;
        return tmp;
    }
    else 
    {
        if (!input.EndOfInput())
        {
            input.GetChar(c);
            if (isDelimiterOfToken(c))
            {
                input.UngetChar(c);
                tmp.lexeme = val;
                tmp.line_no = line_no;
                tmp.token_type = DOUBLESTR;
                return tmp;
            }
            else if (isStringPostfix(c))
            {
                val.push_back(c);
                tmp.lexeme = val;
                tmp.line_no = line_no;
                tmp.token_type = DOUBLESTR;
                return tmp;
            }
            else
            {
                input.UngetChar(c);
                tmp.line_no = line_no;
                tmp.token_type = ERROR;
                return tmp;
            }
        }
        else 
        {
            tmp.lexeme = val;
            tmp.line_no = line_no;
            tmp.token_type = DOUBLESTR;
            return tmp;
        }
    }
}
Token LexicalAnalyzer::DelimitedString()
{
    std::string val;
    Token tmp;
    char c;
    input.GetChar(c);
    val.push_back(c);
    if (c=='q')
    {
        
    }
}
Token LexicalAnalyzer::TokenString()
{
    
}