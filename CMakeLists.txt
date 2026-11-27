# file-format: 1.0.0
cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

include(cmake/target.cmake)
include(cmake/${TOOLCHAIN_FILE})

project(npwt_single_snsr)
add_executable(npwt_single_snsr)
enable_language(C CXX ASM)
include(cmake/files.cmake)
include(cmake/flags.cmake)
include(cmake/components.cmake)

file(GLOB TRICE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/app_utils/trice/*.c"
)

file(GLOB_RECURSE LVGL_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/app_middlewares/lvgl/src/*.c"
)
set_source_files_properties(${LVGL_SOURCES} PROPERTIES COMPILE_OPTIONS "-Os")

set(GUI_MGR_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/gui_mgr/screenhandler.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/gui_mgr/screens/boot_screen.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/gui_mgr/screens/thpy_select_screen.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/gui_mgr/screens/thpy_config_screen.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/gui_mgr/screens/thpy_run_screen.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/gui_mgr/fonts/dot_matrix_26.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/gui_mgr/fonts/dot_matrix_30.c"
)

# Stubs to be able to add compiler flag overrides, e.g. -Os
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
  # Add additional source files here
  app/app_main.c
  app/app_mgr.c
  app/diag_svc.c
  app/gui_mgr.c
  app/ui_platform_embedded.c
  # app/thpy_mgr.c
  app/press_regulator.c
  app/snsr_mgr.c
  app_drivers/lv_port_st7789.c
  bsp/board_v1_0.c
  app_hal/app_hal_delay.c
  app_hal/app_hal_spi.c
  app_hal/app_hal_wdt.c
  manualtests/testsuite.c
  app_utils/assert/app_assert.c
  ${GUI_MGR_SOURCES}
  ${TRICE_SOURCES}
  ${LVGL_SOURCES}
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
  # Add additional include directories here
  ${CMAKE_CURRENT_SOURCE_DIR}/config
  ${CMAKE_CURRENT_SOURCE_DIR}/gui_mgr
  ${CMAKE_CURRENT_SOURCE_DIR}/app_utils/trice
  ${CMAKE_CURRENT_SOURCE_DIR}/app_middlewares/lvgl
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
  # Add additional defined symbols here
)

target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC
  # Add additional compiler flags here
)

target_link_options(${CMAKE_PROJECT_NAME} PUBLIC
  # Add additional linker flags here
  "LINKER:-u,_printf_float"
  "LINKER:-u,_scanf_float"
)

target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC
  # Add additional linker search directories here
)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
  # Add additional linker libraries here
)
