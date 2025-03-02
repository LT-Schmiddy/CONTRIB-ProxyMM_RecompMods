#ifndef __RECOMP_API_H__
#define __RECOMP_API_H__

#include "modding.h"
#include "global.h"

RECOMP_IMPORT("*", void* recomp_alloc(size_t size));
RECOMP_IMPORT("*", void recomp_free(void* ptr));

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));
RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));

#endif
