#include "inputbuf.h"

using namespace std;

bool InputBuffer::EndOfInput()
{
    if (!input_buffer.empty())
        return false;
    else
        return stream->eof();
}

char InputBuffer::UngetChar(char c)
{
    if (c != EOF)
        input_buffer.push_back(c);;
    return c;
}

void InputBuffer::GetChar(char& c)
{
    if (!input_buffer.empty()) {
        c = input_buffer.back();
        input_buffer.pop_back();
    } else {
        stream->get(c);
        if (stream->eof()) {
            c=EOF;
        }
    }
}

string InputBuffer::UngetString(string s)
{
    for (int i = 0; i < s.size(); i++)
        input_buffer.push_back(s[s.size()-i-1]);
    return s;
}

