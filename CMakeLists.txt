cmake_minimum_required(VERSION 3.10)
project(SolarSystem)

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
# Project Structure - Солнечная система
# ============================================================================
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/src/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/src/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/src/solar_system.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/include/camera.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/include/shader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/include/obj_loader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/include/solar_system.h
)

# ============================================================================
# Проверка существования файлов
# ============================================================================
foreach(src ${SOURCES})
    if(NOT EXISTS ${src})
        message(WARNING "Файл не найден: ${src}")
    else()
        message(STATUS "✓ Найден: ${src}")
    endif()
endforeach()

# ============================================================================
# Create Executable
# ============================================================================
add_executable(SolarSystem ${SOURCES} ${HEADERS})

# ============================================================================
# Include Directories
# ============================================================================
target_include_directories(SolarSystem 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/include
        ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/src
        ${OPENGL_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ${SFML_INCLUDE_DIR}
)

# ============================================================================
# Link Libraries
# ============================================================================
target_link_libraries(SolarSystem 
    PRIVATE
        OpenGL::OpenGL           # OpenGL 3.3
        GLEW::GLEW               # GLEW для загрузки функций
        glm::glm                 # GLM для матричных операций
        sfml-graphics            # SFML Graphics (для Image)
        sfml-window              # SFML Window (OpenGL контекст)
        sfml-system              # SFML System
)

# ============================================================================
# Compiler Flags
# ============================================================================
if(MSVC)
    target_compile_options(SolarSystem PRIVATE /W4)
else()
    target_compile_options(SolarSystem PRIVATE -Wall -Wextra -pedantic)
endif()

# ============================================================================
# Build Output - Исполняемый файл в папке build/bin
# ============================================================================
set_target_properties(SolarSystem PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)

# ============================================================================
# ИСПРАВЛЕННО: Копирование моделей и текстур
# ============================================================================
# ВАЖНО: Создаем директории ПЕРЕД копированием!
add_custom_command(TARGET SolarSystem PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_CURRENT_BINARY_DIR}/bin/models"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_CURRENT_BINARY_DIR}/bin/textures"
    COMMENT "📁 Создаём директории для моделей и текстур..."
)

# ============================================================================
# Копируем модель fish.obj
# ============================================================================
add_custom_command(TARGET SolarSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/models/fish.obj"
        "${CMAKE_CURRENT_BINARY_DIR}/bin/models/fish.obj"
    COMMENT "📦 Копирую fish.obj..."
)

# ============================================================================
# Копируем текстуру fish.png
# ============================================================================
add_custom_command(TARGET SolarSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/textures/fish.png"
        "${CMAKE_CURRENT_BINARY_DIR}/bin/textures/fish.png"
    COMMENT "🎨 Копирую fish.png..."
)

# ============================================================================
# Version Information
# ============================================================================
message(STATUS "")
message(STATUS "========== Solar System Configuration ==========")
message(STATUS "Project: Solar System")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenGL Version: ${OPENGL_VERSION_STRING}")
message(STATUS "GLEW Version: ${GLEW_VERSION}")
message(STATUS "SFML Version: ${SFML_VERSION}")
message(STATUS "GLM Version: ${GLM_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output Directory: ${CMAKE_CURRENT_BINARY_DIR}/bin")
message(STATUS "Models Directory: ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/models")
message(STATUS "Textures Directory: ${CMAKE_CURRENT_SOURCE_DIR}/SolarSystem/textures")
message(STATUS "================================================")
message(STATUS "")