cmake_minimum_required(VERSION 3.19)
project(cielvox_tts_cpp LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(CIELVOX_VULKAN "Build ggml with the Vulkan backend" ON)
option(CIELVOX_CUDA "Build ggml with the CUDA backend" OFF)
option(CIELVOX_BUILD_CLI "Build the command line app" ON)
option(CIELVOX_BUILD_SERVER "Build the HTTP server" ON)
option(CIELVOX_BUILD_SHARED "Build the shared C library" ON)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ggml/CMakeLists.txt")
    message(FATAL_ERROR "third_party/ggml is missing. Run: git submodule update --init --recursive")
endif()

set(GGML_VULKAN ${CIELVOX_VULKAN} CACHE BOOL "" FORCE)
set(GGML_CUDA ${CIELVOX_CUDA} CACHE BOOL "" FORCE)
set(GGML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GGML_BUILD_TESTS OFF CACHE BOOL "" FORCE)
if(MINGW)
    # libgomp has no static build here, and it would leak into the shared library
    set(GGML_OPENMP OFF CACHE BOOL "" FORCE)
endif()
add_subdirectory(third_party/ggml)

set(CIELVOX_SOURCES
    src/common.cpp
    src/audio.cpp
    src/config.cpp
    src/gguf_loader.cpp
    src/tokenizer.cpp
    src/text_encoder.cpp
    src/backbone.cpp
    src/depth_decoder.cpp
    src/codec.cpp
    src/codec_conv.cpp
    src/codec_decoder.cpp
    src/codec_transformer.cpp
    src/sampling.cpp
    src/generation.cpp
    src/text_split.cpp
    src/voice.cpp
    src/model.cpp
)

add_library(cielvox_core STATIC ${CIELVOX_SOURCES})
target_include_directories(cielvox_core PUBLIC include)
target_link_libraries(cielvox_core PUBLIC ggml)

# make mingw binaries self contained so they do not pick up stray runtime DLLs
if(MINGW)
    add_link_options(-static-libgcc -static-libstdc++ -static)
endif()

set(CIELVOX_WEBUI_HEADER ${CMAKE_CURRENT_BINARY_DIR}/generated/breeze_webui_assets.h)
add_custom_command(
    OUTPUT ${CIELVOX_WEBUI_HEADER}
    COMMAND ${CMAKE_COMMAND}
        -DWEBUI_DIR=${CMAKE_CURRENT_SOURCE_DIR}/webui
        -DOUT_HEADER=${CIELVOX_WEBUI_HEADER}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/embed_webui.cmake
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/webui/index.html
        ${CMAKE_CURRENT_SOURCE_DIR}/webui/style.css
        ${CMAKE_CURRENT_SOURCE_DIR}/webui/app.js
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/embed_webui.cmake
    COMMENT "Embedding WebUI assets"
    VERBATIM
)
add_custom_target(cielvox_webui DEPENDS ${CIELVOX_WEBUI_HEADER})

if(CIELVOX_BUILD_SHARED)
    add_library(cielvox SHARED src/c_api.cpp)
    target_link_libraries(cielvox PRIVATE cielvox_core)
    target_compile_definitions(cielvox PRIVATE CIELVOX_BUILD_SHARED)
    set_target_properties(cielvox PROPERTIES OUTPUT_NAME cielvox)
endif()

if(CIELVOX_BUILD_CLI)
    add_executable(cielvox-cli apps/cli/main.cpp)
    target_link_libraries(cielvox-cli PRIVATE cielvox_core)

    add_executable(cielvox-convert apps/convert/main.cpp)
    target_link_libraries(cielvox-convert PRIVATE cielvox_core)
endif()

add_executable(cielvox-quantize apps/quantize/main.cpp)
target_link_libraries(cielvox-quantize PRIVATE cielvox_core)

if(CIELVOX_BUILD_SERVER)
    add_executable(cielvox-server apps/server/main.cpp apps/server/server.cpp apps/server/voices.cpp
                                 apps/server/ws.cpp apps/server/ws_api.cpp)
    add_dependencies(cielvox-server cielvox_webui)
    target_include_directories(cielvox-server PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated third_party)
    target_link_libraries(cielvox-server PRIVATE cielvox_core)
    if(WIN32)
        target_link_libraries(cielvox-server PRIVATE ws2_32)
    endif()
    find_package(Threads REQUIRED)
    target_link_libraries(cielvox-server PRIVATE Threads::Threads)
endif()
