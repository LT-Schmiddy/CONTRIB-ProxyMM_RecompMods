#include "proxymm_custom_item.h"

/**
 * Places some non-interactable items at the stand in South Clock Town.
 */

RECOMP_CALLBACK("*", recomp_after_play_init) 
void SouthClockTownDisplay_AfterPlayInit(PlayState* play) {
    if (play->sceneId != SCENE_CLOCKTOWER) return;

    CustomItem_Spawn(play, -487.0f, 34.0f, -245.0f, 0, STOP_BOBBING | STOP_SPINNING, GI_WALLET_ADULT, NULL, NULL);
    CustomItem_Spawn(play, -487.0f, 34.0f, -205.0f, 0, STOP_BOBBING | STOP_SPINNING, GI_WALLET_GIANT, NULL, NULL);
    CustomItem_Spawn(play, -487.0f, 34.0f, -165.0f, 0, STOP_BOBBING | STOP_SPINNING, GI_CHATEAU_BOTTLE, NULL, NULL);
}
