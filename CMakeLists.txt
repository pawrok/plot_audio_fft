cmake_minimum_required(VERSION 3.27)
project(audio_fft_project)

set(CMAKE_CXX_STANDARD 23)

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
    cmake_policy(SET CMP0071 NEW)
endif()

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")

#------------------------------- VTK ---------------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(VTK_DIR "D:/VTK/buildd")
else ()
    set(VTK_DIR "D:/VTK/build")
endif()
find_package(VTK COMPONENTS
        ChartsCore
        ViewsContext2D
        CommonColor
        CommonCore
        CommonDataModel
        FiltersSources
        InteractionStyle
        RenderingContextOpenGL2
        RenderingCore
        RenderingFreeType
        RenderingGL2PSOpenGL2
        RenderingOpenGL2
)
if(NOT VTK_FOUND)
    message(FATAL_ERROR "audio_fft_project: Unable to find the VTK build folder.")
endif()

#------------------------- FFTW & libsndfile -------------------------
# Set the location of FFTW3 (adjust accordingly):
set(FFTW3_ROOT "X:/libraries/fftw")
set(SNDFILE_ROOT "X:/libraries/sndfile")

# Specify where to find FFTW headers
include_directories("${FFTW3_ROOT}")
include_directories("${SNDFILE_ROOT}/include")

# Link to FFTW library files
set(FFTW3_LIB "${FFTW3_ROOT}/libfftw3-3.lib")
set(SndFile_LIBRARIES "${SNDFILE_ROOT}/lib/sndfile.lib")
#---------------------------------------------------------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB CXX_FILES *.cpp)
add_executable(audio_fft_project
        ${CXX_FILES}
        LinePlot.cpp
        LinePlot.hpp
        FFT.cpp
        FFT.hpp
        DebugTimer.hpp
        AudioFile.cpp
        AudioFile.hpp
)

target_link_libraries(audio_fft_project ${VTK_LIBRARIES})
target_link_libraries(audio_fft_project ${SndFile_LIBRARIES})
target_link_libraries(audio_fft_project "${FFTW3_LIB}")

# vtk_module_autoinit is needed
vtk_module_autoinit(
        TARGETS audio_fft_project
        MODULES ${VTK_LIBRARIES}
)

#------------------------- POST BUILD -------------------------------
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
