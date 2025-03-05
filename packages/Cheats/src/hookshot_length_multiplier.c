#include "recomp_api.h"
#include "z64extern.h"

RECOMP_PATCH void ArmsHook_Wait(ArmsHook* this, PlayState* play) {
    if (this->actor.parent == NULL) {
        ArmsHook_SetupAction(this, ArmsHook_Shoot);
        Actor_SetSpeeds(&this->actor, 20.0f);
        this->actor.parent = &GET_PLAYER(play)->actor;
        // @ProxyMM multiply length
        this->timer = 26 * recomp_get_config_u32("hookshot_length_multiplier");
    }
}

// @ProxyMM This function is patched in base recomp for interpolation, so what we do here is hook into before it's called
// and check if the original call will display the reticle, if it doesn't then we do our own calculation with the adjusted length
RECOMP_HOOK("Player_DrawHookshotReticle")
void HookshotLengthMultiplier_Player_DrawHookshotReticle(PlayState* play, Player* player, f32 hookshotDistance) {
    if (recomp_get_config_u32("hookshot_length_multiplier") == 1) return;

    static Vec3f D_801C094C = { -500.0f, -100.0f, 0.0f };
    CollisionPoly* poly;
    s32 bgId;
    Vec3f sp7C;
    Vec3f sp70;
    Vec3f pos;

    D_801C094C.z = 0.0f;
    Matrix_MultVec3f(&D_801C094C, &sp7C);
    D_801C094C.z = hookshotDistance;
    Matrix_MultVec3f(&D_801C094C, &sp70);

    bool vanillaReticleDisplayed = (
        BgCheck_AnyLineTest3(&play->colCtx, &sp7C, &sp70, &pos, &poly, true, true, true, true, &bgId) &&
        (
            !func_800B90AC(play, &player->actor, poly, bgId, &pos) ||
            BgCheck_ProjectileLineTest(&play->colCtx, &sp7C, &sp70, &pos, &poly, true, true, true, true, &bgId)
        )
    );

    if (vanillaReticleDisplayed) return;

    D_801C094C.z = hookshotDistance * recomp_get_config_u32("hookshot_length_multiplier");
    Matrix_MultVec3f(&D_801C094C, &sp70);

    if (BgCheck_AnyLineTest3(&play->colCtx, &sp7C, &sp70, &pos, &poly, true, true, true, true, &bgId)) {
        if (!func_800B90AC(play, &player->actor, poly, bgId, &pos) ||
            BgCheck_ProjectileLineTest(&play->colCtx, &sp7C, &sp70, &pos, &poly, true, true, true, true, &bgId)) {
            Matrix_Push();
            Vec3f sp58;
            f32 sp54;
            f32 scale;

            OPEN_DISPS(play->state.gfxCtx);

            OVERLAY_DISP = Gfx_SetupDL(OVERLAY_DISP, SETUPDL_7);

            SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &pos, &sp58, &sp54);

            scale = (sp54 < 200.0f) ? 0.08f : (sp54 / 200.0f) * 0.08f;

            Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);
            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(OVERLAY_DISP++, play->state.gfxCtx);

            gSPSegment(OVERLAY_DISP++, 0x06, play->objectCtx.slots[player->actor.objectSlot].segment);
            gSPDisplayList(OVERLAY_DISP++, gHookshotReticleDL);

            CLOSE_DISPS(play->state.gfxCtx);
            Matrix_Pop();
        }
    }
}
