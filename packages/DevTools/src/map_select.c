#include "recomp_api.h"
#include "z64extern.h"
#include "proxymm_kv.h"

static GameState* sGameState;

void BetterMapSelect_LoadGame(MapSelectState* mapSelectState, u32 entrance, s32 spawn) {
    KV_Global_Set_S32("ProxyMM_DevTools.MapSelect.currentScene", mapSelectState->currentScene);
    KV_Global_Set_S32("ProxyMM_DevTools.MapSelect.opt", mapSelectState->opt);
    KV_Global_Set_S32("ProxyMM_DevTools.MapSelect.topDisplayedScene", mapSelectState->topDisplayedScene);
    KV_Global_Set_S32("ProxyMM_DevTools.MapSelect.pageDownIndex", mapSelectState->pageDownIndex);

    // TODO: Persist Selection to config
    MapSelect_LoadGame(mapSelectState, entrance, spawn);
}

void BetterMapSelect_LoadFileSelect(MapSelectState* mapSelectState, u32 entrance, s32 spawn) {
    // TODO: Persist Selection to config
    STOP_GAMESTATE(&mapSelectState->state);
    SET_NEXT_GAMESTATE(&mapSelectState->state, FileSelect_Init_NORELOCATE, sizeof(FileSelectState));
}

#define DEFINE_SCENE(_enumValue, entranceSceneId, _betterMapSelectIndex, humanName) \
{ humanName, BetterMapSelect_LoadGame, ENTRANCE(entranceSceneId, 0) },

static SceneSelectEntry sBetterScenes[103] = {
#include "better_scene_table.h"
{ "File Select", BetterMapSelect_LoadFileSelect, 0 },
// { "Title Screen", MapSelect_LoadConsoleLogo, 0 },
};

#undef DEFINE_SCENE

#define DEFINE_SCENE(_enumValue, entranceSceneId, betterMapSelectIndex, humanName) \
{ humanName, ENTRANCE(entranceSceneId, 0), betterMapSelectIndex },

typedef struct {
    char* name;
    s32 entrance;
    s32 index;
} BetterMapSelectInfoEntry;

static BetterMapSelectInfoEntry sBetterMapSelectInfo[102] = {
#include "better_scene_table.h"
};

#undef DEFINE_SCENE

static const char* betterFormLabels[] = {
    "Deity", "Goron", "Zora", "Deku", "Child",
};

RECOMP_HOOK("GameState_Update")
void Pre_GameState_Update(GameState* gameState) {
    sGameState = gameState;

    if (!recomp_get_config_u32("map_select")) {
        return;
    }

    if (CHECK_BTN_ALL(gameState->input[0].press.button, BTN_Z) &&
        CHECK_BTN_ALL(gameState->input[0].cur.button, BTN_L | BTN_R)) {
        STOP_GAMESTATE(gameState);
        gSaveContext.gameMode = GAMEMODE_NORMAL;
        gSaveContext.nextDayTime = NEXT_TIME_NONE;
        gSaveContext.nextTransitionType = TRANS_NEXT_TYPE_DEFAULT;
        gSaveContext.prevHudVisibility = HUD_VISIBILITY_ALL;
        SET_NEXT_GAMESTATE(gameState, MapSelect_Init_NORELOCATE, sizeof(MapSelectState));
    }
}

RECOMP_PATCH void MapSelect_DrawMenu(MapSelectState* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    GfxPrint printer;
    s32 i;
    s32 sceneIndex;
    char* sceneName;
    char* stageName;
    char* dayName;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL28_Opa(gfxCtx);

    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, POLY_OPA_DISP);

    // Header
    GfxPrint_SetColor(&printer, 255, 255, 255, 255);
    GfxPrint_SetPos(&printer, 12, 2);
    GfxPrint_Printf(&printer, "Scene Selection");
    GfxPrint_SetColor(&printer, 255, 255, 255, 255);

    // Scenes
    for (i = 0; i < 20; i++) {
        GfxPrint_SetPos(&printer, 3, i + 4);

        sceneIndex = (this->topDisplayedScene + i + this->count) % this->count;
        if (sceneIndex == this->currentScene) {
            GfxPrint_SetColor(&printer, 255, 100, 100, 255);
        } else {
            GfxPrint_SetColor(&printer, 175, 175, 175, 255);
        }

        sceneName = sBetterScenes[sceneIndex].name;
        GfxPrint_Printf(&printer, "%3d %s", sceneIndex, sceneName);
    };

    // Entrance
    GfxPrint_SetColor(&printer, 100, 100, 100, 255);
    GfxPrint_SetPos(&printer, 28, 26);
    GfxPrint_Printf(&printer, "Entrance:");
    GfxPrint_SetColor(&printer, 200, 200, 50, 255);
    GfxPrint_Printf(&printer, "%2d", this->opt);

    // Form
    GfxPrint_SetPos(&printer, 26, 25);
    GfxPrint_SetColor(&printer, 100, 100, 100, 255);
    GfxPrint_Printf(&printer, "(B)Form:");
    GfxPrint_SetColor(&printer, 55, 200, 50, 255);
    GfxPrint_Printf(&printer, "%s", betterFormLabels[GET_PLAYER_FORM]);

    // Day
    GfxPrint_SetPos(&printer, 1, 25);
    GfxPrint_SetColor(&printer, 100, 100, 100, 255);
    GfxPrint_Printf(&printer, "Day:");
    GfxPrint_SetColor(&printer, 100, 100, 200, 255);
    switch (gSaveContext.save.day) {
        case 1:
            dayName = "First Day";
            break;
        case 2:
            dayName = "Second Day";
            break;
        case 3:
            dayName = "Final Day";
            break;
        case 4:
            dayName = "Clear Day";
            break;
        default:
            gSaveContext.save.day = 1;
            dayName = "First Day";
            break;
    }
    GfxPrint_Printf(&printer, "%s", dayName);

    // Stage
    GfxPrint_SetPos(&printer, 1, 26);
    GfxPrint_SetColor(&printer, 100, 100, 100, 255);
    GfxPrint_Printf(&printer, "(Z/R)Stage:");
    GfxPrint_SetColor(&printer, 200, 100, 200, 255);
    switch (gSaveContext.save.cutsceneIndex) {
        case 0:
            gSaveContext.save.time = CLOCK_TIME(12, 0);
            stageName = "Afternoon";
            break;
        case 0x8000:
            gSaveContext.save.time = CLOCK_TIME(6, 0) + 1;
            stageName = "Morning";
            break;
        case 0x8800:
            gSaveContext.save.time = CLOCK_TIME(18, 1);
            stageName = "Night";
            break;
        case 0xFFF0:
            gSaveContext.save.time = CLOCK_TIME(12, 0);
            stageName = "0";
            break;
        case 0xFFF1:
            stageName = "1";
            break;
        case 0xFFF2:
            stageName = "2";
            break;
        case 0xFFF3:
            stageName = "3";
            break;
        case 0xFFF4:
            stageName = "4";
            break;
        case 0xFFF5:
            stageName = "5";
            break;
        case 0xFFF6:
            stageName = "6";
            break;
        case 0xFFF7:
            stageName = "7";
            break;
        case 0xFFF8:
            stageName = "8";
            break;
        case 0xFFF9:
            stageName = "9";
            break;
        case 0xFFFA:
            stageName = "A";
            break;
        default:
            stageName = "???";
            break;
    }
    GfxPrint_Printf(&printer, "%s", stageName);

    POLY_OPA_DISP = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);

    CLOSE_DISPS(gfxCtx);
};

RECOMP_HOOK_RETURN("MapSelect_Init")
void BetterMapSelect_Init() {
    MapSelectState* this = (MapSelectState*)sGameState;
    static bool isInitialized = false;

    if (!isInitialized) {
        s32 i;
        for (i = 0; i < ARRAY_COUNT(sBetterMapSelectInfo); i++) {
            sBetterScenes[sBetterMapSelectInfo[i].index].name = sBetterMapSelectInfo[i].name;
            sBetterScenes[sBetterMapSelectInfo[i].index].entrance = sBetterMapSelectInfo[i].entrance;
        }
        isInitialized = true;
    }

    this->scenes = sBetterScenes;
    this->count = ARRAY_COUNT(sBetterScenes);
    this->currentScene = KV_Global_Get_S32("ProxyMM_DevTools.MapSelect.currentScene", this->currentScene);
    this->opt = KV_Global_Get_S32("ProxyMM_DevTools.MapSelect.opt", this->opt);
    this->topDisplayedScene = KV_Global_Get_S32("ProxyMM_DevTools.MapSelect.topDisplayedScene", this->topDisplayedScene);
    this->pageDownIndex = KV_Global_Get_S32("ProxyMM_DevTools.MapSelect.pageDownIndex", this->pageDownIndex);
}
