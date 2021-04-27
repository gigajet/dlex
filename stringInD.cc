#include "lexer.h"

Token LexicalAnalyzer::WysString()
{
    std::string val;
    //Token tmp;
    char c;
    input.GetChar(c);
    val.push_back(c);
    //truong hop su dung ' ' 
    if (c == '`')
    {
        bool isEndLoop = false;
        while (!input.EndOfInput() && !isEndLoop)
        {
            input.GetChar(c);
            val.push_back(c);
            if (c=='`')
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
                if (isStringPostfix(c))
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
                    tmp.token_type = WYSIWYGSTR;
                    tmp.lexeme = val;
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
                    if (isStringPostfix(c))
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
                        tmp.token_type = WYSIWYGSTR;
                        tmp.lexeme = val;
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
            input.UngetString(val);
            return ScanIdOrKeyword();
        }
    }
    else 
    {
        input.UngetChar(c);
        return ScanIdOrKeyword();
    }
}

Token LexicalAnalyzer::DoubleQuoteString()
{
    char c;
    input.GetChar(c);
    std::string val;
    val.push_back(c);
    //Token tmp;

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
        return ErrorToken();
    }
    else 
    {
        if (!input.EndOfInput())
        {
            input.GetChar(c);
            if (isStringPostfix(c))
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
                tmp.lexeme = val;
                tmp.line_no = line_no;
                tmp.token_type = DOUBLESTR;
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
    //Token tmp;
    char c;
    input.GetChar(c);
    val.push_back(c);
    if (c=='q')
    {
        input.GetChar(c);
        val.push_back(c);
        if (c=='\"')
        {
            bool isEndLoop = false;
            while (!input.EndOfInput() && !isEndLoop)
            {
                input.GetChar(c);
                val.push_back(c);
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
                if (val.length() < 5) 
                {
                    tmp.line_no = line_no;
                    tmp.token_type = ERROR;
                    return tmp;
                } 
                std::string content = val.substr(2, val.length()-3);
                if (isValidDelimitedString(content))
                {
                    tmp.lexeme = val;
                    tmp.line_no = line_no;
                    tmp.token_type = DELIMITEDSTR;
                    return tmp;
                }
                else 
                {
                    return ErrorToken();
                }
            }
        }
        else 
        {
            input.UngetString(val);
            return ScanIdOrKeyword();
        }
    }
    else 
    {
        return ScanIdOrKeyword();
    }
}
Token LexicalAnalyzer::TokenString()
{
    std::string val;
    //Token tmp;
    char c;
    input.GetChar(c);
    val.push_back(c);
    if (c=='q')
    {
        input.GetChar(c);
        val.push_back(c);
        if (c == '{')
        {
            bool isEndLoop = false;
            while (!input.EndOfInput() && !isEndLoop)
            {
                input.GetChar(c);
                val.push_back(c);
                if (c=='}')
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
                std::string content = val.substr(2, val.length()-3);
                std::istringstream strstream(content);
                LexicalAnalyzer tokenLex(&strstream);
                Token token;
                token = tokenLex.GetToken();
                //token.Print();
                while (token.token_type != END_OF_FILE)
                {
                    if (token.token_type == ERROR)
                    {
                        return ErrorToken();
                    }
                    token = tokenLex.GetToken();
                }
                tmp.lexeme = val;
                tmp.line_no = line_no;
                tmp.token_type = TOKENSTR;
                return tmp;
            }
        }
        else 
        {
            input.UngetString(val);
            return ScanIdOrKeyword();
        }
    }
    else 
    {
        input.UngetChar(c);
        return ScanIdOrKeyword();
    }
}

Token LexicalAnalyzer::ScanString()
{
    char c;
    input.GetChar(c);
    if (c == 'q')
    {
        if (!input.EndOfInput())
        {
            input.GetChar(c);
            if (c=='\"')
            {
                input.UngetChar(c);
                input.UngetChar('q');
                return DelimitedString();
            }
            else if (c=='{')
            {
                input.UngetChar(c);
                input.UngetChar('q');
                return TokenString();
            }
            else 
            {
                input.UngetChar('q');
                return ScanIdOrKeyword();
            }
        }
        else 
        {
            input.UngetChar('q');
            return ScanIdOrKeyword();
        }
    }
    else if (c=='r')
    {
        if (!input.EndOfInput())
        {
            input.GetChar(c);
            if (c=='\"')
            {
                input.UngetChar(c);
                input.UngetChar('r');
                return WysString();
            }
            else 
            {
                input.UngetChar(c);
                input.UngetChar('r');
                return ScanIdOrKeyword();
            }
        }
        else 
        {
            input.UngetChar('q');
            return ScanIdOrKeyword();
        }
    }
    else 
    {
        input.UngetChar(c);
        return ScanIdOrKeyword();
    }
}