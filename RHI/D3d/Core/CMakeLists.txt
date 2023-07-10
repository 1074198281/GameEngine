include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Command)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Pipeline)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Resource)

file(GLOB_RECURSE D3d_CORE_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE D3d_CORE_SOURCE_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
source_group("D3dCore Files" FILES D3d_RHI_HEADER_FILES)
source_group("D3dCoreSrc Files" FILES D3d_RHI_SOURCE_FILES)


add_library(D3dCore ${D3d_CORE_HEADER_FILES} ${D3d_CORE_SOURCE_FILES})
#target_link_libraries(D3dCore d3d12 dxgi d3dcompiler)