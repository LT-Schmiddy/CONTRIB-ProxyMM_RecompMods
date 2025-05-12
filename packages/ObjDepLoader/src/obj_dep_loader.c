#include "recomp_api.h"
#include "z64extern.h"

static bool objectLoaded[OBJECT_ID_MAX];
static void* objectSegments[OBJECT_ID_MAX];
static uintptr_t prevGSegments[NUM_SEGMENTS];

RECOMP_EXPORT bool ObjDepLoader_Load(PlayState* play, u8 segment, s16 objectId) {
    if (segment >= NUM_SEGMENTS) return false;

    if (!objectLoaded[objectId]) {
        size_t size = gObjectTable[objectId].vromEnd - gObjectTable[objectId].vromStart;

        objectSegments[objectId] = recomp_alloc(size);

        // At some point we may want to make this async.
        DmaMgr_RequestSync(objectSegments[objectId], gObjectTable[objectId].vromStart, size);

        objectLoaded[objectId] = true;
    }

    prevGSegments[segment] = gSegments[segment];
    gSegments[segment] = OS_K0_TO_PHYSICAL(objectSegments[objectId]);

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, segment, objectSegments[objectId]);
    gSPSegment(POLY_XLU_DISP++, segment, objectSegments[objectId]);

    CLOSE_DISPS(play->state.gfxCtx);

    return true;
}

RECOMP_EXPORT void ObjDepLoader_Unload(PlayState* play, u8 segment, s16 objectId) {
    gSegments[segment] = prevGSegments[segment];

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, segment, gSegments[segment]);
    gSPSegment(POLY_XLU_DISP++, segment, gSegments[segment]);

    CLOSE_DISPS(play->state.gfxCtx);

    // At some point we may keep track of how many active loads there are and free the memory when it reaches 0.
    // For now, even with everything loaded, the memory usage is minimal so we aren't going to worry about it.
}
