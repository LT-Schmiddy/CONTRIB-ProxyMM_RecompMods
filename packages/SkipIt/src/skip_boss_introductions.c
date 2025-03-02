#include "recomp_api.h"
#include "z64extern.h"

RECOMP_HOOK("Boss01_Init")
void SkipBossIntroductions_Boss01_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("skip_boss_introductions")) {
        SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_ODOLWA);
    }
}

RECOMP_HOOK("Boss02_Init")
void SkipBossIntroductions_Boss02_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("skip_boss_introductions")) {
        SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_TWINMOLD);
    }
}

RECOMP_HOOK("Boss03_Init")
void SkipBossIntroductions_Boss03_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("skip_boss_introductions")) {
        SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_GYORG);
    }
}

RECOMP_HOOK("Boss04_Init")
void SkipBossIntroductions_Boss04_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("skip_boss_introductions")) {
        SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_WART);
    }
}

RECOMP_HOOK("Boss06_Init")
void SkipBossIntroductions_Boss06_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("skip_boss_introductions")) {
        SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_IGOS_DU_IKANA);
    }
}

RECOMP_HOOK("Boss07_Init")
void SkipBossIntroductions_Boss07_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("skip_boss_introductions")) {
        SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_MAJORA);
    }
}

RECOMP_HOOK("BossHakugin_Init")
void SkipBossIntroductions_BossHakugin_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("skip_boss_introductions")) {
        SET_EVENTINF(EVENTINF_ENTR_CS_WATCHED_GOHT);
    }
}

RECOMP_HOOK("EnDeath_Init")
void SkipBossIntroductions_EnDeath_Init(Actor* thisx, PlayState* play) {
    if (recomp_get_config_u32("skip_boss_introductions")) {
        SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_GOMESS);
    }
}
