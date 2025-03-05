#include "recomp_api.h"
#include "z64extern.h"

RECOMP_CALLBACK("*", recomp_on_play_main)
void Infinite_OnPlayMain(PlayState* play) {
    if (recomp_get_config_u32("infinite_magic")) {
        gSaveContext.save.saveInfo.playerData.magic = MAGIC_NORMAL_METER * gSaveContext.save.saveInfo.playerData.magicLevel;
    }
    
    if (recomp_get_config_u32("infinite_health")) {
        gSaveContext.save.saveInfo.playerData.health = gSaveContext.save.saveInfo.playerData.healthCapacity;
    }

    if (recomp_get_config_u32("infinite_rupees")) {
        gSaveContext.save.saveInfo.playerData.rupees = CUR_CAPACITY(UPG_WALLET);
    }

    if (recomp_get_config_u32("infinite_consumables")) {
        if (INV_CONTENT(ITEM_BOW) == ITEM_BOW) {
            AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
        }

        if (INV_CONTENT(ITEM_BOMB) == ITEM_BOMB) {
            AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
        }

        if (INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU) {
            AMMO(ITEM_BOMBCHU) = CUR_CAPACITY(UPG_BOMB_BAG);
        }

        if (INV_CONTENT(ITEM_DEKU_STICK) == ITEM_DEKU_STICK) {
            AMMO(ITEM_DEKU_STICK) = CUR_CAPACITY(UPG_DEKU_STICKS);
        }

        if (INV_CONTENT(ITEM_DEKU_NUT) == ITEM_DEKU_NUT) {
            AMMO(ITEM_DEKU_NUT) = CUR_CAPACITY(UPG_DEKU_NUTS);
        }

        if (INV_CONTENT(ITEM_MAGIC_BEANS) == ITEM_MAGIC_BEANS) {
            AMMO(ITEM_MAGIC_BEANS) = 20;
        }

        if (INV_CONTENT(ITEM_POWDER_KEG) == ITEM_POWDER_KEG) {
            AMMO(ITEM_POWDER_KEG) = 1;
        }
    }
}
