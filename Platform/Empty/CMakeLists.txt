add_executable(Empty EmptyApplication.cpp)
target_link_libraries(Empty Framework ${XG_LIBRARY} ${OPENDDL_LIBRARY} ${OPENGEX_LIBRARY} ${ZLIB_DEBUG_LIBRARY} D3dRHI)

