cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    elseif(WIN32 AND EXISTS "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    endif()
endif()

project(JTetris LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED VCPKG_TARGET_TRIPLET AND WIN32)
    set(VCPKG_TARGET_TRIPLET "x64-windows")
endif()

set(_jtetris_vcpkg_prefix_candidates)

if(DEFINED VCPKG_TARGET_TRIPLET)
    if(EXISTS "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}")
        list(APPEND _jtetris_vcpkg_prefix_candidates "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}")
    endif()

    file(GLOB _jtetris_vcpkg_installed_dirs LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/build/*/vcpkg_installed/${VCPKG_TARGET_TRIPLET}")
    list(APPEND _jtetris_vcpkg_prefix_candidates ${_jtetris_vcpkg_installed_dirs})
endif()

if(_jtetris_vcpkg_prefix_candidates)
    list(REMOVE_DUPLICATES _jtetris_vcpkg_prefix_candidates)
    list(PREPEND CMAKE_PREFIX_PATH ${_jtetris_vcpkg_prefix_candidates})
    list(GET _jtetris_vcpkg_prefix_candidates 0 JTETRIS_VCPKG_TRIPLET_DIR)
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

file(GLOB Nuklear_Headers CONFIGURE_DEPENDS nuklear/*.h)
file(GLOB JTetris_Assets CONFIGURE_DEPENDS Assets/*)

file(GLOB JTetris_Headers CONFIGURE_DEPENDS include/*.h)
file(GLOB JTetris_Sources CONFIGURE_DEPENDS src/*.cpp)

add_executable(JTetris ${Nuklear_Headers} ${JTetris_Headers} ${JTetris_Sources} main.cpp)

target_include_directories(JTetris PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/nuklear
)

target_link_libraries(JTetris PRIVATE
    OpenGL::GL
    GLEW::GLEW
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)

add_custom_command(TARGET JTetris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:JTetris>/Assets
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${JTetris_Assets}
        $<TARGET_FILE_DIR:JTetris>/Assets
    COMMAND ${CMAKE_COMMAND}
        -Dbuild_config=$<CONFIG>
        -Dtriplet_dir=${JTETRIS_VCPKG_TRIPLET_DIR}
        -Dtarget_dir=$<TARGET_FILE_DIR:JTetris>
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyRuntimeDependencies.cmake
    COMMAND_EXPAND_LISTS
)