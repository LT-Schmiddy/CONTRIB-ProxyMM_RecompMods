#include "modding.h"
#include "global.h"
#include "functions.h"
#include "macros.h"
#include "recompui.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));
extern u16 sPersistentCycleWeekEventRegs[ARRAY_COUNT(gSaveContext.save.saveInfo.weekEventReg)];

// TODO: Waiting for this event to be upstream
// RECOMP_CALLBACK("*", recomp_on_init_save) 
void OnInitSave(FileSelectState* fileSelect, SramContext* sramCtx) {
    if (!recomp_get_config_u32("debug_save")) return;

    u8 playerName[8];

    Lib_MemCpy(playerName, gSaveContext.save.saveInfo.playerData.playerName, sizeof(playerName));

    Sram_InitDebugSave();

    Lib_MemCpy(gSaveContext.save.saveInfo.playerData.playerName, playerName, sizeof(playerName));

    gSaveContext.save.entrance = ENTRANCE(SOUTH_CLOCK_TOWN, 0);
    gSaveContext.save.cutsceneIndex = 0;
    gSaveContext.save.saveInfo.playerData.threeDayResetCount = 1;
    gSaveContext.save.saveInfo.playerData.doubleDefense = true;
    gSaveContext.save.saveInfo.playerData.health = 20 * 0x10;
    gSaveContext.save.saveInfo.playerData.healthCapacity = 20 * 0x10;
    gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = true;
    gSaveContext.save.saveInfo.playerData.magicLevel = 2;
    gSaveContext.save.saveInfo.playerData.magic = MAGIC_DOUBLE_METER;
    gSaveContext.save.saveInfo.playerData.owlActivationFlags = (1 << (OWL_WARP_STONE_TOWER + 1)) - 1;
    gSaveContext.save.saveInfo.playerData.owlWarpId = OWL_WARP_CLOCK_TOWN;

    gSaveContext.save.saveInfo.inventory.defenseHearts = 20;
    gSaveContext.save.saveInfo.regionsVisited = (1 << REGION_MAX) - 1;
    gSaveContext.magicCapacity = MAGIC_DOUBLE_METER;

    Inventory_ChangeUpgrade(UPG_WALLET, 2);
    Inventory_ChangeUpgrade(UPG_BOMB_BAG, 3);
    Inventory_ChangeUpgrade(UPG_QUIVER, 3);

    for (s32 i = 0; i < TINGLE_MAP_MAX; i++) {
        Inventory_SetWorldMapCloudVisibility(i);
    }

    gSaveContext.save.saveInfo.playerData.rupees = CUR_CAPACITY(UPG_WALLET);
    AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
    AMMO(ITEM_BOMB) = AMMO(ITEM_BOMBCHU) = CUR_CAPACITY(UPG_BOMB_BAG);
    AMMO(ITEM_DEKU_STICK) = CUR_CAPACITY(UPG_DEKU_STICKS);
    AMMO(ITEM_DEKU_NUT) = CUR_CAPACITY(UPG_DEKU_NUTS);
    AMMO(ITEM_MAGIC_BEANS) = 20;
    AMMO(ITEM_POWDER_KEG) = 1;

    SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_MIRROR);
    SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_GILDED);
    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_SWORD_GILDED;

    for (s32 i = SLOT_BOTTLE_1; i <= SLOT_BOTTLE_6; i++) {
        if (gSaveContext.save.saveInfo.inventory.items[i] == ITEM_NONE) {
            gSaveContext.save.saveInfo.inventory.items[i] = ITEM_BOTTLE;
        }
    }

    for (s32 i = QUEST_REMAINS_ODOLWA; i <= QUEST_BOMBERS_NOTEBOOK; i++) {
        if (i != QUEST_SHIELD && i != QUEST_SWORD && i != QUEST_SONG_SARIA && i != QUEST_SONG_SUN) {
            SET_QUEST_ITEM(i);
        }
    }

    // Use the persistent cycle events to set what a 100% save would normally keep
    for (s32 i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.weekEventReg); i++) {
        u16 isPersistentBits = sPersistentCycleWeekEventRegs[i];

        // Force all bits on
        gSaveContext.save.saveInfo.weekEventReg[i] = 0xFF;

        // Then unset any bits that aren't persistent
        for (s32 j = 0; j < 8; j++) {
            if (!(isPersistentBits & 3)) {
                gSaveContext.save.saveInfo.weekEventReg[i] =
                    gSaveContext.save.saveInfo.weekEventReg[i] & (0xFF ^ (1 << j));
            }
            isPersistentBits >>= 2;
        }
    }

    gSaveContext.save.saveInfo.checksum = Sram_CalcChecksum(&gSaveContext.save, sizeof(Save));
}
