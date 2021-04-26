#ifndef UTIL_H_
#define UTIL_H_

#include <string>

bool isIdentifierChar(char c);
bool isDelimiterOfToken(char c);
bool isStringPostfix(char c);
bool isOpenDelimiter(char c);
bool isValidDelimitedString(std::string str);
bool isOpenBracket(char c);
bool isCloseBracketOf(char c, char openBracket);
bool isIdentifier(std::string str);

static std::string NOTIDENTIFIER[] = {
    "__FILE__",
    "__FILE_FULL_PATH__",
    "__MODULE__",
    "__LINE__",
    "__FUNCTION__",
    "__PRETTY_FUNCTION__",
    "__gshared",
    "__traits",
    "__vector",
    "__parameters",
    "__DATE__",
    "__EOF__",
    "__TIME__",
    "__TIMESTAMP",
    "__VENDOR__",
    "__VERSION__"
};
#endif