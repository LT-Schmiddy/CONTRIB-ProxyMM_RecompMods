#include "proxymm_custom_item.h"

/**
 * When you bonk, 5 rupees will be dropped from your wallet.
 */

void BonkLosesRupees_GiveFunc(Actor* actor, PlayState* play) {
    Item_Give(play, ITEM_RUPEE_GREEN);
}

RECOMP_CALLBACK("*", recomp_should_actor_update) 
void BonkLosesRupees_ShouldActorUpdate(PlayState* play, Actor* actor, bool* should) {
    if (actor->id == ACTOR_EN_ITEM00 && actor->params == ITEM00_NOTHING && CUSTOM_ITEM_PARAM == GI_RUPEE_GREEN) {
        if (actor->bgCheckFlags & BGCHECKFLAG_GROUND) {
            // Waiting to apply the GIVE_OVERHEAD flag until the item is on the ground, so you don't immediately pick it up
            CUSTOM_ITEM_FLAGS |= GIVE_OVERHEAD;
        }
    }
}

RECOMP_HOOK("Actor_SetPlayerImpact")
s32 BonkLosesRupees_Actor_SetPlayerImpact(PlayState* play, PlayerImpactType type, s32 timer, f32 dist, Vec3f* pos) {
    if (type == PLAYER_IMPACT_BONK && gSaveContext.save.saveInfo.playerData.rupees > 0) {
        for (s32 i = 0; i < 5; i++) {
            Vec3f newPos;
            newPos.y = pos->y + 10.0f;
            newPos.x = pos->x + Rand_CenteredFloat(10.0f);
            newPos.z = pos->z + Rand_CenteredFloat(10.0f);

            if (gSaveContext.save.saveInfo.playerData.rupees > 0) {
                gSaveContext.save.saveInfo.playerData.rupees--;
                EnItem00* item = CustomItem_Spawn(play, newPos.x, newPos.y, newPos.z, 0, TOSS_ON_SPAWN, GI_RUPEE_GREEN, BonkLosesRupees_GiveFunc, NULL);
            }
        }
    }
}
