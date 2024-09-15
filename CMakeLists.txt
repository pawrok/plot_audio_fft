cmake_minimum_required(VERSION 3.27)
project(CLion_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
    cmake_policy(SET CMP0071 NEW)
endif()

find_package(VTK COMPONENTS
        ChartsCore
        ViewsContext2D
        CommonColor
        CommonCore
        CommonDataModel
        FiltersSources
        GUISupportQt
        InteractionStyle
        RenderingContextOpenGL2
        RenderingCore
        RenderingFreeType
        RenderingGL2PSOpenGL2
        RenderingOpenGL2
        GUISupportQt
        RenderingQt
)

if(NOT VTK_FOUND)
    message(FATAL_ERROR "CLion_test: Unable to find the VTK build folder.")
endif()

if(NOT(TARGET VTK::GUISupportQt))
    message(FATAL_ERROR "CLion_test: VTK not built with Qt support.")
endif()

if(NOT DEFINED VTK_QT_VERSION)
    set(VTK_QT_VERSION 6)
endif()

set(qt_components Core Gui Widgets)
if(${VTK_QT_VERSION} VERSION_GREATER_EQUAL 6)
    list(APPEND qt_components OpenGLWidgets)
endif()
list(SORT qt_components)
find_package(Qt${VTK_QT_VERSION} QUIET
    REQUIRED COMPONENTS ${qt_components}
)

foreach(_qt_comp IN LISTS qt_components)
    list(APPEND qt_modules "Qt${VTK_QT_VERSION}::${_qt_comp}")
endforeach()

message (STATUS "VTK_VERSION: ${VTK_VERSION}, Qt Version: ${Qt${VTK_QT_VERSION}Widgets_VERSION}")

# Instruct CMake to run moc and uic automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB UI_FILES *.ui)
file(GLOB QT_WRAP *.h)
file(GLOB CXX_FILES *.cpp)

# For VTK versions greater than or equal to 8.90.0:
#  CMAKE_AUTOUIC is ON so we handle uic automatically for Qt targets.
#  CMAKE_AUTOMOC is ON so we handle moc automatically for Qt targets.

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
# CMAKE_AUTOMOC in ON so the MOC headers will be automatically wrapped.

#------------------------- FFTW & libsndfile ---------------------------------------------
if(DEFINED ENV{SNDFILE_PATH})
    include_directories("$ENV{SNDFILE_PATH}\\include")
    set(SNDFILE_LIBRARIES "$ENV{SNDFILE_PATH}\\lib\\sndfile.lib")
else()
    message(SEND_ERROR "SNDFILE_PATH environment variable is not set.")
endif()

if(DEFINED ENV{FFTW_PATH})
    include_directories("$ENV{FFTW_PATH}")
    set(FFTW_LIBRARIES "$ENV{FFTW_PATH}\\libfftw3-3.lib")
else()
    message(SEND_ERROR "FFTW_PATH environment variable is not set.")
endif()
#-----------------------------------------------------------------------------------------

add_executable(CLion_test MACOSX_BUNDLE
        ${CXX_FILES} ${UISrcs} ${QT_WRAP}
        LinePlot.cpp
        LinePlot.h
        FFT.cpp
        FFT.h
)

if (Qt${VTK_QT_VERSION}Widgets_VERSION VERSION_LESS "5.11.0")
    qt5_use_modules(CLion_test ${qt_components})
else()
    target_link_libraries(CLion_test ${qt_modules})
endif()
target_link_libraries(CLion_test ${VTK_LIBRARIES})
# vtk_module_autoinit is needed
vtk_module_autoinit(
        TARGETS CLion_test
        MODULES ${VTK_LIBRARIES}
)

# FFTW & libsndfile
target_link_libraries(CLion_test ${FFTW_LIBRARIES} ${SNDFILE_LIBRARIES})

set(DLL_FILES "$ENV{FFTW_PATH}\\libfftw3-3.dll" "$ENV{SNDFILE_PATH}\\bin\\sndfile.dll")
foreach(dll ${DLL_FILES})
    add_custom_command(TARGET CLion_test POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${dll}"
            "${PROJECT_SOURCE_DIR}/cmake-build-debug-visual-studio")
endforeach()
