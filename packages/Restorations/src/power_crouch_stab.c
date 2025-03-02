#include "recomp_api.h"
#include "z64extern.h"

static PlayState* sPlayState;

// @ProxyMM Patching to store a static PlayState variable for the RECOMP_HOOK_RETURN below
RECOMP_CALLBACK("*", recomp_on_play_init)
void OnPlayInit(PlayState* play) {
    sPlayState = play;
}

RECOMP_PATCH s32 func_8083FD80(Player* this, PlayState* play) {
    if (!Player_IsGoronOrDeku(this) && (Player_GetMeleeWeaponHeld(this) != PLAYER_MELEEWEAPON_NONE) &&
        (this->transformation != PLAYER_FORM_ZORA) && sPlayerUseHeldItem) {
        // #region @ProxyMM Wraps the US/EU Power Crouch Stab patch in an opt-out condition
        if (recomp_get_config_u32("power_crouch_stab") == 0) {
            //! Calling this function sets the meleeWeaponQuads' damage properties correctly, patching "Power Crouch Stab".
            func_8083375C(this, PLAYER_MWA_STAB_1H);
        }
        // #endregion
        Player_Anim_PlayOnce(play, this, &gPlayerAnim_link_normal_defense_kiru);
        this->av1.actionVar1 = 1;
        this->meleeWeaponAnimation = PLAYER_MWA_STAB_1H;
        this->yaw = this->actor.shape.rot.y + this->upperLimbRot.y;
        this->unk_ADD = 0;
        return true;
    }
    return false;
}

// @ProxyMM After Collider_InitAndSetQuad is called within Player_InitCommon, optionally override the 
// dmgFlags to be consistent with Kokiri Sword (To be consistent with OoT)
RECOMP_HOOK_RETURN("Player_InitCommon")
void PostPlayer_InitCommon() {
    if (recomp_get_config_u32("power_crouch_stab") == 2) {
        Player* player = GET_PLAYER(sPlayState);
        player->meleeWeaponQuads[0].elem.atDmgInfo.dmgFlags = 512; // Kokiri Sword
        player->meleeWeaponQuads[1].elem.atDmgInfo.dmgFlags = 512; // Kokiri Sword
    }
}
