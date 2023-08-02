MESSAGE("current path:" ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core/Command)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core/Common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core/Pipeline)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core/Resource)

#Set D3dCore files
file(GLOB D3d_CORE_HEADER_FILES
${CMAKE_CURRENT_SOURCE_DIR}/Core/*.h
${CMAKE_CURRENT_SOURCE_DIR}/Core/Command/*.h
${CMAKE_CURRENT_SOURCE_DIR}/Core/Common/*.h
${CMAKE_CURRENT_SOURCE_DIR}/Core/Pipeline/*.h
${CMAKE_CURRENT_SOURCE_DIR}/Core/Resource/*.h)

file(GLOB D3d_CORE_SOURCE_FILES  
${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Core/Command/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Core/Common/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Core/Pipeline/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Core/Resource/*.cpp)

#D3dRHI
file(GLOB D3d_RHI_HEADER_FILES
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

file(GLOB D3d_RHI_SOURCE_FILES
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


source_group(include/D3dCoreInclude FILES ${D3d_CORE_HEADER_FILES})
source_group(src/D3dCoreSource FILES ${D3d_CORE_SOURCE_FILES})
source_group(include/D3dInclude FILES ${D3d_RHI_HEADER_FILES})
source_group(src/D3dSource FILES ${D3d_RHI_SOURCE_FILES})
source_group(Shaders FILES ${ASSET_SHADERS})

add_library(D3dRHI STATIC ${D3d_CORE_HEADER_FILES} ${D3d_CORE_SOURCE_FILES} ${D3d_RHI_HEADER_FILES} ${D3d_RHI_SOURCE_FILES} ${ASSET_SHADERS})
target_link_libraries(D3dRHI ${ZLIB_DEBUG_LIBRARY} d3d12 dxguid dxgi d3dcompiler)