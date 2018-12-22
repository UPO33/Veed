#include "VMeta.h"
#include "VString.h"


#ifdef VCOMPILER_GCC
#include <cxxabi.h>
#endif


VName VGetTypeidNameFixed(const std::type_info& ti)
{
#ifdef VCOMPILER_MSVC
	char output_buffer[4096];
	strcpy(output_buffer, ti.name());
#else
	int status = 0;
	char output_buffer[4096];
	size_t len = 4096;
	char* demangledName = abi::__cxa_demangle(ti.name(), output_buffer, &len, &status);
	VASSERT(status == 0);
#endif

	VRemoveSubstring(output_buffer, "class ");
	VRemoveSubstring(output_buffer, "struct ");
	VRemoveSubstring(output_buffer, "enum ");
	VRemoveSubstring(output_buffer, "__ptr64");
	VRemoveSubstring(output_buffer, " ");
	return VName(output_buffer);
}

