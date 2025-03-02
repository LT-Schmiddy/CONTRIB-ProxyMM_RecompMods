#include "recomp_api.h"
#include "z64extern.h"

RECOMP_PATCH s32 Player_ActionHandler_Talk(Player* this, PlayState* play) {
    if (gSaveContext.save.saveInfo.playerData.health != 0) {
        Actor* talkOfferActor = this->talkActor;
        Actor* lockOnActor = this->focusActor;
        Actor* cUpTalkActor = NULL;
        s32 forceTalkToTatl = false;
        s32 canTalkToLockOnWithCUp = false;

        if (this->tatlActor != NULL) {
            canTalkToLockOnWithCUp =
                (lockOnActor != NULL) &&
                (CHECK_FLAG_ALL(lockOnActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_TALK_WITH_C_UP) ||
                 (lockOnActor->hintId != TATL_HINT_ID_NONE));

            if (canTalkToLockOnWithCUp || (this->tatlTextId != 0)) {
                //! @bug The comparison `((ABS_ALT(this->tatlTextId) & 0xFF00) != 0x10000)` always evaluates to `true`
                // Likely changed 0x200 -> 0x10000 to disable this check from OoT
                forceTalkToTatl = (this->tatlTextId < 0) && ((ABS_ALT(this->tatlTextId) & 0xFF00) != 0x10000);

                if (forceTalkToTatl || !canTalkToLockOnWithCUp) {
                    // If `lockOnActor` can't be talked to with c-up, the only option left is Tatl
                    cUpTalkActor = this->tatlActor;
                    if (forceTalkToTatl) {
                        // Clearing these pointers guarantees that `cUpTalkActor` will take priority
                        lockOnActor = NULL;
                        talkOfferActor = NULL;
                    }
                } else {
                    // Tatl is not the talk actor, so the only option left for talking with c-up is `lockOnActor`
                    // (though, `lockOnActor` may be NULL at this point).
                    cUpTalkActor = lockOnActor;
                }
            }
        }

        if ((talkOfferActor != NULL) || (cUpTalkActor != NULL)) {
            if ((lockOnActor != NULL) && (lockOnActor != talkOfferActor) && (lockOnActor != cUpTalkActor)) {
                goto dont_talk;
            }

            if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
                if ((this->heldActor == NULL) ||
                    (!forceTalkToTatl && (talkOfferActor != this->heldActor) && (cUpTalkActor != this->heldActor) &&
                     ((talkOfferActor == NULL) || !(talkOfferActor->flags & ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED)))) {
                    goto dont_talk;
                }
            }

            // FAKE: used to maintain matching using goto's. Goto's not required, but improves readability.
            if (1) {}
            if (1) {}

            if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                if (!(this->stateFlags1 & PLAYER_STATE1_800000) && !func_801242B4(this)) {
                    goto dont_talk;
                }
            }

            if (talkOfferActor != NULL) {
                // At this point the talk offer can be accepted.
                // "Speak" or "Check" will appear on the A button in the HUD.
                if ((lockOnActor == NULL) || (lockOnActor == talkOfferActor)) {
                    this->stateFlags2 |= PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER;
                }

                if (!CutsceneManager_IsNext(CS_ID_GLOBAL_TALK)) {
                    return false;
                }

                if (CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_A) ||
                    (talkOfferActor->flags & ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED)) {
                    // Talk Offer has been accepted.
                    // Clearing `cUpTalkActor` guarantees that `talkOfferActor` is the actor that will be spoken to
                    cUpTalkActor = NULL;
                } else if (cUpTalkActor == NULL) {
                    return false;
                }
            }

            if (cUpTalkActor != NULL) {
                if (!forceTalkToTatl) {
                    this->stateFlags2 |= PLAYER_STATE2_200000;
                    // #region @ProxyMM Restore ISG from tatl, similar to OoT
                    bool vanillaCondition = !CutsceneManager_IsNext(CS_ID_GLOBAL_TALK);
                    if (recomp_get_config_u32("tatl_isg")) {
                        vanillaCondition = false;
                    }

                    if (vanillaCondition ||
                        !CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_CUP)) {
                        return false;
                    }
                    // #endregion
                }

                talkOfferActor = cUpTalkActor;
                this->talkActor = NULL;

                if (forceTalkToTatl || !canTalkToLockOnWithCUp) {
                    cUpTalkActor->textId = ABS_ALT(this->tatlTextId);
                } else if (cUpTalkActor->hintId != 0xFF) {
                    cUpTalkActor->textId = cUpTalkActor->hintId + 0x1900;
                }
            }

            // `sSavedCurrentMask` saves the current mask just before the current action runs on this frame.
            // This saved mask value is then restored just before starting a conversation.
            //
            // This handles an edge case where a conversation is started on the same frame that a mask was taken on or
            // off. Because Player updates early before most actors, the text ID being offered comes from the previous
            // frame. If a mask was taken on or off the same frame this function runs, the wrong text will be used.
            this->currentMask = sSavedCurrentMask;
            gSaveContext.save.equippedMask = this->currentMask;

            Player_StartTalking(play, talkOfferActor);

            return true;
        }
    }

dont_talk:
    return false;
}
