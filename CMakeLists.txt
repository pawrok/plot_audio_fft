cmake_minimum_required(VERSION 3.27)
project(audio_fft_project)

set(CMAKE_CXX_STANDARD 23)
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
    message(FATAL_ERROR "audio_fft_project: Unable to find the VTK build folder.")
endif()

if(NOT(TARGET VTK::GUISupportQt))
    message(FATAL_ERROR "audio_fft_project: VTK not built with Qt support.")
endif()

if(NOT DEFINED VTK_QT_VERSION)
    set(VTK_QT_VERSION 6)
endif()

set(qt_components Core Gui Widgets)
if(${VTK_QT_VERSION} VERSION_GREATER_EQUAL 6)
    list(APPEND qt_components OpenGLWidgets)
endif()

list(SORT qt_components)
find_package(Qt${VTK_QT_VERSION} REQUIRED
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

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")

#------------------------- FFTW & libsndfile ---------------------------------------------
#find_package(SndFile REQUIRED)
#find_package(FFTW3 CONFIG REQUIRED)

# Set the location of FFTW3 (adjust accordingly):
set(FFTW3_ROOT "X:/libraries/fftw")
set(SNDFILE_ROOT "X:/libraries/sndfile")

# Specify where to find FFTW headers
include_directories("${FFTW3_ROOT}")
include_directories("${SNDFILE_ROOT}/include")

# Link to FFTW library files
set(FFTW3_LIB "${FFTW3_ROOT}/libfftw3-3.lib")
set(SndFile_LIBRARIES "${SNDFILE_ROOT}/lib/sndfile.lib")
#-----------------------------------------------------------------------------------------

add_executable(audio_fft_project MACOSX_BUNDLE
        ${CXX_FILES} ${UISrcs} ${QT_WRAP}
        LinePlot.cpp
        LinePlot.hpp
        FFT.cpp
        FFT.hpp
        DebugTimer.hpp
        AudioFile.cpp
        AudioFile.hpp
)

if (Qt${VTK_QT_VERSION}Widgets_VERSION VERSION_LESS "5.11.0")
    qt5_use_modules(audio_fft_project ${qt_components})
else()
    target_link_libraries(audio_fft_project ${qt_modules})
endif()

target_link_libraries(audio_fft_project ${VTK_LIBRARIES})
target_link_libraries(audio_fft_project ${SndFile_LIBRARIES})
target_link_libraries(audio_fft_project "${FFTW3_LIB}")

# vtk_module_autoinit is needed
vtk_module_autoinit(
        TARGETS audio_fft_project
        MODULES ${VTK_LIBRARIES}
)

add_custom_command(TARGET audio_fft_project POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FFTW3_ROOT}/libfftw3-3.dll"
        $<TARGET_FILE_DIR:audio_fft_project>
)

add_custom_command(TARGET audio_fft_project POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SNDFILE_ROOT}/bin/sndfile.dll"
        $<TARGET_FILE_DIR:audio_fft_project>
)

# TODO: vcpkg chyba zainstalowalo slaba wersje fftw, co nie ma threadow (?)