#include "modding.h"
#include "global.h"
#include "functions.h"
#include "recompui.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"

void Inventory_DeleteItem(s16 item, s16 slot);
s32 Inventory_ReplaceItem(PlayState* play, u8 oldItem, u8 newItem);
void KaleidoScope_MoveCursorFromSpecialPos(PlayState* play);

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

RecompuiContext context;
RecompuiResource root;
RecompuiResource button;

PlayState* sPlayState;
bool sInventoryEditorTargetState = false;
bool sInventoryEditorState = false;
u16 sCursorItem;
u16 sCursorSlot;

ItemId sDefaultItems[] = {
    ITEM_OCARINA_OF_TIME,    // SLOT_OCARINA
    ITEM_BOW,                // SLOT_BOW
    ITEM_ARROW_FIRE,         // SLOT_ARROW_FIRE
    ITEM_ARROW_ICE,          // SLOT_ARROW_ICE
    ITEM_ARROW_LIGHT,        // SLOT_ARROW_LIGHT
    ITEM_MOONS_TEAR,         // SLOT_TRADE_DEED
    ITEM_BOMB,               // SLOT_BOMB
    ITEM_BOMBCHU,            // SLOT_BOMBCHU
    ITEM_DEKU_STICK,         // SLOT_DEKU_STICK
    ITEM_DEKU_NUT,           // SLOT_DEKU_NUT
    ITEM_MAGIC_BEANS,        // SLOT_MAGIC_BEANS
    ITEM_ROOM_KEY,           // SLOT_TRADE_KEY_MAMA
    ITEM_POWDER_KEG,         // SLOT_POWDER_KEG
    ITEM_PICTOGRAPH_BOX,     // SLOT_PICTOGRAPH_BOX
    ITEM_LENS_OF_TRUTH,      // SLOT_LENS_OF_TRUTH
    ITEM_HOOKSHOT,           // SLOT_HOOKSHOT
    ITEM_SWORD_GREAT_FAIRY,  // SLOT_SWORD_GREAT_FAIRY
    ITEM_LETTER_TO_KAFEI,    // SLOT_TRADE_COUPLE
    ITEM_BOTTLE,             // SLOT_BOTTLE_1
    ITEM_BOTTLE,             // SLOT_BOTTLE_2
    ITEM_BOTTLE,             // SLOT_BOTTLE_3
    ITEM_BOTTLE,             // SLOT_BOTTLE_4
    ITEM_BOTTLE,             // SLOT_BOTTLE_5
    ITEM_BOTTLE,             // SLOT_BOTTLE_6
    ITEM_MASK_POSTMAN,       // SLOT_MASK_POSTMAN
    ITEM_MASK_ALL_NIGHT,     // SLOT_MASK_ALL_NIGHT
    ITEM_MASK_BLAST,         // SLOT_MASK_BLAST
    ITEM_MASK_STONE,         // SLOT_MASK_STONE
    ITEM_MASK_GREAT_FAIRY,   // SLOT_MASK_GREAT_FAIRY
    ITEM_MASK_DEKU,          // SLOT_MASK_DEKU
    ITEM_MASK_KEATON,        // SLOT_MASK_KEATON
    ITEM_MASK_BREMEN,        // SLOT_MASK_BREMEN
    ITEM_MASK_BUNNY,         // SLOT_MASK_BUNNY
    ITEM_MASK_DON_GERO,      // SLOT_MASK_DON_GERO
    ITEM_MASK_SCENTS,        // SLOT_MASK_SCENTS
    ITEM_MASK_GORON,         // SLOT_MASK_GORON
    ITEM_MASK_ROMANI,        // SLOT_MASK_ROMANI
    ITEM_MASK_CIRCUS_LEADER, // SLOT_MASK_CIRCUS_LEADER
    ITEM_MASK_KAFEIS_MASK,   // SLOT_MASK_KAFEIS_MASK
    ITEM_MASK_COUPLE,        // SLOT_MASK_COUPLE
    ITEM_MASK_TRUTH,         // SLOT_MASK_TRUTH
    ITEM_MASK_ZORA,          // SLOT_MASK_ZORA
    ITEM_MASK_KAMARO,        // SLOT_MASK_KAMARO
    ITEM_MASK_GIBDO,         // SLOT_MASK_GIBDO
    ITEM_MASK_GARO,          // SLOT_MASK_GARO
    ITEM_MASK_CAPTAIN,       // SLOT_MASK_CAPTAIN
    ITEM_MASK_GIANT,         // SLOT_MASK_GIANT
    ITEM_MASK_FIERCE_DEITY,  // SLOT_MASK_FIERCE_DEITY
};

RECOMP_CALLBACK("*", recomp_on_play_init)
void OnPlayInit(PlayState* play) {
    sPlayState = play;
}

RECOMP_CALLBACK("*", recomp_on_play_main)
void OnPlayMain(PlayState* play) {
    // If they disabled the feature, turn it off
    if (sInventoryEditorState && !recomp_get_config_u32("inventory_editor")) {
        sInventoryEditorTargetState = false;
    }

    // If they unpause, turn it off
    if (sInventoryEditorState && play->pauseCtx.state == PAUSE_STATE_OFF) {
        sInventoryEditorTargetState = false;
    }

    if (sInventoryEditorState != sInventoryEditorTargetState) {
        // TODO: Mod UI doesn't really work yet
        // if (sInventoryEditorTargetState) {
        //     recompui_show_context(context);
        // } else {
        //     recompui_hide_context(context);
        // }

        sInventoryEditorState = sInventoryEditorTargetState;
        if (sInventoryEditorState) {
            Audio_PlaySfx(NA_SE_SY_DECIDE);
        } else {
            Audio_PlaySfx(NA_SE_SY_CANCEL);
        }
    }
}

// TODO: Mod UI doesn't really work yet
// RECOMP_CALLBACK("*", recomp_on_init) void OnRecompInit() {
//     context = recompui_create_context();

//     recompui_open_context(context);

//     root = recompui_context_root(context);

//     button = recompui_create_button(context, root, "Mod button", BUTTONSTYLE_PRIMARY);

//     recompui_close_context(context);
// }

void ToggleItemSlot() {
    PauseContext* pauseCtx = &sPlayState->pauseCtx;

    switch(pauseCtx->pageIndex) {
        case PAUSE_ITEM: {
            switch (sCursorSlot) {
                case SLOT_BOW: {
                    if (CUR_UPG_VALUE(UPG_QUIVER) >= 3) {
                        Inventory_DeleteItem(ITEM_BOW, SLOT_BOW);
                        Inventory_ChangeUpgrade(UPG_QUIVER, 0);
                    } else {
                        gSaveContext.save.saveInfo.inventory.items[SLOT_BOW] = ITEM_BOW;
                        Inventory_ChangeUpgrade(UPG_QUIVER, CUR_UPG_VALUE(UPG_QUIVER) + 1);
                    }
                    AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
                } break;
                case SLOT_BOMB:
                case SLOT_BOMBCHU: {
                    if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 3) {
                        Inventory_DeleteItem(ITEM_BOMB, SLOT_BOMB);
                        Inventory_DeleteItem(ITEM_BOMBCHU, SLOT_BOMBCHU);
                        Inventory_ChangeUpgrade(UPG_BOMB_BAG, 0);
                    } else {
                        gSaveContext.save.saveInfo.inventory.items[SLOT_BOMB] = ITEM_BOMB;
                        gSaveContext.save.saveInfo.inventory.items[SLOT_BOMBCHU] = ITEM_BOMBCHU;
                        Inventory_ChangeUpgrade(UPG_BOMB_BAG, CUR_UPG_VALUE(UPG_BOMB_BAG) + 1);
                    }
                    AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
                    AMMO(ITEM_BOMBCHU) = CUR_CAPACITY(UPG_BOMB_BAG);
                } break;
                case SLOT_TRADE_DEED: {
                    switch (gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_DEED]) {
                        case ITEM_NONE: {
                            gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_DEED] = ITEM_MOONS_TEAR;
                        } break;
                        case ITEM_MOONS_TEAR: {
                            Inventory_ReplaceItem(sPlayState, ITEM_MOONS_TEAR, ITEM_DEED_LAND);
                        } break;
                        case ITEM_DEED_LAND: {
                            Inventory_ReplaceItem(sPlayState, ITEM_DEED_LAND, ITEM_DEED_SWAMP);
                        } break;
                        case ITEM_DEED_SWAMP: {
                            Inventory_ReplaceItem(sPlayState, ITEM_DEED_SWAMP, ITEM_DEED_MOUNTAIN);
                        } break;
                        case ITEM_DEED_MOUNTAIN: {
                            Inventory_ReplaceItem(sPlayState, ITEM_DEED_MOUNTAIN, ITEM_DEED_OCEAN);
                        } break;
                        case ITEM_DEED_OCEAN: {
                            Inventory_DeleteItem(ITEM_DEED_OCEAN, SLOT_TRADE_DEED);
                        } break;
                    }
                } break;
                case SLOT_TRADE_KEY_MAMA: {
                    switch (gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_KEY_MAMA]) {
                        case ITEM_NONE: {
                            gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_KEY_MAMA] = ITEM_ROOM_KEY;
                        } break;
                        case ITEM_ROOM_KEY: {
                            Inventory_ReplaceItem(sPlayState, ITEM_ROOM_KEY, ITEM_LETTER_MAMA);
                        } break;
                        case ITEM_LETTER_MAMA: {
                            Inventory_DeleteItem(ITEM_LETTER_MAMA, SLOT_TRADE_KEY_MAMA);
                        } break;
                    }
                } break;
                case SLOT_TRADE_COUPLE: {
                    switch (gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_COUPLE]) {
                        case ITEM_NONE: {
                            gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_COUPLE] = ITEM_LETTER_TO_KAFEI;
                        } break;
                        case ITEM_LETTER_TO_KAFEI: {
                            Inventory_ReplaceItem(sPlayState, ITEM_LETTER_TO_KAFEI, ITEM_PENDANT_OF_MEMORIES);
                        } break;
                        case ITEM_PENDANT_OF_MEMORIES: {
                            Inventory_DeleteItem(ITEM_PENDANT_OF_MEMORIES, SLOT_TRADE_COUPLE);
                        } break;
                    }
                } break;
                default: {
                    if (gSaveContext.save.saveInfo.inventory.items[sCursorSlot] == ITEM_NONE) {
                        gSaveContext.save.saveInfo.inventory.items[sCursorSlot] = sDefaultItems[sCursorSlot];
                    } else {
                        Inventory_DeleteItem(gSaveContext.save.saveInfo.inventory.items[sCursorSlot], sCursorSlot);
                    }
                    switch (sCursorItem) {
                        case SLOT_DEKU_NUT: {
                            AMMO(ITEM_DEKU_NUT) = CUR_CAPACITY(UPG_DEKU_NUTS);
                        } break;
                        case SLOT_DEKU_STICK: {
                            AMMO(ITEM_DEKU_STICK) = CUR_CAPACITY(UPG_DEKU_STICKS);
                        } break;
                        case SLOT_MAGIC_BEANS: {
                            AMMO(ITEM_MAGIC_BEANS) = 20;
                        } break;
                        case SLOT_POWDER_KEG: {
                            AMMO(ITEM_POWDER_KEG) = 1;
                        } break;
                    }
                }
            }
        } break;
        case PAUSE_MASK: {
            if (gSaveContext.save.saveInfo.inventory.items[sCursorSlot + 24] == ITEM_NONE) {
                gSaveContext.save.saveInfo.inventory.items[sCursorSlot + 24] = sDefaultItems[sCursorSlot + 24];
            } else {
                Inventory_DeleteItem(gSaveContext.save.saveInfo.inventory.items[sCursorSlot + 24], sCursorSlot + 24);
            }
        } break;
        case PAUSE_QUEST: {
            if (sCursorSlot > QUEST_HEART_PIECE) break;

            switch(sCursorSlot) {
                case QUEST_SHIELD: {
                    switch (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD)) {
                        case EQUIP_VALUE_SHIELD_NONE: {
                            Inventory_ChangeEquipment(EQUIP_VALUE_SHIELD_HERO);
                        } break;
                        case EQUIP_VALUE_SHIELD_HERO: {
                            Inventory_ChangeEquipment(EQUIP_VALUE_SHIELD_MIRROR);
                        } break;
                        case EQUIP_VALUE_SHIELD_MIRROR: {
                            Inventory_DeleteEquipment(sPlayState, EQUIP_TYPE_SHIELD);
                        } break;
                    }
                    Player_SetEquipmentData(sPlayState, GET_PLAYER(sPlayState));
                } break;
                case QUEST_SWORD: {
                    switch (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD)) {
                        case EQUIP_VALUE_SWORD_NONE: {
                            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_KOKIRI);
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_SWORD_KOKIRI;
                            Interface_LoadItemIconImpl(sPlayState, EQUIP_SLOT_B);
                        } break;
                        case EQUIP_VALUE_SWORD_KOKIRI: {
                            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_RAZOR);
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_SWORD_RAZOR;
                            Interface_LoadItemIconImpl(sPlayState, EQUIP_SLOT_B);
                            gSaveContext.save.saveInfo.playerData.swordHealth = 100;
                        } break;
                        case EQUIP_VALUE_SWORD_RAZOR: {
                            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_GILDED);
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_SWORD_GILDED;
                            Interface_LoadItemIconImpl(sPlayState, EQUIP_SLOT_B);
                        } break;
                        case EQUIP_VALUE_SWORD_GILDED: {
                            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_NONE);
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_NONE;
                            Interface_LoadItemIconImpl(sPlayState, EQUIP_SLOT_B);
                        } break;
                    }
                } break;
                case QUEST_QUIVER: {
                    if (CUR_UPG_VALUE(UPG_QUIVER) >= 3) {
                        Inventory_DeleteItem(ITEM_BOW, SLOT_BOW);
                        Inventory_ChangeUpgrade(UPG_QUIVER, 0);
                    } else {
                        gSaveContext.save.saveInfo.inventory.items[SLOT_BOW] = ITEM_BOW;
                        Inventory_ChangeUpgrade(UPG_QUIVER, CUR_UPG_VALUE(UPG_QUIVER) + 1);
                    }
                    AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
                } break;
                case QUEST_BOMB_BAG: {
                    if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 3) {
                        Inventory_DeleteItem(ITEM_BOMB, SLOT_BOMB);
                        Inventory_DeleteItem(ITEM_BOMBCHU, SLOT_BOMBCHU);
                        Inventory_ChangeUpgrade(UPG_BOMB_BAG, 0);
                    } else {
                        gSaveContext.save.saveInfo.inventory.items[SLOT_BOMB] = ITEM_BOMB;
                        gSaveContext.save.saveInfo.inventory.items[SLOT_BOMBCHU] = ITEM_BOMBCHU;
                        Inventory_ChangeUpgrade(UPG_BOMB_BAG, CUR_UPG_VALUE(UPG_BOMB_BAG) + 1);
                    }
                    AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
                    AMMO(ITEM_BOMBCHU) = CUR_CAPACITY(UPG_BOMB_BAG);
                } break;
                case QUEST_SONG_LULLABY: {
                    if (!CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)) {
                        SET_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO);
                        REMOVE_QUEST_ITEM(QUEST_SONG_LULLABY);
                    } else if (!CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
                        SET_QUEST_ITEM(QUEST_SONG_LULLABY);
                        SET_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO);
                    } else {
                        REMOVE_QUEST_ITEM(QUEST_SONG_LULLABY);
                        REMOVE_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO);
                    }
                } break;
                default: {
                    TOGGLE_QUEST_ITEM(sCursorSlot);
                } break;
            }
        } break;
    }
}

void PreKaleidoHandler(PlayState* play) {
    if (!recomp_get_config_u32("inventory_editor")) return;

    if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_L)) {
        sInventoryEditorTargetState = !sInventoryEditorState;
    }

    if (!sInventoryEditorTargetState) return;

    PauseContext* pauseCtx = &sPlayState->pauseCtx;
    // Prevent the cursor from moving all the way to the next page when the inventory is empty
    if (
        (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT && pauseCtx->stickAdjX < -30) ||
        (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT && pauseCtx->stickAdjX > 30)
    ) {
        KaleidoScope_MoveCursorFromSpecialPos(play);
        pauseCtx->stickAdjX = 0;
    }

    if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A)) {
        ToggleItemSlot();
        CONTROLLER1(&play->state)->press.button &= ~BTN_A;
    }
}

RECOMP_HOOK("KaleidoScope_UpdateItemCursor")
void PreKaleidoScope_UpdateItemCursor(PlayState* play) {
    PreKaleidoHandler(play);
}

RECOMP_HOOK("KaleidoScope_UpdateMaskCursor")
void PreKaleidoScope_UpdateMaskCursor(PlayState* play) {
    PreKaleidoHandler(play);
}

RECOMP_HOOK("KaleidoScope_UpdateQuestCursor")
void PreKaleidoScope_UpdateQuestCursor(PlayState* play) {
    PreKaleidoHandler(play);
}

RECOMP_HOOK_RETURN("KaleidoScope_UpdateItemCursor")
void PostKaleidoScope_UpdateItemCursor() {
    if (!recomp_get_config_u32("inventory_editor")) return;

    PauseContext* pauseCtx = &sPlayState->pauseCtx;
    sCursorItem = pauseCtx->cursorItem[PAUSE_ITEM];
    sCursorSlot = pauseCtx->cursorPoint[PAUSE_ITEM];
}

RECOMP_HOOK_RETURN("KaleidoScope_UpdateMaskCursor")
void PostKaleidoScope_UpdateMaskCursor() {
    if (!recomp_get_config_u32("inventory_editor")) return;

    PauseContext* pauseCtx = &sPlayState->pauseCtx;
    sCursorItem = pauseCtx->cursorItem[PAUSE_MASK];
    sCursorSlot = pauseCtx->cursorPoint[PAUSE_MASK];
}

RECOMP_HOOK_RETURN("KaleidoScope_UpdateQuestCursor")
void PostKaleidoScope_UpdateQuestCursor() {
    if (!recomp_get_config_u32("inventory_editor")) return;

    PauseContext* pauseCtx = &sPlayState->pauseCtx;
    sCursorItem = pauseCtx->cursorItem[PAUSE_QUEST];
    sCursorSlot = pauseCtx->cursorPoint[PAUSE_QUEST];
}
