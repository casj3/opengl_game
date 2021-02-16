CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )

set(CMAKE_BUILD_TYPE Release FORCE)

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
src/utils/ArrayPool.cpp
src/Shader.cpp
src/Transform.cpp
src/User.cpp
src/VAO_Data.cpp
)

SET(HEADERS
src/Animation.h
src/Camera.h
src/Draw_VAO.h
src/Input.h
src/utils/LibraryTranslations.h
src/LoadVAO.h
src/LoopManagement.h
src/utils/MathFunctions.h
src/utils/Array.h
src/utils/ArrayPool.h
src/Shader.h
src/Transform.h
src/User.h
src/VAO_Data.h
)

include_directories(
include
src
)

set(ASSIMP_ROOT_DIR "third-party/assimp-master")
set(SDL2_ROOT_DIR "third-party/SDL2-2.0.10")
set(GLM_ROOT_DIR "third-party/glm")
set(GLEW_ROOT_DIR "third-party/glew-2.1.0")

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/${SDL2_ROOT_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/${ASSIMP_ROOT_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/${GLM_ROOT_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/${GLEW_ROOT_DIR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules")

find_package( OPENGL   REQUIRED ) 

find_package( GLEW     REQUIRED )
include_directories(${GLEW_INCLUDE_DIR})

find_package( assimp   REQUIRED )
include_directories(${ASSIMP_INCLUDE_DIR})

find_package( SDL2     REQUIRED )
include_directories(${SDL2_INCLUDE_DIR})

find_package( GLM      REQUIRED )
include_directories(${GLM_INCLUDE_DIR})

include(CopyDLLs)

add_executable(GameOpenGL
${SOURCE} ${HEADERS})

set_property(TARGET GameOpenGL PROPERTY CXX_STANDARD 17)

ASSIMP_COPY_BINARIES(${CMAKE_BINARY_DIR})
SDL2_COPY_BINARIES(${CMAKE_BINARY_DIR})
GLEW_COPY_BINARIES(${CMAKE_BINARY_DIR})

target_link_libraries(GameOpenGL ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${SDL2_LIBRARIES} ${ASSIMP_LIBRARY})

# Copy shaders, models and textures
add_custom_command(TARGET GameOpenGL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets "${CMAKE_BINARY_DIR}/$<CONFIG>/assets")
