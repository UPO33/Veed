#pragma once

#include <cstring>

inline size_t VStrLen(const char* cstr) { return strlen(cstr); }
void VRemoveSubstring(char* str, const char* substr);

struct VString
{

};