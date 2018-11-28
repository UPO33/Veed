#include "VString.h"

// const char* VFindLast(const char* cstr, const char* chars)
// {
//
//}

void VRemoveSubstring(char* s, const char* toremove)
{
	auto toremoveLen = strlen(toremove);
	while (s = strstr(s, toremove))
		memmove(s, s + toremoveLen, 1 + strlen(s + toremoveLen));
}
