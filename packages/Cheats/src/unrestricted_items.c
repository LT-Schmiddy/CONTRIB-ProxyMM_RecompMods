#include "recomp_api.h"
#include "z64extern.h"

// @ProxyMM Hooking here because it's just a convenient spot relevant to player flying. We could do this on PlayInit,
// but having it here accounts for if players opt-in after play init.
RECOMP_HOOK_RETURN("Interface_UpdateButtonsPart1")
void UnrestrictedItems_Interface_UpdateButtonsPart1() {
    if (recomp_get_config_u32("unrestricted_items")) {

        gSaveContext.bButtonStatus = BTN_ENABLED;
        gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] = BTN_ENABLED;
        gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] = BTN_ENABLED;
        gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] = BTN_ENABLED;
        gSaveContext.buttonStatus[EQUIP_SLOT_B] = BTN_ENABLED;
    }
}
