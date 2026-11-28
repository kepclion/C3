# copy_deps.cmake
# Legacy helper only. It is not referenced by CMakeLists.txt and contains a
# machine-specific MinGW path. The MSVC /MT release does not need it.
set(DEPS "")
file(GET_RUNTIME_DEPENDENCIES
        EXECUTABLES "${SRC}"
        RESOLVED_DEPENDENCIES_VAR DEPS
        DIRECTORIES "D:/JetBrains/CLion 2026.3.0.1/bin/mingw/bin"
        PRE_EXCLUDE_REGEXES "api-ms-win-.*|ext-ms-.*|kernel32.*|msvcrt.*|ntdll.*|user32.*|gdi32.*|winspool.*|shell32.*|ole32.*|oleaut32.*|uuid.*|comdlg32.*|advapi32.*"
)
foreach(dll ${DEPS})
    file(COPY ${dll} DESTINATION ${DST})
endforeach()