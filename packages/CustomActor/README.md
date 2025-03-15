# CustomActor
A library for defining custom actors

## Usage
```c
RECOMP_IMPORT("ProxyMM_CustomActor", s16 CustomActor_Register(ActorProfile* profile));

typedef struct MyCustomActor {
    // ...
} MyCustomActor;

ActorProfile MyCustomActor_Profile = {
    ACTOR_ID_MAX, // This will be replaced
    ACTORCAT_BG,
    FLAGS,
    GAMEPLAY_KEEP, // GAMEPLAY_KEEP is always loaded, so safe bet if you're not using stuff from another actor
    sizeof(MyCustomActor),
    MyCustomActor_Init,
    MyCustomActor_Destroy,
    MyCustomActor_Update,
    MyCustomActor_Draw,
};

s16 ACTOR_MY_CUSTOM_ACTOR = ACTOR_ID_MAX;

RECOMP_CALLBACK("*", recomp_on_init) 
void MyCustomActor_OnRecompInit() {
    ACTOR_MY_CUSTOM_ACTOR = CustomActor_Register(&Bg_Haka_Profile);
}

RECOMP_HOOK("Actor_SpawnTransitionActors")
void MyCustomActor_Spawner(PlayState* play, ActorContext* actorCtx) {
    if (play->sceneId == SCENE_CLOCKTOWER) {
        Actor_SpawnAsChildAndCutscene(actorCtx, play, ACTOR_MY_CUSTOM_ACTOR, -367.0f, 0.0f, -245.0f, 0, 0x8000, 0, 0, 0, 0, 0);
    }
}
```

## Note
This library doesn't currently guarantee support for Custom actors being loaded from SceneCommands data. You may get it to work using hardcoded IDs but it's not something I would recommend relying on. For now hooking `Actor_SpawnTransitionActors` and checking sceneId and roomNum should work for most use cases.

This also does not use the overlay loading system, so static variables will not be reset when all instances of the actor are unloaded.

## Examples
You can see an example of how this can be used in the `examples/` directory in the repository. Feel free to submit PR's for more examples.
