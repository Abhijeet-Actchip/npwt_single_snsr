/*
 * app_assert.h  (simulator override)
 *
 * Maps APP_ASSERT to the standard C assert() so failures produce a clean
 * crash trace (file, line, expression) instead of spinning forever.
 *
 * This file is picked up first by the sim CMake include path, shadowing
 * the embedded app_utils/assert/app_assert.h on the simulator build.
 */

#ifndef __APP_ASSERT_H__
#define __APP_ASSERT_H__		1

#include <assert.h>

#define APP_ASSERT(expr)    assert(expr)

/* No-op stubs for embedded-only helpers called from app_assert.c on HW. */
static inline void APP_ASSERT_DUMP_LOGS(void) { }

#endif /* __APP_ASSERT_H__ */
