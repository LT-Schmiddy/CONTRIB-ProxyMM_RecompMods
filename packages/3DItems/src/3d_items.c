#include "proxymm_custom_item.h"
#include "recomp_api.h"
#include "z64extern.h"

RECOMP_IMPORT("ProxyMM_ObjDepLoader", bool ObjDepLoader_Load(PlayState* play, u8 segment, s16 objectId));
RECOMP_IMPORT("ProxyMM_ObjDepLoader", void ObjDepLoader_Unload(PlayState* play, u8 segment, s16 objectId));

void EnItem00_Draw(Actor* thisx, PlayState* play);

typedef struct {
    /* 0x0 */ void (*drawFunc)(PlayState*, s16);
    /* 0x4 */ void* drawResources[8]; // Either display lists (Gfx*) or matrices (Mtx*)
} DrawItemTableEntry;                 // size = 0x24

extern DrawItemTableEntry sDrawItemTable[];

void EnItem00_3DItemsDraw(Actor* actor, PlayState* play) {
    EnItem00* enItem00 = (EnItem00*)actor;
    GetItemDrawId getItemDrawId = GID_NONE;
    float scale = 1.0f;
    ObjectId objectId = OBJECT_SLOT_NONE;

    if (!(enItem00->unk14E & enItem00->unk150)) {

        switch (enItem00->actor.params) {
            case ITEM00_RUPEE_GREEN:
                scale = 25.0f;
                getItemDrawId = GID_RUPEE_GREEN;
                objectId = OBJECT_GI_RUPY;
                break;
            case ITEM00_RUPEE_BLUE:
                scale = 25.0f;
                getItemDrawId = GID_RUPEE_BLUE;
                objectId = OBJECT_GI_RUPY;
                break;
            case ITEM00_RUPEE_RED:
                scale = 25.0f;
                getItemDrawId = GID_RUPEE_RED;
                objectId = OBJECT_GI_RUPY;
                break;
            case ITEM00_RUPEE_HUGE:
                scale = 17.5f;
                getItemDrawId = GID_RUPEE_HUGE;
                objectId = OBJECT_GI_RUPY;
                break;
            case ITEM00_RUPEE_PURPLE:
                scale = 17.5f;
                getItemDrawId = GID_RUPEE_PURPLE;
                objectId = OBJECT_GI_RUPY;
                break;
            case ITEM00_HEART_PIECE:
                scale = 16.0f;
                getItemDrawId = GID_HEART_PIECE;
                objectId = OBJECT_GI_HEARTS;
                break;
            case ITEM00_HEART_CONTAINER:
                scale = 16.0f;
                getItemDrawId = GID_HEART_CONTAINER;
                objectId = OBJECT_GI_HEARTS;
                break;
            case ITEM00_RECOVERY_HEART:
                scale = 16.0f;
                getItemDrawId = GID_RECOVERY_HEART;
                objectId = OBJECT_GI_HEART;
                break;
            case ITEM00_BOMBS_A:
            case ITEM00_BOMBS_B:
            case ITEM00_BOMBS_0:
                scale = 8.0f;
                getItemDrawId = GID_BOMB;
                objectId = OBJECT_GI_BOMB_1;
                break;
            case ITEM00_ARROWS_10:
                scale = 7.0f;
                getItemDrawId = GID_ARROWS_SMALL;
                objectId = OBJECT_GI_ARROW;
                break;
            case ITEM00_ARROWS_30:
                scale = 7.0f;
                getItemDrawId = GID_ARROWS_MEDIUM;
                objectId = OBJECT_GI_ARROW;
                break;
            case ITEM00_ARROWS_40:
            case ITEM00_ARROWS_50:
                scale = 7.0f;
                getItemDrawId = GID_ARROWS_LARGE;
                objectId = OBJECT_GI_ARROW;
                break;
            case ITEM00_MAGIC_JAR_SMALL:
                scale = 8.0f;
                getItemDrawId = GID_MAGIC_JAR_SMALL;
                objectId = OBJECT_GI_MAGICPOT;
                break;
            case ITEM00_MAGIC_JAR_BIG:
                scale = 8.0f;
                getItemDrawId = GID_MAGIC_JAR_BIG;
                objectId = OBJECT_GI_MAGICPOT;
                break;
            case ITEM00_DEKU_STICK:
                scale = 7.5f;
                getItemDrawId = GID_DEKU_STICK;
                objectId = OBJECT_GI_STICK;
                break;
            case ITEM00_SMALL_KEY:
                scale = 8.0f;
                getItemDrawId = GID_KEY_SMALL;
                objectId = OBJECT_GI_KEY;
                break;
            case ITEM00_DEKU_NUTS_1:
            case ITEM00_DEKU_NUTS_10:
                scale = 9.0f;
                getItemDrawId = GID_DEKU_NUTS;
                objectId = OBJECT_GI_NUTS;
                break;
            case ITEM00_SHIELD_HERO:
                scale = 1.0f;
                getItemDrawId = GID_SHIELD_HERO;
                objectId = OBJECT_GI_SHIELD_2;
                break;
            case ITEM00_MAP:
                scale = 1.0f;
                getItemDrawId = GID_DUNGEON_MAP;
                objectId = OBJECT_GI_MAP;
                break;
            case ITEM00_COMPASS:
                scale = 1.0f;
                getItemDrawId = GID_COMPASS;
                objectId = OBJECT_GI_COMPASS;
                break;
        }
    }

    if (getItemDrawId != GID_NONE) {
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        if (ObjDepLoader_Load(play, 0x06, objectId)) { 
            GetItem_Draw(play, getItemDrawId);
            ObjDepLoader_Unload(play, 0x06, objectId);
        }
    }
}

RECOMP_PATCH void GetItem_DrawRupee(PlayState* play, s16 drawId) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, sDrawItemTable[drawId].drawResources[1]);
    gSPDisplayList(POLY_OPA_DISP++, sDrawItemTable[drawId].drawResources[0]);

    // Remove outer shiny shell
    // Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    // MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    // gSPDisplayList(POLY_XLU_DISP++, sDrawItemTable[drawId].drawResources[3]);
    // gSPDisplayList(POLY_XLU_DISP++, sDrawItemTable[drawId].drawResources[2]);

    CLOSE_DISPS(play->state.gfxCtx);
}

RECOMP_PATCH void GetItem_DrawSmallRupee(PlayState* play, s16 drawId) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Scale(0.7f, 0.7f, 0.7f, MTXMODE_APPLY);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, sDrawItemTable[drawId].drawResources[1]);
    gSPDisplayList(POLY_OPA_DISP++, sDrawItemTable[drawId].drawResources[0]);

    // Remove outer shiny shell
    // Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    // MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    // gSPDisplayList(POLY_XLU_DISP++, sDrawItemTable[drawId].drawResources[3]);
    // gSPDisplayList(POLY_XLU_DISP++, sDrawItemTable[drawId].drawResources[2]);

    CLOSE_DISPS(play->state.gfxCtx);
}

RECOMP_CALLBACK("*", recomp_after_actor_init) 
void OnActorInit(PlayState* play, Actor* actor) {
    if (actor->draw == EnItem00_Draw) {
        EnItem00* enItem00 = (EnItem00*)actor;
        if (actor->params != ITEM00_HEART_PIECE || enItem00->getItemId == 0) {       
            actor->draw = EnItem00_3DItemsDraw;
        }
    }
}

RECOMP_HOOK("EnItem00_Update")
void Before_EnItem00_Update(Actor* thisx, PlayState* play) {
    EnItem00* this = (EnItem00*)thisx;

    if (this->actor.draw != EnItem00_3DItemsDraw) {
        return;
    }

    if ((this->actor.params <= ITEM00_RUPEE_RED) || ((this->actor.params == ITEM00_RECOVERY_HEART) && (this->unk152 < 0)) ||
        (this->actor.params == ITEM00_HEART_PIECE) || (this->actor.params == ITEM00_HEART_CONTAINER) ||
        (this->actor.params == ITEM00_RUPEE_HUGE) || (this->actor.params == ITEM00_RUPEE_PURPLE) ||
        (this->actor.params == ITEM00_SHIELD_HERO) || (this->actor.params == ITEM00_MAP) || (this->actor.params == ITEM00_COMPASS)) {
        return;
    }

    this->actor.shape.rot.y += 0x3C0;
}
