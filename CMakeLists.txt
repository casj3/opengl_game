CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )

set(CMAKE_BUILD_TYPE Release FORCE)

project(GameOpenGL)

SET(SOURCE
src/main.cpp
src/animate.cpp
src/camera.cpp
src/draw.cpp
src/input.cpp
src/utils/lib_convert.cpp
src/load.cpp
src/scene.cpp
src/utils/math_functions.cpp
src/utils/array_pool.cpp
src/gpu_program.cpp
src/transform.cpp
src/player.cpp
src/gpu_store.cpp
)

SET(HEADERS
src/animate.h
src/camera.h
src/draw.h
src/input.h
src/utils/lib_convert.h
src/load.h
src/scene.h
src/utils/math_functions.h
src/utils/array_pool.h
src/gpu_program.h
src/transform.h
src/player.h
src/gpu_store.h
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
