#include "recomp_api.h"
#include "z64extern.h"

#include "overlays/actors/ovl_Obj_Syokudai/z_obj_syokudai.h"

static PlayState* sPlayState;

RECOMP_CALLBACK("*", recomp_on_play_init)
void SkipOnePointCutscenes_OnPlayInit(PlayState* play) {
    sPlayState = play;
}

RECOMP_CALLBACK(".", SkipIt_ShouldCutsceneStart)
void SkipOnePointCutscenes_ShouldCutsceneStart(s16 csId, Actor* actor, bool* should) {
    if (csId == -1 || actor == NULL || !recomp_get_config_u32("skip_one_point_cutscenes")) {
        return;
    }

    switch (actor->id) {
        case ACTOR_OBJ_SYOKUDAI: { // Torch
            if (sPlayState->sceneId == SCENE_33ZORACITY) {
                // Currently this softlocks you, making you unable to interact with any actors
                break;
            }

            ObjSyokudai* torch = (ObjSyokudai*)actor;
            torch->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
            *should = false;
            break;
        }
        case ACTOR_OBJ_COMB:
            if (sPlayState->sceneId != SCENE_PIRATE) {
                actor->csId = -1;
                *should = false;
            }
            break;
        case ACTOR_OBJ_BEAN:        // Bean Patch
        case ACTOR_OBJ_MAKEKINSUTA: // Bean Patch
        case ACTOR_OBJ_SPIDERTENT:
            actor->csId = -1;
            *should = false;
            break;
        case ACTOR_EN_BOX: // Chest
            // Currently this breaks the treasure chest minigame, so we're not skipping there
            if (sPlayState->sceneId != SCENE_TAKARAYA) {
                *should = false;
            }
            break;
        case ACTOR_BG_SPDWEB: // Spider Web
        case ACTOR_DOOR_SHUTTER:
        case ACTOR_BG_NUMA_HANA: // Big wooden flower in Woodfall Temple
        case ACTOR_BG_LADDER:
        case ACTOR_OBJ_RAILLIFT: // Moving Platform
        case ACTOR_OBJ_SWITCH:
        case ACTOR_OBJ_FIRESHIELD:
        case ACTOR_OBJ_ICE_POLY:
        case ACTOR_BG_DBLUE_MOVEBG: // Moveable block in Great Bay Temple
        case ACTOR_OBJ_HUNSUI:      // Geyser in Great Bay Temple
        case ACTOR_BG_KIN2_BOMBWALL:
        case ACTOR_BG_ASTR_BOMBWALL:
        case ACTOR_BG_KIN2_PICTURE:
        case ACTOR_BG_HAKUGIN_ELVPOLE:
        case ACTOR_BG_HAKUGIN_SWITCH:
        case ACTOR_BG_HAKUGIN_POST:
        case ACTOR_OBJ_Y2SHUTTER:
        case ACTOR_OBJ_LIGHTBLOCK:
        case ACTOR_OBJ_LIGHTSWITCH:
        case ACTOR_BG_IKANA_BOMBWALL:
        case ACTOR_BG_HAKUGIN_BOMBWALL:
        case ACTOR_EN_SW:
        case ACTOR_OBJ_CHAN:
        case ACTOR_EN_MM:
        case ACTOR_BG_F40_BLOCK:
        case ACTOR_EN_BAL:
        case ACTOR_BG_TOBIRA01:
        case ACTOR_OBJ_BIGICICLE:
        case ACTOR_OBJ_HAKAISI:
        case ACTOR_BG_HAKA_BOMBWALL:
        case ACTOR_EN_DRAGON:
        case ACTOR_BG_DBLUE_BALANCE:
        case ACTOR_BG_DBLUE_WATERFALL:
        case ACTOR_OBJ_ICEBLOCK:
        case ACTOR_BG_IKNIN_SUSCEIL:
        case ACTOR_BG_IKANA_DHARMA:
        case ACTOR_OBJ_HUGEBOMBIWA:
            *should = false;
            break;
        default:
            break;
    }
}

RECOMP_PATCH void EnBal_PopBalloon(EnBal* this, PlayState* play) {
    this->picto.actor.focus.pos.x = this->picto.actor.world.pos.x;
    this->picto.actor.focus.pos.y = this->picto.actor.world.pos.y + 20.0f;
    this->picto.actor.focus.pos.z = this->picto.actor.world.pos.z + 30.0f;
    if (this->picto.actor.csId != CS_ID_NONE) {
        if (CutsceneManager_IsNext(this->picto.actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->picto.actor.csId, &this->picto.actor);
            // @ProxyMM We don't want to set the camera focus to the actor if we're skipping cutscenes
            if (!recomp_get_config_u32("skip_one_point_cutscenes")) {
                Camera_SetFocalActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId)),
                                     &this->picto.actor);
            }
        } else {
            CutsceneManager_Queue(this->picto.actor.csId);
        }
    }

    if (this->timer > 30) {
        this->timer = 0;
        EnBal_SetupFall(this);
    } else if (this->timer > 10) {
        // Freeze animation and induce blink
        this->skelAnime.playSpeed = 0.0f;
        if (this->eyeTimer > 6) {
            this->eyeTimer = 6;
        }
        this->timer++;
    } else {
        this->timer++;
    }
}
