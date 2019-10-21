CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )
project(GameOpenGL)

SET(SOURCE
src/main.cpp
src/Animation.cpp
src/Camera.cpp
src/Draw_VAO.cpp
src/Input.cpp
src/utils/LibraryTranslations.cpp
src/LoadVAO.cpp
src/LoopManagement.cpp
src/utils/MathFunctions.cpp
src/utils/Array.cpp
src/utils/Allocator.cpp
src/Shader.cpp
src/Transform.cpp
src/User.cpp
src/VAO_Data.cpp
src/main.cpp
)

include_directories(
include
src
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules")

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third-party/SDL2-2.0.10")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third-party/assimp-master")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third-party/glm")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third-party/glew-2.1.0")

find_package( OPENGL   REQUIRED )

find_package( GLEW     REQUIRED )
include_directories(${GLEW_INCLUDE_DIR})

find_package( assimp   REQUIRED )
include_directories(${ASSIMP_INCLUDE_DIR})

find_package( SDL2     REQUIRED )
include_directories(${SDL2_INCLUDE_DIR})

find_package( GLM      REQUIRED )
include_directories(${GLM_INCLUDE_DIR})

add_executable(GameOpenGL
${SOURCE})

ASSIMP_COPY_BINARIES(${CMAKE_BINARY_DIR})
target_link_libraries(GameOpenGL ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${SDL2_LIBRARIES} ${ASSIMP_LIBRARY})

# Copy shaders, models and textures
add_custom_command(TARGET GameOpenGL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/$<CONFIG>")
