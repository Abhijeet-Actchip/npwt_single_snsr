# CMakeLists.txt  —  NPWT UI Simulator (WSL2 / Linux)
#
# Build from WSL2:
#   cd /mnt/d/STMWorkspcae_2.3.0/npwt_single_cubemx2/npwt_single_snsr_cmake/sim
#   mkdir -p build && cd build
#   cmake .. -DCMAKE_BUILD_TYPE=Debug
#   make -j$(nproc)
#   ./npwt_sim

cmake_minimum_required(VERSION 3.16)
project(npwt_sim C)
set(CMAKE_C_STANDARD 11)

# ---------------------------------------------------------------------------
# Project root (one level up from sim/)
# ---------------------------------------------------------------------------
set(PROJ_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# ---------------------------------------------------------------------------
# SDL2
# ---------------------------------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# ---------------------------------------------------------------------------
# LVGL sources — same library as the embedded target.
# SDL driver sources are compiled in because LV_USE_SDL=1 in sim/lv_conf.h.
# ---------------------------------------------------------------------------
file(GLOB_RECURSE LVGL_SOURCES CONFIGURE_DEPENDS
    "${PROJ_ROOT}/app_middlewares/lvgl/src/*.c"
)
# Match embedded build: compile LVGL at -Os to keep binary small.
set_source_files_properties(${LVGL_SOURCES} PROPERTIES COMPILE_OPTIONS "-Os")

set(GUI_MGR_SOURCES
    "${PROJ_ROOT}/gui_mgr/screenhandler.c"
    "${PROJ_ROOT}/gui_mgr/screens/boot_screen.c"
    "${PROJ_ROOT}/gui_mgr/screens/thpy_select_screen.c"
    "${PROJ_ROOT}/gui_mgr/screens/thpy_config_screen.c"
    "${PROJ_ROOT}/gui_mgr/screens/thpy_run_screen.c"
    "${PROJ_ROOT}/gui_mgr/fonts/mono_16.c"
    "${PROJ_ROOT}/gui_mgr/fonts/mono_24.c"
    "${PROJ_ROOT}/gui_mgr/fonts/mono_cm_96.c"
)

# ---------------------------------------------------------------------------
# Simulator executable
# ---------------------------------------------------------------------------
add_executable(npwt_sim
    # Sim entry point
    main.c

    # Platform abstraction — pthread implementation
    ui_platform_sim.c

    # SDL2 HAL (copied from ref_ui_sim)
    hal/hal_sdl.c

    # Simulator display port (same API as lv_port_st7789)
    port/lv_port_sim.c

    # Simulator BSP — keypad → SDL keyboard
    bsp/board_sim.c

    # Mock sensor data feed
    mock/mock_data.c

    # SDL2 mouse cursor image (copied from ref_ui_sim)
    mouse_cursor_icon.c

    # -----------------------------------------------------------------------
    # Shared UI sources — compiled unchanged for both embedded and simulator.
    # Only the platform abstraction layer differs.
    # -----------------------------------------------------------------------
    ${PROJ_ROOT}/app/gui_mgr.c
    ${GUI_MGR_SOURCES}

    # LVGL library
    ${LVGL_SOURCES}
)

# ---------------------------------------------------------------------------
# Include paths
# ---------------------------------------------------------------------------
target_include_directories(npwt_sim PRIVATE
    # sim/ root — found first so sim/lv_conf.h shadows config/lv_conf.h,
    # and sim/app_utils/assert/app_assert.h shadows the embedded one.
    ${CMAKE_CURRENT_SOURCE_DIR}

    # Project root — resolves app/, bsp/, app_hal/, config/ etc.
    ${PROJ_ROOT}

    # gui_mgr/ — screenhandler.h and screens/*.h
    ${PROJ_ROOT}/gui_mgr

    # config/ — appconfig.h (DISP_HOR_RES, DISP_VER_RES, error codes)
    ${PROJ_ROOT}/config

    # LVGL umbrella header
    ${PROJ_ROOT}/app_middlewares/lvgl
    ${PROJ_ROOT}/app_middlewares/lvgl/include
    ${PROJ_ROOT}/app_middlewares/lvgl/include/lvgl

    # SDL2
    ${SDL2_INCLUDE_DIRS}
)

# ---------------------------------------------------------------------------
# Compile definitions
# ---------------------------------------------------------------------------
target_compile_definitions(npwt_sim PRIVATE
    LV_SIM_BUILD=1
    LV_CONF_INCLUDE_SIMPLE
)

# ---------------------------------------------------------------------------
# Compile options
# ---------------------------------------------------------------------------
target_compile_options(npwt_sim PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -g
)

# ---------------------------------------------------------------------------
# Link libraries
# ---------------------------------------------------------------------------
target_link_libraries(npwt_sim PRIVATE
    ${SDL2_LIBRARIES}
    pthread
    m
)
