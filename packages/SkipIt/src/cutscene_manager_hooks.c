#include "recomp_api.h"
#include "z64extern.h"

RECOMP_DECLARE_EVENT(SkipIt_ShouldCutsceneStart(s16 csId, Actor* actor, bool* should));
RECOMP_DECLARE_EVENT(SkipIt_ShouldCutsceneQueue(s16 csId, bool* should));

RECOMP_PATCH s16 CutsceneManager_Start(s16 csId, Actor* actor) {
    // #region @ProxyMM Allow preventing the cutscene from starting
    bool shouldStart = true;
    SkipIt_ShouldCutsceneStart(csId, actor, &shouldStart);
    if (!shouldStart) {
        return CS_ID_NONE;
    }
    // #endregion

    CutsceneEntry* csEntry;
    Camera* subCam;
    Camera* retCam;
    s32 csType = 0;

    if ((csId <= CS_ID_NONE) || (sCutsceneMgr.csId != CS_ID_NONE)) {
        return csId;
    }

    sCutsceneMgr.startMethod = CS_START_0;
    csEntry = CutsceneManager_GetCutsceneEntryImpl(csId);

    ShrinkWindow_Letterbox_SetSizeTarget(csEntry->letterboxSize);
    CutsceneManager_SetHudVisibility(csEntry->hudVisibility);

    if (csId == CS_ID_GLOBAL_END) {
        csType = 1;
    } else if (csEntry->scriptIndex != CS_SCRIPT_ID_NONE) {
        // scripted cutscene
        csType = 1;
    } else if ((csId != CS_ID_GLOBAL_DOOR) && (csId != CS_ID_GLOBAL_TALK)) {
        csType = 2;
    }

    if (csType != 0) {
        sCutsceneMgr.retCamId = Play_GetActiveCamId(sCutsceneMgr.play);
        sCutsceneMgr.subCamId = Play_CreateSubCamera(sCutsceneMgr.play);

        subCam = Play_GetCamera(sCutsceneMgr.play, sCutsceneMgr.subCamId);
        retCam = Play_GetCamera(sCutsceneMgr.play, sCutsceneMgr.retCamId);

        if ((retCam->setting == CAM_SET_START0) || (retCam->setting == CAM_SET_START2) ||
            (retCam->setting == CAM_SET_START1)) {
            if (CutsceneManager_FindEntranceCsId() != csId) {
                func_800E0348(retCam);
            } else {
                Camera_UnsetStateFlag(retCam, CAM_STATE_2);
            }
        }

        Lib_MemCpy(subCam, retCam, sizeof(Camera));
        subCam->camId = sCutsceneMgr.subCamId;
        Camera_UnsetStateFlag(subCam, CAM_STATE_6 | CAM_STATE_0);

        Play_ChangeCameraStatus(sCutsceneMgr.play, sCutsceneMgr.retCamId, CAM_STATUS_WAIT);
        Play_ChangeCameraStatus(sCutsceneMgr.play, sCutsceneMgr.subCamId, CAM_STATUS_ACTIVE);

        subCam->target = sCutsceneMgr.targetActor = actor;
        subCam->behaviorFlags = 0;

        if (csType == 1) {
            Camera_ChangeSetting(subCam, CAM_SET_FREE0);
            Cutscene_StartScripted(sCutsceneMgr.play, csEntry->scriptIndex);
            sCutsceneMgr.length = csEntry->length;
        } else {
            if (csEntry->csCamId != CS_CAM_ID_NONE) {
                Camera_ChangeActorCsCamIndex(subCam, csEntry->csCamId);
            } else {
                Camera_ChangeSetting(subCam, CAM_SET_FREE0);
            }
            sCutsceneMgr.length = csEntry->length;
        }
    }
    sCutsceneMgr.csId = csId;

    return csId;
}

RECOMP_PATCH void CutsceneManager_Queue(s16 csId) {
    // #region @ProxyMM Allow preventing the cutscene from queueing
    bool shouldQueue = true;
    SkipIt_ShouldCutsceneQueue(csId, &shouldQueue);
    if (!shouldQueue) {
        return;
    }
    // #endregion

    if (csId > CS_ID_NONE) {
        sWaitingCutsceneList[csId >> 3] |= 1 << (csId & 7);
    }
}
