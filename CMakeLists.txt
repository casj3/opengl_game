cmake_minimum_required (VERSION 3.11.4)
project(GameOpenGL)

MESSAGE("Current compiler should be MSVC and is ${CMAKE_C_COMPILER_ID}")

SET(SOURCE
src/main.cpp
src/Animation.cpp
src/Camera.cpp
src/Draw_VAO.cpp
src/Input.cpp
src/LibraryTranslations.cpp
src/LoadVAO.cpp
src/LoopManagement.cpp
src/MathFunctions.cpp
src/Shader.cpp
src/Transform.cpp
src/User.cpp
src/VAO_Data.cpp
src/main.cpp
)

include_directories(include)

#set(GLEW_INCLUDE_DIR include/GL)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR})
set(LIB ${CMAKE_SOURCE_DIR}/lib)
#set(SDL2_LIBRARIES lib/SDL2.lib lib/SDL2main.lib lib/SDL2test.lib)

find_package( OpenGL   REQUIRED )
find_package( GLEW     REQUIRED )
find_package( SDL2     REQUIRED )

add_executable(GameOpenGL
${SOURCE})

target_include_directories(GameOpenGL PUBLIC ${INCLUDE_DIRS})

target_link_libraries(GameOpenGL ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${SDL2_LIBRARIES} ${LIB}/assimp.lib)
