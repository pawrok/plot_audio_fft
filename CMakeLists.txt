cmake_minimum_required(VERSION 3.27)
project(audio_fft_project)

set(CMAKE_CXX_STANDARD 23)

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
    cmake_policy(SET CMP0071 NEW)
endif()

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /arch:AVX2 /fp:fast /openmp")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -ffast-math -fopenmp")
endif()

# Prevent a "command line is too long" failure in Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")

#------------------------------- VTK ---------------------------------
find_package(VTK COMPONENTS
    ChartsCore
    ViewsContext2D
    CommonColor
    CommonCore
    CommonDataModel
    InteractionStyle
    RenderingContextOpenGL2
    RenderingCore
    RenderingFreeType
    RenderingGL2PSOpenGL2
    RenderingOpenGL2
    GUISupportQt
    REQUIRED
)
if(NOT VTK_FOUND)
    message(FATAL_ERROR "audio_fft_project: Unable to find the VTK build folder.")
endif()
if(NOT(TARGET VTK::GUISupportQt))
  message(FATAL_ERROR "VTK not built with Qt support.")
endif()

#------------------------------- Qt ---------------------------------
find_package(Qt6 COMPONENTS Core Gui Widgets OpenGLWidgets Concurrent REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#------------------------- FFTW & libsndfile -------------------------
# Set the location of FFTW3 (adjust accordingly):
set(FFTW3_ROOT "X:/libraries/fftw")
set(SNDFILE_ROOT "X:/libraries/sndfile")

# Specify where to find FFTW headers
include_directories("${FFTW3_ROOT}")
include_directories("${SNDFILE_ROOT}/include")

# Link to FFTW library files
set(FFTW3_LIB "${FFTW3_ROOT}/libfftw3f-3.lib")
set(SndFile_LIBRARIES "${SNDFILE_ROOT}/lib/sndfile.lib")
#---------------------------------------------------------------------

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DAQ
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DSP
    ${CMAKE_CURRENT_SOURCE_DIR}/src/View
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "src/*.hpp")
add_executable(audio_fft_project
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

target_link_libraries(audio_fft_project ${VTK_LIBRARIES})
target_link_libraries(audio_fft_project ${SndFile_LIBRARIES})
target_link_libraries(audio_fft_project "${FFTW3_LIB}")
target_link_libraries(audio_fft_project Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGLWidgets Qt6::Concurrent)

# Ensure all header directories are included in the target
target_include_directories(audio_fft_project PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DAQ
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DSP
    ${CMAKE_CURRENT_SOURCE_DIR}/src/View
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

# vtk_module_autoinit is needed
vtk_module_autoinit(
    TARGETS audio_fft_project
    MODULES ${VTK_LIBRARIES}
)
