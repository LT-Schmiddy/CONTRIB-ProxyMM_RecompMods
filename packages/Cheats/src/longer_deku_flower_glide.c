#include "recomp_api.h"
#include "z64extern.h"

// @ProxyMM Hooking here because it's just a convenient spot relevant to player flying. We could do this on PlayInit,
// but having it here accounts for if players opt-in after play init.
RECOMP_HOOK("Player_Action_94")
void LongerDekuFlowerGlide_Player_Action_94(Player* this, PlayState* play) {
    if (recomp_get_config_u32("longer_deku_flower_glide")) {
        D_8085D958[0] = 99999.9f;
        D_8085D958[1] = 99999.9f;
    } else {
        D_8085D958[0] = 600.0f;
        D_8085D958[1] = 960.0f;
    }
}
