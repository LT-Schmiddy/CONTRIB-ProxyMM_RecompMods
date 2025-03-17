#include "recomp_api.h"
#include "z64extern.h"

RECOMP_PATCH void Play_PostWorldDraw(PlayState* this) {
    if (IS_PAUSED(&this->pauseCtx)) {
        KaleidoScopeCall_Draw(this);
    }

    if (gSaveContext.gameMode == GAMEMODE_NORMAL && !recomp_get_config_u32("disable_hud")) {
        Interface_Draw(this);
    }

    if ((!IS_PAUSED(&this->pauseCtx) || (this->msgCtx.currentTextId != 0xFF)) && !recomp_get_config_u32("disable_hud")) {
        Message_Draw(this);
    }

    if (this->gameOverCtx.state != GAMEOVER_INACTIVE) {
        GameOver_FadeLights(this);
    }

    // Shrink the whole screen display (at the end of First and Second Day by default)
    if (gSaveContext.screenScaleFlag || recomp_get_config_u32("screen_scale") != 100) {
        Gfx* gfx;
        Gfx* gfxHead;
        GraphicsContext* gfxCtx = this->state.gfxCtx;

        if (recomp_get_config_u32("screen_scale") != 100) {
            sPlayVisFbufInstance->scale = recomp_get_config_u32("screen_scale") / 100.0f;
        } else {
            sPlayVisFbufInstance->scale = gSaveContext.screenScale / 1000.0f;
        }

        OPEN_DISPS(gfxCtx);

        gfxHead = POLY_OPA_DISP;
        gfx = Gfx_Open(gfxHead);
        gSPDisplayList(OVERLAY_DISP++, gfx);

        VisFbuf_Draw(sPlayVisFbufInstance, &gfx, this->unk_18E60);

        gSPEndDisplayList(gfx++);
        Gfx_Close(gfxHead, gfx);
        POLY_OPA_DISP = gfx;

        CLOSE_DISPS(gfxCtx);
    }
}

RECOMP_HOOK("ShrinkWindow_Draw")
void BeforeShrinkWindow_Draw(GraphicsContext* gfxCtx) {
    if (recomp_get_config_u32("letterbox_size") != -1) {
        sShrinkWindowPtr->letterboxSize = recomp_get_config_u32("letterbox_size");
    }
}
