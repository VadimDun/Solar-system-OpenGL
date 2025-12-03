cmake_minimum_required(VERSION 3.10)
project(3DObjects)

# ============================================================================
# C++ Standard
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# ============================================================================
# OpenGL Configuration
# ============================================================================
set(OpenGL_GL_PREFERENCE GLVND)

# ============================================================================
# Find Required Packages
# ============================================================================
find_package(OpenGL 3.3 REQUIRED)
find_package(GLEW 2.0 REQUIRED)
find_package(glm REQUIRED)
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

# ============================================================================
# Project Structure
# ============================================================================
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/3d-objects/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/3d-objects/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/3d-objects/tetrahedron.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/3d-objects/cube1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/3d-objects/cube2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/3d-objects/circle.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/3d-objects/shader.h
)

# ============================================================================
# Create Executable
# ============================================================================
add_executable(lab12 ${SOURCES} ${HEADERS})

# ============================================================================
# Link Libraries
# ============================================================================
target_link_libraries(lab12 
    PRIVATE
        OpenGL::OpenGL           # OpenGL 3.3
        GLEW::GLEW               # GLEW для загрузки функций
        glm::glm                 # GLM для матричных операций
        sfml-graphics            # SFML Graphics
        sfml-window              # SFML Window (OpenGL контекст)
        sfml-system              # SFML System
)

# ============================================================================
# Include Directories
# ============================================================================
target_include_directories(lab12 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/3d-objects
        ${OPENGL_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
)

# ============================================================================
# Compiler Flags for Different Configurations
# ============================================================================
if(MSVC)
    target_compile_options(lab12 PRIVATE /W4)
else()
    target_compile_options(lab12 PRIVATE -Wall -Wextra -pedantic)
endif()

# ============================================================================
# Build Output - Исполняемый файл в папке build
# ============================================================================
set_target_properties(lab12 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# ============================================================================
# Version Information
# ============================================================================
message(STATUS "")
message(STATUS "========== 3D Objects Configuration ==========")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenGL Version: ${OPENGL_VERSION_STRING}")
message(STATUS "GLEW Version: ${GLEW_VERSION}")
message(STATUS "SFML Version: ${SFML_VERSION}")
message(STATUS "GLM Version: ${GLM_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output Directory: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "=============================================")
message(STATUS "")
