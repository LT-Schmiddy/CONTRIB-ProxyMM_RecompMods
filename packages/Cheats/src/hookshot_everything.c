#include "recomp_api.h"
#include "z64extern.h"

RECOMP_PATCH u32 SurfaceType_IsHookshotSurface(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    if (recomp_get_config_u32("hookshot_everything")) {
        return true;
    }

    return SurfaceType_GetData(colCtx, poly, bgId, 1) >> 17 & 1;
}
