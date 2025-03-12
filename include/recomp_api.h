#ifndef __RECOMP_API_H__
#define __RECOMP_API_H__

#include "modding.h"
#include "global.h"

RECOMP_IMPORT("*", void* recomp_alloc(unsigned long size));
RECOMP_IMPORT("*", void recomp_free(void* memory));
RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));
RECOMP_IMPORT("*", double recomp_get_config_double(const char* key));
RECOMP_IMPORT("*", char* recomp_get_config_string(const char* key));
RECOMP_IMPORT("*", void recomp_free_config_string(char* str));

#endif
