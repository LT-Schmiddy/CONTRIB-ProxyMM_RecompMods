#include "recomp_api.h"
#include "z64extern.h"

RECOMP_IMPORT(".", bool KV_Get(const char* key, void* dest, u32 size, u8 slot));
RECOMP_IMPORT(".", bool KV_Set(const char* key, void* data, u32 size, u8 slot));
RECOMP_IMPORT(".", bool KV_Has(const char* key, u8 slot));
RECOMP_IMPORT(".", bool KV_Remove(const char* key, u8 slot));

RECOMP_EXPORT bool KV_Global_Get(const char* key, void* dest, u32 size) {
    return KV_Get(key, dest, size, 255);
}

#define DEFINE_KV_GLOBAL_GETTER(type, suffix) \
RECOMP_EXPORT type KV_Global_Get_##suffix(const char* key, type defaultValue) { \
    type value = defaultValue; \
    KV_Get(key, &value, sizeof(type), 255); \
    return value; \
}

DEFINE_KV_GLOBAL_GETTER(u8, U8)
DEFINE_KV_GLOBAL_GETTER(s8, S8)
DEFINE_KV_GLOBAL_GETTER(u16, U16)
DEFINE_KV_GLOBAL_GETTER(s16, S16)
DEFINE_KV_GLOBAL_GETTER(u32, U32)
DEFINE_KV_GLOBAL_GETTER(s32, S32)

RECOMP_EXPORT bool KV_Global_Set(const char* key, void* data, u32 size) {
    return KV_Set(key, data, size, 255);
}

#define DEFINE_KV_GLOBAL_SETTER(type, suffix) \
RECOMP_EXPORT bool KV_Global_Set_##suffix(const char* key, type value) { \
    return KV_Set(key, &value, sizeof(type), 255); \
}

DEFINE_KV_GLOBAL_SETTER(u8, U8)
DEFINE_KV_GLOBAL_SETTER(s8, S8)
DEFINE_KV_GLOBAL_SETTER(u16, U16)
DEFINE_KV_GLOBAL_SETTER(s16, S16)
DEFINE_KV_GLOBAL_SETTER(u32, U32)
DEFINE_KV_GLOBAL_SETTER(s32, S32)

RECOMP_EXPORT bool KV_Global_Has(const char* key) {
    return KV_Has(key, 255);
}

RECOMP_EXPORT bool KV_Global_Remove(const char* key) {
    return KV_Remove(key, 255);
}

RECOMP_EXPORT bool KV_Slot_Get(const char* key, void* data, u32 size) {
    return KV_Get(key, data, size, gSaveContext.fileNum);
}

#define DEFINE_KV_SLOT_GETTER(type, suffix) \
RECOMP_EXPORT type KV_Slot_Get_##suffix(const char* key, type defaultValue) { \
    type value = defaultValue; \
    KV_Get(key, &value, sizeof(type), gSaveContext.fileNum); \
    return value; \
}

DEFINE_KV_SLOT_GETTER(u8, U8)
DEFINE_KV_SLOT_GETTER(s8, S8)
DEFINE_KV_SLOT_GETTER(u16, U16)
DEFINE_KV_SLOT_GETTER(s16, S16)
DEFINE_KV_SLOT_GETTER(u32, U32)
DEFINE_KV_SLOT_GETTER(s32, S32)

RECOMP_EXPORT bool KV_Slot_Set(const char* key, void* data, u32 size) {
    return KV_Set(key, data, size, gSaveContext.fileNum);
}

#define DEFINE_KV_SLOT_SETTER(type, suffix) \
RECOMP_EXPORT bool KV_Slot_Set_##suffix(const char* key, type value) { \
    return KV_Set(key, &value, sizeof(type), gSaveContext.fileNum); \
}

DEFINE_KV_SLOT_SETTER(u8, U8)
DEFINE_KV_SLOT_SETTER(s8, S8)
DEFINE_KV_SLOT_SETTER(u16, U16)
DEFINE_KV_SLOT_SETTER(s16, S16)
DEFINE_KV_SLOT_SETTER(u32, U32)
DEFINE_KV_SLOT_SETTER(s32, S32)

RECOMP_EXPORT bool KV_Slot_Has(const char* key) {
    return KV_Has(key, gSaveContext.fileNum);
}

RECOMP_EXPORT bool KV_Slot_Remove(const char* key) {
    return KV_Remove(key, gSaveContext.fileNum);
}
