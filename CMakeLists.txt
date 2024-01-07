cmake_minimum_required(VERSION 3.11)

set(PROJECT_N opty)
project(${PROJECT_N}
    VERSION 1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

option(WITH_NETWORK "build OpenTyrian with SDL network support (currently unsupported)" FALSE) # need to figure out the dependencies
option(TYRIAN2000 "use Tyrian2000 assets (currently unsupported)" FALSE)

include(FetchContent)

FetchContent_Declare(
    dl_SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG 4cd981609b50ed273d80c635c1ca4c1e5518fb21
)

FetchContent_MakeAvailable(dl_SDL2)

if (WITH_NETWORK)
    FetchContent_Declare(
        dl_SDL2net
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
        GIT_TAG 669e75b84632e2c6cc5c65974ec9e28052cb7a4e
    )

    set(SDL2_LIBRARY SDL2)
    FetchContent_GetProperties(dl_SDL2 SOURCE_DIR SDL2_LIBRARY_DIR)
    set(SDL2_INCLUDE_DIR "${SDL2_LIBRARY_DIR}/include")
    set(SDL2NET_SAMPLES OFF)

    FetchContent_MakeAvailable(dl_SDL2net)

    set(SDL2NET_LIB SDL2_net)
endif()


if (NOT $ENV{TYRIAN_DIR} STREQUAL "")
    message("setting TYRIAN_DIR to envvar value $ENV{TYRIAN_DIR}")
    set(TYRIAN_DIR $ENV{TYRIAN_DIR})
elseif (WIN32)
    set(TYRIAN_DIR "C:\\TYRIAN")
else()
    set(TYRIAN_DIR "$ENV{gamesdir}/tyrian")
endif()

message("TYRIAN_DIR set to ${TYRIAN_DIR}")

set(SRCS
    src/animlib.c
    src/arg_parse.c
    src/backgrnd.c
    src/config.c
    src/config_file.c
    src/destruct.c
    src/editship.c
    src/episodes.c
    src/file.c
    src/font.c
    src/fonthand.c
    src/game_menu.c
    src/helptext.c
    src/joystick.c
    src/jukebox.c
    src/keyboard.c
    src/lds_play.c
    src/loudness.c
    src/lvllib.c
    src/lvlmast.c
    src/mainint.c
    src/menus.c
    src/mouse.c
    src/mtrand.c
    src/musmast.c
    src/network.c
    src/nortsong.c
    src/nortvars.c
    src/opentyr.c
    src/opl.c
    src/palette.c
    src/params.c
    src/pcxload.c
    src/pcxmast.c
    src/picload.c
    src/player.c
    src/shots.c
    src/sizebuf.c
    src/sndmast.c
    src/sprite.c
    src/starlib.c
    src/tyrian2.c
    src/varz.c
    src/vga256d.c
    src/vga_palette.c
    src/video.c
    src/video_scale.c
    src/video_scale_hqNx.c
    src/xmas.c
)

add_executable(${PROJECT_N} ${SRCS})

set_target_properties(${PROJECT_N}
PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED TRUE
)

target_include_directories(${PROJECT_N}
PUBLIC
    include
    ${source_dir}/include
    ${SDLCOMPAT_SOURCE_DIR}/include/SDL
)

if (WIN32)
endif()

set(EXTRA_COMPILE_DEFINITIONS
    TYRIAN_DIR="${TYRIAN_DIR}"

    # SDL options
    _REENTRANT
    SDL_MAIN_HANDLED
)

if(WITH_NETWORK)
    set(EXTRA_COMPILE_DEFINITIONS ${EXTRA_COMPILE_DEFINITIONS} WITH_NETWORK)
endif()
if(TYRIAN2000)
    message("TYRIAN2000 is not currently supported, ignoring")
    #message("using TYRIAN2000 asset files")
    #set(EXTRA_COMPILE_DEFINITIONS ${EXTRA_COMPILE_DEFINITIONS} TYRIAN2000)
endif()

target_compile_definitions(${PROJECT_N}
PUBLIC
    ${EXTRA_COMPILE_DEFINITIONS}
)
target_link_libraries(${PROJECT_N}
PRIVATE
    SDL2
    ${SDL2NET_LIB}
)

if (WIN32)
    add_custom_command(TARGET ${PROJECT_N} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_N}> $<TARGET_FILE_DIR:${PROJECT_N}>
        COMMAND_EXPAND_LISTS
    )
else()
    target_link_libraries(${PROJECT_N}
    PRIVATE
        m
    )
endif()
