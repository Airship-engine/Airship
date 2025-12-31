if(CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy not found!")
    endif()
endif()

function(enable_clang_tidy tgt)
    if(CLANG_TIDY_EXE)
        set_target_properties(${tgt} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
        )
    endif()
endfunction()
