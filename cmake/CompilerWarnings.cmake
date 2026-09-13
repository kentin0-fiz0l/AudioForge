# CompilerWarnings.cmake
# Strict compiler warnings to prevent bugs like the v2.0.2 use-after-free
#
# Usage: include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/CompilerWarnings.cmake)
#        add_audioforge_warnings(target_name)

function(add_audioforge_warnings target)
    # Get the compiler warnings appropriate for this compiler
    set(CLANG_WARNINGS
        -Wall                       # Enable all warnings
        -Wextra                     # Enable extra warnings
        -Wpedantic                  # Warn about non-standard C++

        # CRITICAL: Catch the v2.0.2 bug type
        -Werror=address-of-temporary    # ERROR: Taking address of temporary (THE BUG WE FIXED!)
        -Werror=return-stack-address    # ERROR: Returning address of local variable
        -Werror=dangling                # ERROR: Dangling references

        # Memory safety
        -Wdangling-field            # Warn about dangling field references
        -Wunused-variable           # Catch unused variables
        -Wunused-parameter          # Catch unused parameters
        -Wshadow                    # Warn about variable shadowing

        # Modernization
        -Wdeprecated                # Warn about deprecated features
        -Wc++17-extensions          # Warn about C++17 usage (we require it)

        # Code quality
        -Wconversion                # Warn about implicit conversions
        -Wsign-conversion           # Warn about sign conversions
        -Wcast-align                # Warn about alignment changes in casts
        -Wformat=2                  # Extra format string checking
        -Wnull-dereference          # Warn about potential null dereferences
    )

    set(GCC_WARNINGS
        ${CLANG_WARNINGS}
        -Wmisleading-indentation    # Warn about misleading indentation
        -Wduplicated-cond           # Warn about duplicated conditions
        -Wduplicated-branches       # Warn about duplicated branches
        -Wlogical-op                # Warn about logical operation issues
        -Wuseless-cast              # Warn about useless casts
    )

    set(MSVC_WARNINGS
        /W4                         # Warning level 4
        /permissive-                # Standards conformance mode
        /w14242                     # Conversion, possible loss of data
        /w14254                     # Conversion, possible loss of data
        /w14263                     # Member function does not override
        /w14265                     # Class has virtual functions, but destructor is not virtual
        /w14287                     # Unsigned/negative constant mismatch
        /w14296                     # Expression is always true/false
        /w14311                     # Pointer truncation
        /w14545                     # Expression before comma evaluates to function missing argument list
        /w14546                     # Function call before comma missing argument list
        /w14547                     # Operator before comma has no effect
        /w14549                     # Operator before comma has no effect
        /w14555                     # Expression has no effect
        /w14619                     # #pragma warning: there is no warning number 'number'
        /w14640                     # Enable warning on thread unsafe static member initialization
        /w14826                     # Conversion from 'type1' to 'type2' is sign-extended
        /w14905                     # Wide string literal cast to 'LPSTR'
        /w14906                     # String literal cast to 'LPWSTR'
        /w14928                     # Illegal copy-initialization
    )

    # Apply warnings based on compiler
    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(PROJECT_WARNINGS ${CLANG_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(PROJECT_WARNINGS ${GCC_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(PROJECT_WARNINGS ${MSVC_WARNINGS})
    else()
        message(WARNING "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
    endif()

    target_compile_options(${target} PRIVATE ${PROJECT_WARNINGS})

    # Also make the critical warnings errors
    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target} PRIVATE
            -Werror=address-of-temporary
            -Werror=return-stack-address
        )
    endif()

    message(STATUS "AudioForge: Strict compiler warnings enabled for ${target}")
endfunction()

# Convenience function to add warnings to all AudioForge shared libraries
function(add_audioforge_warnings_to_shared)
    if(TARGET AudioForgeDSP)
        # Header-only library - add compile options via interface
        target_compile_options(AudioForgeDSP INTERFACE
            $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Werror=address-of-temporary>
            $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Werror=return-stack-address>
        )
        message(STATUS "AudioForge: Warnings enabled for AudioForgeDSP (interface)")
    endif()

    if(TARGET AudioForgeSynth)
        add_audioforge_warnings(AudioForgeSynth)
    endif()

    if(TARGET AudioForgePresets)
        add_audioforge_warnings(AudioForgePresets)
    endif()

    if(TARGET AudioForgeUI)
        add_audioforge_warnings(AudioForgeUI)
    endif()
endfunction()
