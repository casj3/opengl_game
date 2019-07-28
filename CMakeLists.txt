cmake_minimum_required (VERSION 3.11.4)
project(GameOpenGL)

MESSAGE("Current compiler should be MSVC and is ${CMAKE_C_COMPILER_ID}")

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
src/Shader.cpp
src/Transform.cpp
src/User.cpp
src/VAO_Data.cpp
src/main.cpp
)

include_directories(include src)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR})
set(LIB ${CMAKE_SOURCE_DIR}/lib)

find_package( OpenGL   REQUIRED )
find_package( GLEW     REQUIRED )
find_package( ASSIMP   REQUIRED )
find_package( SDL2     REQUIRED )

add_executable(GameOpenGL
${SOURCE})

target_include_directories(GameOpenGL PUBLIC ${INCLUDE_DIRS})

target_link_libraries(GameOpenGL ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${SDL2_LIBRARIES} ${ASSIMP_LIBRARIES})

# Copy dll:s
add_custom_command(TARGET GameOpenGL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/dll" "${CMAKE_BINARY_DIR}/$<CONFIG>")

# Copy shaders, models and textures
add_custom_command(TARGET GameOpenGL POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/$<CONFIG>")
