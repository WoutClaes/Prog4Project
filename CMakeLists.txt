cmake_minimum_required(VERSION 3.26)
project(Prog4Project)

# ============================================================
# Dependencies (shared between engine and game)
# ============================================================
include(FetchContent)

find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
  FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
    DOWNLOAD_NO_PROGRESS ON
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  )
  FetchContent_MakeAvailable(glm)
endif()

find_package(SDL3 3.4 CONFIG QUIET)
if(NOT SDL3_FOUND)
  if(WIN32)
    FetchContent_Declare(
      SDL3
      URL https://www.libsdl.org/release/SDL3-devel-3.4.0-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_SOURCE_DIR}")
    find_package(SDL3 CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG release-3.4.0
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(SDL3)
  endif()
endif()

find_package(SDL3_ttf 3.2.2 CONFIG QUIET)
if(NOT SDL3_ttf_FOUND)
  if(WIN32)
    FetchContent_Declare(
      SDL3_ttf
      URL https://www.libsdl.org/projects/SDL_ttf/release/SDL3_ttf-devel-3.2.2-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3_ttf)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_ttf_SOURCE_DIR}")
    find_package(SDL3_ttf CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3_ttf
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
      GIT_TAG release-3.2.2
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    set(SDL3TTF_INSTALL OFF)
    if(EMSCRIPTEN)
      list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/emscripten")
      set(SDL3TTF_USE_SYSTEM_FREETYPE ON)
      set(SDL3TTF_VENDORED OFF)
      set(SDLTTF_PLUTOSVG OFF)
      add_compile_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
      add_link_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
    endif()
    FetchContent_MakeAvailable(SDL3_ttf)
  endif()
endif()

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.92.5-docking
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${sdl3_SOURCE_DIR}/include)
target_link_libraries(imgui PUBLIC SDL3::SDL3)
target_compile_features(imgui PUBLIC cxx_std_20)

if(WIN32)
  find_package(VLD CONFIG)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/steam")
find_package(Steamworks QUIET)

# ============================================================
# Minigin — static library (the engine)
# ============================================================
add_library(minigin_lib STATIC
  Minigin/Font.cpp
  Minigin/GameObject.cpp
  Minigin/InputManager.cpp
  Minigin/Minigin.cpp
  Minigin/Renderer.cpp
  Minigin/ResourceManager.cpp
  Minigin/Scene.cpp
  Minigin/SceneManager.cpp
  Minigin/Texture2D.cpp
  Minigin/Transform.cpp
  Minigin/Controller.cpp

  Minigin/Components/RenderComponent.cpp
  Minigin/Components/TextComponent.cpp
  Minigin/Components/FPSComponent.cpp
  Minigin/Components/TransformComponent.cpp
  Minigin/Components/OrbitComponent.cpp
  Minigin/Components/CacheBenchmarkComponent.cpp
  Minigin/Components/HealthComponent.cpp
  Minigin/Components/ScoreComponent.cpp
  Minigin/Components/LivesDisplayComponent.cpp
  Minigin/Components/ScoreDisplayComponent.cpp
  Minigin/Components/SpriteComponent.cpp

  Minigin/Commands/MoveCommand.cpp
  Minigin/Commands/DamageCommand.cpp
  Minigin/Commands/AddPointsCommand.cpp
  Minigin/Commands/ResetAchievementsCommand.cpp

  Minigin/Observer/Subject.cpp
  Minigin/Observer/SteamAchievementObserver.cpp

  Minigin/Sound/ServiceLocator.cpp
  Minigin/Sound/SDLSoundSystem.cpp
)

target_compile_features(minigin_lib PUBLIC cxx_std_20)

target_include_directories(minigin_lib PUBLIC
  ${CMAKE_SOURCE_DIR}/Minigin/
  ${CMAKE_SOURCE_DIR}/Minigin/Sound/
  ${glm_SOURCE_DIR}
  ${sdl3_SOURCE_DIR}/include
  ${sdl3_ttf_SOURCE_DIR}/include
  ${imgui_SOURCE_DIR}
  ${VLD_INCLUDE_DIR}
)

target_link_libraries(minigin_lib PUBLIC
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
  imgui
  glm::glm
  ${VLD_LIBRARIES}
)

if(WIN32 AND NOT EMSCRIPTEN)
  target_link_libraries(minigin_lib PUBLIC xinput)
endif()

if(Steamworks_FOUND)
  target_link_libraries(minigin_lib PUBLIC Steamworks::Steamworks)
  target_compile_definitions(minigin_lib PUBLIC USE_STEAMWORKS)
endif()

if(MSVC)
  target_compile_options(minigin_lib PRIVATE /W4 /WX)
else()
  target_compile_options(minigin_lib PRIVATE -Wall -Wextra -Werror)
endif()

# ============================================================
# Game — executable (replace Game/ with your game folder)
# ============================================================
if(EMSCRIPTEN)
  set(GAME_TARGET game_web)
  set(GAME_PARAM)
else()
  set(GAME_TARGET game)
  set(GAME_PARAM WIN32)
endif()

add_executable(${GAME_TARGET} ${GAME_PARAM}
  Game/Main.cpp
  # Add your game source files here
)

target_compile_features(${GAME_TARGET} PUBLIC cxx_std_20)

target_include_directories(${GAME_TARGET} PRIVATE
  ${CMAKE_SOURCE_DIR}/Game/
)

# Links against the engine — gets all engine headers and libs transitively
target_link_libraries(${GAME_TARGET} PRIVATE minigin_lib)

if(MSVC)
  target_compile_options(${GAME_TARGET} PRIVATE /W4 /WX)
else()
  target_compile_options(${GAME_TARGET} PRIVATE -Wall -Wextra -Werror)
endif()

# ============================================================
# Post-build: copy DLLs and Data
# ============================================================
if(EMSCRIPTEN)
  set(EMSCRIPTEN_COMPILE_FLAGS "-g")
  set(EMSCRIPTEN_LINK_FLAGS "-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -g -sALLOW_MEMORY_GROWTH --preload-file \"${CMAKE_SOURCE_DIR}/Data@/\"")
  set_target_properties(${GAME_TARGET} PROPERTIES
    COMPILE_FLAGS "${EMSCRIPTEN_COMPILE_FLAGS}"
    LINK_FLAGS    "${EMSCRIPTEN_LINK_FLAGS}"
  )
  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/web" "$<TARGET_FILE_DIR:${GAME_TARGET}>/"
  )
  set(CMAKE_EXECUTABLE_SUFFIX ".html")

elseif(WIN32)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${GAME_TARGET})
  set_target_properties(${GAME_TARGET} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${GAME_TARGET}>"
  )

  foreach(dllTarget IN ITEMS SDL3::SDL3 SDL3_ttf::SDL3_ttf)
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:${dllTarget}>"
        "$<TARGET_FILE_DIR:${GAME_TARGET}>"
      VERBATIM
    )
  endforeach()

  if(Steamworks_FOUND)
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy "${Steamworks_RUNTIME_DLL}" "$<TARGET_FILE_DIR:${GAME_TARGET}>")
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${GAME_TARGET}>/steam_appid.txt"
    )
  endif()

  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${GAME_TARGET}>/Data"
  )

elseif(APPLE)
  set_target_properties(${GAME_TARGET} PROPERTIES
    MACOSX_BUNDLE TRUE
    INSTALL_RPATH "@executable_path/../Frameworks"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_BUNDLE_DIR:${GAME_TARGET}>/Contents/Frameworks"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3-shared>"     "$<TARGET_BUNDLE_DIR:${GAME_TARGET}>/Contents/Frameworks/"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>" "$<TARGET_BUNDLE_DIR:${GAME_TARGET}>/Contents/Frameworks/"
  )
  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${GAME_TARGET}>/Data"
  )

else()
  add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${GAME_TARGET}>/Data"
  )
endif()
