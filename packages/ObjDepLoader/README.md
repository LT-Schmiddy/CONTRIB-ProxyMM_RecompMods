# ObjDepLoader
A simple workaround for object dependency.

## Usage
This is primarily to work around objects needing to be loaded for drawing commands. For instance you cannot simply draw the Deku Mask GI from anywhere, or draw DL's from different objects within the same actor without a bunch of complicated logic. With this library this is made simple:

```cpp
// Import the functions
RECOMP_IMPORT("ProxyMM_ObjDepLoader", bool ObjDepLoader_Load(PlayState* play, u8 segment, s16 objectId));
RECOMP_IMPORT("ProxyMM_ObjDepLoader", void ObjDepLoader_Unload(u8 segment, s16 objectId));

void MyActor_Draw(Actor* actor, PlayState* play) {
    if (ObjDepLoader_Load(play, 0x06, OBJECT_GI_NUTSMASK)) {
        Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
        GetItem_Draw(play, GID_MASK_DEKU);

        ObjDepLoader_Unload(0x06, OBJECT_GI_NUTSMASK);
    }

    if (ObjDepLoader_Load(play, 0x06, OBJECT_GI_GOLONMASK)) {
        Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
        GetItem_Draw(play, GID_MASK_GORON);

        ObjDepLoader_Unload(0x06, OBJECT_GI_GOLONMASK);
    }
}
```

## Technical
Currently, this is achieved with a very naive approach, a static location in which all objects can be simultaneously loaded into and never unload. Later on I may improve it to be more dilegent and clean up it's un-used objects as things are "unloaded", but the API will stay the same.
