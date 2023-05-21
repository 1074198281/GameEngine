file(GLOB_RECURSE COMMON_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
 ${CMAKE_CURRENT_SOURCE_DIR}/../GeomMath/geommath.hpp ${CMAKE_CURRENT_SOURCE_DIR}/glTF/*.h ${CMAKE_CURRENT_SOURCE_DIR}/glTF/*.hpp)
source_group("Header Files" FILES ${COMMON_HEADER_FILES}) 


set(COMMON_SRC_LIST
Allocator.cpp
AssetLoader.cpp
BaseApplication.cpp
GraphicsManager.cpp
Image.cpp
MemoryManager.cpp
SceneManager.cpp
SceneObject.cpp
SceneParser.cpp
glTF/glTFParser.cpp
main.cpp
)

add_library(Common ${COMMON_HEADER_FILES} ${COMMON_SRC_LIST})
