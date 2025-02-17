#include "modding.h"
#include "global.h"
#include "functions.h"

RECOMP_IMPORT("*", int recomp_printf(const char* fmt, ...));
RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));
void Player_StopHorizontalMovement(Player* this);

static Input* sControlInput;
static u32 sNoclipEnabled = false;

/**
 * Copied from "overlays/actors/ovl_player_actor/z_player.c" of Ocarina of Time
 * 
 * Updates the "Noclip" debug feature, which allows the player to fly around anywhere
 * in the world and clip through any collision.
 *
 * Noclip can be toggled on and off with two different button combos:
 * Hold L + R + A and press B
 * or
 * Hold L and press D-pad right
 *
 * To control Noclip mode:
 * - Move horizontally with the 4 D-pad directions
 * - Move up with B
 * - Move down with A
 * - Hold R to move faster
 *
 * With Noclip enabled, another button combination can be pressed to set all "temp clear" flags
 * in the current room. To do so hold L and press D-pad left.
 *
 * @return  true if Noclip is disabled, false if enabled
 */
s32 Player_UpdateNoclip(Player* this, PlayState* play) {
    sControlInput = &play->state.input[0];

    if ((CHECK_BTN_ALL(sControlInput->cur.button, BTN_A | BTN_L | BTN_R) &&
         CHECK_BTN_ALL(sControlInput->press.button, BTN_B)) ||
        (CHECK_BTN_ALL(sControlInput->cur.button, BTN_L) && CHECK_BTN_ALL(sControlInput->press.button, BTN_DRIGHT))) {
        
        sNoclipEnabled ^= 1;

        if (sNoclipEnabled) {
            Camera_ChangeMode(Play_GetCamera(play, CAM_ID_MAIN), CAM_MODE_FOLLOWTARGET);
        }
    }

    if (sNoclipEnabled) {
        f32 speed;

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_R)) {
            speed = 100.0f;
        } else {
            speed = 20.0f;
        }

        if (!CHECK_BTN_ALL(sControlInput->cur.button, BTN_L)) {
            if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_B)) {
                this->actor.world.pos.y += speed;
            } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_A)) {
                this->actor.world.pos.y -= speed;
            }

            if (CHECK_BTN_ANY(sControlInput->cur.button, BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT)) {
                s16 angle;
                s16 temp;

                angle = temp = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));

                if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DDOWN)) {
                    angle = temp + 0x8000;
                } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DLEFT)) {
                    angle = temp + 0x4000;
                } else if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DRIGHT)) {
                    angle = temp - 0x4000;
                }

                this->actor.world.pos.x += speed * Math_SinS(angle);
                this->actor.world.pos.z += speed * Math_CosS(angle);
            }
        }

        Player_StopHorizontalMovement(this);

        this->actor.gravity = 0.0f;
        this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;

        if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_L) && CHECK_BTN_ALL(sControlInput->press.button, BTN_DLEFT)) {
            Flags_SetClearTemp(play, play->roomCtx.curRoom.num);
        }

        Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);

        return false;
    }

    return true;
}

RECOMP_CALLBACK("*", recomp_should_actor_update) 
void ShouldActorUpdate(PlayState* play, Actor* actor, bool* should) {
    if (actor->id != ACTOR_PLAYER) return;
    if (!recomp_get_config_u32("no_clip")) return;

    if (!Player_UpdateNoclip((Player*)actor, play)) {
        *should = false;
    }
}
