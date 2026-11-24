cmake_minimum_required(VERSION 3.16)
project(C3)

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

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
target_link_libraries(C3 PRIVATE -static-libgcc -static-libstdc++)
