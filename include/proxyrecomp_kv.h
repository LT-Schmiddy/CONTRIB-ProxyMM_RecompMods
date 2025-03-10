#ifndef PROXYRECOMP_KV_H
#define PROXYRECOMP_KV_H

#include "modding.h"
#include "global.h"

RECOMP_IMPORT(".", int KV_Get(const char* key, void* dest, u32 size));
RECOMP_IMPORT(".", int KV_Set(const char* key, void* data, u32 size));
RECOMP_IMPORT(".", int KV_Remove(const char* key));

#endif // PROXYRECOMP_KV_H
