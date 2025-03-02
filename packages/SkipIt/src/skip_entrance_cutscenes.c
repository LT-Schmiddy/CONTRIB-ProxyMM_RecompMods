#include "recomp_api.h"
#include "z64extern.h"

static PlayState* sPlayState;

RECOMP_CALLBACK("*", recomp_on_play_init)
void SkipEntranceCutscenes_OnPlayInit(PlayState* play) {
    sPlayState = play;
}

RECOMP_PATCH void Cutscene_HandleEntranceTriggers(PlayState* play) {
    s32 pad;
    s16 csId;
    SceneTableEntry* scene;
    s32 scriptIndex;

    if (((gSaveContext.gameMode == GAMEMODE_NORMAL) || (gSaveContext.gameMode == GAMEMODE_TITLE_SCREEN)) &&
        (gSaveContext.respawnFlag <= 0)) {
        // Try to find an actor cutscene that's triggered by the current spawn
        csId = CutsceneManager_FindEntranceCsId();
        if (csId != CS_ID_NONE) {
            scriptIndex = CutsceneManager_GetCutsceneScriptIndex(csId);
            if (scriptIndex != CS_SCRIPT_ID_NONE) {
                // A scripted cutscene is triggered by a spawn
                if ((play->csCtx.scriptList[scriptIndex].spawnFlags != CS_SPAWN_FLAG_NONE) &&
                    (gSaveContext.respawnFlag == 0)) {
                    if (play->csCtx.scriptList[scriptIndex].spawnFlags == CS_SPAWN_FLAG_ALWAYS) {
                        // Entrance cutscenes that always run
                        CutsceneManager_Start(csId, NULL);
                        gSaveContext.showTitleCard = false;

                    } else if (!CHECK_CS_SPAWN_FLAG_WEEKEVENTREG(play->csCtx.scriptList[scriptIndex].spawnFlags)) {
                        // Entrance cutscenes that only run once
                        SET_CS_SPAWN_FLAG_WEEKEVENTREG(play->csCtx.scriptList[scriptIndex].spawnFlags);
                        if (!recomp_get_config_u32("skip_entrance_cutscenes")) {
                            CutsceneManager_Start(csId, NULL);
                        }
                        // The title card will be used by the cs misc command if necessary.
                        gSaveContext.showTitleCard = false;
                    }
                }
            } else {
                // A non-scripted cutscene is triggered by a spawn
                CutsceneManager_StartWithPlayerCs(csId, NULL);
            }
        }
    }

    if ((gSaveContext.respawnFlag == 0) || (gSaveContext.respawnFlag == -2)) {
        scene = play->loadedScene;
        if ((scene->titleTextId != 0) && gSaveContext.showTitleCard) {
            if ((Entrance_GetTransitionFlags(((void)0, gSaveContext.save.entrance) +
                                             ((void)0, gSaveContext.sceneLayer)) &
                 0x4000) != 0) {
                Message_DisplaySceneTitleCard(play, scene->titleTextId);
            }
        }

        gSaveContext.showTitleCard = true;
    }
}

RECOMP_CALLBACK(".", SkipIt_ShouldCutsceneStart)
void SkipEntranceCutscenes_ShouldCutsceneStart(s16 csId, Actor* actor, bool* should) {
    if (actor == NULL || actor->id != ACTOR_OBJ_DEMO || !recomp_get_config_u32("skip_entrance_cutscenes")) {
        return;
    }

    // Ikana Castle entrance cutscene
    if (sPlayState->sceneId == SCENE_CASTLE && (csId == 0 || csId == 1)) {
        *should = false;
    }

    // Snowhead entrance cutscene
    if (sPlayState->sceneId == SCENE_HAKUGIN && (csId == 24 || csId == 25)) {
        *should = false;
    }

    // Pirate Fortress entrance cutscene
    if (sPlayState->sceneId == SCENE_TORIDE && csId == 12) {
        *should = false;
    }
}
