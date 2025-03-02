#include "recomp_api.h"
#include "z64extern.h"
#include "z64recomp_api.h"

ActorExtensionId sActorListIndexExtensionId;
s32 currentActorListIndex = 0;

RECOMP_CALLBACK("*", recomp_on_init) void OnRecompInit() {
    sActorListIndexExtensionId = z64recomp_extend_actor_all(sizeof(s32));
}

RECOMP_EXPORT s32 GetActorListIndex(Actor* actor) {
    s32* actorListIndex = z64recomp_get_extended_actor_data(actor, sActorListIndexExtensionId);
    // The memory in the actor extension is zeroed out by default, we make use of this to indicate nothing has been stored, 
    // so this actor was spawned outside of an ActorList command. In return, when we store the actual ActorList index, we 
    // store it + 1, then undo that offset here.
    return *actorListIndex - 1;
}

RECOMP_PATCH void Actor_SpawnSetupActors(PlayState* play, ActorContext* actorCtx) {
    if (play->numSetupActors > 0) {
        ActorEntry* actorEntry = play->setupActorList;
        s32 prevHalfDaysBitValue = actorCtx->halfDaysBit;
        s32 shiftedHalfDaysBit;
        s32 actorEntryHalfDayBit;
        s32 i;

        Actor_InitHalfDaysBit(actorCtx);
        Actor_KillAllOnHalfDayChange(play, &play->actorCtx);

        // Shift to previous halfDay bit, but ignoring DAY0_NIGHT.
        // In other words, if the current halfDay is DAY1_DAY then this logic is ignored and this variable is zero
        shiftedHalfDaysBit = (actorCtx->halfDaysBit << 1) & (HALFDAYBIT_ALL & ~HALFDAYBIT_DAY0_NIGHT);

        for (i = 0; i < play->numSetupActors; i++) {
            // #region @ProxyMM store the actor's index + 1 to be used for identification
            currentActorListIndex = i + 1;
            // #endregion

            actorEntryHalfDayBit = ((actorEntry->rot.x & 7) << 7) | (actorEntry->rot.z & 0x7F);
            if (actorEntryHalfDayBit == 0) {
                actorEntryHalfDayBit = HALFDAYBIT_ALL;
            }

            if (!(actorEntryHalfDayBit & prevHalfDaysBitValue) && (actorEntryHalfDayBit & actorCtx->halfDaysBit) &&
                (!CHECK_EVENTINF(EVENTINF_17) || !(actorEntryHalfDayBit & shiftedHalfDaysBit) ||
                 !(actorEntry->id & 0x800))) {
                Actor_SpawnEntry(&play->actorCtx, actorEntry, play);
            }
            actorEntry++;
        }
        // #region @ProxyMM Reset the current index
        currentActorListIndex = 0;
        // #endregion

        // Prevents re-spawning the setup actors
        play->numSetupActors = -play->numSetupActors;
    }
}

// @ProxyMM This is a bit of a hack, but we need to store the ActorListIndex prior to `should_actor_init` being called,
// but after the ActorExtension memory is made available. Unfortunately these both happen in `Actor_Init` which base
// recomp patches. We are lucky though, because in between these events, the following function is called, and never
// used again in the codebase. So we'll hook in here to store our ActorListIndex
RECOMP_PATCH void Actor_SetWorldToHome(Actor* actor) {
    actor->world = actor->home;

    // #region @ProxyMM
    if (currentActorListIndex != 0) {
        s32* actorListIndex = z64recomp_get_extended_actor_data(actor, sActorListIndexExtensionId);
        *actorListIndex = currentActorListIndex;
        currentActorListIndex = 0;
    }
    // #endregion
}
