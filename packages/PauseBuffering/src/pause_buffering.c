#include "modding.h"
#include "global.h"

RECOMP_IMPORT("*", u32 recomp_get_config_u32(const char* key));

static int easyFrameAdvanceTimer = 0;
static int inputBufferTimer = 0;
static u16 prePauseInputs = 0;
static u16 pauseInputs = 0;

RECOMP_CALLBACK("*", recomp_on_play_main)
void OnPlayMain(PlayState* play) {
    Input* input = &play->state.input[0];
    PauseContext* pauseCtx = &play->pauseCtx;

    // STEP #1: If the user opts to include held inputs in the buffer window, store the held inputs at the beginning 
    // of the pause process, minus the START input (Because they just pressed pause)
    if (pauseCtx->state == PAUSE_STATE_OPENING_1 && recomp_get_config_u32("include_held_inputs_buffer_window")) {
        prePauseInputs = input->cur.button & ~BTN_START;
    }

    // STEP #2: The unpause process has begun, so let's reset the input buffer timer
    if (pauseCtx->state == PAUSE_STATE_UNPAUSE_SETUP || (
        // These are for if I port the pause saving from OoT
        pauseCtx->state == PAUSE_STATE_SAVEPROMPT && (
            pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_RETURN_TO_MENU || 
            pauseCtx->savePromptState == PAUSE_SAVEPROMPT_STATE_5
        )
    )) {
        inputBufferTimer = 0;
    }

    if (pauseCtx->state == PAUSE_STATE_UNPAUSE_CLOSE || pauseCtx->state == (PAUSE_STATE_UNPAUSE_CLOSE + 1)) {
        // STEP #3: If the user opts to include held inputs in the buffer window, buffer the currently held inputs, minus 
        // the held inputs when the pause menu was opened. This only applies to the first frame of the buffer window
        if (recomp_get_config_u32("include_held_inputs_buffer_window") && inputBufferTimer == 0) {
            pauseInputs |= input->cur.button & ~prePauseInputs;
            prePauseInputs = 0;
        }

        // STEP #4: Buffer all inputs that were pressed during the buffer window
        pauseInputs |= input->press.button;

        // STEP #5: Wait a specified number of frames before continuing the unpause
        inputBufferTimer++;
        if (inputBufferTimer < recomp_get_config_u32("pause_buffer_window")) {
            pauseCtx->state = PAUSE_STATE_UNPAUSE_CLOSE + 1;
        } else {
            pauseCtx->state = PAUSE_STATE_UNPAUSE_CLOSE;
        }
    }

    // STEP #6: If the input buffer timer is not 0 and the pause state is off, then the player just finished unpausing
    if (inputBufferTimer != 0 && pauseCtx->state == PAUSE_STATE_OFF) {
        // If the user opts into easy frame advance, remove START input
        if (recomp_get_config_u32("easy_frame_advance")) {
            pauseInputs &= ~BTN_START;
        }

        // Apply the buffered inputs that were pressed during the buffer window
        input->press.button |= pauseInputs;

        // Reset the used variables
        pauseInputs = 0;
        inputBufferTimer = 0;
    }
    
    // STEP #7: If the user opts into easy frame advance, wait a single frame, and if they are still holding the 
    // start button, re-apply the press state to re-open kaleido one frame later
    if (recomp_get_config_u32("easy_frame_advance")) {
        if (easyFrameAdvanceTimer > 0 && pauseCtx->state == PAUSE_STATE_OFF) {
            easyFrameAdvanceTimer--;
            if (easyFrameAdvanceTimer == 0 && CHECK_BTN_ALL(input->cur.button, BTN_START)) {
                input->press.button |= BTN_START;
            }
        }
    
        if (pauseCtx->state == PAUSE_STATE_UNPAUSE_CLOSE) {
            easyFrameAdvanceTimer = 2;
        }
    }
}


