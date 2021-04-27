#include "Util.h"

bool isDelimiterOfToken(char c)
{
    return c==';' || c=='+' || c=='=' || c=='!' || c=='/' || c=='-' || c=='*' || c=='.' || c=='>' || c=='<' || c=='^' || c=='&' || c=='|' || c==',' || c=='?' || c==':' || c==' ' || c=='\n' || c==')' || c=='}' || c==']' || c=='(' || c=='[' || c=='{' || c=='\t' || c=='@' || c=='$' || c=='%' || c=='#';
}

// bool isDelimiterOfDelimiterStr(char c)
// {
//     return isDelimiterOfToken(c) || c=='\'' || c=='\"';
// }

bool isIdentifierChar(char c)
{
    return c=='_' || isalpha(c) || isdigit(c);
}

//luu y: chỉ sử dụng hàm này khi TẤT CẢ ký tự của string là digit/alpha/_
bool isIdentifier(std::string str)
{
    return (str[0] == '_' || isalpha(str[0]));
}

bool isOpenDelimiter(char c) //for delimited string;
{
    return c==';' || c=='+' || c=='=' || c=='!' || c=='/' || c=='-' || c=='*' || c=='.' || c=='<' || c=='^' || c=='&' || c=='|' || c==',' || c=='?' || c==':' || c==' ' || c=='\n' || c=='(' || c=='{' || c=='[' || c=='\t' || c=='@' || c=='$' || c=='%' || c=='#' || c=='\'' || c=='\"';
}

bool isOpenBracket(char c)
{
    return c=='<' || c=='{' || c=='(' || c=='[';
}

bool isCloseBracketOf(char c, char openBracket)
{
    return (c=='>' && openBracket=='<') || (c==')' && openBracket=='(') || (c=='}' && openBracket=='{') || (c==']' && openBracket=='[');
}

bool isStringPostfix(char c)
{
    return c=='d' || c=='c' || c=='w';
}
bool isValidDelimitedString(std::string str)
{
    
    if (!isOpenDelimiter(str[0]))
    {
        // <id-name>\n ... \n<id-name>
        std::string id_name = "";
        int i = 0;
        while (i < str.length() && isIdentifierChar(i))
        {
            id_name.push_back(str[i]);
            ++i;
        }
        if (isIdentifier(id_name))
        {
            //nếu id_name là keyword đặc biệt
            for (std::string &s : NOTIDENTIFIER)
            {
                if (id_name == s)
                    return false;
            }
            //nếu sau id_name là ký tự cuối
            if (i == str.length()) 
                return false;
            else 
            {

                if (str[i] == '\n')
                {
                    //int idx = i;
                    id_name = "\n" + id_name;
                    std::size_t id_len = id_name.length();
                    std::size_t found = str.find(id_name);
                    if (found != std::string::npos)
                    {
                        //xét xem ví trí tìm thấy có nằm cuối ko
                        return  (found+id_len == str.length());
                    } 
                    else 
                        return false;
                }
                else 
                    return false;
            }
        }
        return false;
    }

    char openChar = str[0];
    char closeChar = str[str.length()-1];
    if (isOpenBracket(openChar))
    {
        int pivot = 0;
        if (isCloseBracketOf(closeChar, openChar))
        {
            for (int i = 1; i < str.length()-1; ++i)
            {
                if (isCloseBracketOf(str[i], openChar))
                    --pivot;
                if (str[i]==openChar)
                    ++pivot;
                if (pivot < 0)
                    return false;
            }
            return true;
        }
        else 
            return false;
    }
    else 
    {
        if (openChar == closeChar)
        {
            for (int i = 1; i < str.length()-1; ++i)
            {
                if (str[i] == openChar)
                    return false;
            }
            return true;
        }
        else 
            return false;
    }
}
