cmake_minimum_required(VERSION 3.28)
project(wave LANGUAGES C)

set(CMAKE_C_STANDARD           99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS         ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.c)

option(SANITIZE "Build with sanitizers" ON)

if(SANITIZE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(SAN_FLAGS
        -fsanitize=address,undefined
        -fno-sanitize-recover=all
        -fno-omit-frame-pointer
    )
    add_compile_options(${SAN_FLAGS} -g -O0)
    add_link_options(${SAN_FLAGS})
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE 
        /W4
        /permissive-
    )
    #/fsanitize=address
else() # GCC or Clang
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
        -Wconversion 
        -Wshadow
        -Wstrict-prototypes
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_WARNING_AS_ERROR ON)

add_executable(${PROJECT_NAME}c main.c)
target_link_libraries(wavec PRIVATE wave)

option(WAVE_SANITIZE "Build with ASan" ON)

if(MSVC AND WAVE_SANITIZE)
    string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT ProgramDatabase)
    foreach(tgt wave wavec)
        target_compile_options(${tgt} PRIVATE /fsanitize=address)
    endforeach()
endif()

# mk: on windows use `cmake -G"MinGW Makefiles" ..` to generate `compile_commands.json`
# mk: to build on msvc: `cmake -G"Visual Studio 17 2022" ..` then `cmake --build .`