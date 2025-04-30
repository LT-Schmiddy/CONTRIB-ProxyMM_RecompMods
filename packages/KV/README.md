# KV
A simple KV Store for N64Recomp projects

## Usage
> Note, this interface may change in the future, or I may provide additional shorthands for getting/setting primitive datatypes

Add the dependency in your `mod.toml`
```diff
dependencies = [
    "ProxyMM_KV:0.0.3"
]
```

Add the header to your project
```h
#ifndef PROXYMM_KV_H
#define PROXYMM_KV_H

#include "modding.h"
#include "global.h"

// Global
RECOMP_IMPORT("ProxyMM_KV", bool KV_Global_Get(const char* key, void* dest, u32 size));
RECOMP_IMPORT("ProxyMM_KV", u8   KV_Global_Get_U8(const char* key, u8 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", s8   KV_Global_Get_S8(const char* key, s8 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", u16  KV_Global_Get_U16(const char* key, u16 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", s16  KV_Global_Get_S16(const char* key, s16 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", u32  KV_Global_Get_U32(const char* key, u32 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", s32  KV_Global_Get_S32(const char* key, s32 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Global_Set(const char* key, void* data, u32 size));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Global_Set_U8(const char* key, u8 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Global_Set_S8(const char* key, s8 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Global_Set_U16(const char* key, u16 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Global_Set_S16(const char* key, s16 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Global_Set_U32(const char* key, u32 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Global_Set_S32(const char* key, s32 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Global_Has(const char* key));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Global_Remove(const char* key));

// Save Slot based
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Get(const char* key, void* dest, u32 size));
RECOMP_IMPORT("ProxyMM_KV", u8   KV_Slot_Get_U8(const char* key, u8 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", s8   KV_Slot_Get_S8(const char* key, s8 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", u16  KV_Slot_Get_U16(const char* key, u16 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", s16  KV_Slot_Get_S16(const char* key, s16 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", u32  KV_Slot_Get_U32(const char* key, u32 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", s32  KV_Slot_Get_S32(const char* key, s32 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Set(const char* key, void* data, u32 size));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Set_U8(const char* key, u8 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Set_S8(const char* key, s8 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Set_U16(const char* key, u16 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Set_S16(const char* key, s16 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Set_U32(const char* key, u32 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Set_S32(const char* key, s32 value));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Has(const char* key));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Remove(const char* key));

#endif // PROXYMM_KV_H
```

Start Getting/Setting values

```cpp
u32 myValue = 4941;
KV_Slot_Set_U32("MyMod.MyValue", myValue);

u32 otherValue = KV_Slot_Get_U32("MyMod.MyValue", 1337);

assert(myValue == otherValue);
```

Getting complex values
```cpp
struct MyStruct {
    u32 a;
    u32 b;
};

MyStruct myStruct = { 1, 2 };
KV_Slot_Set("MyMod.MyStruct", &myStruct, sizeof(MyStruct));
MyStruct otherStruct;
// Get returns truthy if the value was retrieved
if (KV_Slot_Get("MyMod.MyStruct", &otherStruct, sizeof(MyStruct))) {
    assert(myStruct.a == otherStruct.a);
    assert(myStruct.b == otherStruct.b);
}
```
