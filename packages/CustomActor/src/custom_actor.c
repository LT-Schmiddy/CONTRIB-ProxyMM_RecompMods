#include "proxymm_custom_actor.h"
#include "recomp_api.h"
#include "z64extern.h"

ActorProfile* customActorProfiles[9999]; // Arbitrary size, Doubtful anyone would ever reach this
s16 nextCustomActorId = ACTOR_ID_MAX;
ActorOverlay customActorDummyOverlay = {0};

RECOMP_EXPORT s16 CustomActor_Register(ActorProfile* profile) {
    s16 id = ++nextCustomActorId;
    profile->id = id;
    customActorProfiles[id] = profile;
    return id;
}

RECOMP_PATCH Actor* Actor_SpawnAsChildAndCutscene(
    ActorContext* actorCtx, PlayState* play, s16 index, 
    f32 x, f32 y, f32 z, s16 rotX, s16 rotY, s16 rotZ, 
    s32 params, u32 csId, u32 halfDaysBits, Actor* parent
) {
    s32 pad;
    Actor* actor;
    ActorProfile* profile;
    s32 objectSlot;
    ActorOverlay* overlayEntry;

    if (actorCtx->totalLoadedActors >= 255) {
        return NULL;
    }

    // @ProxyMM: If this is a custom actor, use our storage instead
    if (index > ACTOR_ID_MAX) {
        profile = customActorProfiles[index];
    } else {
        profile = Actor_LoadOverlay(actorCtx, index);
    }

    if (profile == NULL) {
        return NULL;
    }

    objectSlot = Object_GetSlot(&play->objectCtx, profile->objectId);
    if ((objectSlot <= OBJECT_SLOT_NONE) ||
        ((profile->type == ACTORCAT_ENEMY) && Flags_GetClear(play, play->roomCtx.curRoom.num) &&
         (profile->id != ACTOR_BOSS_05))) {
        Actor_FreeOverlay(&gActorOverlayTable[index]);
        return NULL;
    }

    actor = ZeldaArena_Malloc(profile->instanceSize);
    if (actor == NULL) {
        Actor_FreeOverlay(&gActorOverlayTable[index]);
        return NULL;
    }

    // @ProxyMM: If this is a custom actor, use our dummy overlay
    if (index > ACTOR_ID_MAX) {
        overlayEntry = &customActorDummyOverlay;
    } else {
        overlayEntry = &gActorOverlayTable[index];
    }

    if (overlayEntry->vramStart != NULL) {
        overlayEntry->numLoaded++;
    }

    bzero(actor, profile->instanceSize);
    actor->overlayEntry = overlayEntry;
    actor->id = profile->id;
    actor->flags = profile->flags;

    if (profile->id == ACTOR_EN_PART) {
        actor->objectSlot = rotZ;
        rotZ = 0;
    } else {
        actor->objectSlot = objectSlot;
    }

    actor->init = profile->init;
    actor->destroy = profile->destroy;
    actor->update = profile->update;
    actor->draw = profile->draw;

    if (parent != NULL) {
        actor->room = parent->room;
        actor->parent = parent;
        parent->child = actor;
    } else {
        actor->room = play->roomCtx.curRoom.num;
    }

    actor->home.pos.x = x;
    actor->home.pos.y = y;
    actor->home.pos.z = z;
    actor->home.rot.x = rotX;
    actor->home.rot.y = rotY;
    actor->home.rot.z = rotZ;
    actor->params = params & 0xFFFF;
    actor->csId = csId & 0x7F;

    if (actor->csId == 0x7F) {
        actor->csId = CS_ID_NONE;
    }

    if (halfDaysBits != 0) {
        actor->halfDaysBits = halfDaysBits;
    } else {
        actor->halfDaysBits = HALFDAYBIT_ALL;
    }

    Actor_AddToCategory(actorCtx, actor, profile->type);

    {
        uintptr_t prevSeg = gSegments[0x06];

        Actor_Init(actor, play);
        gSegments[0x06] = prevSeg;
    }

    return actor;
}