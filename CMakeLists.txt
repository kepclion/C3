cmake_minimum_required(VERSION 3.16)
project(C3)

# 可选：MSVC 下静态链接 C/C++ 运行库（Release=/MT，Debug=/MTd）。
# 默认 OFF 保持原有 /MD 行为；发布免 Redistributable 版本时使用 -DC3_STATIC_MSVC_RUNTIME=ON。
option(C3_STATIC_MSVC_RUNTIME "Statically link the MSVC runtime (/MT)" OFF)

if(MSVC AND C3_STATIC_MSVC_RUNTIME)
    # Debug 使用 MultiThreadedDebug，其他配置使用 MultiThreaded。
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(C3 main.cpp
        Lines/Line.cpp
        Animation.cpp
        ConsoleUtils.cpp
        Game.cpp
        SaveData.cpp
        Story.cpp
        AniUtils.cpp
)


if(MSVC)
    target_compile_options(C3 PRIVATE /utf-8)
endif()

if(MINGW OR CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(C3 PRIVATE -static-libgcc -static-libstdc++)
endif()

target_include_directories(C3 PRIVATE third_party)

if(MINGW)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_C_COMPILER}" DIRECTORY)


    set(MINGW_RUNTIME_DLLS
            "${MINGW_BIN_DIR}/libwinpthread-1.dll"
            "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "${MINGW_BIN_DIR}/libstdc++-6.dll"
    )

    # 在构建后复制到 exe 所在目录
    add_custom_command(TARGET C3 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${MINGW_RUNTIME_DLLS}
            $<TARGET_FILE_DIR:C3>
            COMMENT "Copying MinGW runtime DLLs to output directory"
    )
endif()

