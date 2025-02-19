#include "proxymm_custom_item.h"

/**
 * Replaces freestanding PoH with a custom item, which uses the more detailed 
 * heart piece model, and skips the Give Item cutscene when picked up.
 */

void BetterPoH_GiveFunc(Actor* actor, PlayState* play) {
    Item_Give(play, ITEM_HEART_PIECE);
}

RECOMP_CALLBACK("*", recomp_should_actor_init) 
void BetterPoH_ShouldActorInit(PlayState* play, Actor* actor, bool* should) {
    if (actor->id == ACTOR_EN_ITEM00) {
        s32 params = actor->params & 0xFF;
        if (params == ITEM00_HEART_PIECE) {
            *should = false;

            CustomItem_Spawn(play, 
                actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, actor->shape.rot.y, 
                GIVE_OVERHEAD, GI_HEART_PIECE, BetterPoH_GiveFunc, NULL);
        }
    }
}
