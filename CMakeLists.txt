cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)
set(VCPKG_TARGET_TRIPLET "x64-windows-static")
set(USE_OPEN_SSL ON)
set(USE_TLS ON)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type configuration" FORCE)
    message(STATUS "Setting default build type: ${CMAKE_BUILD_TYPE}")
endif ()

if (UNIX)
    set(CMAKE_SYSTEM_NAME Windows)
    set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

    # cross compilers to use for C, C++ and Fortran
    set(PRIVATE_MINGW_PATH /usr/sbin)
    set(CMAKE_C_COMPILER ${PRIVATE_MINGW_PATH}/${TOOLCHAIN_PREFIX}-gcc)
    set(CMAKE_CXX_COMPILER ${PRIVATE_MINGW_PATH}/${TOOLCHAIN_PREFIX}-g++)
    set(CMAKE_Fortran_COMPILER ${PRIVATE_MINGW_PATH}/${TOOLCHAIN_PREFIX}-gfortran)
    set(CMAKE_RC_COMPILER /usr/bin/${TOOLCHAIN_PREFIX}-windres)

    # target environment on the build host system
    set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})

    # modify default behavior of FIND_XXX() commands
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -O2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -O2")
endif ()

project(nexus_drf_ui)

include_directories(include)

# load imgui as a cmake project
add_subdirectory(src/imgui)

find_package(boost_asio CONFIG REQUIRED)
find_package(boost_random REQUIRED CONFIG)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(websocketpp CONFIG REQUIRED)

add_library(nexus_drf_ui SHARED src/main.cpp src/globals.cpp src/gui.cpp src/settings.cpp src/drf_client.cpp)

target_link_libraries(nexus_drf_ui PRIVATE imgui)
target_link_libraries(nexus_drf_ui PRIVATE Boost::asio)
target_link_libraries(nexus_drf_ui PRIVATE Boost::random)
target_link_libraries(nexus_drf_ui PRIVATE OpenSSL::SSL)
target_link_libraries(nexus_drf_ui PRIVATE OpenSSL::Crypto)
target_link_libraries(nexus_drf_ui PRIVATE ZLIB::ZLIB)
target_link_libraries(nexus_drf_ui PRIVATE websocketpp::websocketpp)

set_property(TARGET nexus_drf_ui PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# copy dll to game directory
add_custom_command(TARGET nexus_drf_ui
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:nexus_drf_ui>
        "C:\\Games\\Guild Wars 2\\addons\\nexus_drf_ui.dll.update"
        COMMENT "Copying nexus_drf_ui.dll.update to game directory"
)
