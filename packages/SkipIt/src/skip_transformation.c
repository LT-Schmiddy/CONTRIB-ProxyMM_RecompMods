#include "modding.h"
#include "global.h"

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));
extern AdjLightSettings D_80862B50;
extern PlayerAnimationHeader* D_8085D160[PLAYER_FORM_MAX];
extern ActorOverlay gActorOverlayTable[];
void Player_Action_86(Player* this, PlayState* play);
void Player_Anim_PlayOnceMorphAdjusted(PlayState* play, Player* this, PlayerAnimationHeader* anim);
void Player_SetAction_PreserveItemAction(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 arg3);
void func_8082DE50(PlayState* play, Player* this);

void CustomTransform(Player* this, PlayState* play) {
    // This was mostly copied directly from func_8012301C within z_player_lib.c
    s16 objectId = gPlayerFormObjectIds[GET_PLAYER_FORM];

    gActorOverlayTable[ACTOR_PLAYER].profile->objectId = objectId;
    func_8012F73C(&play->objectCtx, this->actor.objectSlot, objectId);
    this->actor.objectSlot = Object_GetSlot(&play->objectCtx, GAMEPLAY_KEEP);

    s32 objectSlot = Object_GetSlot(&play->objectCtx, gActorOverlayTable[ACTOR_PLAYER].profile->objectId);
    this->actor.objectSlot = objectSlot;
    this->actor.shape.rot.z = GET_PLAYER_FORM + 1;
    this->actor.init = PlayerCall_Init;
    this->actor.update = PlayerCall_Update;
    this->actor.draw = PlayerCall_Draw;
    gSaveContext.save.equippedMask = PLAYER_MASK_NONE;

    TransitionFade_SetColor(&play->unk_18E48, 0x000000);
    R_TRANS_FADE_FLASH_ALPHA_STEP = -1;
    Player_PlaySfx(GET_PLAYER(play), NA_SE_SY_TRANSFORM_MASK_FLASH);

    // Clear previous mask to prevent crashing with masks being drawn while we switch transformations
    if (this->transformation == PLAYER_FORM_HUMAN) {
        this->prevMask = PLAYER_MASK_NONE;
    }
}

RECOMP_PATCH void func_808388B8(PlayState* play, Player* this, PlayerTransformation playerForm) {
    func_8082DE50(play, this);
    // @ProxyMM Use our own ActionFunc instead of Player_Action_86
    if (recomp_get_config_u32("skip_transformation")) {
        Player_SetAction_PreserveItemAction(play, this, CustomTransform, 0);
    } else {
        Player_SetAction_PreserveItemAction(play, this, Player_Action_86, 0);
    }
    Player_Anim_PlayOnceMorphAdjusted(play, this, D_8085D160[this->transformation]);
    gSaveContext.save.playerForm = playerForm;
    this->stateFlags1 |= PLAYER_STATE1_2;

    D_80862B50 = play->envCtx.adjLightSettings;
    this->actor.velocity.y = 0.0f;
    Actor_DeactivateLens(play);
}
