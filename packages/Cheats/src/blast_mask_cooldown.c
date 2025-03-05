#include "recomp_api.h"
#include "z64extern.h"

PlayState* sPlayState;

RECOMP_CALLBACK("*", recomp_on_play_init)
void OnPlayInit(PlayState* play) {
    sPlayState = play;
}

RECOMP_HOOK_RETURN("Player_ProcessItemButtons")
void BlastMaskCooldown_Player_ProcessItemButtons() {
    Player* player = GET_PLAYER(sPlayState);

    if (
        recomp_get_config_u32("blast_mask_cooldown") == 15 ||
        player->blastMaskTimer != 310
    ) {
        return;
    }

    player->blastMaskTimer = recomp_get_config_u32("blast_mask_cooldown") * 20;
}
