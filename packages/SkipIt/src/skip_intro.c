#include "recomp_api.h"
#include "z64extern.h"

RECOMP_CALLBACK("*", recomp_on_play_init)
void SkipIntro_OnPlayInit(PlayState* play) {
    if (!recomp_get_config_u32("skip_intro")) return;

    // If this is not intro cutscene return
    if (
        gSaveContext.save.entrance != ENTRANCE(CUTSCENE, 0) || 
        gSaveContext.save.cutsceneIndex != 0xFFF0 ||
        gSaveContext.save.isFirstCycle
    ) return;

    // These are the things that happen throughout the intro sequence and first cycle, specifically in this order:

    // Changed to Deku
    gSaveContext.save.playerForm = PLAYER_FORM_DEKU;
    // Tatl joins you
    gSaveContext.cycleSceneFlags[SCENE_OPENINGDAN].switch0 |= (1 << 0);
    gSaveContext.save.hasTatl = true;
    // Open the chest with Deku nuts
    gSaveContext.cycleSceneFlags[SCENE_OPENINGDAN].chest |= (1 << 0);
    Item_Give(play, ITEM_DEKU_NUTS_10);
    // Tatl prompts you to target baby deku tree
    gSaveContext.cycleSceneFlags[SCENE_OPENINGDAN].switch0 |= (1 << 2);
    // Talk to Mask Salesman
    gSaveContext.cycleSceneFlags[SCENE_INSIDETOWER].switch0 |= (1 << 0);
    // Enter South Clock Town
    gSaveContext.save.entrance = ENTRANCE(SOUTH_CLOCK_TOWN, 0);
    // Set first time entering South Clock Town. This variable name isn't really accurate it should be "finishedIntro" or similar
    gSaveContext.save.isFirstCycle = true;
    // Talk to tatl first time in South Clock Town
    SET_WEEKEVENTREG(WEEKEVENTREG_59_04);

    if (recomp_get_config_u32("skip_intro") < 2) return;

    // Collected clock town stray fairy - This is reset after cycle so we don't want to set it
    // gSaveContext.cycleSceneFlags[SCENE_ALLEY].switch0 |= (1 << 31);
    // SET_WEEKEVENTREG(WEEKEVENTREG_08_80);
    // Tatl's text at seeing the broken great fairy
    gSaveContext.cycleSceneFlags[SCENE_YOUSEI_IZUMI].switch0 |= (1 << 10);
    // Magic acquired
    gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
    // Skull kid drops Ocarina
    Item_Give(play, ITEM_OCARINA_OF_TIME);
    // Zelda teaches you Song of Time
    gSaveContext.save.saveInfo.inventory.questItems |= (1 << QUEST_SONG_TIME);
    // Time travel
    AMMO(ITEM_DEKU_NUT) = 0;
    gSaveContext.save.saveInfo.playerData.threeDayResetCount = 1;
    // Mask salesman teaches you Song of Healing, which transforms you
    gSaveContext.save.saveInfo.inventory.questItems |= (1 << QUEST_SONG_HEALING);
    gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
    Item_Give(play, ITEM_MASK_DEKU);
    // Talk to tatl second time in South Clock Town
    SET_WEEKEVENTREG(WEEKEVENTREG_31_04);
}
