#include "recomp_api.h"
#include "z64extern.h"

RECOMP_PATCH void func_80836B3C(PlayState* play, Player* this, f32 arg2) {
    if (!recomp_get_config_u32("side_roll")) {
        this->yaw = this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }

    if (this->transformation == PLAYER_FORM_GORON) {
        func_80836AD8(play, this);
        PlayerAnimation_Change(play, &this->skelAnime, D_8085BE84[PLAYER_ANIMGROUP_landing_roll][this->modelAnimType],
                               1.5f * sWaterSpeedFactor, 0.0f, 6.0f, ANIMMODE_ONCE, 0.0f);
    } else {
        PlayerAnimationHeader* anim = D_8085BE84[PLAYER_ANIMGROUP_landing_roll][this->modelAnimType];

        Player_SetAction(play, this, Player_Action_26, 0);
        PlayerAnimation_Change(play, &this->skelAnime, anim, 1.25f * sWaterSpeedFactor, arg2,
                               Animation_GetLastFrame(anim), ANIMMODE_ONCE, 0.0f);
    }
}
