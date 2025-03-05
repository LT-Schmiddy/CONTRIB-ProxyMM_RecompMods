#include "recomp_api.h"
#include "z64extern.h"

static PlayState* sPlayState;
static s32 actorCatExplosivesLength = -1;

RECOMP_HOOK("Player_UseItem")
void NoExplosiveLimit_Player_UseItem(PlayState* play, Player* this, ItemId item) {
    sPlayState = play;

    if (!recomp_get_config_u32("no_explosive_limit")) return;
    if (item != ITEM_BOMB && item != ITEM_BOMBCHU && item != ITEM_POWDER_KEG) return;

    actorCatExplosivesLength = sPlayState->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].length;
    sPlayState->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].length = 0;
}

RECOMP_HOOK_RETURN("Player_UseItem")
void NoExplosiveLimit_After_Player_UseItem() {
    if (actorCatExplosivesLength != -1) {
        sPlayState->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].length = actorCatExplosivesLength;
        actorCatExplosivesLength = -1;
    }
}
