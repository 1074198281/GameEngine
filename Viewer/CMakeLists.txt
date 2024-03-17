add_executable(MyEngineViewer main.cpp)
target_link_libraries(MyEngineViewer PlatformInterface Framework ${OPENGEX_LIBRARY} ${OPENDDL_LIBRARY} ${XG_LIBRARY} ${ZLIB_DEBUG_LIBRARY} d3d12 dxgi d3dcompiler D3dRHI)
if(DEBUG_FLAG EQUAL 1)
    target_link_libraries(MyEngineViewer 
    ${BULLETLINEARMATH_DEBUG_LIBRARY}
    ${BULLETDYNAMICS_DEBUG_LIBRARY}
    ${BULLETCOLLISION_DEBUG_LIBRARY})
elseif(DEBUG_FLAG EQUAL 0)
    target_link_libraries(MyEngineViewer 
    ${BULLETLINEARMATH_RELEASE_LIBRARY}
    ${BULLETDYNAMICS_RELEASE_LIBRARY}
    ${BULLETCOLLISION_RELEASE_LIBRARY})
endif()