file(GLOB_RECURSE WINDOWS_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE EXCLUDE_WINDOWS_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/*.h ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/*.hpp)
list(REMOVE_ITEM WINDOWS_HEADER_FILES  ${EXCLUDE_WINDOWS_HEADER_FILES} )

MESSAGE( STATUS "path :" ${CMAKE_CURRENT_SOURCE_DIR})
source_group("Header Files" FILES ${WINDOWS_HEADER_FILES}) 


add_library(WindowsApplication WindowsApplication.cpp)

set(WINDOWS_SRC_LIST
WindowsApplication.cpp
D3d12Application.cpp
)

add_executable(MyGameEngineD3d12 ${WINDOWS_HEADER_FILES} ${WINDOWS_SRC_LIST})
target_link_libraries(MyGameEngineD3d12 Common ${OPENGEX_LIBRARY} ${OPENDDL_LIBRARY} ${XG_LIBRARY} d3d12 dxgi d3dcompiler D3dRHI)
