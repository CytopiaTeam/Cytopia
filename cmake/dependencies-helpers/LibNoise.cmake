if (${_PREFIX}USE_LIBNOISE STREQUAL "CONAN")
    list(APPEND _compile_definitions NOISE_IN_SUBDIR)
endif()