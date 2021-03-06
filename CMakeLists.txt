cmake_minimum_required(VERSION 2.8)

# define a macro that helps defining an option
macro(lxgui_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# set defaults
# this has to be done before the project() instruction!
lxgui_set_option(CMAKE_BUILD_TYPE Release STRING "Choose the type of build (Debug or Release)")
lxgui_set_option(LXGUI_BUILD_GUI_GL_IMPL TRUE BOOL "Build the OpenGL gui implementation")
lxgui_set_option(LXGUI_BUILD_INPUT_SFML_IMPL TRUE BOOL "Build the SFML input implementation")
lxgui_set_option(LXGUI_BUILD_INPUT_GLFW_IMPL TRUE BOOL "Build the GLFW input implementation")
lxgui_set_option(LXGUI_BUILD_INPUT_OIS_IMPL TRUE BOOL "Build the OIS input implementation")
lxgui_set_option(LXGUI_BUILD_TEST TRUE BOOL "Build the test program")

# project name
project(lxgui)

# setup version numbers
set(VERSION_MAJOR 1)
set(VERSION_MINOR 2)

# check compiler version for C++11 features
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-Wall)

    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)

    string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
    list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
    list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)
    set(GCC_VERSION ${GCC_MAJOR}.${GCC_MINOR})

    if(GCC_VERSION VERSION_EQUAL 4.6)
        message(STATUS "gcc version >= 4.6 (${GCC_VERSION})")
        add_definitions(-std=c++0x)
    elseif(GCC_VERSION VERSION_GREATER 4.6)
        message(STATUS "gcc version >= 4.6 (${GCC_VERSION})")
        add_definitions(-std=c++11)
    else()
        message(ERROR ": lxgui requires advanced features from the C++11 norm that are only available with gcc 4.6 or higher (your version: ${GCC_VERSION}). Please upgrade your compiler.")
    endif()
elseif(MSVC)
    add_definitions(/DMSVC)
    add_definitions(/W4)
    add_definitions(/GR)
    add_definitions(/EHs)

    add_definitions(/DNO_CPP11_EXPLICIT_CONV)
    add_definitions(/DNO_CPP11_DELETE_FUNCTION)
    add_definitions(/DNO_CPP11_FUNCTION_TEMPLATE_DEFAULT)
    add_definitions(/DNO_CPP11_CONSTEXPR)
    add_definitions(/DNO_CPP11_UNICODE_LITTERAL)
    add_definitions(/D_CRT_SECURE_NO_DEPRECATE)

    if(NOT (MSVC_VERSION VERSION_LESS 1600))
        message(STATUS "Using MSVC 2010: some C++11 features are not supported by this compiler. Workarounds are used but never perfectly mimic the C++11 code.")
    else()
        message(ERROR ": lxgui requires advanced features from the C++11 norm that are only available with MSVC 2010 or higher (your version: ${MSVC_VERSION}). Please upgrade your compiler.")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 2.99)
        message(STATUS "clang version >= 3.0 (${CMAKE_CXX_COMPILER_VERSION})")
    else()
        message(ERROR ": lxgui requires advanced features from the C++11 norm that are only available with clang 3.0 or higher (your version: ${CMAKE_CXX_COMPILER_VERSION}). Please upgrade your compiler.")
    endif()

    add_definitions(-Weverything)
    add_definitions(-Wno-c++98-compat-pedantic)
    add_definitions(-Wno-c++98-compat)
    add_definitions(-Wno-unused-parameter)
    add_definitions(-Wno-sign-conversion)
    add_definitions(-Wno-conversion)
    add_definitions(-Wno-missing-variable-declarations)
    add_definitions(-Wno-missing-prototypes)
    add_definitions(-Wno-padded)
    add_definitions(-Wno-float-equal)
    add_definitions(-Wno-unused-variable)
    add_definitions(-Wno-global-constructors)
    add_definitions(-Wno-exit-time-destructors)
    add_definitions(-Wno-weak-vtables)
    add_definitions(-Wno-covered-switch-default)
    add_definitions(-Wno-documentation-unknown-command)
    add_definitions(-Wno-unneeded-internal-declaration)
    add_definitions(-Wno-unused-function)
    add_definitions(-Wno-unused-macros)
    add_definitions(-Wno-switch-enum)
    add_definitions(-std=c++11)
else()
    message(WARNING "Warning: your compiler has not been setup by the CMake script, do not expect it to work")
endif()

# set OS preprocessor defines
if (UNIX)
    add_definitions(-DLINUX)
endif()
if (WINDOWS)
    add_definitions(-DWIN32)
endif()

# find libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(Lua51 REQUIRED)
find_package(Freetype)
find_package(PNG)
find_package(ZLIB)
find_package(GLFW)
find_package(SFML 2 COMPONENTS system window)
find_package(GLEW)
find_package(OpenGL)
find_package(OIS)

# add the subdirectories
add_subdirectory(utils)
add_subdirectory(xml)
add_subdirectory(luapp)
add_subdirectory(gui)
if(LXGUI_BUILD_GUI_GL_IMPL)
    if(OPENGL_FOUND AND GLEW_FOUND AND FREETYPE_FOUND AND PNG_FOUND AND ZLIB_FOUND)
        add_subdirectory(gui/impl/gui/gl)
    else()
        message(ERROR ": the OpenGL implementation of the GUI requires OpenGL, GLEW, freetype, libpng and zlib")
        set(LXGUI_BUILD_GUI_GL_IMPL FALSE)
    endif()
endif()
if(LXGUI_BUILD_INPUT_GLFW_IMPL)
    if(GLFW_FOUND)
        add_subdirectory(gui/impl/input/glfw)
    else()
        message(ERROR ": the GLFW implementation of the input requires the GLFW library")
        set(LXGUI_BUILD_INPUT_GLFW_IMPL FALSE)
    endif()
endif()
if(LXGUI_BUILD_INPUT_SFML_IMPL)
    if(SFML_FOUND)
        add_subdirectory(gui/impl/input/sfml)
    else()
        message(ERROR ": the SFML implementation of the input requires the SFML library")
        set(LXGUI_BUILD_INPUT_SFML_IMPL FALSE)
    endif()
endif()
if(LXGUI_BUILD_INPUT_OIS_IMPL)
    if(OIS_FOUND)
        add_subdirectory(gui/impl/input/ois)
    else()
        message(ERROR ": the OIS implementation of the input requires the OIS library")
        set(LXGUI_BUILD_INPUT_OIS_IMPL FALSE)
    endif()
endif()
if(LXGUI_BUILD_TEST)
    if(OPENGL_FOUND AND GLEW_FOUND AND FREETYPE_FOUND AND PNG_FOUND AND ZLIB_FOUND AND SFML_FOUND)
        add_subdirectory(gui/test)
    else()
        message(ERROR ": the test program requires OpenGL, GLEW, freetype, libpng, zlib and SFML.")
    endif()
endif()

