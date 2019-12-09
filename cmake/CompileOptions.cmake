set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED YES)

# Set warnings as errors flag
option(TREAT_WARNINGS_AS_ERRORS "Treat all warnings as errors" ON)

if(TREAT_WARNINGS_AS_ERRORS)
	if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
		set(WARN_AS_ERROR_FLAGS	"/WX")
	else()
		set(WARN_AS_ERROR_FLAGS "-Werror")
	endif()
endif()

# MSVC compiler options
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	# remove default warning level from CMAKE_CXX_FLAGS
	string (REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# MSVC compiler options
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
        /MP           # -> build with multiple processes
        /W4           # -> warning level 4
        /EHsc         # -> allow exception handlers
        ${WARN_AS_ERROR_FLAGS}

        /wd4996       # -> disable warning: 'localtime': This function or variable may be unsafe. Consider using localtime_s instead.
        # /wd4251     # -> disable warning: 'identifier': class 'type' needs to have dll-interface to be used by clients of class 'type2'
        # /wd4592     # -> disable warning: 'identifier': symbol will be dynamically initialized (implementation limitation)
        # /wd4201     # -> disable warning: nonstandard extension used: nameless struct/union (caused by GLM)
        # /wd4127     # -> disable warning: conditional expression is constant (caused by Qt)

        #$<$<CONFIG:Debug>:
        #/RTCc        # -> value is assigned to a smaller data type and results in a data loss
        #>

        $<$<CONFIG:Release>:
        /Gw           # -> whole program global optimization
        /GS-          # -> buffer security check: no
        /GL           # -> whole program optimization: enable link-time code generation (disables Zi)
        /GF           # -> enable string pooling
        >

    )
endif ()

# GCC and Clang compiler options
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if (BUILD_TEST)
    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
    		-O0
    		-Wall
    		-fprofile-arcs
    		-ftest-coverage
    )
  else()
      set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
          -Wall
          -Wno-missing-braces
          ${WARN_AS_ERROR_FLAGS}
      )
  endif (BUILD_TEST)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
        -Wno-int-in-bool-context
    )
endif ()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
        -stdlib=libc++
    )
endif ()

