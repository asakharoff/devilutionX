# Options that control whether to use system dependencies or build them from source,
# and whether to link them statically.
include(functions/dependency_options)
include(functions/emscripten_system_library)

if(EMSCRIPTEN)
  emscripten_system_library("zlib" ZLIB::ZLIB USE_ZLIB=1)
else()
  dependency_options("zlib" DEVILUTIONX_SYSTEM_ZLIB ON DEVILUTIONX_STATIC_ZLIB)
  if(NOT DEVILUTIONX_SYSTEM_ZLIB)
    add_subdirectory(3rdParty/zlib)
  endif()
endif()

dependency_options("bzip2" DEVILUTIONX_SYSTEM_BZIP2 ON DEVILUTIONX_STATIC_BZIP2)
if(DEVILUTIONX_SYSTEM_BZIP2)
  find_package(BZip2 REQUIRED)
else()
  add_subdirectory(3rdParty/bzip2)
endif()

if(EMSCRIPTEN)
  # We use `USE_PTHREADS=1` here to get a version of SDL2 that supports threads.
  emscripten_system_library("SDL2" SDL2::SDL2 USE_SDL=2 USE_PTHREADS=1)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/SDL2/CMake")
elseif(USE_SDL1)
  find_package(SDL REQUIRED)
  include_directories(${SDL_INCLUDE_DIR})
else()
  dependency_options("SDL2" DEVILUTIONX_SYSTEM_SDL2 ON DEVILUTIONX_STATIC_SDL2)
  if(DEVILUTIONX_SYSTEM_SDL2)
    find_package(SDL2 REQUIRED)
    if(TARGET SDL2::SDL2)
      set(SDL2_MAIN SDL2::SDL2main)
    elseif(TARGET SDL2::SDL2-static)
      # On some distros, such as vitasdk, only the SDL2::SDL2-static target is available.
      # Alias to SDL2::SDL2 because some finder scripts may refer to SDL2::SDL2.
      if(CMAKE_VERSION VERSION_LESS "3.18")
        # Aliasing local targets is not supported on CMake < 3.18, so make it global.
        set_target_properties(SDL2::SDL2-static PROPERTIES IMPORTED_GLOBAL TRUE)
      endif()
      set(SDL2_MAIN SDL2::SDL2main)
    else()
      # Assume an older Debian derivate that comes with an sdl2-config.cmake
      # that only defines `SDL2_LIBRARIES` (as -lSDL2) and `SDL2_INCLUDE_DIRS`.
      add_library(SDL2_lib INTERFACE)
      target_link_libraries(SDL2_lib INTERFACE ${SDL2_LIBRARIES})
      target_include_directories(SDL2_lib INTERFACE ${SDL2_INCLUDE_DIRS})
      # Can't define an INTERFACE target with ::, so alias instead
      add_library(SDL2::SDL2 ALIAS SDL2_lib)
    endif()
  elseif(UWP_LIB)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/SDL2/CMake")
    find_package(SDL2 REQUIRED)
  else()
    add_subdirectory(3rdParty/SDL2)
    set(SDL2_MAIN SDL2::SDL2main)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/SDL2/CMake")
  endif()
endif()

add_library(DevilutionX::SDL INTERFACE IMPORTED GLOBAL)
if(USE_SDL1)
  target_link_libraries(DevilutionX::SDL INTERFACE ${SDL_LIBRARY})
  target_compile_definitions(DevilutionX::SDL INTERFACE USE_SDL1)
else()
  if(TARGET SDL2::SDL2)
    target_link_libraries(DevilutionX::SDL INTERFACE SDL2::SDL2)
  elseif(TARGET SDL2::SDL2-static)
    target_link_libraries(DevilutionX::SDL INTERFACE SDL2::SDL2-static)
  endif()
  if(NOT UWP_LIB)
    target_link_libraries(DevilutionX::SDL INTERFACE ${SDL2_MAIN})
  endif()
endif()

macro(_find_SDL_image QUIET_OR_REQUIRED)
  if(USE_SDL1)
    find_package(SDL_image ${QUIET_OR_REQUIRED})
  else()
    # vcpkg uses sdl2-image as the package name
    find_package(sdl2-image QUIET)
    set(SDL_image_FOUND ${sdl2-image_FOUND})

    if(NOT SDL_image_FOUND)
      # Fall back on PkgConfig via FindSDL2_image.cmake
      find_package(SDL2_image ${QUIET_OR_REQUIRED})
      set(SDL_image_FOUND ${SDL2_image_FOUND})
    endif()
  endif()
endmacro()

if(EMSCRIPTEN)
  emscripten_system_library("SDL_image" SDL2::SDL2_image USE_SDL_IMAGE=2 "SDL2_IMAGE_FORMATS='[\"png\"]'")
else()
  if(NOT DEFINED DEVILUTIONX_SYSTEM_SDL_IMAGE)
    _find_SDL_image(QUIET)
    if(SDL_image_FOUND)
      message("-- Found SDL_image")
    else()
      message("-- Suitable system SDL_image package not found, will use SDL_image from source")
      set(DEVILUTIONX_SYSTEM_SDL_IMAGE OFF)
    endif()
  endif()
  dependency_options("SDL_image" DEVILUTIONX_SYSTEM_SDL_IMAGE ON DEVILUTIONX_STATIC_SDL_IMAGE)
  if(DEVILUTIONX_SYSTEM_SDL_IMAGE)
    _find_SDL_image(REQUIRED)
  else()
    add_subdirectory(3rdParty/SDL_image)
  endif()
endif()

add_library(DevilutionX::SDL_image INTERFACE IMPORTED GLOBAL)
if(USE_SDL1)
  target_link_libraries(DevilutionX::SDL_image INTERFACE SDL_image)
else()
  target_link_libraries(DevilutionX::SDL_image INTERFACE SDL2::SDL2_image)
endif()

if(NOT NOSOUND)
  dependency_options("SDL_audiolib" DEVILUTIONX_SYSTEM_SDL_AUDIOLIB OFF DEVILUTIONX_STATIC_SDL_AUDIOLIB)
  if(DEVILUTIONX_SYSTEM_SDL_AUDIOLIB)
    find_package(SDL_audiolib REQUIRED)
  else()
    add_subdirectory(3rdParty/SDL_audiolib)
  endif()
endif()

if(PACKET_ENCRYPTION)
  dependency_options("libsodium" DEVILUTIONX_SYSTEM_LIBSODIUM ON DEVILUTIONX_STATIC_LIBSODIUM)
  if(DEVILUTIONX_SYSTEM_LIBSODIUM)
    set(sodium_USE_STATIC_LIBS ${DEVILUTIONX_STATIC_LIBSODIUM})
    find_package(sodium REQUIRED)
  else()
    add_subdirectory(3rdParty/libsodium)
  endif()
endif()

if(NOT DEFINED DEVILUTIONX_SYSTEM_LIBFMT)
  find_package(fmt 7.0.0 QUIET)
  if(fmt_FOUND)
    message("-- Found fmt ${fmt_VERSION}")
  else()
    message("-- Suitable system fmt package not found, will use fmt from source")
    set(DEVILUTIONX_SYSTEM_LIBFMT OFF)
  endif()
endif()
dependency_options("libfmt" DEVILUTIONX_SYSTEM_LIBFMT ON DEVILUTIONX_STATIC_LIBFMT)
if(DEVILUTIONX_SYSTEM_LIBFMT)
  find_package(fmt 7.0.0 REQUIRED)
else()
  add_subdirectory(3rdParty/libfmt)
endif()

add_subdirectory(3rdParty/libsmackerdec)

if(WIN32 AND NOT UWP_LIB)
  add_subdirectory(3rdParty/find_steam_game)
endif()

add_subdirectory(3rdParty/simpleini)

add_subdirectory(3rdParty/libmpq)

add_subdirectory(3rdParty/hoehrmann_utf8)

add_subdirectory(3rdParty/PKWare)

if(NOT NONET AND NOT DISABLE_TCP)
  add_subdirectory(3rdParty/asio)
endif()

if(NOT NONET AND NOT DISABLE_ZERO_TIER)
  add_subdirectory(3rdParty/libzt)
endif()

if(DISCORD_INTEGRATION)
  add_subdirectory(3rdParty/discord)
endif()

if(BUILD_TESTING)
  dependency_options("googletest" DEVILUTIONX_SYSTEM_GOOGLETEST ON DEVILUTIONX_STATIC_GOOGLETEST)
  if(DEVILUTIONX_SYSTEM_GOOGLETEST)
    find_package(GTest REQUIRED)
    if(NOT TARGET GTest::gtest)
      if(CMAKE_VERSION VERSION_LESS "3.18")
        # Aliasing local targets is not supported on CMake < 3.18, so make it global.
        set_target_properties(GTest::GTest PROPERTIES IMPORTED_GLOBAL TRUE)
      endif()
      add_library(GTest::gtest ALIAS GTest::GTest)
    endif()
  else()
    add_subdirectory(3rdParty/googletest)
  endif()
endif()

if(GPERF)
  find_package(Gperftools REQUIRED)
  message("INFO: ${GPERFTOOLS_LIBRARIES}")
endif()
