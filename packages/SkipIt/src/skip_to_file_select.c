#include "recomp_api.h"
#include "z64extern.h"

RECOMP_HOOK("ConsoleLogo_Main")
void SkipToFileSelect_ConsoleLogo_Main(GameState* thisx) {
    if (!recomp_get_config_u32("skip_to_file_select")) return;

    // Normally the PRNG seed is set at least once from the title opening running Play_Init
    // We need to call it manually before file select creates RNG values for new saves
    Rand_Seed(osGetTime());
    // Normally called on console logo screen
    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
    gSaveContext.gameMode = GAMEMODE_TITLE_SCREEN;

    STOP_GAMESTATE(thisx);
    SET_NEXT_GAMESTATE(thisx, FileSelect_Init_NORELOCATE, sizeof(FileSelectState));
}
