# add algorithm files
file(GLOB ALGORITHM_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/Algorithm/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Algorithm/*.hpp)
file(GLOB ALGORITHM_SOURCE_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/Algorithm/*.cpp)
source_group(include/Algorithm FILES ${ALGORITHM_HEADER_FILES})
source_group(src/Algorithm FILES ${ALGORITHM_SOURCE_FILES})

# add common files
file(GLOB COMMON_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/Common/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Common/*.hpp)
file(GLOB COMMON_SOURCE_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/Common/*.cpp)
source_group(include/Common FILES ${COMMON_HEADER_FILES})
source_group(src/Common FILES ${COMMON_SOURCE_FILES})

# add math calc files
file(GLOB MATH_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/GeomMath/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/GeomMath/*.h)
file(GLOB MATH_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/GeomMath/*.cpp )
source_group(include/Math FILES ${MATH_HEADER_FILES})
source_group(src/Math FILES ${MATH_SOURCE_FILES})

# add Interface files
file(GLOB INTERFACE_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/Interface/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Interface/*.hpp)
source_group(include/Interface FILES ${INTERFACE_HEADER_FILES})

# add Manager files
file(GLOB MANAGER_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/Manager/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Manager/*.hpp)
file(GLOB MANAGER_SOURCE_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/Manager/*.cpp)
source_group(include/Manager FILES ${MANAGER_HEADER_FILES})
source_group(src/Manager FILES ${MANAGER_SOURCE_FILES})

# add Parser files
file(GLOB PARSER_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/Parser/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Parser/*.hpp)
source_group(include/Parser FILES ${PARSER_HEADER_FILES})

# add SceneGraph files
file(GLOB SCENEGRAPH_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/SceneGraph/*.h ${CMAKE_CURRENT_SOURCE_DIR}/SceneGraph/*.hpp)
file(GLOB SCENEGRAPH_SOURCE_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/SceneGraph/*.cpp)
source_group(include/SceneGraph FILES ${SCENEGRAPH_HEADER_FILES})
source_group(src/SceneGraph FILES ${SCENEGRAPH_SOURCE_FILES})

# add Pass files
file(GLOB DISPATCHPASS_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Pass/DispatchPass/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Pass/DispatchPass/*.hpp)
file(GLOB DISPATCHPASS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Pass/DispatchPass/*.cpp)
source_group(include/Pass/DispatchPass FILES ${DISPATCHPASS_HEADER_FILES})
source_group(src/Pass/DispatchPass FILES ${DISPATCHPASS_SOURCE_FILES})

file(GLOB DRAWPASS_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Pass/DrawPass/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Pass/DrawPass/*.hpp)
file(GLOB DRAWPASS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Pass/DrawPass/*.cpp)
source_group(include/Pass/DrawPass FILES ${DRAWPASS_HEADER_FILES})
source_group(src/Pass/DrawPass FILES ${DRAWPASS_SOURCE_FILES})

file(GLOB DRAWSUBPASS_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Pass/DrawSubPass/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Pass/DrawSubPass/*.hpp)
file(GLOB DRAWSUBPASS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Pass/DrawSubPass/*.cpp)
source_group(include/Pass/DrawSubPass FILES ${DRAWSUBPASS_HEADER_FILES})
source_group(src/Pass/DrawSubPass FILES ${DRAWSUBPASS_SOURCE_FILES})


# add Physics files Physics library PhysX/Bullet3/My(no realization)
if(${PHYSICS_TYPE} STREQUAL "PhysX")
    file(GLOB PHYSICS_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Physics/PhysXPhysics/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Physics/PhysXPhysics/*.hpp)
    file(GLOB PHYSICS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Physics/PhysXPhysics/*.cpp)
    source_group(include/Physics/Phy FILES ${PHYSICS_HEADER_FILES})
    source_group(src/Physics/Phy FILES ${PHYSICS_SOURCE_FILES})
elseif(${PHYSICS_TYPE} STREQUAL "Bullet3")
    file(GLOB PHYSICS_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Physics/Bullet3Physics/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Physics/Bullet3Physics/*.hpp)
    file(GLOB PHYSICS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Physics/Bullet3Physics/*.cpp)
    source_group(include/Physics/Phy FILES ${PHYSICS_HEADER_FILES})
    source_group(src/Physics/Phy FILES ${PHYSICS_SOURCE_FILES})
elseif(${PHYSICS_TYPE} STREQUAL "MyPhysics")
    # no realization
endif()

file(GLOB CHEMISTRY_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Physics/MyChemistry/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Physics/MyChemistry/*.hpp)
file(GLOB CHEMISTRY_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Physics/MyChemistry/*.cpp)
source_group(include/Physics/Chem FILES ${CHEMISTRY_HEADER_FILES})
source_group(src/Physics/Chem FILES ${CHEMISTRY_SOURCE_FILES})

# add library
add_library(Framework
${ALGORITHM_HEADER_FILES} ${ALGORITHM_SOURCE_FILES}
${COMMON_HEADER_FILES} ${COMMON_SOURCE_FILES}
${MATH_HEADER_FILES} ${MATH_SOURCE_FILES}
${INTERFACE_HEADER_FILES}
${MANAGER_HEADER_FILES} ${MANAGER_SOURCE_FILES}
${PARSER_HEADER_FILES}
${DISPATCHPASS_HEADER_FILES} ${DISPATCHPASS_SOURCE_FILES}
${DRAWPASS_HEADER_FILES} ${DRAWPASS_SOURCE_FILES}
${DRAWSUBPASS_HEADER_FILES} ${DRAWSUBPASS_SOURCE_FILES}
${SCENEGRAPH_HEADER_FILES} ${SCENEGRAPH_SOURCE_FILES}
${PHYSICS_HEADER_FILES} ${PHYSICS_SOURCE_FILES}
${CHEMISTRY_HEADER_FILES} ${CHEMISTRY_SOURCE_FILES} 
)

MESSAGE("DEBUG_FLAG")

if(DEBUG_FLAG EQUAL 1)
    MESSAGE("Using Debug External Library")
    target_link_libraries(Framework 
    ${IMGUI_DEBUG_LIBRARY})

    if(${PHYSICS_TYPE} STREQUAL "PhysX")
        target_link_libraries(Framework ${PHYSX_DEBUG_LIBRARY} ${PHYSXCOMMON_DEBUG_LIBRARY} ${PHYSXFOUNDATION_DEBUG_LIBRARY} ${PHYSXPVDSDK_DEBUG_LIBRARY}  ${PHYSXEXTENSIONS_DEBUG_LIBRARY})
    elseif(${PHYSICS_TYPE} STREQUAL "Bullet3")
        target_link_libraries(Framework ${BULLETLINEARMATH_DEBUG_LIBRARY} ${BULLETDYNAMICS_DEBUG_LIBRARY} ${BULLETCOLLISION_DEBUG_LIBRARY})
    elseif(${PHYSICS_TYPE} STREQUAL "MyPhysics")
        # no realization
    endif()
elseif(DEBUG_FLAG EQUAL 0)
    MESSAGE("Using Release External Library")
    target_link_libraries(Framework 
    ${IMGUI_RELEASE_LIBRARY})

    if(${PHYSICS_TYPE} STREQUAL "PhysX")
        target_link_libraries(Framework ${PHYSX_RELEASE_LIBRARY} ${PHYSXCOMMON_RELEASE_LIBRARY} ${PHYSXFOUNDATION_RELEASE_LIBRARY} ${PHYSXPVDSDK_RELEASE_LIBRARY}  ${PHYSXEXTENSIONS_RELEASE_LIBRARY})
    elseif(${PHYSICS_TYPE} STREQUAL "Bullet3")
        target_link_libraries(Framework ${BULLETLINEARMATH_RELEASE_LIBRARY} ${BULLETDYNAMICS_RELEASE_LIBRARY} ${BULLETCOLLISION_RELEASE_LIBRARY})
    elseif(${PHYSICS_TYPE} STREQUAL "MyPhysics")
        # no realization
    endif()
endif()