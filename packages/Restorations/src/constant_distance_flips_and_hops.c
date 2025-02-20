#include "modding.h"
#include "global.h"
#include "functions.h"

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

extern PlayerAnimationHeader gPlayerAnim_link_normal_newroll_jump_20f;
extern PlayerAnimationHeader gPlayerAnim_pn_attack;
extern PlayerAnimationHeader gPlayerAnim_link_normal_landing;
extern PlayerAnimationHeader* D_8085BE84[PLAYER_ANIMGROUP_MAX][PLAYER_ANIMTYPE_MAX];
bool Player_UpdateUpperBody(Player* this, PlayState* play);
void func_80840980(Player* this, u16 sfxId);
s32 func_8083D860(Player* this, PlayState* play);
s32 func_8084C124(PlayState* play, Player* this);
void Player_AnimSfx_PlayVoice(Player* this, u16 sfxId);
void func_80837CEC(PlayState* play, Player* this, CollisionPoly* arg2, f32 arg3, PlayerAnimationHeader* anim);
s32 Player_GetMovementSpeedAndYaw(Player* this, f32* outSpeedTarget, s16* outYawTarget, f32 speedMode, PlayState* play);
bool func_808313A8(PlayState* play, Player* this, Actor* actor);
void func_808409A8(PlayState* play, Player* this, f32 speed, f32 yVelocity);
void func_808378FC(PlayState* play, Player* this);
s32 func_8083CBC4(Player* this, f32 arg1, s16 arg2, f32 arg3, f32 arg4, f32 arg5, s16 arg6);
s32 func_80839770(Player* this, PlayState* play);
void func_80837134(PlayState* play, Player* this);
s32 Player_ActionHandler_13(Player* this, PlayState* play);
extern Input* sPlayerControlInput;
extern f32 sPlayerYDistToFloor;
#define SPEED_MODE_LINEAR 0.0f
#define GET_PLAYER_ANIM(group, type) ((PlayerAnimationHeader**)D_8085BE84)[group * PLAYER_ANIMTYPE_MAX + type]

RECOMP_PATCH void Player_Action_25(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    Actor* heldActor;

    if (Player_CheckHostileLockOn(this)) {
        this->actor.gravity = -1.2f;
    }

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Player_GetMovementSpeedAndYaw(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
            heldActor = this->heldActor;
            if (!func_808313A8(play, this, heldActor) && (heldActor->id == ACTOR_EN_NIW) &&
                CHECK_BTN_ANY(sPlayerControlInput->press.button, BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_B | BTN_A)) {
                func_808409A8(play, this, this->speedXZ + 2.0f, this->actor.velocity.y + 2.0f);
            }
        }

        PlayerAnimation_Update(play, &this->skelAnime);
        if ((this->skelAnime.animation == &gPlayerAnim_link_normal_newroll_jump_20f) &&
            PlayerAnimation_OnFrame(&this->skelAnime, 4.0f)) {
            Player_PlaySfx(this, NA_SE_PL_ROLL);
        }

        if (this->transformation == PLAYER_FORM_DEKU) {
            s16 prevYaw = this->yaw;

            func_808378FC(play, this);
            func_8083CBC4(this, speedTarget * 0.5f, yawTarget, 2.0f, 0.2f, 0.1f, 0x190);

            if (this->skelAnime.animation == &gPlayerAnim_pn_attack) {
                this->stateFlags2 |= (PLAYER_STATE2_20 | PLAYER_STATE2_40);

                this->unk_B10[0] += -800.0f;
                this->actor.shape.rot.y += BINANG_ADD(TRUNCF_BINANG(this->unk_B10[0]), BINANG_SUB(this->yaw, prevYaw));
                Math_StepToF(&this->unk_B10[1], 0.0f, this->unk_B10[0]);
            }
        } else {
            // #region @ProxyMM If the user opted-in, they are flipping or hopping and it's not a front flip prevent air control
            if (
                !recomp_get_config_u32("constant_distance_flips_and_hops") ||
                !(this->stateFlags2 & PLAYER_STATE2_80000 && this->av1.actionVar1 != 5)
            ) {
                func_8083CBC4(this, speedTarget, yawTarget, 1.0f, 0.05f, 0.1f, 0xC8);
            }
            // #endregion
        }

        Player_UpdateUpperBody(this, play);
        if ((((this->stateFlags2 & PLAYER_STATE2_80000) &&
              ((this->av1.actionVar1 == 2) || (this->av1.actionVar1 >= 4))) ||
             !func_80839770(this, play)) &&
            (this->actor.velocity.y < 0.0f)) {
            if (this->av2.actionVar2 >= 0) {
                if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->av2.actionVar2 == 0) ||
                    (this->fallDistance > 0)) {
                    if ((sPlayerYDistToFloor > 800.0f) || (this->stateFlags3 & PLAYER_STATE3_10000)) {
                        func_80840980(this, NA_SE_VO_LI_FALL_S);
                        this->stateFlags3 &= ~PLAYER_STATE3_10000;
                    }
                    PlayerAnimation_Change(play, &this->skelAnime, &gPlayerAnim_link_normal_landing,
                                           PLAYER_ANIM_NORMAL_SPEED, 0.0f, 0.0f, ANIMMODE_ONCE, 8.0f);
                    this->av2.actionVar2 = -1;
                }
            } else {
                if ((this->av2.actionVar2 == -1) && (this->fallDistance > 120) && (sPlayerYDistToFloor > 280.0f)) {
                    this->av2.actionVar2 = -2;
                    func_80840980(this, NA_SE_VO_LI_FALL_L);
                }

                if ((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) &&
                    !(this->stateFlags1 & (PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_8000000)) &&
                    (this->speedXZ > 0.0f)) {
                    if ((this->transformation != PLAYER_FORM_GORON) &&
                        ((this->transformation != PLAYER_FORM_DEKU) || (this->remainingHopsCounter != 0))) {
                        if ((this->yDistToLedge >= 150.0f) &&
                            (this->controlStickDirections[this->controlStickDataIndex] == PLAYER_STICK_DIR_FORWARD)) {
                            if (func_8083D860(this, play)) {
                                func_8084C124(play, this);
                            }
                        } else if ((this->ledgeClimbType >= PLAYER_LEDGE_CLIMB_2) &&
                                   ((this->yDistToLedge < (150.0f * this->ageProperties->unk_08)) &&
                                    (((this->actor.world.pos.y - this->actor.floorHeight) + this->yDistToLedge)) >
                                        (70.0f * this->ageProperties->unk_08))) {
                            AnimTaskQueue_DisableTransformTasksForGroup(play);
                            if (this->stateFlags3 & PLAYER_STATE3_10000) {
                                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_HOOKSHOT_HANG);
                            } else {
                                Player_AnimSfx_PlayVoice(this, NA_SE_VO_LI_HANG);
                            }

                            this->actor.world.pos.y += this->yDistToLedge;
                            func_80837CEC(play, this, this->actor.wallPoly, this->distToInteractWall,
                                          GET_PLAYER_ANIM(PLAYER_ANIMGROUP_jump_climb_hold, this->modelAnimType));
                            this->yaw += 0x8000;
                            this->actor.shape.rot.y = this->yaw;
                            this->stateFlags1 |= PLAYER_STATE1_2000;

                            func_8084C124(play, this);
                        }
                    }
                }
            }
        }
    } else {
        func_80837134(play, this);
        Player_UpdateUpperBody(this, play);
    }

    Player_ActionHandler_13(this, play);
}
