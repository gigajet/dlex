#include "lexer.h"

using namespace std;

Token LexicalAnalyzer::inreal()
{
    int base = 10;
    bool ishex = false;
    char c;
    input.GetChar(c);
    string val = "";
    val.push_back(c);
    if (c == '0')
    {
        //check co phai 0x ko 
        input.GetChar(c);
        val.push_back(c);
        if (c == 'x' || c == 'X')
        {
            base = 16;
            ishex = true;
        }
        goto leftdigit;
    }
    else if (c == '.')
    {
        goto rightdigit;
    } else
    {
        goto leftdigit;
    }
    leftdigit:
        while (1)
        {
            input.GetChar(c);
            if (c == '.')
            {
                val.push_back(c);
                break;
            }
            if (isdigit(c) || (ishex && ishexdigit(c)) || c=='_')
            {
                val.push_back(c);
                continue;
            }
            input.UngetChar(c);
            break;
        }
    rightdigit:
        while (1)
        {
            input.GetChar(c);
            if (isdigit(c) || (ishex && ishexdigit(c)) || c=='_')
            {
                val.push_back(c);
                continue;
            }
            input.UngetChar(c);
            break;
        }
    exponent:
        input.GetChar(c);
        val.push_back(c);
        if (toupper(c) == 'E' || (toupper(c) == 'P' && ishex))
        {
            input.GetChar(c);
            val.push_back(c);
            if (c == '+' || c == '-')
            {

            }
            else 
            {
                val.pop_back();
                input.UngetChar(c);
                bool validExp = false;
                while (1)
                {
                    input.GetChar(c);
                    if (isdigit(c))
                    {
                        validExp = true;
                        val.push_back(c);
                        continue;
                    }
                    else if (c=='_')
                    {
                        val.push_back(c);
                        continue;
                    }
                    input.UngetChar(c);
                    if (!validExp)
                    {
                        return ErrorToken();
                    }
                    break;
                }
            }
        }
        else if (ishex)
        {
            val.pop_back();
            input.UngetChar(c);
            return ErrorToken();
        }
        else 
        {
            val.pop_back();
            input.UngetChar(c);
        }
    suffix:
        input.GetChar(c);
        if (toupper(c) == 'F')
        {
            tmp.token_type = FLOAT;
            val.push_back(c);
        }
        else if (c == 'L')
        {
            tmp.token_type = REAL;
            val.push_back(c);
        }
        else 
        {
            input.UngetChar(c);
            tmp.token_type = DOUBLE;
        }

        input.GetChar(c);
        if (c == 'i')
        {
            val.push_back(c);
            if (tmp.token_type == FLOAT)
                tmp.token_type = COMPLEXFLOAT;
            else if (tmp.token_type == REAL)
                tmp.token_type = COMPLEXREAL;
            else if (tmp.token_type == DOUBLE)
                tmp.token_type = COMPLEXDOUBLE;
        }
        else 
            input.UngetChar(c);
        tmp.line_no = line_no;
        tmp.lexeme = val;
        return tmp;
}