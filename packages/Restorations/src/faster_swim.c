#include "recomp_api.h"
#include "z64extern.h"

RECOMP_PATCH void func_808477D0(PlayState* play, Player* this, Input* input, f32 arg3) {
    f32 var_fv0;

    if ((input != NULL) && CHECK_BTN_ANY(input->press.button, BTN_B | BTN_A)) {
        var_fv0 = 1.0f;
    } else {
        var_fv0 = 0.5f;
    }

    var_fv0 *= arg3;
    // #region @ProxyMM If the player is human, swimming, and opted-in, use the clamping from OoT
    if (this->actionFunc == Player_Action_57 && 
        this->transformation == PLAYER_FORM_HUMAN && 
        recomp_get_config_u32("faster_swim")
    ) {
        if (var_fv0 < 1.0f) {
            var_fv0 = 1.0f;
        }
    } else {
        var_fv0 = CLAMP(var_fv0, 1.0f, 2.5f);
    }
    // #endregion
    this->skelAnime.playSpeed = var_fv0;

    PlayerAnimation_Update(play, &this->skelAnime);
}
