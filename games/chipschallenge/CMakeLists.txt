# Chip's Challenge - recompiled executable.
#
# Links the per-game host (src/main.c) + the recompiler-generated C against the
# lynxrecomp runtime. The generated file is produced from your own ROM:
#   m65c02recomp emit "roms/Chip's Challenge (USA, Europe).lnx" games/chipschallenge/generated
add_executable(chipschallenge
    src/main.c
    generated/recomp_funcs.c
)

target_include_directories(chipschallenge PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)

target_link_libraries(chipschallenge PRIVATE lynxrecomp)

# The generated C is machine-emitted; keep its warnings quiet.
if(MSVC)
    target_compile_options(chipschallenge PRIVATE /W1 /wd4244)
else()
    target_compile_options(chipschallenge PRIVATE -w)
endif()
