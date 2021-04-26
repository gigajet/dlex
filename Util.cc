#include "Util.h"

bool isDelimeterOfToken(char c)
{
    return c==';' || c=='+' || c=='=' || c=='!' || c=='/' || c=='-' || c=='*' || c=='.' || c=='>' || c=='<' || c=='^' || c=='&' || c=='|' || c==',' || c=='?' || c==':' || c==' ' || c=='\n' || c==')' || c=='}' || c==']';
}

bool isStringPostfix(char c)
{
    return c=='d' || c=='s' 
}