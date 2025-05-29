cmake_minimum_required(VERSION 3.11.0)
project(implot_demos VERSION 0.1.0)

###############################################################################
# 3RD PARTY LIBS
###############################################################################

# LOCAL LIBS

# GLFW
add_subdirectory(3rdparty/glfw)

# GLAD
add_subdirectory(3rdparty/glad)

# NFD
add_subdirectory(3rdparty/nativefiledialog-extended)

# KISS FFT
set(KISS_SRC "3rdparty/kissfft/kiss_fft.h" 
             "3rdparty/kissfft/kiss_fftr.h" 
             "3rdparty/kissfft/kiss_fft.c" 
             "3rdparty/kissfft/kiss_fftr.c")

# EVERYTHING ELSE
include_directories("3rdparty" "3rdparty/kissfft")

###############################################################################
# IMGUI
###############################################################################

set(IMGUI_HEADERS
    ../imconfig.h
    ../imgui_internal.h
    ../imgui.h
    ../imstb_rectpack.h
    ../imstb_textedit.h
    ../imstb_truetype.h    
    ../backends/imgui_impl_glfw.h
    ../backends/imgui_impl_opengl3.h
    ../backends/imgui_impl_opengl3_loader.h
    ../misc/cpp/imgui_stdlib.h
)

set(IMGUI_SRC
    ../imgui_demo.cpp
    ../imgui_draw.cpp
    ../imgui_tables.cpp
    ../imgui_widgets.cpp
    ../imgui.cpp
    ../backends/imgui_impl_glfw.cpp
    ../backends/imgui_impl_opengl3.cpp
    ../misc/cpp/imgui_stdlib.cpp
)

find_package(OpenGL REQUIRED)

add_library(imgui ${IMGUI_HEADERS} ${IMGUI_SRC})
target_link_libraries(imgui PUBLIC glfw glad OpenGL::GL)
target_compile_definitions(imgui PRIVATE IMGUI_DLL_EXPORT)

include_directories(../ ../examples ../examples/libs/gl3w ../backends ../misc/cpp)

# imnodes
add_library(imnodes 3rdparty/imnodes/imnodes.h 3rdparty/imnodes/imnodes_internal.h 3rdparty/imnodes/imnodes.cpp)
target_link_libraries(imnodes imgui)
include_directories(3rdparty/imnodes)

###############################################################################
# IMPLOT
###############################################################################

set(IMPLOT_HEADERS ../implot.h ../implot_internal.h)
set(IMPLOT_SRC     ../implot.cpp ../implot_items.cpp ../implot_demo.cpp) 

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../implot/backends")
list(APPEND IMPLOT_HEADERS ../backends/implot_backend.h ../backends/implot_impl_opengl3.h)
list(APPEND IMPLOT_SRC ../backends/implot_backend.h ../backends/implot_impl_opengl3.cpp)
endif()

add_library(implot ${IMPLOT_HEADERS} ${IMPLOT_SRC})
target_link_libraries(implot PUBLIC imgui)
target_compile_definitions(implot PUBLIC IMPLOT_DEBUG IMPLOT_DLL_EXPORT IMPLOT_BACKEND_ENABLE_OPENGL3 IMGUI_IMPL_OPENGL_LOADER_GLAD)
set_property(TARGET implot PROPERTY CXX_STANDARD 11)


include_directories(../implot/)

###############################################################################
# APP FRAMEWORK
###############################################################################

add_library(app 
  common/App.h
  common/App.cpp
  common/Shader.h
  common/Native.h
  common/Native.cpp
  common/Image.h
  common/Fonts/Fonts.h
  common/Fonts/FontAwesome5.cpp
  common/Fonts/RobotoRegular.cpp
  common/Fonts/RobotoBold.cpp
  common/Fonts/RobotoItalic.cpp
  common/Fonts/RobotoMonoRegular.cpp
  common/Fonts/RobotoMonoBold.cpp
  common/Fonts/RobotoMonoItalic.cpp
)
target_include_directories(app PUBLIC common)
target_link_libraries(app implot nfd)
target_compile_features(app PRIVATE cxx_std_17)

###############################################################################
# DEMO APPS
###############################################################################

# main ImPlot demo
add_executable(demo "demos/demo.cpp")
target_link_libraries(demo app)

# perlin demo
add_executable(perlin "demos/perlin.cpp")
target_link_libraries(perlin app)
target_include_directories(perlin PRIVATE "3rdparty")

# graph demo
add_executable(graph "demos/graph.cpp")
target_link_libraries(graph app)
target_include_directories(graph PRIVATE "3rdparty")

# spectrogram demo
add_executable(spectrogram "demos/spectrogram.cpp" ${KISS_SRC})              
target_link_libraries(spectrogram app)        
target_compile_features(spectrogram PRIVATE cxx_std_17)
add_custom_command(TARGET spectrogram POST_BUILD  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/audio/aphex_twin_formula.wav $<TARGET_FILE_DIR:spectrogram>)
