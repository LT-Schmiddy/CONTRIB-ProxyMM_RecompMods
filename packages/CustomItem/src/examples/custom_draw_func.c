#include "proxymm_custom_item.h"

RECOMP_IMPORT("ProxyMM_ObjDepLoader", bool ObjDepLoader_Load(PlayState* play, u8 segment, s16 objectId));
RECOMP_IMPORT("ProxyMM_ObjDepLoader", void ObjDepLoader_Unload(u8 segment, s16 objectId));

/**
 * Replaces the heart pieces in South clock town with a scarecrow item that kills you.
 */

void CustomDrawFunc_GiveFunc(Actor* actor, PlayState* play) {
    gSaveContext.save.saveInfo.playerData.health = 0;
}

extern SkeletonHeader object_ka_Skel_0065B0;
extern AnimationHeader object_ka_Anim_000214;

void CustomDrawFunc_DrawFunc(Actor* actor, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    static bool initialized = false;
    static SkelAnime skelAnime;
    static u32 lastUpdate = 0;

    if (ObjDepLoader_Load(play, 0x06, OBJECT_KA)) {
        if (!initialized) {
            initialized = true;
            SkelAnime_InitFlex(play, &skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);
        }
        if (play != NULL && lastUpdate != play->state.frames) {
            lastUpdate = play->state.frames;
            SkelAnime_Update(&skelAnime);
        }

        Matrix_Translate(0.0f, -1000.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(0.2f, 0.2f, 0.2f, MTXMODE_APPLY);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        SkelAnime_DrawFlexOpa(play, skelAnime.skeleton, skelAnime.jointTable, skelAnime.dListCount, NULL, NULL, NULL);

        ObjDepLoader_Unload(0x06, OBJECT_KA);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

RECOMP_CALLBACK("*", recomp_should_actor_init) 
void CustomDrawFunc_ShouldActorInit(PlayState* play, Actor* actor, bool* should) {
    if (play->sceneId == SCENE_CLOCKTOWER && actor->id == ACTOR_EN_ITEM00) {
        s32 params = actor->params & 0xFF;
        if (params == ITEM00_HEART_PIECE) {
            *should = false;

            CustomItem_Spawn(play, 
                actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, actor->shape.rot.y, 
                GIVE_OVERHEAD, 0, CustomDrawFunc_GiveFunc, CustomDrawFunc_DrawFunc);
        }
    }
}

