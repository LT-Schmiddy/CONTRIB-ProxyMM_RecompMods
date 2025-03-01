#include "modding.h"
#include "global.h"
#include "overlays/actors/ovl_En_Torch2/z_en_torch2.h"

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));
extern u8 sPlaybackState;

#define NOT_OCARINA_ACTION_BALAD_WIND_FISH                                       \
    (sPlayState->msgCtx.ocarinaAction < OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN || \
     sPlayState->msgCtx.ocarinaAction > OCARINA_ACTION_PROMPT_WIND_FISH_DEKU)

static PlayState* sPlayState;

RECOMP_CALLBACK("*", recomp_on_play_init)
void SkipSongPlayback_OnPlayInit(PlayState* play) {
    sPlayState = play;
}

RECOMP_HOOK_RETURN("Player_Update")
void SkipSongPlayback_OnPlayerUpdate() {
    if (!recomp_get_config_u32("skip_song_playback")) return;

    Player* player = GET_PLAYER(sPlayState);

    if (sPlayState->msgCtx.msgMode >= MSGMODE_SONG_PLAYED && sPlayState->msgCtx.msgMode <= MSGMODE_17 &&
        !sPlayState->csCtx.state && NOT_OCARINA_ACTION_BALAD_WIND_FISH) {
        if (sPlayState->msgCtx.stateTimer > 1) {
            sPlayState->msgCtx.stateTimer = 1;
        }
        sPlayState->msgCtx.ocarinaSongEffectActive = 0;
        sPlaybackState = 0;
    }
}

RECOMP_CALLBACK("*", recomp_should_actor_init) 
void SkipSongPlayback_ShouldActorInit(PlayState* play, Actor* actor, bool* should) {
    if (actor->id != ACTOR_EFF_CHANGE) return;

    if (!recomp_get_config_u32("skip_song_playback")) return;

    *should = false;
    Player* player = GET_PLAYER(sPlayState);

    if (player->av2.actionVar2 < 10) {
        player->av2.actionVar2 = 10;
    } else if (player->av2.actionVar2 < 90) {
        EnTorch2* torch2 = sPlayState->actorCtx.elegyShells[player->transformation];
        if (torch2 != NULL) {
            torch2->framesUntilNextState = 1;
        }

        player->av2.actionVar2 = 90;
    }
}

void EnTorch2_Update(Actor* thisx, PlayState* play);

RECOMP_CALLBACK("*", recomp_on_play_main)
void SkipSongPlayback_OnPlayMain(PlayState* play) {
    if (!recomp_get_config_u32("skip_song_playback")) return;

    Actor* actor = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;

    while (actor != NULL) {
        if (actor->id == ACTOR_EN_TORCH2) {
            EnTorch2* enTorch2 = actor;
            if (enTorch2->framesUntilNextState == 0 && enTorch2->state != TORCH2_STATE_SOLID) {
                // Run the update func another 2 times to speed up the fade in/out
                EnTorch2_Update(actor, play);
                EnTorch2_Update(actor, play);
            }
        }

        actor = actor->next;
    }
}
