cmake_minimum_required(VERSION 3.16)
project(BasketballSimulator C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include Directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm/glm
)

# Source Files
set(SOURCES
    src/glad.c
    src/main.cpp
    src/Camera.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/Mesh.cpp
    src/Court.cpp
    src/Hoop.cpp
    src/Ball.cpp
    src/Physics.cpp
    src/Collision.cpp
    src/FontRenderer.cpp
    src/UI.cpp
    src/Game.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Link Libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw/lib-mingw/libglfw3dll.a opengl32 gdi32 user32 shell32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL dl pthread)
endif()

# Copy GLFW DLL and Shaders to build bin directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw/lib-mingw/glfw3.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/glfw3.dll
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)
