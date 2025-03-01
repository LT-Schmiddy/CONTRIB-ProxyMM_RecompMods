# SkipIt
Various cutscene skip options

## Features
- Skip One Point Cutscenes (Chests Spawning, Switch Pressed, Etc)
- Skip Entrance Cutscenes
- Skip Boss Introductions
- Skip Intro or First Cycle
- Skip Song Playback
- More planned

## API
Because this patches commonly patched functions (CutsceneManager_Start & CutsceneManager_Queue), we are also exposing events so that other mods may make use of this pattern without having to also patch them.

```cpp
RECOMP_CALLBACK("ProxyMM_SkipIt", SkipIt_ShouldCutsceneStart)
void MyMod_ShouldCutsceneStart(s16 csId, Actor* actor, bool* should) {
    if (csId == 1) {
        *should = false;
    }
}

RECOMP_CALLBACK("ProxyMM_SkipIt", SkipIt_ShouldCutsceneQueue)
void MyMod_ShouldCutsceneQueue(s16 csId, bool* should) {
    if (csId == 1) {
        *should = false;
    }
}
```
