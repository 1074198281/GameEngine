add_subdirectory(Core)

file(GLOB D3d_RHI_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/*.h )
file(GLOB D3d_RHI_SOURCE_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
source_group("D3dRHI Files" FILES D3d_RHI_HEADER_FILES)
source_group("D3dRHISrc Files" FILES D3d_RHI_SOURCE_FILES)


add_library(D3dRHI ${D3d_RHI_HEADER_FILES} ${D3d_RHI_SOURCE_FILES})
#target_link_libraries(D3dRHI D3dCore d3d12 dxgi d3dcompiler)