#ifndef PROXYMM_Z64_EXTERN_H
#define PROXYMM_Z64_EXTERN_H

#include "global.h"
#include "zelda_arena.h"
#include "gfxalloc.h"
#include "overlays/gamestates/ovl_select/z_select.h"
#include "overlays/gamestates/ovl_title/z_title.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
#include "overlays/actors/ovl_Arms_Hook/z_arms_hook.h"
#include "overlays/actors/ovl_En_Bal/z_en_bal.h"

// This file just houses any externs I need throughout my mods that aren't available in headers already

#define SPEED_MODE_LINEAR 0.0f
#define GET_PLAYER_ANIM(group, type) ((PlayerAnimationHeader**)D_8085BE84)[group * PLAYER_ANIMTYPE_MAX + type]

typedef enum {
    /* 0 */ CS_START_0,
    /* 1 */ CS_START_1,
    /* 2 */ CS_START_2
} ActorCutsceneStartMethod;

typedef struct {
    /* 0x00 */ s16 csId;
    /* 0x02 */ s16 length;
    /* 0x04 */ s16 endCsId;
    /* 0x06 */ s16 subCamId;
    /* 0x08 */ Actor* targetActor;
    /* 0x0C */ s32 startMethod;
    /* 0x10 */ PlayState* play;
    /* 0x14 */ s16 retCamId;
    /* 0x16 */ s16 isCameraStored;
} CutsceneManager; // size = 0x18

typedef struct GetItemEntry {
    /* 0x0 */ u8 itemId;
    /* 0x1 */ u8 field; // various bit-packed data
    /* 0x2 */ s8 gid;   // defines the draw id and chest opening animation
    /* 0x3 */ u8 textId;
    /* 0x4 */ u16 objectId;
} GetItemEntry; // size = 0x6

typedef struct {
    /* 0x0 */ s8 letterboxTarget;
    /* 0x1 */ s8 letterboxSize;
    /* 0x2 */ s8 pillarboxTarget;
    /* 0x3 */ s8 pillarboxSize;
} ShrinkWindow; // size = 0x4

extern ActorOverlay gActorOverlayTable[];
extern AdjLightSettings D_80862B50;
extern CutsceneManager sCutsceneMgr;
extern f32 D_8085D958[2];
extern f32 sPlayerYDistToFloor;
extern f32 sWaterSpeedFactor;
extern GetItemEntry sGetItemTable[GI_MAX - 1];
extern Gfx gHookshotReticleDL[];
extern Input* sPlayerControlInput;
extern PlayerAnimationHeader gPlayerAnim_link_normal_defense_kiru;
extern PlayerAnimationHeader gPlayerAnim_link_normal_landing;
extern PlayerAnimationHeader gPlayerAnim_link_normal_newroll_jump_20f;
extern PlayerAnimationHeader gPlayerAnim_pn_attack;
extern PlayerAnimationHeader* D_8085BE84[PLAYER_ANIMGROUP_MAX][PLAYER_ANIMTYPE_MAX];
extern PlayerAnimationHeader* D_8085D160[PLAYER_FORM_MAX];
extern s32 D_801F6B08;
extern s32 sCharTexScale;
extern s32 sCharTexSize;
extern s32 sPlayerUseHeldItem;
extern s32 sSavedCurrentMask;
extern u16 sPersistentCycleWeekEventRegs[ARRAY_COUNT(gSaveContext.save.saveInfo.weekEventReg)];
extern u8 D_801C6A70;
extern u8 sPlaybackState;
extern u8 sWaitingCutsceneList[16];
extern uintptr_t gSegments[NUM_SEGMENTS];
extern PlayerAgeProperties sPlayerAgeProperties[PLAYER_FORM_MAX];
extern FlexSkeletonHeader* gKafeiSkel;
extern struct VisFbuf* sPlayVisFbufInstance;
extern ShrinkWindow* sShrinkWindowPtr;

PlayerAnimationHeader* Player_GetIdleAnim(Player* this);
void Player_Anim_PlayOnceMorph(PlayState* play, Player* this, PlayerAnimationHeader* anim);
PlayerAnimationHeader* func_8082ED20(Player* this);
Actor* Actor_SpawnEntry(ActorContext* actorCtx, ActorEntry* actorEntry, PlayState* play);
ActorProfile* Actor_LoadOverlay(ActorContext* actorCtx, s16 index);
bool func_808313A8(PlayState* play, Player* this, Actor* actor);
bool Player_UpdateUpperBody(Player* this, PlayState* play);
CutsceneEntry* CutsceneManager_GetCutsceneEntryImpl(s16 csId);
s16 CutsceneManager_SetHudVisibility(s16 csHudVisibility);
s32 func_80839770(Player* this, PlayState* play);
s32 func_8083CBC4(Player* this, f32 arg1, s16 arg2, f32 arg3, f32 arg4, f32 arg5, s16 arg6);
s32 func_8083D860(Player* this, PlayState* play);
s32 func_8084C124(PlayState* play, Player* this);
s32 Inventory_ReplaceItem(PlayState* play, u8 oldItem, u8 newItem);
s32 Player_ActionHandler_13(Player* this, PlayState* play);
s32 Player_GetMovementSpeedAndYaw(Player* this, f32* outSpeedTarget, s16* outYawTarget, f32 speedMode, PlayState* play);
s32 Player_SetAction(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 arg3);
u32 SurfaceType_GetData(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId, s32 dataIdx);
void Actor_AddToCategory(ActorContext* actorCtx, Actor* actor, u8 actorCategory);
void Actor_FreeOverlay(ActorOverlay* entry);
void Actor_Init(Actor* actor, PlayState* play);
void Actor_KillAllOnHalfDayChange(PlayState* play, ActorContext* actorCtx);
void ArmsHook_SetupAction(ArmsHook* this, ArmsHookActionFunc actionFunc);
void ArmsHook_Shoot(ArmsHook* this, PlayState* play);
void EnBal_SetupFall(EnBal* this);
void EnTorch2_Update(Actor* thisx, PlayState* play);
void FileSelect_Init_NORELOCATE(GameState*);
void func_8082DE50(PlayState* play, Player* this);
void func_8083375C(Player* this, PlayerMeleeWeaponAnimation meleeWeaponAnim);
void func_80836AD8(PlayState* play, Player* this);
void func_80837134(PlayState* play, Player* this);
void func_808378FC(PlayState* play, Player* this);
void func_80837CEC(PlayState* play, Player* this, CollisionPoly* arg2, f32 arg3, PlayerAnimationHeader* anim);
void func_80840980(Player* this, u16 sfxId);
void func_808409A8(PlayState* play, Player* this, f32 speed, f32 yVelocity);
void Inventory_DeleteItem(s16 item, s16 slot);
void KaleidoScope_MoveCursorFromSpecialPos(PlayState* play);
void MapSelect_Draw(MapSelectState* this);
void MapSelect_Init_NORELOCATE(GameState*);
void MapSelect_LoadConsoleLogo(MapSelectState* this, u32 entrance, s32 spawn);
void MapSelect_LoadGame(MapSelectState* this, u32 entrance, s32 spawn);
void Message_FindMessage(PlayState* play, u16 textId);
void Player_Action_26(Player* this, PlayState* play);
void Player_Action_57(Player* this, PlayState* play);
void Player_Action_86(Player* this, PlayState* play);
void Player_Anim_PlayOnce(PlayState* play, Player* this, PlayerAnimationHeader* anim);
void Player_Anim_PlayOnceMorphAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim);
void Player_AnimSfx_PlayVoice(Player* this, u16 sfxId);
void Player_DrawGameplay(PlayState* play, Player* this, s32 lod, Gfx* cullDList, OverrideLimbDrawFlex overrideLimbDraw);
void Player_DrawHookshotReticle(PlayState* play, Player* player, f32 hookshotDistance);
void Player_SetAction_PreserveItemAction(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 arg3);
void Player_StartTalking(PlayState* play, Actor* actor);
void Player_StopHorizontalMovement(Player* this);
void ShrinkWindow_Letterbox_SetSizeTarget(s32 target);

#endif // PROXYMM_Z64_EXTERN_H
