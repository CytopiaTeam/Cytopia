# The MIT License (MIT)
#
# Copyright (c) 2015 Fabian Killus
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Find AngelScript
# ----------------
#
# Find AngelScript include directories and libraries.
# This module will set the following variables:
#
# * Angelscript_FOUND       - True if AngelScript is found
# * Angelscript_INCLUDE_DIR - The include directory
# * Angelscript_LIBRARY     - The libraries to link against
#
# In addition the following imported targets are defined:
#
# * Angelscript::angelscript
# * AngelScript::use_namespace
#

find_path(Angelscript_INCLUDE_DIR angelscript.h)
find_library(Angelscript_LIBRARY angelscript PATH_SUFFIXES Debug Release)

set(Angelscript_INCLUDE_DIRS ${Angelscript_INCLUDE_DIR})
set(Angelscript_LIBRARIES ${Angelscript_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Angelscript FOUND_VAR Angelscript_FOUND
        REQUIRED_VARS Angelscript_INCLUDE_DIRS Angelscript_LIBRARIES
        )

if (Angelscript_FOUND)
    add_library(Angelscript::Angelscript INTERFACE IMPORTED)
    set_target_properties(Angelscript::Angelscript PROPERTIES
            INTERFACE_LINK_LIBRARIES "${Angelscript_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${Angelscript_INCLUDE_DIRS}"
            )
    add_library(Angelscript::use_namespace INTERFACE IMPORTED)
    set_target_properties(Angelscript::use_namespace PROPERTIES INTERFACE_COMPILE_DEFINITIONS AS_USE_NAMESPACE)
endif ()

mark_as_advanced(Angelscript_INCLUDE_DIR Angelscript_LIBRARY)
