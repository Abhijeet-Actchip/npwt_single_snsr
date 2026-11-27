/*
 * board_sim.c
 *
 * Simulator BSP implementation.
 *
 * Keypad reads poll SDL2 keyboard state so gui_mgr.c's KeypadReadCb
 * behaves identically to hardware (arrow keys + Enter map 1:1 to the
 * 5-way switch).
 *
 * Note: KEYPAD_SW_ACTIVE_STATE in gui_mgr.c is defined as 'false' (active-low
 * switches on hardware). Therefore Board_ReadSwXX returns 'false' when the
 * corresponding PC key is pressed and 'true' when released.
 */

#include "board_sim.h"
#include <SDL2/SDL.h>
#include <stdint.h>

/* --------------------------------------------------------------------------
 * Internal helper
 * -------------------------------------------------------------------------- */

static bool IsKeyDown(SDL_Scancode sc)
{
    const uint8_t *state = SDL_GetKeyboardState(NULL);
    return (state != NULL) && (state[sc] != 0U);
}

/* --------------------------------------------------------------------------
 * Keypad (active-low to match KEYPAD_SW_ACTIVE_STATE == false)
 * -------------------------------------------------------------------------- */

bool Board_ReadSwUP(void)
{
    return !IsKeyDown(SDL_SCANCODE_UP);
}

bool Board_ReadSwDOWN(void)
{
    return !IsKeyDown(SDL_SCANCODE_DOWN);
}

bool Board_ReadSwENTER(void)
{
    return !IsKeyDown(SDL_SCANCODE_RETURN);
}

bool Board_ReadSwRIGHT(void)
{
    return !IsKeyDown(SDL_SCANCODE_RIGHT);
}

bool Board_ReadSwLEFT(void)
{
    return !IsKeyDown(SDL_SCANCODE_LEFT);
}

/* --------------------------------------------------------------------------
 * No-op hardware control stubs
 * -------------------------------------------------------------------------- */

int8_t Board_InitPeripherals(void)
{
    return 0; /* APP_ERR_NONE */
}

int8_t Board_EnableDisplayCS(bool en)
{
    (void)en;
    return 0;
}

int8_t Board_CtrlDisplayReset(bool en)
{
    (void)en;
    return 0;
}

int8_t Board_CtrlDisplayRS(bool en)
{
    (void)en;
    return 0;
}

int8_t Board_CtrlDisplayBacklight(bool en)
{
    (void)en;
    return 0;
}
