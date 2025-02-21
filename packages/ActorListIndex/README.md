# ActorListIndex
Exposes the index an Actor was placed at for the scene's ActorList command.

## Usage
This can be used to reliably identify actors within a given scene with some exceptions:
- A mod can augment the scene's original ActorList command. I'm sort of hoping this doesn't become a common practice, as there are easier ways to remove/add actors to a given scene.
- Some actors are spawned _after_ this ActorList command, like individual bushes of grass.

```cpp
// Import function
RECOMP_IMPORT("ProxyMM_ActorListIndex", s32 GetActorListIndex(Actor* actor));

RECOMP_CALLBACK("*", recomp_after_actor_init)
void AfterActorInit(PlayState* play, Actor* actor) {
    s32 actorListIndex = GetActorListIndex(actor);

    // This will always be the front right barrel near the turtle's right fin in GBT
    if (
        play->sceneId == SCENE_SEA &&
        play->roomCtx.curRoom.num == 13 &&
        actorListIndex == 6
    ) {
        Actor_Kill(actor);
    }

    if (actorListIndex == -1) {
        // Spawned outside of an ActorList command
    }
}
```
